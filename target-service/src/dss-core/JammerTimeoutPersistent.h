#pragma once

#include "dss-core/DeviceId.h"
#include "dss-core/JammerTimeoutControl.h"
#include "dss-core/database/JammerTimeoutStatusDbHelper.h"

#include <QObject>

namespace mpk::dss::core
{

class JammerTimeoutControl;

class JammerTimeoutPersistent : public QObject
{
    Q_OBJECT

public:
    explicit JammerTimeoutPersistent(JammerTimeoutControl& timeoutControl);
    void load(DeviceId id);

private slots:
    void updateStatus(JammerTimeoutControl::StatusEvent event);

private:
    JammerTimeoutControl* m_timeoutControl;
    QString m_deviceId;
    JammerTimeoutStatusDbHelper m_dbHelper;
};

} // namespace mpk::dss::core
