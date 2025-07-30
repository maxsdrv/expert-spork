#include "httpws/Call.h"

#include "httpws/HttpClient.h"
#include "httpws/HttpRoute.h"

namespace http
{

Call::Call(QObject* parent) : QObject(parent)
{
}

HttpMethod::Value Call::method() const
{
    return m_method;
}

const QJsonObject& Call::body() const
{
    return m_body;
}

QString Call::path(int version) const
{
    return resolvePath(version, m_path);
}

void Call::run()
{
    qobject_cast<HttpClient*>(parent())->run(this);
}

Call& Call::onSuccess(std::function<void(const QJsonObject&)> const& handler)
{
    connect(this, &http::Call::successJson, this, handler);
    return *this;
}

Call& Call::onSuccess(std::function<void(const QString&)> const& handler)
{
    connect(this, &http::Call::successText, this, handler);
    return *this;
}

Call& Call::onError(std::function<void(int, const QString&)> const& handler)
{
    connect(this, &http::Call::error, this, handler);
    return *this;
}

Call& Call::onComplete(std::function<void()> const& handler)
{
    connect(this, &http::Call::complete, this, handler);
    return *this;
}

Call::Call(
    HttpMethod::Value method, QString path, QJsonObject body, gsl::not_null<HttpClient*> parent) :
  QObject(parent), m_method(method), m_path(std::move(path)), m_body(std::move(body))
{
}

void Call::setSuccess(const QJsonObject& result)
{
    emit successJson(result);
    emit complete();
}

void Call::setSuccess(const QString& result)
{
    emit successText(result);
    emit complete();
}

void Call::setFailed(int code, const QString& text)
{
    emit error(code, text);
    emit complete();
}

} // namespace http
