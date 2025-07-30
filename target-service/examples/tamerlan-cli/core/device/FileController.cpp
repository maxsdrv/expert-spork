#include "FileController.h"

#include "core/device/DeviceStatus.h"

#include "core/modbus/ModbusData.h"

#include "core/proto/Protocol.h"

#include "log/Log2.h"

#include <QProcess>

namespace mpk::drone::core
{

constexpr auto filePath = "/tmp/testFile";
const uint16_t zeroUint16 = 0U;

FileController::FileController(
    gsl::not_null<ModbusData*> modbusData,
    gsl::not_null<DeviceStatusHolder*> deviceStatusHolder,
    QObject* parent) :
  QObject(parent),
  m_filePath(filePath),
  m_modbusData(modbusData),
  m_deviceStatusHolder(deviceStatusHolder),
  m_modbusFileTransfer(
      std::make_unique<ModbusFileTransfer>(m_filePath, m_modbusData))
{
    QObject::connect(
        m_modbusFileTransfer.get(),
        &ModbusFileTransfer::fileReceived,
        m_modbusFileTransfer.get(),
        [this](const auto& filename)
        {
            if (filename == m_filePath)
            {
                m_fileReceived = true;
            }
        });
    m_modbusData->emplace(
        proto::UpdateControl::id,
        proto::UpdateControl::address,
        proto::UpdateControl::type,
        zeroUint16);
    m_modbusData->emplace(
        proto::LicenseControl::id,
        proto::LicenseControl::address,
        proto::LicenseControl::type,
        zeroUint16);

    QObject::connect(
        m_modbusData,
        &ModbusData::modified,
        m_modbusData,
        [this](const auto& id)
        {
            if (id == proto::UpdateControl::id
                || id == proto::LicenseControl::id)
            {
                std::uint16_t reg = 0U;
                m_modbusData->read(id, reg, proto::LicenseControl::type);
                if (reg != 0)
                {
                    updateSoftware(id);
                }
            }
        });
}

void FileController::updateSoftware(const std::string& regId)
{
    if (!m_fileReceived)
    {
        LOG_ERROR << "Error updating software: file not received";
        return;
    }
    m_deviceStatusHolder->setStatus(DeviceStatus::SERVICE_MODE);
    QProcess process;
    process.start("bash", QStringList{m_filePath});
    process.waitForFinished();
    m_fileReceived = false;
    m_modbusData->write(regId, zeroUint16, QModbusDataUnit::Coils);
    emit softwareUpdated();
}

} // namespace mpk::drone::core
