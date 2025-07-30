#pragma once

#include <QDate>
#include <QObject>

namespace mpk::drone::core
{

struct Version
{
    int major;
    int minor;
    int number;
    int build;
};

struct DeviceInfo
{
    std::string model;
    std::string serial;
    Version version;
    QString revision;
    QDate lastUpdated;
};

class DeviceInfoHolder : public QObject
{
    Q_OBJECT

public:
    explicit DeviceInfoHolder(DeviceInfo info, QObject* parent = nullptr);

    [[nodiscard]] DeviceInfo info() const;

private:
    DeviceInfo m_info;
};

} // namespace mpk::drone::core
