#include "DeviceStatus.h"

namespace mpk::drone::core
{

std::ostream& operator<<(std::ostream& stream, DeviceStatus status)
{
    switch (status)
    {
        case DeviceStatus::NORMAL_MODE:
            stream << "NORMAL_MODE";
            break;
        case DeviceStatus::SERVICE_MODE:
            stream << "SERVICE_MODE";
            break;
        case DeviceStatus::SIGNAL_DEVICE_ERROR:
            stream << "SIGNAL_DEVICE_ERROR";
            break;
        case DeviceStatus::EXTERNAL_DEVICE_ERROR:
            stream << "EXTERNAL_DEVICE_ERROR";
            break;
        case DeviceStatus::LICENSE_ERROR:
            stream << "LICENSE_ERROR";
            break;
        case DeviceStatus::SOFTWARE_ERROR:
            stream << "SOFTWARE_ERROR";
            break;
        default:
            stream << "Unknown";
            break;
    }

    return stream;
}

DeviceStatusHolder::DeviceStatusHolder(DeviceStatus status, QObject* parent) :
  QObject(parent), m_status(status)
{
}

void DeviceStatusHolder::setStatus(DeviceStatus status)
{
    if (m_status != status)
    {
        m_status = status;
        emit changed(status);
    }
}

DeviceStatus DeviceStatusHolder::status() const
{
    return m_status;
}

} // namespace mpk::drone::core
