#include "httpws/AuthUtils.h"

#include "httpws/AuthType.h"
#include "httpws/Definitions.h"
#include "httpws/HttpException.h"

#include "exception/InvalidValue.h"

#include "json/JsonUtilities.h"

#include <QByteArray>
#include <QString>
#include <QStringList>

using namespace exception::http;

namespace http
{

namespace
{

constexpr auto SKIP_EMPTY_PARTS =
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    Qt::SkipEmptyParts;
#else
    QString::SkipEmptyParts;
#endif

} // namespace

Credentials parseBasicAuthData(const QString& data)
{
    auto encodedData = QString(QByteArray::fromBase64(data.toLocal8Bit()));
    QStringList params(encodedData.split(QChar(':'), SKIP_EMPTY_PARTS));

    const int BASIC_AUTH_PARTS_COUNT = 2;
    if (params.size() != BASIC_AUTH_PARTS_COUNT)
    {
        BOOST_THROW_EXCEPTION(
            Exception() << ResponseStatus(::http::HttpCode::BadRequest)
                        << ResponseMessage(std::string("Basic auth parameters not complete")));
    }
    return {params.first(), params.last()};
}

AuthHeaderData parseAuthHeaderParams(const std::string& authHeader)
{
    if (authHeader.empty())
    {
        BOOST_THROW_EXCEPTION(
            Exception() << ResponseStatus(::http::HttpCode::BadRequest)
                        << ResponseMessage(std::string("Empty auth header")));
    }

    QStringList headersParts(QString::fromStdString(authHeader)
                                 .split(QChar(' '), SKIP_EMPTY_PARTS, Qt::CaseInsensitive));

    const int HEADER_PARTS_COUNT = 2;
    if (headersParts.size() != HEADER_PARTS_COUNT)
    {
        BOOST_THROW_EXCEPTION(
            Exception() << ResponseStatus(::http::HttpCode::BadRequest)
                        << ResponseMessage(std::string("Auth header not complete")));
    }

    try
    {
        return {AuthType::fromString(headersParts.first()), headersParts.last()};
    }
    catch (const exception::InvalidValue&)
    {
        BOOST_THROW_EXCEPTION(
            Exception() << ResponseStatus(::http::HttpCode::NotAcceptable)
                        << ResponseMessage(std::string("Auth type not supported")));
    }
}

AuthTokens parseAuthResponseBody(const QByteArray& body)
{
    return json::fromValue<AuthTokens>(json::fromByteArray(body));
}

} // namespace http
