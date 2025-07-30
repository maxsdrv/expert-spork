#include "SslTransportCreator.h"

#include "rpc/transport/TransportExceptions.h"

#include "SslClient.h"
#include "SslData.h"
#include "SslMultiServer.h"
#include "SslSingleServer.h"

#include "json/parsers/Containers.h"

#include <QFile>
#include <QSsl>

namespace rpc
{

QByteArray readFileContent(const QString& filename)
{
    QFile file(filename);
    if (!filename.isEmpty() && !file.open(QIODevice::ReadOnly))
    {
        BOOST_THROW_EXCEPTION(exception::TransportCreationFailed(
            std::string("Failed to open ssl file: ") + filename.toStdString()));
    }
    return file.readAll();
}

std::unique_ptr<Transport> createSslTransport(const QJsonObject& parameters)
{
    constexpr auto modeTag = "mode";
    constexpr auto hostTag = "host";
    constexpr auto portTag = "port";
    constexpr auto privateKeyTag = "private_key";
    constexpr auto certificateTag = "certificate";
    constexpr auto caCertificateTag = "ca_certificate";
    constexpr auto ignoredSslErrorsTag = "ignored_ssl_errors";

    auto mode = json::fromObject<QString>(parameters, modeTag);
    auto host = json::fromObject<QHostAddress>(parameters, hostTag);
    auto port = json::fromObject<quint16>(parameters, portTag);

    auto privateKeyFilename = json::fromObject<QString>(parameters, privateKeyTag);
    auto certificateFilename = json::fromObject<QString>(parameters, certificateTag);
    auto caCertificateFilename = json::fromObject<QString>(parameters, caCertificateTag);

    SslData sslData;
    if (!privateKeyFilename.isEmpty())
    {
        sslData.privateKey = QSslKey(readFileContent(privateKeyFilename), QSsl::Rsa);
    }
    if (!certificateFilename.isEmpty())
    {
        sslData.certificate = QSslCertificate(readFileContent(certificateFilename));
    }
    if (!caCertificateFilename.isEmpty())
    {
        sslData.caCertificate = QSslCertificate(readFileContent(caCertificateFilename));
    }

    SocketAddress socketAddress{host, port};

    if (mode == "client")
    {
        auto ignoredSslErrors = json::fromObjectWithDefault<QList<QSslError::SslError>>(
            parameters, ignoredSslErrorsTag, QList<QSslError::SslError>());
        return std::make_unique<SslClient>(std::move(socketAddress), sslData, ignoredSslErrors);
    }
    if (mode == "single_server")
    {
        return std::make_unique<SslSingleServer>(socketAddress, std::move(sslData));
    }
    if (mode == "multi_server")
    {
        return std::make_unique<SslMultiServer>(socketAddress, std::move(sslData));
    }

    BOOST_THROW_EXCEPTION(exception::TransportCreationFailed("Invalid ssl transport type"));
}

} // namespace rpc
