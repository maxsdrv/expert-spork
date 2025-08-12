#pragma once

#include "core/modbus/ModbusFileTransfer.h"

namespace mpk::drone::core
{

class ModbusData;
class DeviceStatusHolder;

class FileController : public QObject
{
    Q_OBJECT

public:
    FileController(
        gsl::not_null<ModbusData*> modbusData,
        gsl::not_null<DeviceStatusHolder*> deviceStatusHolder,
        QObject* parent = nullptr);

signals:
    void softwareUpdated();

private:
    void updateSoftware(const std::string& regId);

private:
    QString m_filePath;
    bool m_fileReceived = false;
    gsl::not_null<ModbusData*> m_modbusData;
    gsl::not_null<DeviceStatusHolder*> m_deviceStatusHolder;
    std::unique_ptr<ModbusFileTransfer> m_modbusFileTransfer;
};

} // namespace mpk::drone::core
