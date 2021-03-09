//
// Created by Parti on 2021/2/5.
//

#pragma once

#include <drogon/WebSocketController.h>

namespace tech::utils {
    namespace WebSocket {
        enum class Type {
            App = 0,
            User = 1,
            Chat = 2,
            Play = 3,
            Stream = 4
        };

        std::string fromJson(
                const Json::Value &value
        );

        std::string toJson(
                const std::string &rawData,
                Json::Value &result
        );

        void initPing(
                const drogon::WebSocketConnectionPtr &wsConnPtr,
                const Json::Value &initMessage,
                const std::chrono::duration<long double> &interval
        );

        void close(
                const drogon::WebSocketConnectionPtr &webSocketConnectionPtr,
                drogon::CloseCode _code,
                const std::string &_reason
        );
    }
}