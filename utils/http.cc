//
// Created by Parti on 2021/2/4.
//

#include <utils/http.h>

using namespace tech::utils;
using namespace drogon;
using namespace std;

void http::fromJson(
        const HttpStatusCode &code,
        const Json::Value &body,
        const function<void(const HttpResponsePtr &)> &callback
) {
    auto httpJsonResponse = HttpResponse::newHttpJsonResponse(body);
    httpJsonResponse->setStatusCode(code);
    callback(httpJsonResponse);
}

void http::fromPlain(
        const HttpStatusCode &code, const string &body,
        const function<void(const HttpResponsePtr &)> &callback
) { from(code, CT_TEXT_PLAIN, body, callback); }

void http::fromHtml(
        const HttpStatusCode &code,
        const string &body,
        const function<void(const HttpResponsePtr &)> &callback
) { from(code, CT_TEXT_HTML, body, callback); }

void http::from(
        const HttpStatusCode &code,
        const ContentType &type,
        const string &body,
        const function<void(const HttpResponsePtr &)> &callback
) {
    auto httpResponse = HttpResponse::newHttpResponse();
    httpResponse->setStatusCode(code);
    httpResponse->setContentTypeCode(type);
    httpResponse->setBody(body);
    callback(httpResponse);
}

string http::toJson(
        const HttpRequestPtr &req,
        Json::Value &result
) {
    LOG_DEBUG << req->getBody();
    result = *(req->getJsonObject());
    return req->getJsonError();
}