#include "SslClient.h"

#include "SslData.h"

#include "transport/tcp/TcpMessenger.h"

#include "transport/IncomingMessageImpl.h"
#include "transport/Socket.h"
#include "transport/ssl/SslData.h"

#include <QSslConfiguration>
#include <QSslSocket>

namespace rpc
{

SslClient::SslClient(
    SocketAddress address,
    const SslData& sslData,
    const QList<QSslError::SslError>& ignoredSslErrors) :
  m_address(std::move(address)),
  m_socket(std::make_unique<QSslSocket>()),
  m_messenger(std::make_shared<TcpMessenger>(m_socket.get()))
{
    QObject::connect(
        m_messenger.get(),
        &TcpMessenger::received,
        m_messenger.get(),
        [this](const auto& buffer)
        { m_Received(std::make_shared<IncomingMessageImpl>(buffer, m_messenger)); });

    QObject::connect(
        m_socket.get(),
        &QAbstractSocket::stateChanged,
        m_socket.get(),
        [this](auto state)
        {
            if (state == QAbstractSocket::UnconnectedState)
            {
                m_queue.clear();
            }
        });

    QObject::connect(
        m_socket.get(),
        &QSslSocket::encrypted,
        m_socket.get(),
        [this]()
        {
            while (!m_queue.isEmpty())
            {
                m_messenger->send(m_queue.dequeue());
            }
        });

    QObject::connect(
        m_socket.get(),
        qOverload<const QList<QSslError>&>(&QSslSocket::sslErrors),
        m_socket.get(),
        [this, ignoredSslErrors](const QList<QSslError>& errors)
        {
            if (std::all_of(
                    errors.begin(),
                    errors.end(),
                    [ignoredSslErrors](const auto& error)
                    { return ignoredSslErrors.contains(error.error()); }))
            {
                m_socket->ignoreSslErrors(errors);
            }
        });

    m_socket->setPrivateKey(sslData.privateKey);
    m_socket->setLocalCertificate(sslData.certificate);
    if (!sslData.caCertificate.isNull())
    {
#if QT_MAJOR < 6
        m_socket->addCaCertificate(sslData.caCertificate);
#else
        QSslConfiguration config = m_socket->sslConfiguration();
        config.addCaCertificate(sslData.caCertificate);
        m_socket->setSslConfiguration(config);
#endif
    }
    m_socket->setPeerVerifyMode(QSslSocket::VerifyPeer);
}

SslClient::~SslClient() = default;

void SslClient::send(const QByteArray& array)
{
    if (m_socket->isEncrypted() && m_socket->state() == QAbstractSocket::ConnectedState)
    {
        m_messenger->send(array);
        return;
    }

    if (m_socket->state() == QAbstractSocket::UnconnectedState)
    {
        m_socket->connectToHostEncrypted(m_address.host.toString(), m_address.port);
    }

    m_queue.enqueue(array);
}

} // namespace rpc
