#pragma once

#include "JammerBandOption.h"
#include "JammerMode.h"
#include "JammerTimeoutStatus.h"
#include "LocalLock.h"

#include <QObject>
#include <QTimer>

#include <chrono>
#include <limits>
#include <optional>

namespace mpk::dss::core
{

using seconds = std::chrono::seconds;

class JammerBandControl;
class JammerBandSignals;
class SensorJammerControl;

const auto JAMMER_RESULT_OK = QString();

class JammerTimeoutControl : public QObject
{
    Q_OBJECT

public:
    enum StatusEvent
    {
        STARTED,
        STOPPED
    };
    Q_ENUM(StatusEvent);

    ~JammerTimeoutControl() override;

    JammerTimeoutControl(
        JammerBandControl& jammer,
        core::JammerBandSignals* bandSignals,
        int timeoutMax = std::numeric_limits<int>::max(),
        QObject* parent = nullptr);

    [[nodiscard]] std::optional<JammerTimeoutStatus> status() const;
    [[nodiscard]] int timeoutMax() const
    {
        return m_timeoutMax;
    }

public slots:
    /* bands - chosen single band option to set; provide JAMMER_BANDS_OFF to
                stop suppression
       sec - unset bands timeout
    */

    virtual bool jammerStartStop(
        const mpk::dss::core::JammerBandOption& bandsActive, int sec);
    virtual bool jammerStartStop(
        const mpk::dss::core::JammerBandOption& bandsActive,
        int sec,
        QString& message);
    void timerFinish(bool unsetBands = true);

private slots:
    void bandsOffAfter(
        int sec, const mpk::dss::core::JammerBandOption& bandsActive);

private:
    QTimer m_timer;
    bool m_started;
    QTimer m_notifyTimer;
    JammerBandControl& m_jammer;
    core::JammerBandSignals* m_bandSignals;
    std::optional<JammerTimeoutStatus> m_status;
    int m_timeoutMax;

signals:
    void statusEvent(StatusEvent);
};

} // namespace mpk::dss::core
