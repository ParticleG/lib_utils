//
// Created by Parti on 2021/2/4.
//

#pragma once

#include <drogon/HttpController.h>
#include <drogon/WebSocketController.h>

namespace tech::utils {
    namespace authorizer {
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
        Status accessToken(
                const int64_t &id,
                const std::string &accessToken,
                const std::string &newExpireTime,
                Json::Value &result
        );

        Status authToken(
                const int64_t &id,
                const std::string &authToken,
                const std::string &newExpireTime,
                Json::Value &result
        );

        Status password(
                const std::string &email,
                const std::string &password,
                const std::string &newExpireTime,
                Json::Value &result
        );

        Status versionCode(
                const int &versionCode,
                Json::Value &result
        );
    }
}