#include "SkycopeJammer.h"

#include "SkycopeController.h"

#include "dss-backend/skycope/SkycopeSensor.h"
#include "dss-core/JammerBand.h"
#include "dss-core/JammerBandOption.h"
#include "dss-core/audit/AuditEventLogger.h"

#include "exception/InvalidValue.h"
#include "log/Log2.h"

#include <algorithm>

namespace mpk::dss::core
{
qint32 freqToInt(const QString& frequency)
{
    constexpr quint32 GHZ_TO_KHZ = 1000000;
    constexpr quint32 MHZ_TO_KHZ = 1000;

    static const QRegularExpression re("[^0-9\\.]");

    auto numericFrequency = frequency;
    numericFrequency.remove(re);

    double freq = numericFrequency.toDouble();

    if (frequency.contains("GHz"))
    {
        return static_cast<qint32>(freq * GHZ_TO_KHZ);
    }
    if (frequency.contains("MHz"))
    {
        return static_cast<qint32>(freq * MHZ_TO_KHZ);
    }

    return static_cast<qint32>(freq);
}
} // namespace mpk::dss::core

namespace mpk::dss::backend::skycope
{

constexpr auto TIMEOUT_MAX_SECONDS = 131072;
constexpr auto JAMMER_DEFAULT_MODEL = "Tamerlan J-200";

SkycopeJammer::SkycopeJammer(
    std::shared_ptr<SkycopeConnection> connection,
    gsl::not_null<SkycopeSensor*> sensor,
    int distance,
    const QString& model,
    const std::optional<GroupId>& groupId,
    QObject* parent) :
  core::JammerTimeoutDeviceControl(new core::JammerTimeoutControl(
      *this, &m_bandSignals, TIMEOUT_MAX_SECONDS)),
  QObject(parent),
  m_connection(std::move(connection)),
  m_model{model.isEmpty() ? JAMMER_DEFAULT_MODEL : model},
  m_groupId(groupId),
  m_positionSignals{std::make_unique<core::DevicePositionSignals>()},
  m_workzoneSignals{std::make_unique<core::DeviceWorkzoneSignals>()},
  m_disabledSignals{std::make_unique<core::DeviceDisabledSignals>()},
  m_hwInfoSignals{std::make_unique<core::DeviceHwInfoSignals>()},
  m_sensor{sensor}
{
    constexpr double circleAngle = 360.0;
    m_workzone.sectors.emplace_back(core::DeviceWorkzoneSector{
        0, static_cast<double>(distance), 0, circleAngle});

    connect(
        m_connection.get(),
        &SkycopeConnection::defenseDataUpdated,
        this,
        &SkycopeJammer::updateDefenseData);
    connect(
        this,
        &SkycopeJammer::postDefenseDataSignal,
        m_connection.get(),
        &SkycopeConnection::postDefenseDataStart,
        Qt::QueuedConnection);
    connect(
        this,
        &SkycopeJammer::postDefenseDataStopSignal,
        m_connection.get(),
        &SkycopeConnection::postDefenseDataStop,
        Qt::QueuedConnection);
    connect(
        m_connection.get(),
        &SkycopeConnection::postDefenseDataSignalError,
        this,
        &SkycopeJammer::postDefenseDataSignalError);
}

SkycopeJammer::~SkycopeJammer() = default;

auto SkycopeJammer::serial() const -> Serial
{
    return m_serial;
}

auto SkycopeJammer::model() const -> Model
{
    return m_model;
}

auto SkycopeJammer::swVersion() const -> Version
{
    return m_sensor->swVersion();
}

auto SkycopeJammer::fingerprint() const -> Fingerprint
{
    if (!m_serial.isEmpty())
    {
        return QString(nodeKey) + "-" + m_serial;
    }
    return {};
}

core::DevicePosition SkycopeJammer::position() const
{
    return m_positionMode == core::DevicePositionMode::AUTO ?
               m_sensor->position() :
               m_manualPosition;
}

void SkycopeJammer::setPosition(const core::DevicePosition& position)
{
    if (m_manualPosition != position)
    {
        m_manualPosition = position;
        if (m_positionMode == core::DevicePositionMode::MANUAL
            || m_positionMode == core::DevicePositionMode::ALWAYS_MANUAL)
        {
            emit m_positionSignals->positionChanged();
        }
    }
}

core::DevicePositionMode SkycopeJammer::positionMode() const
{
    return m_positionMode;
}

void SkycopeJammer::setPositionMode(const core::DevicePositionMode& mode)
{
    if (mode != m_positionMode)
    {
        m_positionMode = mode;
        if (mode == core::DevicePositionMode::AUTO)
        {
            connect(
                m_sensor->positionSignals(),
                &core::DevicePositionSignals::positionChanged,
                m_positionSignals.get(),
                &core::DevicePositionSignals::positionChanged);
        }
        else
        {
            disconnect(
                m_sensor->positionSignals(),
                nullptr,
                m_positionSignals.get(),
                nullptr);
        }
        emit m_positionSignals->positionChanged();
    }
}

gsl::not_null<const core::DevicePositionSignals*> SkycopeJammer::positionSignals()
    const
{
    return m_positionSignals.get();
}

bool SkycopeJammer::disabled() const
{
    return m_disabled;
}

void SkycopeJammer::setDisabled(bool disabled)
{
    if (disabled != m_disabled)
    {
        m_disabled = disabled;

        core::auditLogger().logJammerDisabled(disabled, nodeKey, m_serial);
        emit m_disabledSignals->disabledChanged(disabled);
    }
}

gsl::not_null<const core::DeviceDisabledSignals*> SkycopeJammer::disabledSignals()
    const
{
    return m_disabledSignals.get();
}

core::DeviceWorkzone SkycopeJammer::workzone() const
{
    return m_workzone;
}

gsl::not_null<const core::DeviceWorkzoneSignals*> SkycopeJammer::workzoneSignals()
    const
{
    return m_workzoneSignals.get();
}

core::DeviceState::Value SkycopeJammer::state() const
{
    return m_sensor->state();
}

gsl::not_null<const core::DeviceStateSignals*> SkycopeJammer::stateSignals()
    const
{
    return m_sensor->stateSignals();
}
core::JammerMode::Value SkycopeJammer::jammerMode() const
{
    return m_sensor->jammerMode();
}

core::JammerBands SkycopeJammer::bands()
{
    return m_bands;
}

[[nodiscard]] core::JammerBandOptionVec SkycopeJammer::bandOptions()
{
    return m_bandOptions;
}

std::optional<core::JammerGroupControl::GroupId> SkycopeJammer::groupId() const
{
    return m_groupId;
}

bool SkycopeJammer::setBands(const mpk::dss::core::JammerBandOption& bandsActive)
{
    if (std::find(m_bandOptions.begin(), m_bandOptions.end(), bandsActive)
        == m_bandOptions.end())
    {
        return false;
    }

    LOG_DEBUG << "setBands(): postDefenseDataSignal, secs, deviceTimeout="
              << TIMEOUT_MAX_SECONDS;
    emit postDefenseDataSignal(
        m_serial,
        m_oaDefenseData.getAzimuth(),
        TIMEOUT_MAX_SECONDS,
        QList<QString>(bandsActive.begin(), bandsActive.end()),
        m_oaDefenseData.getStrategy());

    for (const auto& bandLabel: bandsActive)
    {
        core::auditLogger().logSuppressionManualRequested(
            core::JammerBand{bandLabel, true}, nodeKey, m_serial);
    }

    return true;
}

void SkycopeJammer::unsetAllBands()
{
    LOG_DEBUG << "unsetAllBands(): postDefenseDataStopSignal";
    emit postDefenseDataStopSignal(m_serial);

    for (const auto& band: bands())
    {
        core::auditLogger().logSuppressionManualRequested(
            core::JammerBand{band.label, false}, nodeKey, m_serial);
    }
}

void SkycopeJammer::postDefenseDataSignalError()
{
    LOG_DEBUG << "postDefenseDataSignalError()";
    timeoutControl().timerFinish(false);
}

gsl::not_null<const core::JammerBandSignals*> SkycopeJammer::bandSignals() const
{
    return &m_bandSignals;
}

core::DeviceHwInfo SkycopeJammer::hwInfo() const
{
    return m_hwInfo;
}

gsl::not_null<const core::DeviceHwInfoSignals*> SkycopeJammer::hwInfoSignals()
    const
{
    return m_hwInfoSignals.get();
}

core::DeviceId SkycopeJammer::sensorId() const
{
    return m_sensorId;
}

void SkycopeJammer::setSensorId(const core::DeviceId& sensorId)
{
    m_sensorId = sensorId;
}

void SkycopeJammer::updateDefenseData(
    const QMap<QString, OASkycopeMassDefense>& content, bool initial)
{
    m_oaDefenseData = content.first();

    if (initial)
    {
        m_serial = content.firstKey();
        m_bandOptions.clear();
        const auto frequencyOptions = m_oaDefenseData.getFrequencyOptions();
        for (const auto& fOption: frequencyOptions)
        {
            core::JammerBandOption bandOption;
            auto freq = fOption.getFreq();
            std::copy(
                freq.begin(),
                freq.end(),
                std::inserter(bandOption, bandOption.end()));

            m_bandOptions.push_back(bandOption);
            auto fs = std::set(freq.begin(), freq.end());
            m_allFreqLabels.insert(fs.begin(), fs.end());
        }

        m_bands = toBands(m_allFreqLabels, false);
    }

    auto frequency = m_oaDefenseData.isInProgress() ?
                         m_oaDefenseData.getFrequency() :
                         QList<QString>();
    // assume both sorted
    if (initial || m_frequencies != frequency)
    {
        for (auto& band: m_bands)
        {
            bool active = frequency.contains(band.label);
            if (initial || active != band.active)
            {
                band.active = active;
                core::auditLogger().logSuppressionState(band, nodeKey, m_serial);
            }
        }

        emit m_bandSignals.bandsChanged();
        m_frequencies = frequency;
    }
}

core::JammerBandOption SkycopeJammer::maxBands()
{
#ifndef NDEBUG
    if (max_element(
            m_bandOptions.begin(),
            m_bandOptions.end(),
            [](auto l, auto r) { return l.size() < r.size(); })
        != m_bandOptions.end() - 1)
    {
        throw ::exception::InvalidValue();
    }
#endif
    if (!m_bandOptions.empty())
    {
        return *(m_bandOptions.end() - 1);
    }
    return {};
}

} // namespace mpk::dss::backend::skycope
