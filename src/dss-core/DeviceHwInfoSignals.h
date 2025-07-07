#pragma once

#include "DeviceHwInfo.h"

#include <QMetaType>
#include <QObject>

namespace mpk::dss::core
{

class DeviceHwInfoSignals : public QObject
{
    Q_OBJECT

public:
    explicit DeviceHwInfoSignals(QObject* parent = nullptr) : QObject(parent)
    {
    }

signals:
    void hwInfoChanged(DeviceHwInfo);
};

} // namespace mpk::dss::core

Q_DECLARE_METATYPE(mpk::dss::core::DeviceHwInfo)
