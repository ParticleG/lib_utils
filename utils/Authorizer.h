//
// Created by Parti on 2021/2/4.
//

#pragma once

#include <drogon/HttpController.h>
#include <drogon/WebSocketController.h>

namespace tech::utils {
    class Authorizer {
    public:
        enum class Status {
            OK,
            InvalidComponents,
            NotFound,
            Incorrect,
            Expired,
            InternalError,
        };
        enum class Type {
            GetAuthToken,
            CheckAuthToken,
            GetAccessToken,
            CheckAccessToken,
        };
    public:
        static Status accessToken(
                const int64_t &id,
                const std::string &accessToken,
                const std::string &newExpireTime,
                Json::Value &result
        );

        static Status authToken(
                const int64_t &id,
                const std::string &authToken,
                const std::string &newExpireTime,
                Json::Value &result
        );

        static Status password(
                const drogon::WebSocketConnectionPtr &wsConnPtr,
                const std::string &email,
                const std::string &password,
                const std::string &newExpireTime,
                Json::Value &result
        );

        static Status password(
                const std::string &email,
                const std::string &password,
                const std::string &newExpireTime,
                Json::Value &result
        );

        static Authorizer::Status versionCode(
                const int &versionCode,
                Json::Value &result
        );
    };
}