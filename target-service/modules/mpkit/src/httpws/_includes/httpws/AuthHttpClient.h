#pragma once

#include "httpws/AuthTokens.h"
#include "httpws/AuthType.h"
#include "httpws/ConnectionSettings.h"

#include <QFuture>
#include <QObject>

namespace httplib
{

class Client;
class Result;

} // namespace httplib

namespace http
{

class AuthHttpClient : public QObject
{
    Q_OBJECT

public:
    AuthHttpClient(
        ConnectionSettings connectionSettings,
        Credentials credentials,
        QObject* parent = nullptr);

    QFuture<http::AuthTokens> authenticate(const Token& refreshToken = Token{});

    void setConnectionSettings(const ConnectionSettings& connectionSettings);
    ConnectionSettings connectionSettings() const;

    void setCredentials(const Credentials& credentials);
    Credentials credentials() const;

signals:
    void authenticated(http::AuthTokens);
    void authenticationFailed();

private:
    httplib::Client createConnection(const Token& refreshToken) const;
    static void checkResult(const httplib::Result& result);

private:
    ConnectionSettings m_connectionSettings;
    Credentials m_credentials;
};

} // namespace http
