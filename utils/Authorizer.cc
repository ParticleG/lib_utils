//
// Created by Parti on 2021/2/4.
//

#include <models/App.h>
#include <models/Auth.h>
#include <plugins/Configurator.h>
#include <structures/App.h>
#include <utils/Authorizer.h>
#include <utils/Crypto.h>

using namespace tech::structures;
using namespace tech::utils;
using namespace drogon_model;
using namespace drogon;
using namespace std;

bool Authorizer::accessToken(const string &email, const string &accessToken, CloseCode &code, string &reason) {
    if (email.empty() || accessToken.empty()) {
        code = CloseCode::kInvalidMessage;
        reason = "Invalid parameters";
        return false;
    }
    try {
        orm::Mapper<Techmino::Auth> authMapper(app().getDbClient());
        auto matchedUsers = authMapper.findBy(Criteria(Techmino::Auth::Cols::_email, CompareOperator::EQ, email));
        if (matchedUsers.empty()) {
            code = CloseCode::kInvalidMessage;
            reason = "User not found";
            return false;
        }
        if (accessToken != matchedUsers[0].getValueOfAccessToken()) {
            code = CloseCode::kInvalidMessage;
            reason = "Access Token is incorrect";
            return false;
        }
        if (trantor::Date::now() >
            trantor::Date::fromDbStringLocal(matchedUsers[0].getValueOfAccessTokenExpireTime())) {
            code = CloseCode::kInvalidMessage;
            reason = "Access Token is expired";
            return false;
        }
        return true;
    } catch (const orm::DrogonDbException &e) {
        LOG_ERROR << "error:" << e.base().what();
        code = CloseCode::kUnexpectedCondition;
        reason = "Internal error";
        return false;
    }
}

bool Authorizer::accessToken(
        const string &email,
        const string &accessToken,
        HttpStatusCode &code, Json::Value &body
) {
    if (email.empty() || accessToken.empty()) {
        code = k400BadRequest;
        body["message"] = "Invalid parameters";
        return false;
    }
    try {
        orm::Mapper<Techmino::Auth> authMapper(app().getDbClient());
        auto matchedUsers = authMapper.findBy(Criteria(Techmino::Auth::Cols::_email, CompareOperator::EQ, email));
        if (matchedUsers.empty()) {
            code = k404NotFound;
            body["message"] = "User not found";
            return false;
        }
        if (accessToken != matchedUsers[0].getValueOfAccessToken()) {
            code = k403Forbidden;
            body["message"] = "Access Token is incorrect";
            return false;
        }
        if (trantor::Date::now() >
            trantor::Date::fromDbStringLocal(matchedUsers[0].getValueOfAccessTokenExpireTime())) {
            code = k401Unauthorized;
            body["message"] = "Access Token is expired";
            return false;
        }
        return true;
    } catch (const orm::DrogonDbException &e) {
        LOG_ERROR << "error:" << e.base().what();
        code = k500InternalServerError;
        body["message"] = "Internal error";
        return false;
    }
}

bool Authorizer::authToken(
        const string &email,
        const string &authToken,
        HttpStatusCode &code,
        Json::Value &body
) {
    if (email.empty() || authToken.empty()) {
        code = k400BadRequest;
        body["message"] = "Invalid parameters";
        return false;
    }
    try {
        orm::Mapper<Techmino::Auth> authMapper(app().getDbClient());
        auto matchedUsers = authMapper.findBy(Criteria(Techmino::Auth::Cols::_email, CompareOperator::EQ, email));
        if (matchedUsers.empty()) {
            code = k404NotFound;
            body["message"] = "User not found";
            return false;
        }
        if (authToken != matchedUsers[0].getValueOfAuthToken()) {
            code = k403Forbidden;
            body["message"] = "Auth Token is incorrect";
            return false;
        }
        if (trantor::Date::now() >
            trantor::Date::fromDbStringLocal(matchedUsers[0].getValueOfAuthTokenExpireTime())) {
            code = k401Unauthorized;
            body["message"] = "Auth Token is expired";
            return false;
        }
        return true;
    } catch (const orm::DrogonDbException &e) {
        LOG_ERROR << "error:" << e.base().what();
        code = k500InternalServerError;
        body["message"] = "Internal error";
        return false;
    }
}

bool Authorizer::password(const string &email, const string &password, HttpStatusCode &code, Json::Value &body) {
    if (email.empty() || password.empty()) {
        code = k400BadRequest;
        body["message"] = "Invalid parameters";
        return false;
    }
    try {
        auto matchedUsers = app().getDbClient()->execSqlSync(
                "select * from auth "
                "where email = $1 "
                "and crypt($2, password) = password",
                email, password
        );
        if (matchedUsers.empty()) {
            code = k403Forbidden;
            body["message"] = "Email:Password pair is incorrect.";
            return false;
        }
        return true;
    } catch (const orm::DrogonDbException &e) {
        LOG_ERROR << "error:" << e.base().what();
        code = k500InternalServerError;
        body["message"] = "Internal error";
        return false;
    }
}

bool Authorizer::password(
        const WebSocketConnectionPtr &wsConnPtr,
        const string &email,
        const string &password,
        CloseCode &code,
        Json::Value &result
) {
    if (email.empty() || password.empty()) {
        result["message"] = "Wrong format";
        result["reason"] = "Required string type 'email' and 'password' in 'data'";
        return false;
    }
    try {
        auto matchedUsers = app().getDbClient()->execSqlSync(
                "select * from auth "
                "where email = $1 "
                "and crypt($2, password) = password",
                email, password
        );
        if (matchedUsers.empty()) {
            result["message"] = "Email:Password pair is incorrect.";
            return false;
        } else {
            result["message"] = "OK";

            auto configurator = app().getPlugin<tech::plugin::Configurator>();
            Mapper<Techmino::Auth> authMapper(app().getDbClient());
            Techmino::Auth auth;
            auth.setId(matchedUsers[0]["_id"].as<int64_t>());
            auth.setAuthToken(drogon::utils::getUuid());
            auth.setAuthTokenExpireTime(trantor::Date::now().after(configurator->getAuthExpire()).toDbStringLocal());
            authMapper.update(auth);

            auto app = wsConnPtr->getContext<App>();
            app->setID(auth.getValueOfId());
            app->setAuthToken(auth.getValueOfAuthToken());
            wsConnPtr->setContext(app);

            return true;
        }
    } catch (const orm::DrogonDbException &e) {
        code = CloseCode::kUnexpectedCondition;
        LOG_ERROR << "error:" << e.base().what();
        result["message"] = "Internal error";
        return false;
    }
}

bool Authorizer::versionCode(
        const WebSocketConnectionPtr &wsConnPtr,
        const int &versionCode,
        CloseCode &code,
        Json::Value &result
) {
    orm::Mapper<Techmino::App> appMapper(app().getDbClient());
    auto leastApp = appMapper.orderBy(Techmino::App::Cols::_version_code, SortOrder::DESC).limit(1)
            .findBy(Criteria(Techmino::App::Cols::_compatible, CompareOperator::EQ, false))[0];

    if (versionCode < leastApp.getValueOfVersionCode()) {
        code = CloseCode::kViolation;
        result["message"] = "Outdated version";
        return false;
    } else {
        result["message"] = "OK";
        wsConnPtr->setContext(make_shared<App>(versionCode));
        return true;
    }
}
