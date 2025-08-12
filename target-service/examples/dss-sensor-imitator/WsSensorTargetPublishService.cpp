#include "WsSensorTargetPublishService.h"

#include "DiagnosticDataGenerator.h"
#include "SensorTargetDataGenerator.h"

#include "dss-backend/rest/json/DiagnosticDataVsJson.h"
#include "dss-backend/rest/json/SensorTargetDataVsJson.h"

#include "httpws/ws/WebSocketClient.h"
#include "httpws/ws/WebSocketServer.h"

#include <QJsonObject>
#include <QWebSocket>
#include <QWebSocketServer>

#include <QObject>
#include <QTimer>

#include <memory>

using namespace http;

namespace mpk::dss
{

WsSensorTargetPublishService::WsSensorTargetPublishService(
    const http::ConnectionSettings& wsConnectionSettings,
    gsl::not_null<SensorTargetDataGenerator*> sensorTargetGenerator,
    gsl::not_null<DiagnosticDataGenerator*> diagnosticDataGenerator,
    std::chrono::milliseconds publishPeriod) :
  m_webSocketServer(
      std::make_unique<http::WebSocketServer>(wsConnectionSettings)),
  m_sensorTargetGenerator(sensorTargetGenerator),
  m_diagnosticDataGenerator(diagnosticDataGenerator),
  m_timer(std::make_unique<QTimer>())
{
    m_webSocketServer->listen();

    QObject::connect(
        m_timer.get(),
        &QTimer::timeout,
        m_timer.get(),
        [this]() { sendTargetData(); });
    QObject::connect(
        m_timer.get(),
        &QTimer::timeout,
        m_timer.get(),
        [this]() { sendDiagnosticData(); });

    m_timer->setInterval(publishPeriod);
    m_timer->start();
}

WsSensorTargetPublishService::~WsSensorTargetPublishService()
{
    m_webSocketServer->deleteLater();
}

void WsSensorTargetPublishService::sendTargetData() const
{
    auto targetData = m_sensorTargetGenerator->generate();
    m_webSocketServer->broadcast(json::toValue(targetData).toObject());
}

void WsSensorTargetPublishService::sendDiagnosticData() const
{
    auto diagnosticData = m_diagnosticDataGenerator->generate();
    m_webSocketServer->broadcast(json::toValue(diagnosticData).toObject());
}

} // namespace mpk::dss
