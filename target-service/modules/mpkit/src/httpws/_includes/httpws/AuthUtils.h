#pragma once

#include "AuthTokens.h"
#include "AuthType.h"

class QByteArray;
class QString;

namespace http
{

struct AuthHeaderData
{
    AuthType::Value type;
    QString data;
};

AuthHeaderData parseAuthHeaderParams(const std::string& authHeader);

Credentials parseBasicAuthData(const QString& data);

AuthTokens parseAuthResponseBody(const QByteArray& body);

} // namespace http
