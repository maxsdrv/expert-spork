#pragma once

#include "ServiceRole.h"

#include <QModbusDataUnit>
#include <QObject>

namespace mpk::dss::imitator::tamerlan
{

class ModbusServer : public QObject
{
    Q_OBJECT

public:
    explicit ModbusServer(QObject* parent = nullptr) : QObject(parent)
    {
    }

    Q_INVOKABLE virtual void start() = 0;
    virtual void stop() = 0;
    [[nodiscard]] virtual std::vector<int> serverAddresses() const = 0;
    [[nodiscard]] virtual ServiceRole serviceRole(int serverAddress) const = 0;
    virtual bool data(int serverAddress, QModbusDataUnit* newData) const = 0;
    virtual bool data(
        int serverAddress,
        QModbusDataUnit::RegisterType table,
        quint16 address,
        quint16* data) const = 0;
    virtual bool setData(
        int serverAddress,
        QModbusDataUnit::RegisterType table,
        quint16 address,
        quint16 data) = 0;
    virtual bool setData(int serverAddress, const QModbusDataUnit& newData) = 0;
};

} // namespace mpk::dss::imitator::tamerlan
