#include "httpws/HttpClient.h"

#include "httpws/Call.h"
#include "httpws/Definitions.h"
#include "httpws/HttpException.h"
#include "httpws/HttpUtils.h"

#include "json/FromJson.h"
#include "json/JsonUtilities.h"
#include "json/parsers/Containers.h"
#include "json/parsers/General.h"

#include "log/Log2.h"
#include "qt/Strings.h"

#include "httplib.h"

#include <QByteArray>
#include <QJsonObject>
#include <QThreadPool>
#include <QtConcurrentRun>

#undef DELETE

namespace
{

constexpr auto DefaultConnectionTimeout = std::chrono::seconds(3);
constexpr auto MaxDebugBodySize = 1024;

} // namespace

namespace http
{

HttpClient::HttpClient(const ConnectionSettings& settings, int maxThreadsCount, QObject* parent) :
  QObject(parent),
  m_client(std::make_unique<httplib::Client>(strings::toUtf8(settings.host), settings.port)),
  m_pool(new QThreadPool(this)),
  m_settings(settings)
{
    setConnectionTimeout(DefaultConnectionTimeout);
    m_pool->setMaxThreadCount(maxThreadsCount);
}

HttpClient::~HttpClient()
{
    m_pool->waitForDone();
}

Call& HttpClient::create(HttpMethod::Value method, QString path, QJsonObject body)
{
    return *(new Call(method, std::move(path), std::move(body), this));
}

Call& HttpClient::httpRequest(HttpMethod::Value method, QString path, QJsonObject body)
{
    return HttpClient::create(method, std::move(path), std::move(body));
}

Call& HttpClient::httpDelete(QString path, QJsonObject body)
{
    return HttpClient::create(HttpMethod::DELETE, std::move(path), std::move(body));
}

Call& HttpClient::httpGet(QString path, QJsonObject params)
{
    return HttpClient::create(HttpMethod::GET, std::move(path), std::move(params));
}

Call& HttpClient::httpPost(QString path, QJsonObject body)
{
    return HttpClient::create(HttpMethod::POST, std::move(path), std::move(body));
}

Call& HttpClient::httpPut(QString path, QJsonObject body)
{
    return HttpClient::create(HttpMethod::PUT, std::move(path), std::move(body));
}

ConnectionSettings HttpClient::settings() const
{
    return m_settings;
}

QString HttpClient::authToken() const
{
    return m_authToken;
}

void HttpClient::run(Call* call)
{
#if QT_MAJOR < 6
    QtConcurrent::run(
        m_pool,
#else
    m_pool->start(
#endif
        [this, call]()
        {
            auto callDeleter = [](Call* object) { object->deleteLater(); };

            const std::unique_ptr<Call, decltype(callDeleter)> callPtr(call, callDeleter);

            auto path = call->path(API_VERSION);
            LOG_DEBUG << "Running request " << HttpMethod::toString(call->method()) << " "
                      << m_settings.host << ":" << m_settings.port << path << " with body: \n"
                      << bodyToLog(call->body());

            try
            {
                auto strPath = path.toStdString();
                auto result = makeCall(call->method(), strPath.c_str(), call->body());
                if (result.error() == httplib::Error::Success)
                {
                    handleResult(result, call, path);
                }
                else
                {
                    const int error = static_cast<int>(result.error());
                    QString errorStr = "Request " + path + " is failed with a code = "
                                       + QString::number(error) + ": " + errorToString(error);

                    call->setFailed(static_cast<int>(result.error()), errorToString(error));
                    LOG_WARNING << errorStr;
                }
            }
            catch (const std::exception& ex)
            {
                call->setFailed(static_cast<int>(HttpCode::InternalServerError), ex.what());

                LOG_WARNING
                    << "Request " << path
                    << " failed with a code = " << static_cast<int>(HttpCode::InternalServerError)
                    << " " << ex.what();
            }
        });
}

void HttpClient::setSettings(const ConnectionSettings& settings)
{
    m_pool->waitForDone();

    m_settings = settings;
    m_client =
        std::make_unique<httplib::Client>(strings::toUtf8(m_settings.host), m_settings.port);

    setConnectionTimeout(DefaultConnectionTimeout);
}

void HttpClient::setAuthToken(const QString& token)
{
    m_authToken = token;
    m_client->set_bearer_token_auth(token.toLatin1().data());
}

void HttpClient::setReadTimeout(std::chrono::milliseconds timeout)
{
    checkTimeout(timeout);
    m_client->set_read_timeout(timeout);
}

void HttpClient::setWriteTimeout(std::chrono::milliseconds timeout)
{
    checkTimeout(timeout);
    m_client->set_write_timeout(timeout);
}

void HttpClient::setConnectionTimeout(std::chrono::milliseconds timeout)
{
    checkTimeout(timeout);
    m_client->set_connection_timeout(timeout);
}

const ConnectionSettings& HttpClient::networkSettings() const
{
    return m_settings;
}

httplib::Result HttpClient::makeCall(
    HttpMethod::Value method, const char* path, const QJsonObject& body)
{
    switch (method)
    {
        case HttpMethod::DELETE:
        {
            return m_client->Delete(path, toHttpBody(body), jsonContentType);
        }
        case HttpMethod::PUT:
        {
            return m_client->Put(path, toHttpBody(body), jsonContentType);
        }
        case HttpMethod::POST:
        {
            return m_client->Post(path, toHttpBody(body), jsonContentType);
        }
        case HttpMethod::GET:
        {
            const httplib::Headers headers;
            return m_client->Get(path, toHttpParams(body), headers);
        }
        default:
        {
            BOOST_THROW_EXCEPTION(exception::http::UnsupportedMethod(method));
        }
    }
}

std::string HttpClient::toHttpBody(const QJsonObject& json)
{
    if (!json.isEmpty())
    {
        return json::toByteArray(json).toStdString();
    }

    return {};
}

std::string HttpClient::bodyToLog(const QJsonObject& json)
{
    auto body = toHttpBody(json);
    if (body.size() > MaxDebugBodySize)
    {
        return body.substr(0, MaxDebugBodySize) + "\n...";
    }

    return body;
}

HttpClient::HttpParams HttpClient::toHttpParams(const QJsonObject& json)
{
    return json::fromValue<HttpParams>(json);
}

void HttpClient::handleResult(const httplib::Result& result, Call* call, const QString& path)
{
    auto body = strings::fromUtf8(result->body);

    if (result->status == static_cast<int>(HttpCode::Ok))
    {
        if (result->has_header(contentTypeHeader))
        {
            auto headerValue = result->get_header_value(contentTypeHeader);
            if (headerValue == textContentType)
            {
                call->setSuccess(body);
            }
            else if (headerValue == jsonContentType)
            {
                call->setSuccess(
                    !result->body.empty() ? json::fromStdString(result->body) : QJsonObject{});
            }
            else
            {
                LOG_WARNING << QStringLiteral("Unsupported %1 value: %2")
                                   .arg(contentTypeHeader, headerValue.data());
                call->setSuccess(body);
            }
        }
        else
        {
            LOG_WARNING << QStringLiteral("Header %1 is required for %2, but not present")
                               .arg(contentTypeHeader, path);
            call->setSuccess(body);
        }
    }
    else
    {
        call->setFailed(result->status, body);
        LOG_WARNING << "Request " << path << " is failed with a code = " << result->status << " "
                    << result->body;
    }
}

void HttpClient::checkTimeout(std::chrono::milliseconds timeout)
{
    using seconds = std::chrono::seconds;
    using hours = std::chrono::hours;
    using std::chrono::duration_cast;
    constexpr auto hoursInDay = 24;

    if (duration_cast<seconds>(timeout).count() < 1)
    {
        LOG_WARNING << "Http timeout less 1 second";
    }
    else if (duration_cast<hours>(timeout).count() > hoursInDay)
    {
        LOG_WARNING << "Http timeout more 1 day";
    }
}

} // namespace http
