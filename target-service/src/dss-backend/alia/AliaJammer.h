#pragma once

#include "dss-core/JammerBand.h"
#include "dss-core/JammerProperties.h"

#include "dss-common/modbus/connectiondetails.h"
#include "dss-common/modbus/modbusclient.h"

#include <QModbusReply>
#include <QObject>
#include <QTimer>

#include <map>
#include <memory>
#include <optional>
#include <vector>

namespace mpk::dss::core
{
class SensorJammerControl;
} // namespace mpk::dss::core

namespace mpk::dss::backend::alia
{

class AliaJammer final : public QObject, public core::JammerProperties
{
    Q_OBJECT
public:
    struct Config
    {
        struct ControllerDetails
        {
            ConnectionDetails connectionParams;
            int deviceId;
            std::map<QString, quint16> coilAddresses;
        };

        std::vector<ControllerDetails> items;
        QString model;
        double distance;
        int updateInterval;
        std::optional<GroupId> groupId = std::nullopt;

        [[nodiscard]] QString connectionString() const;
    };

    struct ErrorInfo
    {
        QString message;
        int code{0};
    };

    explicit AliaJammer(const Config& config, QObject* parent = nullptr);
    ~AliaJammer() override;

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
    void updateState(core::DeviceState::Value state) const;
    [[nodiscard]] gsl::not_null<const core::DeviceStateSignals*> stateSignals()
        const override;

    [[nodiscard]] core::JammerBands bands() override;
    [[nodiscard]] gsl::not_null<const core::JammerBandSignals*> bandSignals()
        const override;
    bool setBands(const core::JammerBandOption& bandsActive) override;
    [[nodiscard]] core::JammerBandOption maxBands() override;
    [[nodiscard]] core::JammerMode::Value jammerMode() const override
    {
        return core::JammerMode::MANUAL;
    };

    [[nodiscard]] std::optional<GroupId> groupId() const override;

    [[nodiscard]] core::DeviceHwInfo hwInfo() const override;
    [[nodiscard]] gsl::not_null<const core::DeviceHwInfoSignals*> hwInfoSignals()
        const override;

    [[nodiscard]] core::DeviceId sensorId() const override;

private slots:
    static void onModbusError(const QString& error, int requestId);
    static void onModbusConnectionError(const QString& error);
    static void onModbusConnecting(const ConnectionDetails& details);
    void onModbusConnected(const ConnectionDetails& details);
    void onModbusDisconnected(const ConnectionDetails& details);
    void onModbusReply(QModbusReply* reply);
    void onSuppressStateChanged(const core::JammerLabel& label, bool active);
    void onOperationError(const core::JammerLabel&, const ErrorInfo&);

private:
    struct BandSuppressionContext
    {
        int deviceId{0};
        quint16 coilAddress{0x0000};
        mpk::dss::ModbusClient* modbusClient{nullptr};
        bool isSuppressionActive{false};
    };

    enum ModbusConnectionValue : bool
    {
        Offline = false,
        Online = true
    };

    struct ModbusRequestContext
    {
        core::JammerLabel band;
        quint16 value;
        QModbusRequest request;
    };
    using ModbusRequestContextsStorage =
        std::unordered_map<int, ModbusRequestContext>;

    using ControllerId = int;
    using ModbusClients =
        std::vector<std::pair<ControllerId, std::unique_ptr<ModbusClient>>>;
    using SuppressionContextsByController =
        std::multimap<ControllerId, BandSuppressionContext*>;

private:
    void createSuppressionContexts(const Config& config);
    void connectSignalsToSlots();
    void connectControllers(const Config& config);
    void initModbusConnectionStatus();
    void initSuppressionContextsByController(const Config& config);
    bool isModbusConnected();
    void readCoils();
    void writeSingleCoil(const QString& band, quint16 value);
    void startSuppressSingleBand(const core::JammerLabel& label);
    void stopSuppressSingleBand(const core::JammerLabel& label);
    void setState(core::DeviceState::Value state);
    void increaseRequestId();
    std::optional<ErrorInfo> validateModbusReply(QModbusReply* reply);
    static std::optional<ErrorInfo> validateReadCoilsReply(QModbusReply* reply);
    std::optional<ErrorInfo> validateWriteSingleCoilReply(QModbusReply* reply);
    void handleWriteSingleCoilReply(QModbusReply* reply);
    void handleReadCoilsReply(QModbusReply* reply);
    std::optional<ControllerId> getControllerId(const QObject* clientPtr);
    std::optional<BandSuppressionContext*> getSuppressionContext(
        ControllerId, int coilAddress);
    void updateBands();

private slots:
    void startSuppression(const mpk::dss::core::JammerLabel& band);
    void stopSuppression(const mpk::dss::core::JammerLabel& band);

private:
    QString m_serial;
    QString m_connectionString;
    QString m_model;
    QString m_swVersion;
    core::DevicePosition m_position;
    core::DeviceWorkzone m_workzone;
    bool m_disabled = false;
    core::DeviceState::Value m_state = core::DeviceState::UNAVAILABLE;
    core::JammerBands m_bands;
    core::DeviceHwInfo m_hwInfo;
    bool m_bandsInitialDataReceived = false;

    std::optional<GroupId> m_groupId;

    std::unique_ptr<QTimer> m_updateTimer;

    ModbusClients m_modbusClients;
    std::map<core::JammerLabel, BandSuppressionContext> m_suppressionContexts;
    SuppressionContextsByController m_suppressionContextsByController;
    std::map<ControllerId, ModbusConnectionValue> m_modbusConnectionStatus;
    ModbusRequestContextsStorage m_modbusRequestContexts;
    int m_requestId{-1};

    core::JammerBandSignals m_bandSignals;
    std::unique_ptr<core::DevicePositionSignals> m_positionSignals;
    std::unique_ptr<core::DeviceWorkzoneSignals> m_workzoneSignals;
    std::unique_ptr<core::DeviceDisabledSignals> m_disabledSignals;
    std::unique_ptr<core::DeviceStateSignals> m_stateSignals;
    std::unique_ptr<core::DeviceHwInfoSignals> m_hwInfoSignals;
};

} // namespace mpk::dss::backend::alia
