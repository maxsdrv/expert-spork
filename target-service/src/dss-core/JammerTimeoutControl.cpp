#include "JammerTimeoutControl.h"
#include "JammerBandControl.h"
#include "JammerBandSignals.h"
#include "SensorJammerControl.h"

#include "log/Log2.h"

namespace mpk::dss::core
{

constexpr auto SecsMSecs = 1000;

JammerTimeoutControl::JammerTimeoutControl(
    JammerBandControl& jammer,
    core::JammerBandSignals* bandSignals,
    int timeoutMax,
    QObject* parent) :
  QObject(parent),
  m_started(false),
  m_jammer(jammer),
  m_bandSignals(bandSignals),
  m_timeoutMax(timeoutMax)
{
    m_timer.setSingleShot(true);
    connect(&m_timer, &QTimer::timeout, this, [this] { timerFinish(); });
}

JammerTimeoutControl::~JammerTimeoutControl()
{
    if (m_status)
    {
        timerFinish();
    }
}

bool JammerTimeoutControl::jammerStartStop(
    const mpk::dss::core::JammerBandOption& bandsActive, int sec)
{
    QString message;
    return jammerStartStop(bandsActive, sec, message);
}

bool JammerTimeoutControl::jammerStartStop(
    const mpk::dss::core::JammerBandOption& bandsActive,
    int sec,
    QString& message)
{
    LOG_DEBUG
        << "JammerTimeoutControl: jammerStartStop requested with soft timeout sec="
        << sec;

    if (sec > m_timeoutMax)
    {
        message =
            QString(
                "JammerTimeoutControl: Requested timeout is greater than maximum, s: %1")
                .arg(m_timeoutMax);
        LOG_ERROR << message << "\n";
        return false;
    }

    if (m_status && JAMMER_BANDS_OFF == bandsActive)
    {
        timerFinish(true);
        message = QString("JammerTimeoutControl: OK stop requested");
        LOG_INFO << message;
        return true;
    }

    if (m_jammer.setBands(bandsActive))
    {
        bandsOffAfter(sec, bandsActive);
        message = QString("JammerTimeoutControl: OK start requested");
        LOG_INFO << message;
        return true;
    }

    message = QString(
        "JammerTimeoutControl: incorrect band option requested. Bands are not set");
    LOG_ERROR << message << "\n";
    return false;
}

void JammerTimeoutControl::bandsOffAfter(
    int sec, const mpk::dss::core::JammerBandOption& bandsActive)
{
    LOG_DEBUG << "JammerTimeoutControl: bandsOffAfter sec=" << sec;

    m_timer.start(sec * SecsMSecs);

    auto notifyStart = std::chrono::system_clock::now();
    auto updateStatus = [notifyStart, sec, bandsActive, this]
    {
        m_status = {
            notifyStart, std::chrono::system_clock::now(), sec, bandsActive};
        emit m_bandSignals->bandsChanged();
    };

    disconnect(&m_notifyTimer, nullptr, this, nullptr);
    connect(
        &m_notifyTimer,
        &QTimer::timeout,
        this,
        [updateStatus] { updateStatus(); });

    updateStatus();
    emit statusEvent(STARTED);
    constexpr auto notifyIntervalSec = 1;
    m_notifyTimer.start(notifyIntervalSec * SecsMSecs);
}

void JammerTimeoutControl::timerFinish(bool unsetBands)
{
    LOG_DEBUG << "JammerTimeoutControl: timerFinish()";

    if (unsetBands)
    {
        LOG_DEBUG << "JammerTimeoutControl "
                  << "Calling unsetAllBands()";

        m_jammer.unsetAllBands();
    }
    m_timer.stop();
    m_notifyTimer.stop();

    m_status.reset();
    emit statusEvent(STOPPED);
}

std::optional<JammerTimeoutStatus> JammerTimeoutControl::status() const
{
    return m_status;
}

} // namespace mpk::dss::core
