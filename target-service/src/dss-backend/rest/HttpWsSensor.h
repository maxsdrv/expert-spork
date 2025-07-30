#pragma once

#include "dss-backend/rest/channel/ChannelInfo.h"
#include "dss-backend/rest/diagnostic/DeviceDiagnosticData.h"
#include "dss-core/SensorProperties.h"

#include <QTimer>

#include <chrono>

namespace mpk::rest
{

class Client;
class HttpWsConnection;

} // namespace mpk::rest

namespace mpk::dss::backend::rest
{

class HttpWsSensor final : public QObject, public core::SensorProperties
{
    Q_OBJECT
public:
    HttpWsSensor(
        std::unique_ptr<mpk::rest::HttpWsConnection> connection,
        const std::chrono::milliseconds& updateIntervalSettings,
        const std::chrono::milliseconds& timeoutSettings,
        QObject* parent = nullptr);
    ~HttpWsSensor() override;

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

    [[nodiscard]] gsl::not_null<mpk::rest::HttpWsConnection*> connection() const;

    [[nodiscard]] std::vector<core::DeviceId> jammerIds() const override
    {
        return m_jammerIds;
    }

signals:
    void toggleWsConnection(bool enabled);
    void updateTimerDisabled();

private slots:
    void restartTimer();
    void updateData();

private:
    void connectSignalsToSlots();
    bool isConnectionOk();
    [[nodiscard]] bool isDeviceConditionOk() const;

    void handleWsMessage(
        const core::DeviceId& deviceId, const QJsonObject& json);

    void requestChannelInfo();
    void requestDiagnosticData();
    void requestPosition();
    void requestWorkzone();

    void updateJammerIds(const ChannelInfo&);
    void updateHwInfo(double temp, double voltage, double current);
    void updateState();
    void updateWorkzone(const ChannelInfo&);

private:
    std::unique_ptr<mpk::rest::HttpWsConnection> m_connection;
    std::unique_ptr<mpk::rest::Client> m_client;
    QTimer m_updateTimer;
    bool m_updateTimeout{false};
    bool m_httpError{false};
    std::chrono::milliseconds m_updateInterval;
    bool m_deviceConditionOk{true};

    QString m_serial;
    QString m_model;
    QString m_swVersion;
    bool m_disabled = false;
    core::DevicePosition m_autoPosition;
    core::DevicePosition m_manualPosition;
    core::DevicePositionMode m_positionMode = core::DevicePositionMode::AUTO;
    core::DeviceWorkzone m_workzone;
    core::DeviceState::Value m_state = core::DeviceState::UNAVAILABLE;
    core::DeviceHwInfo m_hwInfo;
    core::JammerMode::Value m_jammerMode = core::JammerMode::UNDEFINED;
    std::vector<core::DeviceId> m_jammerIds;

    std::unique_ptr<core::DevicePositionSignals> m_positionSignals;
    std::unique_ptr<core::DeviceWorkzoneSignals> m_workzoneSignals;
    std::unique_ptr<core::DeviceDisabledSignals> m_disabledSignals;
    std::unique_ptr<core::DeviceStateSignals> m_stateSignals;
    std::unique_ptr<core::SensorJammerSignals> m_jammerSignals;
    std::unique_ptr<core::DeviceHwInfoSignals> m_hwInfoSignals;
};

} // namespace mpk::dss::backend::rest
