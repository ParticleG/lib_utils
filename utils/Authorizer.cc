//
// Created by Parti on 2021/2/4.
//

#include <models/App.h>
#include <models/Auth.h>
#include <models/Message.h>
#include <plugins/Configurator.h>
#include <structures/User.h>
#include <utils/Authorizer.h>
#include <utils/Crypto.h>

using namespace drogon;
using namespace drogon_model;
using namespace tech::plugins;
using namespace tech::structures;
using namespace tech::utils;
using namespace std;

Authorizer::Status Authorizer::accessToken(
        const int64_t &id,
        const string &accessToken,
        Json::Value &result
) {
    if (id < 0 || accessToken.empty()) {
        return Authorizer::Status::InvalidComponents;
    }
    try {
        orm::Mapper<Techmino::Auth> authMapper(app().getDbClient());
        auto matchedAuths = authMapper.findBy(Criteria(Techmino::Auth::Cols::__id, CompareOperator::EQ, id));
        if (matchedAuths.empty()) {
            return Authorizer::Status::NotFound;
        }
        auto auth = matchedAuths[0];
        if (accessToken != auth.getValueOfAccessToken()) {
            return Authorizer::Status::Incorrect;
        }
        if (trantor::Date::now() > trantor::Date::fromDbStringLocal(auth.getValueOfAccessTokenExpireTime())) {
            return Authorizer::Status::Expired;
        }
        auto configurator = app().getPlugin<Configurator>();
        auth.setAccessTokenExpireTime(trantor::Date::now().after(configurator->getAccessExpire()).toDbStringLocal());
        authMapper.update(auth);

        result["id"] = id;

        return Authorizer::Status::OK;
    } catch (const orm::DrogonDbException &e) {
        LOG_ERROR << "error:" << e.base().what();
        return Authorizer::Status::InternalError;
    }
}

bool Authorizer::authToken(
        const WebSocketConnectionPtr &wsConnPtr,
        const shared_ptr<Techmino::Auth> &auth,
        CloseCode &code,
        Json::Value &result
) {
    try {
        orm::Mapper<Techmino::Auth> authMapper(app().getDbClient());
        auto configurator = app().getPlugin<Configurator>();
        auth->setAuthTokenExpireTime(trantor::Date::now().after(configurator->getAuthExpire()).toDbStringLocal());
        auth->setAccessToken(Crypto::keccak(drogon::utils::getUuid()));
        auth->setAccessTokenExpireTime(trantor::Date::now().after(configurator->getAccessExpire()).toDbStringLocal());
        authMapper.update(*auth);
        result["message"] = "OK";
        return true;
    } catch (const orm::DrogonDbException &e) {
        LOG_ERROR << "error:" << e.base().what();
        code = CloseCode::kUnexpectedCondition;
        result["message"] = "Internal error";
        return false;
    }
}

Authorizer::Status Authorizer::authToken(
        const int64_t &id,
        const string &authToken,
        Json::Value &result
) {
    if (id < 0 || authToken.empty()) {
        return Authorizer::Status::InvalidComponents;
    }
    try {
        orm::Mapper<Techmino::Auth> authMapper(app().getDbClient());
        auto matchedAuths = authMapper.findBy(Criteria(Techmino::Auth::Cols::__id, CompareOperator::EQ, id));
        if (matchedAuths.empty()) {
            return Authorizer::Status::NotFound;
        }
        auto auth = matchedAuths[0];
        if (authToken != auth.getValueOfAuthToken()) {
            return Authorizer::Status::Incorrect;
        }
        if (trantor::Date::now() > trantor::Date::fromDbStringLocal(auth.getValueOfAuthTokenExpireTime())) {
            return Authorizer::Status::Expired;
        }
        auto configurator = app().getPlugin<Configurator>();
        auth.setAuthTokenExpireTime(trantor::Date::now().after(configurator->getAuthExpire()).toDbStringLocal());
        authMapper.update(auth);

        result["id"] = id;

        return Authorizer::Status::OK;
    } catch (const orm::DrogonDbException &e) {
        LOG_ERROR << "error:" << e.base().what();
        return Authorizer::Status::InternalError;
    }
}

Authorizer::Status Authorizer::password(
        const WebSocketConnectionPtr &wsConnPtr,
        const string &email,
        const string &password,
        Json::Value &result
) {
    if (email.empty() || password.empty()) {
        result["message"] = "Wrong format";
        result["reason"] = "Empty email or password";
        return Authorizer::Status::InvalidComponents;
    }
    try {
        auto matchedUsers = app().getDbClient()->execSqlSync(
                "select * from auth "
                "where email = $1 "
                "and crypt($2, password) = password",
                email, password
        );
        if (matchedUsers.empty()) {
            result["message"] = "Email or Password is incorrect";
            return Authorizer::Status::Incorrect;
        }

        auto configurator = app().getPlugin<Configurator>();
        Mapper<Techmino::Auth> authMapper(app().getDbClient());
        Techmino::Auth auth;
        auth.setId(matchedUsers[0]["_id"].as<int64_t>());
        auth.setAuthToken(drogon::utils::getUuid());
        auth.setAuthTokenExpireTime(trantor::Date::now().after(configurator->getAuthExpire()).toDbStringLocal());
        authMapper.update(auth);

        result["message"] = "OK";
        result["id"] = auth.getValueOfId();
        result["authToken"] = auth.getValueOfAuthToken();
        return Authorizer::Status::OK;
    } catch (const orm::DrogonDbException &e) {
        LOG_ERROR << "error:" << e.base().what();
        result["message"] = "Internal error";
        return Authorizer::Status::InternalError;
    }
}

