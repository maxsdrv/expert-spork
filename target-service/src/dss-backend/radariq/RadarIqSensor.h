#pragma once

#include "RadarIqConnection.h"

#include "dss-core/DeviceId.h"
#include "dss-core/DevicePosition.h"
#include "dss-core/DeviceType.h"
#include "dss-core/DeviceWorkzone.h"
#include "dss-core/SensorProperties.h"

#include <chrono>

namespace mpk::dss::backend::radariq
{

class RadarIqSensor : public QObject, public core::SensorProperties
{
    Q_OBJECT

public:
    explicit RadarIqSensor(
        std::shared_ptr<RadarIqConnection> connection,
        QString model,
        QObject* parent);
    ~RadarIqSensor() override;

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

    [[nodiscard]] core::DevicePosition autoPosition() const;
    [[nodiscard]] core::DevicePosition manualPosition() const;

public slots:
    void setImitatorMark(bool isImitator);

private slots:
    void updateState(bool connected, bool faultless);
    void updateModel(const QString& model);
    void updateGeneralSettings(const OARadarIqDevice_general_settings& settings);
    void updateRadarPosition(const OARadarIqGeo_position& position);
    void updateRadarWorkzone(const QList<OARadarIqSector>& sectors);
    void updateRadarTemperature(double temperature);
    void updateRadarMode(const OARadarIqRadar_mode& mode);
    void updateRadarPowerStatus(bool isPowerOn);

private:
    std::shared_ptr<RadarIqConnection> m_connection;

    QString m_serial;
    QString m_model;
    QString m_swVersion{"Undefined"};
    bool m_disabled = false;
    bool m_isImitator = false;
    core::DevicePosition m_autoPosition;
    core::DevicePosition m_manualPosition;
    core::DevicePositionMode m_positionMode = core::DevicePositionMode::AUTO;
    core::DeviceWorkzone m_workzone;
    core::DeviceState::Value m_state = core::DeviceState::UNAVAILABLE;
    core::DeviceHwInfo m_hwInfo;
    core::JammerMode::Value m_jammerMode = core::JammerMode::MANUAL;

    std::unique_ptr<core::DevicePositionSignals> m_positionSignals;
    std::unique_ptr<core::DeviceWorkzoneSignals> m_workzoneSignals;
    std::unique_ptr<core::DeviceDisabledSignals> m_disabledSignals;
    std::unique_ptr<core::DeviceStateSignals> m_stateSignals;
    std::unique_ptr<core::DeviceHwInfoSignals> m_hwInfoSignals;

    bool m_radarPowerOn{false};
    bool m_radarConditionOk{true};
};

} // namespace mpk::dss::backend::radariq
