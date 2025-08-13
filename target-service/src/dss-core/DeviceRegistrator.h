#pragma once

#include "dss-core/DeviceDataControl.h"
#include "dss-core/DeviceId.h"
#include "dss-core/DeviceIdMappingStorage.h"

#include "gsl/pointers"

#include <QObject>

namespace mpk::dss::core
{

class DeviceRegistrator : public QObject
{
    Q_OBJECT

public:
    DeviceRegistrator(
        std::shared_ptr<DeviceIdMappingStorage> deviceIdMapping,
        gsl::not_null<core::DeviceDataControl*> dataControl,
        DeviceClass::Value deviceClass,
        QObject* parent = nullptr);

signals:
    void registered(core::DeviceId);
    void failed();

private:
    void tryRegister();
    void tryRegisterMain();
    void tryRegisterFallback();

    std::chrono::milliseconds m_retryBackoff;
    std::shared_ptr<DeviceIdMappingStorage> m_deviceIdMapping;
    gsl::not_null<core::DeviceDataControl*> m_dataControl;
    DeviceClass::Value m_deviceClass;
    std::shared_ptr<bool> m_guard;
};

} // namespace mpk::dss::core
