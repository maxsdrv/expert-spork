#pragma once

#include "ConnectionSettings.h"
#include "HttpMethod.h"

#include <QJsonObject>
#include <QObject>

#include <chrono>
#include <memory>

class QThreadPool;

namespace httplib
{

class Client;
class Result;

} // namespace httplib

namespace http
{

class Call;
class HttpClient : public QObject
{
    Q_OBJECT
    friend class Call;

public:
    explicit HttpClient(
        const ConnectionSettings& settings, int maxThreadsCount = 2, QObject* parent = nullptr);
    ~HttpClient() override;

public:
    Call& httpRequest(HttpMethod::Value method, QString path, QJsonObject body);

    Call& httpDelete(QString path, QJsonObject body);
    Call& httpGet(QString path, QJsonObject params = {});
    Call& httpPost(QString path, QJsonObject body);
    Call& httpPut(QString path, QJsonObject body);

    ConnectionSettings settings() const;
    QString authToken() const;

    void setSettings(const ConnectionSettings& settings);
    void setAuthToken(const QString& token);

    void setReadTimeout(std::chrono::milliseconds timeout);
    void setWriteTimeout(std::chrono::milliseconds timeout);
    void setConnectionTimeout(std::chrono::milliseconds timeout);

    const ConnectionSettings& networkSettings() const;

private:
    virtual void run(Call* call);

private:
    using HttpParams = std::multimap<std::string, std::string>;

    Call& create(HttpMethod::Value method, QString path, QJsonObject body);
    httplib::Result makeCall(HttpMethod::Value method, const char* path, const QJsonObject& body);

    static std::string toHttpBody(const QJsonObject& json);
    static std::string bodyToLog(const QJsonObject& json);
    static HttpParams toHttpParams(const QJsonObject& json);

    static void handleResult(const httplib::Result& result, Call* call, const QString& path);
    static void checkTimeout(std::chrono::milliseconds timeout);

private:
    std::unique_ptr<httplib::Client> m_client;
    QThreadPool* m_pool;
    ConnectionSettings m_settings;
    QString m_authToken;
};

} // namespace http
