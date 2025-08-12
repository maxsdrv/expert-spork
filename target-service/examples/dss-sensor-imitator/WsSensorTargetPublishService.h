#pragma once

#include "DiagnosticDataGenerator.h"
#include "httpws/ConnectionSettings.h"

#include "gsl/pointers"

#include <chrono>
#include <memory>

class QTimer;

namespace http
{

class WebSocketServer;

} // namespace http

namespace mpk::dss
{

class SensorTargetDataGenerator;

class WsSensorTargetPublishService
{
public:
    WsSensorTargetPublishService(
        const http::ConnectionSettings& wsConnectionSettings,
        gsl::not_null<SensorTargetDataGenerator*> sensorTargetGenerator,
        gsl::not_null<DiagnosticDataGenerator*> diagnosticDataGenerator,
        std::chrono::milliseconds publishPeriod);
    ~WsSensorTargetPublishService();

private:
    void sendTargetData() const;
    void sendDiagnosticData() const;

private:
    std::unique_ptr<http::WebSocketServer> m_webSocketServer;
    gsl::not_null<SensorTargetDataGenerator*> m_sensorTargetGenerator;
    gsl::not_null<DiagnosticDataGenerator*> m_diagnosticDataGenerator;
    std::unique_ptr<QTimer> m_timer;
};

} // namespace mpk::dss
