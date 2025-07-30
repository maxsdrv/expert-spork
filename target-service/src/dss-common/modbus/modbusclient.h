#pragma once

// clang-format off
// NOLINTBEGIN

#include "connectiondetails.h"
#include "modbuswriteparams.h"

#include <QModbusClient>

#include <chrono>

Q_DECLARE_METATYPE(QModbusDataUnit)

namespace mpk::dss
{
///
/// \brief The ModbusClient class
///
class ModbusClient : public QObject
{
    Q_OBJECT
public:
    explicit ModbusClient(QObject *parent = nullptr);
    ~ModbusClient() override;

    void connectDevice(const ConnectionDetails& cd);
    void disconnectDevice();

    bool isValid() const;
    QModbusDevice::State state() const;

    ConnectionType connectionType() const {
        return _connectionType;
    }

    int timeout() const;
    void setTimeout(int newTimeout);

    uint numberOfRetries() const;
    void setNumberOfRetries(uint number);

    void sendRawRequest(const QModbusRequest& request, int server, int requestId);
    void sendReadRequest(QModbusDataUnit::RegisterType pointType, int startAddress, quint16 valueCount, int server, int requestId);
    void writeRegister(QModbusDataUnit::RegisterType pointType, const ModbusWriteParams& params, int requestId);
    void maskWriteRegister(const ModbusMaskWriteParams& params, int requestId);

signals:
    void modbusRequest(int requestId, int deviceId, int transactionId, const QModbusRequest& request);
    void modbusReply(QModbusReply* reply);
    void modbusError(const QString& error, int requestId);
    void modbusConnectionError(const QString& error);
    void modbusConnecting(const ConnectionDetails& cd);
    void modbusConnected(const ConnectionDetails& cd);
    void modbusDisconnected(const ConnectionDetails& cd);

private slots:
    void on_readReply();
    void on_writeReply();
    void on_errorOccurred(QModbusDevice::Error error);
    void on_stateChanged(QModbusDevice::State state);

private:
    void tryReconnect();

private:
    int _transactionId = -1;
    QModbusClient* _modbusClient;
    ConnectionType _connectionType;
    bool m_needToReconnect = true;
    bool m_reconnecting = false;
    std::chrono::milliseconds m_retryBackoff;
    std::shared_ptr<bool> m_guard;
};

} // namespace mpk::dss

// NOLINTEND
// clang-format on
