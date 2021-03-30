//
// Created by Parti on 2021/2/5.
//

#include <utils/websocket.h>

using namespace tech::utils;
using namespace drogon;
using namespace std;

string websocket::fromJson(const Json::Value &value) {
    Json::StreamWriterBuilder writerBuilder;
    writerBuilder.settings_["indentation"] = "";
    unique_ptr<Json::StreamWriter> jsonWriter(writerBuilder.newStreamWriter());
    ostringstream oss;
    jsonWriter->write(value, &oss);
    return oss.str();
}

string websocket::toJson(
        const string &rawData,
        Json::Value &result
) {
    string errorMessage;
    auto charReader = Json::CharReaderBuilder().newCharReader();

    charReader->parse(rawData.data(), rawData.data() + rawData.size(), &result, &errorMessage);
    return errorMessage;
}

void websocket::initPing(
        const WebSocketConnectionPtr &wsConnPtr,
        const Json::Value &initMessage,
        const chrono::duration<long double> &interval
) {
    LOG_DEBUG << websocket::fromJson(initMessage);
    wsConnPtr->send(websocket::fromJson(initMessage));
    wsConnPtr->setPingMessage("", interval);
}

void websocket::close(
        const WebSocketConnectionPtr &webSocketConnectionPtr,
        CloseCode _code,
        const string &_reason
) {
    webSocketConnectionPtr->shutdown(_code, _reason);
}
