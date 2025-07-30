#pragma once

#include "mpk/rest/ConditionWrapper.h"

#include "httpws/HttpClient.h"

#include <QObject>

namespace mpk::rest
{

class AsyncHttpRequest : public QObject
{
    Q_OBJECT

public:
    using ConditionObject = ConditionWrapper<QJsonObject>;
    using ErrorOpt = std::optional<QString>;

    explicit AsyncHttpRequest(QObject* parent = nullptr);
    explicit AsyncHttpRequest(const http::ConnectionSettings& settings, QObject* parent = nullptr);

    void setSettings(const http::ConnectionSettings& settings);
    void setAuthToken(const QString& token);
    const http::ConnectionSettings& settings() const;

    void get(const QString& path) const;
    void get(const QString& path, const QJsonObject& params) const;

    const QJsonObject& waitResult() const;

    ErrorOpt error() const;

private:
    http::HttpClient* m_client;
    mutable ConditionWrapper<QJsonObject> m_conditionObject;
    mutable QJsonObject m_result;
};

} // namespace mpk::rest
