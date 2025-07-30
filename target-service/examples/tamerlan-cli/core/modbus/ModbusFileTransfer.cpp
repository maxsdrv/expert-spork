#include "ModbusFileTransfer.h"

#include "ModbusData.h"
#include "core/proto/Protocol.h"

#include "log/Log2.h"

#include "qt/Strings.h"

namespace mpk::drone::core
{

std::ostream& operator<<(std::ostream& stream, FileTransferStatus status)
{
    switch (status)
    {
        case FileTransferStatus::CLR:
            stream << "CLR";
            break;
        case FileTransferStatus::RTS:
            stream << "RTS";
            break;
        case FileTransferStatus::RTR:
            stream << "RTR";
            break;
        case FileTransferStatus::CHW:
            stream << "CHW";
            break;
        case FileTransferStatus::CHR:
            stream << "CHR";
            break;
        case FileTransferStatus::EOT:
            stream << "EOT";
            break;
        case FileTransferStatus::ERR:
            stream << "ERR";
            break;
        default:
            stream << "UNDEF";
            break;
    }

    return stream;
}

ModbusFileTransfer::ModbusFileTransfer(
    const QString& filename,
    gsl::not_null<ModbusData*> modbusData,
    QObject* parent) :
  QObject(parent), m_file{filename}, m_chunk{}, m_modbusData{modbusData}
{
    m_modbusData->emplace(
        proto::FileTransferChunk::id,
        proto::FileTransferChunk::address,
        proto::FileTransferChunk::type,
        m_chunk);
    m_modbusData->emplace(
        proto::FileTransferStatus::id,
        proto::FileTransferStatus::address,
        proto::FileTransferStatus::type,
        static_cast<std::uint16_t>(FileTransferStatus::CLR));

    connect(
        m_modbusData,
        &ModbusData::modified,
        this,
        [this](const auto& id)
        {
            if (id == proto::FileTransferStatus::id)
            {
                std::uint16_t reg = 0;
                m_modbusData->read(
                    proto::FileTransferStatus::id,
                    reg,
                    proto::FileTransferStatus::type);
                onStatusChanged(FileTransferStatus{reg});
            }
        });
}

// NOLINTNEXTLINE (readability-function-cognitive-complexity)
void ModbusFileTransfer::onStatusChanged(FileTransferStatus status)
{
    LOG_TRACE << "File transfer: status changed to " << status;

    switch (status)
    {
        case FileTransferStatus::RTS:
            if (m_file.open(QIODevice::ReadWrite | QIODevice::Truncate))
            {
                setStatus(FileTransferStatus::RTR);
                LOG_DEBUG << "File transfer: Opened " << m_file.fileName();
            }
            else
            {
                setStatus(FileTransferStatus::ERR);
                LOG_WARNING << "File transfer: Failed to open "
                            << m_file.fileName();
            }
            break;

        case FileTransferStatus::CHW:
            if (m_file.isOpen())
            {
                m_modbusData->read(
                    proto::FileTransferChunk::id,
                    m_chunk,
                    QModbusDataUnit::HoldingRegisters);

                auto count = static_cast<qint64>(m_chunk.size);
                // NOLINTBEGIN (cppcoreguidelines-pro-type-reinterpret-cast)
                auto written = m_file.write(
                    reinterpret_cast<const char*>(m_chunk.bytes.data()), count);
                // NOLINTEND (cppcoreguidelines-pro-type-reinterpret-cast)
                if (count == written)
                {
                    setStatus(FileTransferStatus::CHR);
                    LOG_DEBUG << "File transfer: written " << written
                              << " bytes to " << m_file.fileName();
                }
                else
                {
                    setStatus(FileTransferStatus::ERR);
                    LOG_WARNING << "File transfer: failed to write to "
                                << m_file.fileName() << " , (" << written
                                << ")";
                }
            }
            else
            {
                setStatus(FileTransferStatus::ERR);
                LOG_WARNING << "File transfer: " << m_file.fileName()
                            << " is not opened";
            }
            break;

        case FileTransferStatus::EOT:
            if (m_file.isOpen() && m_file.size() > 0)
            {
                m_file.close();
                emit fileReceived(m_file.fileName());
                LOG_DEBUG << "File transfer: Received " << m_file.fileName();
            }
            else
            {
                LOG_WARNING << "File transfer: empty file";
            }
            setStatus(FileTransferStatus::CLR);
            break;

        case FileTransferStatus::ERR:
            if (m_file.isOpen())
            {
                m_file.close();
                LOG_DEBUG << "File transfer: Closed " << m_file.fileName();
            }
            break;

        default:
            // Nothing to do
            break;
    }
}

void ModbusFileTransfer::setStatus(FileTransferStatus status)
{
    m_modbusData->write(
        proto::FileTransferStatus::id,
        static_cast<std::uint16_t>(status),
        proto::FileTransferStatus::type);
}

} // namespace mpk::drone::core
