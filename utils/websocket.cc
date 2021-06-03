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
    /*
     * TODO: Replace with better implementation:
     * https://github.com/open-source-parsers/jsoncpp/blob/375a1119f8bbbf42e5275f31b281b5d87f2e17f2/include/json/reader.h#L390
    */
    unique_ptr<Json::CharReader> const charReader(Json::CharReaderBuilder().newCharReader());

    charReader->parse(rawData.data(), rawData.data() + rawData.size(), &result, &errorMessage);
    return errorMessage;
}

void websocket::initPing(
        const WebSocketConnectionPtr &wsConnPtr,
        const Json::Value &initMessage,
        const chrono::duration<long double> &interval
) {
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
