#include "mpk/rest/AsyncHttpRequest.h"

#include "httpws/Call.h"

namespace
{
constexpr auto errorCodeTag = "error_tag";
const int maxThreadsCount = 2;
} // namespace

namespace mpk::rest
{

AsyncHttpRequest::AsyncHttpRequest(const http::ConnectionSettings& settings, QObject* parent) :
  AsyncHttpRequest(parent)
{
    setSettings(settings);
}

AsyncHttpRequest::AsyncHttpRequest(QObject* parent) :
  QObject{parent}, m_client{new http::HttpClient{{}, maxThreadsCount, this}}
{
}

void AsyncHttpRequest::setSettings(const http::ConnectionSettings& settings)
{
    m_client->setSettings(settings);
}

void AsyncHttpRequest::setAuthToken(const QString& token)
{
    m_client->setAuthToken(token);
}

const http::ConnectionSettings& AsyncHttpRequest::settings() const
{
    return m_client->networkSettings();
}

void AsyncHttpRequest::get(const QString& path) const
{
    QJsonObject params;
    get(path, params); // NOLINT
}

void AsyncHttpRequest::get(const QString& path, const QJsonObject& params) const
{
    m_conditionObject.notify();

    QMetaObject::invokeMethod( // NOLINT
        m_client,
        [this, path, params]()
        {
            m_client->httpGet(path, params)
                .onSuccess([this](const QJsonObject& obj) { m_conditionObject.setResult(obj); })
                .onError(
                    [this](int /*errorCode*/, const QString& text)
                    {
                        QJsonObject object{{errorCodeTag, text}};
                        m_conditionObject.setResult(object);
                    })
                .run();
        },
        Qt::QueuedConnection);
}

const QJsonObject& AsyncHttpRequest::waitResult() const
{
    m_result = m_conditionObject.waitResult();
    return m_result;
}

AsyncHttpRequest::ErrorOpt AsyncHttpRequest::error() const
{
    if (m_result.contains(errorCodeTag))
    {
        return m_result.value(errorCodeTag).toString();
    }

    return std::nullopt;
}

} // namespace mpk::rest
