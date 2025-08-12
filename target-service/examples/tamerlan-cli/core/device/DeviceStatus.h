#pragma once

#include <QObject>

#include <cstdint>
#include <ostream>

namespace mpk::drone::core
{

enum class DeviceStatus : std::uint16_t
{
    NORMAL_MODE = 0x0000,
    SERVICE_MODE = 0x0001,
    SIGNAL_DEVICE_ERROR = 0x8000,
    EXTERNAL_DEVICE_ERROR = 0x8010,
    LICENSE_ERROR = 0x8020,
    SOFTWARE_ERROR = 0x80F0
};

std::ostream& operator<<(std::ostream& stream, DeviceStatus status);

class DeviceStatusHolder : public QObject
{
    Q_OBJECT

public:
    explicit DeviceStatusHolder(DeviceStatus status, QObject* parent = nullptr);

    void setStatus(DeviceStatus status);
    [[nodiscard]] DeviceStatus status() const;

signals:
    void changed(DeviceStatus);

private:
    DeviceStatus m_status;
};

} // namespace mpk::drone::core
