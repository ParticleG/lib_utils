//
// Created by Parti on 2021/2/4.
//

#include <models/App.h>
#include <models/Auth.h>
#include <models/Message.h>
#include <utils/Authorizer.h>
#include <utils/Crypto.h>
#include <utils/Utils.h>

using namespace drogon;
using namespace drogon_model;
using namespace tech::utils;
using namespace std;

Authorizer::Status Authorizer::accessToken(
        const int64_t &id,
        const string &accessToken,
        const string &newExpireTime,
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
        if (Utils::toDate() > Utils::toDate(auth.getValueOfAccessTokenExpireTime())) {
            return Authorizer::Status::Expired;
        }

        auth.setAccessTokenExpireTime(newExpireTime);
        authMapper.update(auth);

        result["id"] = id;

        return Authorizer::Status::OK;
    } catch (const orm::DrogonDbException &e) {
        LOG_ERROR << "error:" << e.base().what();
        return Authorizer::Status::InternalError;
    }
}

Authorizer::Status Authorizer::authToken(
        const int64_t &id,
        const string &authToken,
        const string &newExpireTime,
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
        if (Utils::toDate() > Utils::toDate(auth.getValueOfAuthTokenExpireTime())) {
            return Authorizer::Status::Expired;
        }
        auth.setAuthTokenExpireTime(newExpireTime);
        authMapper.update(auth);

        result["id"] = id;

        return Authorizer::Status::OK;
    } catch (const orm::DrogonDbException &e) {
        LOG_ERROR << "error:" << e.base().what();
        return Authorizer::Status::InternalError;
    }
}

Authorizer::Status Authorizer::password(
        const string &email,
        const string &password,
        const string &newExpireTime,
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
        Mapper<Techmino::Auth> authMapper(app().getDbClient());
        Techmino::Auth newAuth;
        newAuth.setId(auth["_id"].as<int64_t>());
        newAuth.setAuthToken(drogon::utils::getUuid());
        newAuth.setAuthTokenExpireTime(newExpireTime);
        authMapper.update(newAuth);

        result["id"] = newAuth.getValueOfId();

        return Authorizer::Status::OK;
    } catch (const orm::DrogonDbException &e) {
        LOG_ERROR << "error:" << e.base().what();
        return Authorizer::Status::InternalError;
    }
}

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
