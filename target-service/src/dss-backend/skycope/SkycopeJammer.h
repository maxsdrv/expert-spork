#pragma once

#include "SkycopeConnection.h"

#include "OASkycopeMassDefense.h"

#include "dss-core/JammerBandOption.h"
#include "dss-core/JammerMode.h"
#include "dss-core/JammerProperties.h"
#include "dss-core/LocalLock.h"

#include "log/Log2.h"

#include <QObject>

#include <memory>

namespace mpk::dss::core
{
qint32 freqToInt(const QString& frequency);
struct strFreqComp
{
    bool operator()(const QString& lhs, const QString& rhs) const
    {
        return freqToInt(lhs) < freqToInt(rhs);
    }
};

class JammerTimeoutControl;
} // namespace mpk::dss::core

namespace mpk::dss::backend::skycope
{

class SkycopeSensor;
class SkycopeJammerTimeoutControl;

class SkycopeJammer final : public QObject, public core::JammerProperties
{
    Q_OBJECT

public:
    SkycopeJammer(
        std::shared_ptr<SkycopeConnection> connection,
        gsl::not_null<SkycopeSensor*> sensor,
        int distance,
        const QString& model,
        const std::optional<GroupId>& groupId,
        QObject* parent = nullptr);
    ~SkycopeJammer() override;

    [[nodiscard]] Serial serial() const override;
    [[nodiscard]] Model model() const override;
    [[nodiscard]] Version swVersion() const override;
    [[nodiscard]] Fingerprint fingerprint() const override;

    [[nodiscard]] core::DevicePosition position() const override;
    void setPosition(const core::DevicePosition& position) override;
    [[nodiscard]] core::DevicePositionMode positionMode() const override;
    void setPositionMode(const core::DevicePositionMode& mode) override;
    [[nodiscard]] gsl::not_null<const core::DevicePositionSignals*>
    positionSignals() const override;

    [[nodiscard]] core::DeviceWorkzone workzone() const override;
    [[nodiscard]] gsl::not_null<const core::DeviceWorkzoneSignals*>
    workzoneSignals() const override;

    [[nodiscard]] bool disabled() const override;
    void setDisabled(bool disabled) override;
    [[nodiscard]] gsl::not_null<const core::DeviceDisabledSignals*>
    disabledSignals() const override;

    [[nodiscard]] core::DeviceState::Value state() const override;
    [[nodiscard]] gsl::not_null<const core::DeviceStateSignals*> stateSignals()
        const override;

    [[nodiscard]] core::JammerBands bands() override;
    [[nodiscard]] gsl::not_null<const core::JammerBandSignals*> bandSignals()
        const override;
    [[nodiscard]] core::JammerBandOptionVec bandOptions() override;

    [[nodiscard]] std::optional<GroupId> groupId() const override;

    [[nodiscard]] core::DeviceHwInfo hwInfo() const override;
    [[nodiscard]] gsl::not_null<const core::DeviceHwInfoSignals*> hwInfoSignals()
        const override;

    [[nodiscard]] core::DeviceId sensorId() const override;
    void setSensorId(const core::DeviceId& sensorId);

    bool setBands(const core::JammerBandOption& bandsActive) override;
    void unsetAllBands() override;
    [[nodiscard]] core::JammerBandOption maxBands() override;
    [[nodiscard]] core::JammerMode::Value jammerMode() const override;

signals:
    void postDefenseDataSignal(
        const QString& sensorId,
        qint32 azimuth,
        qint32 sDuration,
        const QList<QString>& frequency,
        const QString& strategy);
    void postDefenseDataStopSignal(const QString& sensorId);

private slots:
    void updateDefenseData(
        const QMap<QString, OASkycopeMassDefense>& content, bool initial);
    void postDefenseDataSignalError();

private:
    std::shared_ptr<SkycopeConnection> m_connection;

    QString m_serial;
    QString m_model;
    bool m_disabled = false;
    core::DevicePosition m_manualPosition;
    core::DevicePositionMode m_positionMode = core::DevicePositionMode::AUTO;
    core::DeviceWorkzone m_workzone;
    core::DeviceHwInfo m_hwInfo;
    core::DeviceId m_sensorId;
    core::JammerBands m_bands;
    QList<QString> m_frequencies;
    core::JammerBandLabelSet m_allFreqLabels;
    core::JammerBandOptionVec m_bandOptions;

    std::optional<GroupId> m_groupId;

    core::JammerBandSignals m_bandSignals;
    std::unique_ptr<core::DevicePositionSignals> m_positionSignals;
    std::unique_ptr<core::DeviceWorkzoneSignals> m_workzoneSignals;
    std::unique_ptr<core::DeviceDisabledSignals> m_disabledSignals;
    std::unique_ptr<core::DeviceHwInfoSignals> m_hwInfoSignals;

    gsl::not_null<SkycopeSensor*> m_sensor;

    OASkycopeMassDefense m_oaDefenseData;
};

} // namespace mpk::dss::backend::skycope
