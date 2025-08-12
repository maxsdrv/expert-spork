#pragma once

#include "dss-core/DevicePosition.h"
#include "dss-core/DeviceWorkzone.h"
#include "dss-core/SensorProperties.h"
#include "dss-core/TrackId.h"

#include "dss-backend/tamerlan/ChannelStatus.h"
#include "dss-backend/tamerlan/modbus/ChannelData.h"
#include "dss-backend/tamerlan/modbus/CommandBatch.h"
#include "dss-backend/tamerlan/modbus/JammerData.h"
#include "dss-backend/tamerlan/modbus/TransmissionData.h"

#include "dss-common/sensors/RfdTargetAttributes.h"

#include <QObject>

#include <deque>
#include <memory>

class QTimer;

namespace mpk::dss::backend::tamerlan
{

struct SensorRawData;
class TamerlanJammer;
class AsyncModbusCommandExecutor;

class TamerlanSensor final : public QObject, public core::SensorProperties
{
    Q_OBJECT

public:
    TamerlanSensor(
        std::shared_ptr<AsyncModbusCommandExecutor> executor,
        uint16_t modbusAddress,
        int sensorUpdateInterval,
        int objectsUpdateInterval,
        int minDetectionCount,
        const QString& model,
        core::JammerAutoDefenseData autoData,
        gsl::not_null<const core::LicenseController*> licenseController,
        QObject* parent = nullptr);

    ~TamerlanSensor() override;

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

    void addJammer(
        const core::DeviceId& id, gsl::not_null<TamerlanJammer*> jammer);

signals:
    void newObject(core::TrackId id, QJsonObject attributes);
    void objectChanged(
        core::TrackId id,
        QJsonObject rawData,
        QJsonObject attributes,
        qint64 detectCount);
    void objectRemoved(core::TrackId id);

private:
    static int channelAddress(int channelIndex);

    void requestMainData();
    void requestSensorData();
    void requestObjectData();

    void updateMainData();
    void updateSensorData();
    void updateObjectsData();
    void updateObjects(
        int channelIndex, const std::vector<TransmissionData>& transmissions);
    void setOnline(bool online);
    void setState(core::DeviceState::Value state);

    void runNextCommand();

    static sensors::RfdTargetAttributes getTargetAttributes(
        const TransmissionData& transmission);

private:
    struct TrackData
    {
        core::TrackId trackId;
    };
    struct ChannelData
    {
        ChannelStatus status;
        ChannelZone zone;
        ChannelParams params;
        std::vector<TransmissionData> transmissions;
        std::map<int, TrackData> trackIds;
    };

    std::shared_ptr<AsyncModbusCommandExecutor> m_executor;
    int m_sensorUpdateInterval;
    int m_objectsUpdateInterval;
    int m_minDetectionCount;

    QString m_serial;
    QString m_model;
    bool m_modelPredefined;
    QString m_swVersion;
    bool m_disabled = false;
    bool m_mainDataReceived = false;
    core::DevicePosition m_autoPosition;
    core::DevicePosition m_manualPosition;
    core::DevicePositionMode m_positionMode = core::DevicePositionMode::AUTO;
    core::DeviceWorkzone m_workzone;
    core::DeviceState::Value m_state = core::DeviceState::UNAVAILABLE;
    core::DeviceHwInfo m_hwInfo;

    std::vector<ChannelData> m_channels;
    std::chrono::seconds m_jammerTimeout;
    core::JammerMode::Value m_jammerMode = core::JammerMode::UNDEFINED;
    std::array<uint16_t, JammerData::jammersCount> m_jammerAddresses = {};

    std::unique_ptr<core::DevicePositionSignals> m_positionSignals;
    std::unique_ptr<core::DeviceWorkzoneSignals> m_workzoneSignals;
    std::unique_ptr<core::DeviceDisabledSignals> m_disabledSignals;
    std::unique_ptr<core::DeviceStateSignals> m_stateSignals;
    std::unique_ptr<core::DeviceHwInfoSignals> m_hwInfoSignals;

    std::unique_ptr<SensorRawData> m_rawData;
    std::unique_ptr<QTimer> m_mainDataUpdateTimer;
    std::unique_ptr<QTimer> m_sensorDataUpdateTimer;
    std::unique_ptr<QTimer> m_objectsDataUpdateTimer;

    uint16_t m_jammerModeToSet = 0;
    uint16_t m_jammerTimeoutToSet = 0;

    std::vector<std::pair<core::DeviceId, gsl::not_null<TamerlanJammer*>>>
        m_jammers;

    std::deque<std::unique_ptr<CommandBatch>> m_commandBatches;
};

} // namespace mpk::dss::backend::tamerlan
