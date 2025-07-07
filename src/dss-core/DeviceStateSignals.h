#pragma once

#include "dss-core/DeviceState.h"

#include <QMetaType>
#include <QObject>

namespace mpk::dss::core
{

class DeviceStateSignals : public QObject
{
    Q_OBJECT

public:
    explicit DeviceStateSignals(QObject* parent = nullptr) : QObject(parent)
    {
    }

signals:
    void stateChanged(DeviceState::Value);
};

} // namespace mpk::dss::core
