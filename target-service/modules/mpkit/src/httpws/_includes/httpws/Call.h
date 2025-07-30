#pragma once

#include "httpws/HttpMethod.h"

#include "gsl/pointers"

#include <QJsonObject>
#include <QObject>

namespace http
{

class HttpClient;

/**
 * Single call from client to server.
 * If call complete successfully, the following signals are emitted in sequence:
 * Success(Reply (json)) -> Complete
 * If call fails, the following signals are emitted in sequence:
 * Error(http error code, text body) -> Complete
 */
class Call : public QObject
{
    Q_OBJECT
    friend class HttpClient;

signals:
    void complete();
    void successJson(const QJsonObject& result);
    void successText(const QString& result);
    void error(int code, const QString& text);

public:
    explicit Call(QObject* parent = nullptr);

    HttpMethod::Value method() const;
    const QJsonObject& body() const;
    QString path(int version) const;

    // Equivalent to HttpClient::run(this)
    void run();

    Call& onSuccess(const std::function<void(const QJsonObject&)>& handler);
    Call& onSuccess(const std::function<void(const QString&)>& handler);
    Call& onError(const std::function<void(int, const QString&)>& handler);
    Call& onComplete(const std::function<void()>& handler);

    template <typename Handler>
    Call& onSuccess(
        const typename QtPrivate::FunctionPointer<Handler>::Object* receiver, Handler slot)
    {
        connect(this, &http::Call::successJson, receiver, slot);
        return *this;
    }

    template <typename Handler>
    Call& onError(
        const typename QtPrivate::FunctionPointer<Handler>::Object* receiver, Handler slot)
    {
        connect(this, &http::Call::error, receiver, slot);
        return *this;
    }

    template <typename Handler>
    Call& onComplete(
        const typename QtPrivate::FunctionPointer<Handler>::Object* receiver, Handler slot)
    {
        connect(this, &http::Call::complete, receiver, slot);
        return *this;
    }

private:
    Call(
        HttpMethod::Value method,
        QString path,
        QJsonObject body,
        gsl::not_null<HttpClient*> parent);

    void setSuccess(const QJsonObject& result);
    void setSuccess(const QString& result);

    void setFailed(int code, const QString& text);

private:
    HttpMethod::Value m_method;
    QString m_path;
    QJsonObject m_body;
};

} // namespace http
