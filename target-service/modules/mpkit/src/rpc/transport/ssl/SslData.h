#pragma once

#include <QSslCertificate>
#include <QSslKey>

namespace rpc
{

struct SslData
{
    QSslKey privateKey;
    QSslCertificate certificate;
    QSslCertificate caCertificate;
};

} // namespace rpc
