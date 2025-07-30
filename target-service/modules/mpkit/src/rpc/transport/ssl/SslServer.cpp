#include "SslServer.h"

#include <QSslConfiguration>

namespace rpc
{

SslServer::SslServer(SslData sslData, QObject* parent) :
  QTcpServer(parent), m_sslData(std::move(sslData))
{
}

QSslSocket* SslServer::nextPendingConnection()
{
    return qobject_cast<QSslSocket*>(QTcpServer::nextPendingConnection());
}

void SslServer::incomingConnection(qintptr descriptor)
{
    auto* socket = new QSslSocket();
    socket->setPrivateKey(m_sslData.privateKey);
    socket->setLocalCertificate(m_sslData.certificate);
    if (!m_sslData.caCertificate.isNull())
    {
#if QT_MAJOR < 6
        socket->addCaCertificate(m_sslData.caCertificate);
#else
        QSslConfiguration config = socket->sslConfiguration();
        config.addCaCertificate(m_sslData.caCertificate);
        socket->setSslConfiguration(config);
#endif
    }
    socket->setSocketDescriptor(descriptor);
    socket->startServerEncryption();

    addPendingConnection(socket);
}

} // namespace rpc
