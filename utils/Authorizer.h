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
                Json::Value &result
        );

        static bool authToken(
                const drogon::WebSocketConnectionPtr &wsConnPtr,
                const std::shared_ptr<drogon_model::Techmino::Auth> &auth,
                drogon::CloseCode &code,
                Json::Value &result
        );

        static Status authToken(
                const int64_t &id,
                const std::string &authToken,
                Json::Value &result
        );

        static Status password(
                const drogon::WebSocketConnectionPtr &wsConnPtr,
                const std::string &email,
                const std::string &password,
                Json::Value &result
        );

        static Status password(
                const std::string &email,
                const std::string &password,
                Json::Value &result
        );

//        static bool versionCode(
//                const drogon::WebSocketConnectionPtr &wsConnPtr,
//                const int &versionCode,
//                drogon::CloseCode &code,
//                Json::Value &result
//        );

        static Authorizer::Status versionCode(
                const int &versionCode,
                Json::Value &result
        );
    };
}