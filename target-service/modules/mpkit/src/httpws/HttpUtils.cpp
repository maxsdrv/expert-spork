#include "httpws/HttpUtils.h"

#include "httplib.h"

#include <QString>

namespace http
{

QString errorToString(int errorCode)
{
    using httplib::Error;
    const auto error = static_cast<Error>(errorCode);

    switch (error)
    {
        case Error::Success: return QStringLiteral("No error");
        case Error::Connection: return QStringLiteral("Connection error");
        case Error::BindIPAddress: return QStringLiteral("Bind IP address error");
        case Error::Read: return QStringLiteral("Read error");
        case Error::Write: return QStringLiteral("Write error");
        case Error::ExceedRedirectCount: return QStringLiteral("Redirect count exceeded");
        case Error::Canceled: return QStringLiteral("Call cancelled");
        case Error::SSLConnection: return QStringLiteral("SSL connection error");
        case Error::SSLLoadingCerts: return QStringLiteral("SSL certificate loading error");
        case Error::SSLServerVerification: return QStringLiteral("SSL server verification error");
        case Error::UnsupportedMultipartBoundaryChars:
            return QStringLiteral("Unsupported multipart boundary chars error");
        case Error::Compression: return QStringLiteral("Compression error");
        case Error::ConnectionTimeout: return QStringLiteral("Connection timeout error");
        case Error::Unknown:
        default: return QStringLiteral("Unknown error");
    }
}

} // namespace http
