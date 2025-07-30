#pragma once

#include "transport/ssl/SslData.h"

#include <QSslSocket>
#include <QTcpServer>

namespace rpc
{

class SslServer : public QTcpServer
{
    Q_OBJECT

public:
    explicit SslServer(SslData sslData, QObject* parent = nullptr);
    QSslSocket* nextPendingConnection() override;

protected:
    void incomingConnection(qintptr descriptor) override;

private:
    SslData m_sslData;
};

} // namespace rpc
