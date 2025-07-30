#pragma once

#include "dss-core/DeviceId.h"

#include <QObject>

namespace mpk::dss::core
{

class DeviceProviderSignals : public QObject
{
    Q_OBJECT

public:
    explicit DeviceProviderSignals(QObject* parent = nullptr) : QObject(parent)
    {
    }

signals:
    void registered(core::DeviceId);
};

} // namespace mpk::dss::core
