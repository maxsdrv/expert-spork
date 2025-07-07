#pragma once

#include "SkycopeConnection.h"

#include "dss-common/sensors/RfdTargetAttributes.h"
#include "dss-core/AlarmLevel.h"
#include "dss-core/DeviceId.h"
#include "dss-core/DevicePosition.h"
#include "dss-core/DeviceType.h"
#include "dss-core/DeviceWorkzone.h"
#include "dss-core/JammerAutoDefenseControl.h"
#include "dss-core/SensorProperties.h"
#include "dss-core/TargetClass.h"
#include "dss-core/TrackId.h"

#include <QObject>

namespace mpk::dss::backend::skycope
{

class SkycopeJammer;

class SkycopeSensor final : public QObject, public core::SensorProperties
{
    Q_OBJECT

public:
    explicit SkycopeSensor(
        std::shared_ptr<SkycopeConnection> connection,
        const QString& model,
        core::JammerAutoDefenseData autoData,
        gsl::not_null<const core::LicenseController*> licenseController,
        QObject* parent = nullptr);
    ~SkycopeSensor() override;

    [[nodiscard]] core::DeviceType::Value type() const override;
    [[nodiscard]] Serial serial() const override;
    [[nodiscard]] Model model() const override;
    [[nodiscard]] Version swVersion() const override;
    [[nodiscard]] Fingerprint fingerprint() const override;

    [[nodiscard]] bool disabled() const override;
    void setDisabled(bool disabled) override;
    [[nodiscard]] gsl::not_null<const core::DeviceDisabledSignals*>
    disabledSignals() const override;

    [[nodiscard]] core::DevicePosition position() const override;
    void setPosition(const core::DevicePosition& position) override;
    [[nodiscard]] core::DevicePositionMode positionMode() const override;
    void setPositionMode(const core::DevicePositionMode& mode) override;
    [[nodiscard]] gsl::not_null<const core::DevicePositionSignals*>
    positionSignals() const override;

    [[nodiscard]] core::DeviceWorkzone workzone() const override;
    [[nodiscard]] gsl::not_null<const core::DeviceWorkzoneSignals*>
    workzoneSignals() const override;

    [[nodiscard]] core::DeviceState::Value state() const override;
    [[nodiscard]] gsl::not_null<const core::DeviceStateSignals*> stateSignals()
        const override;

    [[nodiscard]] core::DeviceHwInfo hwInfo() const override;
    [[nodiscard]] gsl::not_null<const core::DeviceHwInfoSignals*> hwInfoSignals()
        const override;

    [[nodiscard]] std::vector<core::DeviceId> jammerIds() const override;
    void setJammer(
        const core::DeviceId& id, gsl::not_null<SkycopeJammer*> jammer);

signals:
    void targetDetected(
        QString id,
        core::TargetClass::Value targetClass,
        sensors::RfdTargetAttributes attributes,
        std::optional<QGeoCoordinate> coordinate,
        std::optional<double> bearing);

private slots:
    void updateState(bool connected, bool faultless);
    void updateSystemData(const OASkycopeModel& content);

private:
    void parseDronesInfo(
        const QMap<QString, OASkycopeDrone>& drones, const QString& sensorName);
    void parseNormalDrone(
        const QString& id,
        const OASkycopeDrone& device,
        const QString& sensorName,
        const sensors::RfdTargetAttributes& attributes);
    void parseWifiInfo(const QMap<QString, OASkycopeDroneWifiDevice>& consoles);
    [[nodiscard]] QGeoCoordinate correctCoord(const QGeoCoordinate& coord) const;

private:
    std::shared_ptr<SkycopeConnection> m_connection;

    QString m_serial;
    QString m_model;
    QString m_swVersion;
    bool m_disabled = false;
    core::DevicePosition m_autoPosition;
    core::DevicePosition m_manualPosition;
    core::DevicePositionMode m_positionMode = core::DevicePositionMode::AUTO;
    core::DeviceWorkzone m_workzone;
    core::DeviceState::Value m_state{core::DeviceState::UNAVAILABLE};
    core::DeviceHwInfo m_hwInfo;

    std::unique_ptr<core::DevicePositionSignals> m_positionSignals;
    std::unique_ptr<core::DeviceWorkzoneSignals> m_workzoneSignals;
    std::unique_ptr<core::DeviceDisabledSignals> m_disabledSignals;
    std::unique_ptr<core::DeviceStateSignals> m_stateSignals;
    std::unique_ptr<core::DeviceHwInfoSignals> m_hwInfoSignals;

    bool m_deviceConditionOk = false;

    std::optional<std::pair<core::DeviceId, gsl::not_null<SkycopeJammer*>>>
        m_jammer = std::nullopt;
};

} // namespace mpk::dss::backend::skycope
