#pragma once

#include "dss-core/DeviceWorkzone.h"

#include <QMetaType>
#include <QObject>

namespace mpk::dss::core
{

class DeviceWorkzoneSignals : public QObject
{
    Q_OBJECT

public:
    explicit DeviceWorkzoneSignals(QObject* parent = nullptr) : QObject(parent)
    {
    }

signals:
    void workzoneChanged(DeviceWorkzone);
};

} // namespace mpk::dss::core

Q_DECLARE_METATYPE(mpk::dss::core::DeviceWorkzone)
