#pragma once

#include "RadarIqSensor.h"

#include "dss-core/CameraProperties.h"
#include "dss-core/DeviceId.h"
#include "dss-core/DeviceType.h"
#include "dss-core/SensorProperties.h"
#include "dss-core/TrackId.h"

namespace mpk::dss::backend::radariq
{
using namespace OAService;

class RadarIqCameraControl final : public QObject, public core::CameraProperties
{
    Q_OBJECT

public:
    explicit RadarIqCameraControl(
        QString radarIqHost,
        QObject* parent);
    ~RadarIqCameraControl() override;

    [[nodiscard]] QString id() const override;

private:
    const QString m_id;
};

} // namespace mpk::dss::backend::radariq
