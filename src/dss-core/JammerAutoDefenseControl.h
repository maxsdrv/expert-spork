#pragma once

#include "dss-backend/BackendSignals.h"
#include "dss-core/AlarmStatus.h"
#include "dss-core/DeviceId.h"
#include "dss-core/JammerBand.h"
#include "dss-core/JammerDevice.h"
#include "dss-core/JammerTimeoutControl.h"
#include "dss-core/TargetClass.h"
#include "dss-core/Track.h"

#include <QObject>

#include <chrono>
#include <optional>

#include "gsl/pointers"

namespace mpk::dss::core
{

using namespace std::chrono_literals;

class SensorJammerControl;
class LicenseController;

struct JammerAutoDefenseData
{
    JammerProviderPtr jammers;
    gsl::not_null<backend::BackendSignals*> bSignals;
};

seconds timeoutOrMaxTimeout(JammerProviderPtr jammers, seconds timeout);

class JammerAutoDefenseControl : public QObject
{
    Q_OBJECT

public:
    JammerAutoDefenseControl(
        JammerAutoDefenseData data,
        seconds& timeout,
        const LicenseController* licenseController,
        QObject* parent = nullptr);

private slots:
    void handleTargetUpdated(
        const core::DeviceId& devId,
        const core::TrackId& trackId,
        const core::AlarmStatus& alarmSt,
        core::TargetClass::Value);

private:
    JammerProviderPtr m_jammerProvider;
    std::shared_ptr<JammerTimeoutControl> m_timeoutControl;
    seconds m_timeout;
    const LicenseController* m_licenseController;
};

} // namespace mpk::dss::core