Authorizer::Status Authorizer::password(
        const string &email,
        const string &password,
        Json::Value &result
) {
    if (email.empty() || password.empty()) {
        return Authorizer::Status::InvalidComponents;
    }
    try {
        auto matchedAuths = app().getDbClient()->execSqlSync(
                "select * from auth "
                "where email = $1 "
                "and crypt($2, password) = password",
                email, password
        );
        if (matchedAuths.empty()) {
            return Authorizer::Status::Incorrect;
        }
        auto auth = matchedAuths[0];
        auto configurator = app().getPlugin<Configurator>();
        Mapper<Techmino::Auth> authMapper(app().getDbClient());
        Techmino::Auth newAuth;
        newAuth.setId(auth["_id"].as<int64_t>());
        newAuth.setAuthToken(drogon::utils::getUuid());
        newAuth.setAuthTokenExpireTime(trantor::Date::now().after(configurator->getAuthExpire()).toDbStringLocal());
        authMapper.update(newAuth);

        result["id"] = newAuth.getValueOfId();

        return Authorizer::Status::OK;
    } catch (const orm::DrogonDbException &e) {
        LOG_ERROR << "error:" << e.base().what();
        return Authorizer::Status::InternalError;
    }
}


//bool Authorizer::versionCode(
//        const WebSocketConnectionPtr &wsConnPtr,
//        const int &versionCode,
//        CloseCode &code,
//        Json::Value &result
//) {
//    orm::Mapper<Techmino::App> appMapper(app().getDbClient());
//    auto leastApp = appMapper.orderBy(Techmino::App::Cols::_version_code, SortOrder::DESC).limit(1)
//            .findBy(Criteria(Techmino::App::Cols::_compatible, CompareOperator::EQ, false))[0];
//
//    if (versionCode < leastApp.getValueOfVersionCode()) {
//        code = CloseCode::kViolation;
//        result["message"] = "Outdated version";
//        return false;
//    } else {
//        result["message"] = "OK";
//        wsConnPtr->setContext(make_shared<App>(versionCode));
//        return true;
//    }
//}

Authorizer::Status Authorizer::versionCode(
        const int &versionCode,
        Json::Value &result
) {
    if (versionCode < 0) {
        return Authorizer::Status::InvalidComponents;
    }
    try {
        orm::Mapper<Techmino::App> appMapper(app().getDbClient());
        orm::Mapper<Techmino::Message> messageMapper(app().getDbClient());
        auto newestApp = appMapper.orderBy(Techmino::App::Cols::_version_code, SortOrder::DESC).limit(1).findAll()[0];
        auto leastApp = appMapper.orderBy(Techmino::App::Cols::_version_code, SortOrder::DESC).limit(1)
                .findBy(Criteria(Techmino::App::Cols::_compatible, CompareOperator::EQ, false))[0];
        auto notice = messageMapper.orderBy(Techmino::Message::Cols::_id, SortOrder::DESC)
                .findBy(Criteria(Techmino::Message::Cols::_type, CompareOperator::EQ, "notice"))[0];

        if (versionCode < leastApp.getValueOfVersionCode()) {
            result["newest"]["code"] = newestApp.getValueOfVersionCode();
            result["newest"]["name"] = newestApp.getValueOfVersionName();
            result["newest"]["content"] = newestApp.getValueOfVersionContent();
            result["least"]["code"] = leastApp.getValueOfVersionCode();
            result["least"]["name"] = leastApp.getValueOfVersionName();
            result["least"]["content"] = leastApp.getValueOfVersionContent();
            result["notice"] = notice.getValueOfContent();
            return Authorizer::Status::Expired;
        } else {
            result["versionCode"] = versionCode;
            result["content"]["newest"]["code"] = newestApp.getValueOfVersionCode();
            result["content"]["newest"]["name"] = newestApp.getValueOfVersionName();
            result["content"]["newest"]["content"] = newestApp.getValueOfVersionContent();
            result["content"]["notice"] = notice.getValueOfContent();
            return Authorizer::Status::OK;
        }
    } catch (const orm::DrogonDbException &e) {
        LOG_ERROR << "error:" << e.base().what();
        return Authorizer::Status::InternalError;
    }
}
