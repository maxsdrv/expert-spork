#pragma once

#include "dss-backend/tamerlan/modbus/CommandBatch.h"
#include "dss-core/DeviceId.h"
#include "dss-core/JammerProperties.h"
#include "dss-core/SensorJammerControl.h"

#include <QObject>

#include <deque>
#include <memory>

class QTimer;

namespace mpk::dss::core
{
class SensorJammerControl;
} // namespace mpk::dss::core

namespace mpk::dss::backend::tamerlan
{

struct JammerRawData;
class AsyncModbusCommandExecutor;

struct TamerlanBandParams
{
    int number;
    QString name;
};

class TamerlanJammer final : public QObject, public core::JammerProperties
{
    Q_OBJECT

public:
    TamerlanJammer(
        std::shared_ptr<AsyncModbusCommandExecutor> executor,
        uint16_t modbusAddress,
        int updateInterval,
        const std::vector<TamerlanBandParams>& bandParams,
        int distance,
        const QString& model,
        core::SensorJammerControl& sensor,
        const std::optional<GroupId>& groupId,
        QObject* parent = nullptr);
    ~TamerlanJammer() override;

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
    bool setBands(const core::JammerBandOption& bandsActive) override;
    [[nodiscard]] core::JammerBandOption maxBands() override;
    [[nodiscard]] core::JammerMode::Value jammerMode() const override
    {
        return m_sensor.jammerMode();
    };

    [[nodiscard]] std::optional<GroupId> groupId() const override;

    [[nodiscard]] core::DeviceHwInfo hwInfo() const override;
    [[nodiscard]] gsl::not_null<const core::DeviceHwInfoSignals*> hwInfoSignals()
        const override;

    [[nodiscard]] core::DeviceId sensorId() const override;
    void setSensorId(const core::DeviceId& sensorId);

    void changeModbusAddress(uint16_t value);
    void setSuppression(bool value, bool manual);
    void setSuppression(const core::JammerLabel& label, bool value);

private:
    static int channelAddress(int channelIndex);

    void requestMainData();
    void requestData();
    void updateMainData();
    void updateData();
    void setOnline(bool online);
    void setState(core::DeviceState::Value state);

    void runNextCommand();

private:
    std::shared_ptr<AsyncModbusCommandExecutor> m_executor;
    int m_updateInterval;

    core::DeviceId m_sensorId;
    QString m_serial;
    QString m_model;
    bool m_modelPredefined;
    QString m_swVersion;
    core::DevicePosition m_autoPosition;
    core::DevicePosition m_manualPosition;
    core::DevicePositionMode m_positionMode = core::DevicePositionMode::AUTO;
    core::DeviceWorkzone m_workzone;
    bool m_disabled = false;
    bool m_mainDataReceived = false;
    bool m_bandsInitialDataReceived = false;
    core::DeviceState::Value m_state = core::DeviceState::UNAVAILABLE;
    core::DeviceHwInfo m_hwInfo;

    struct BandData
    {
        bool active = false;
        TamerlanBandParams params;
        uint8_t emissionToSet = 0;
    };
    std::vector<BandData> m_bandData;

    std::optional<GroupId> m_groupId;

    core::JammerBandSignals m_bandSignals;
    std::unique_ptr<core::DevicePositionSignals> m_positionSignals;
    std::unique_ptr<core::DeviceWorkzoneSignals> m_workzoneSignals;
    std::unique_ptr<core::DeviceDisabledSignals> m_disabledSignals;
    std::unique_ptr<core::DeviceStateSignals> m_stateSignals;
    std::unique_ptr<core::DeviceHwInfoSignals> m_hwInfoSignals;

    std::unique_ptr<JammerRawData> m_rawData;
    std::unique_ptr<QTimer> m_mainDataUpdateTimer;
    std::unique_ptr<QTimer> m_updateTimer;

    uint8_t m_emissionToSet = 0;

    std::deque<std::unique_ptr<CommandBatch>> m_commandBatches;
    core::SensorJammerControl& m_sensor;
};

} // namespace mpk::dss::backend::tamerlan
