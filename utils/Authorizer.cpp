//
// Created by Parti on 2021/2/4.
//

#include <models/Auth.h>
#include <utils/Authorizer.h>

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
