#pragma once

#include "DevicePosition.h"

#include <QMetaType>
#include <QObject>

namespace mpk::dss::core
{

class DevicePositionSignals : public QObject
{
    Q_OBJECT

public:
    explicit DevicePositionSignals(QObject* parent = nullptr) : QObject(parent)
    {
    }

signals:
    void positionChanged();
};

} // namespace mpk::dss::core

Q_DECLARE_METATYPE(mpk::dss::core::DevicePosition)
