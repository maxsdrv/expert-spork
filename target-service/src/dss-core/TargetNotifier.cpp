#include "TargetNotifier.h"

#include "TargetController.h"

#include "json/TargetDataVsJson.h"
#include "json/ToJson.h"

#include "log/Log2.h"

namespace mpk::dss::core
{

namespace
{

constexpr auto targetIdTag = "target_id";

constexpr auto targetDetectedMsg = "target_detected";
constexpr auto targetUpdatedMsg = "target_data_updated";
constexpr auto targetLostMsg = "target_lost";

} // namespace

TargetNotifier::TargetNotifier(
    gsl::not_null<TargetController*> controller, QObject* parent) :
  QObject(parent), m_controller(controller)
{
    connect(
        m_controller,
        &TargetController::detected,
        this,
        [this](const auto& targetId)
        {
            auto data = QJsonObject{};
            data[targetIdTag] = types::weak_cast(targetId);
            emit notify(targetDetectedMsg, data);
        });

    connect(
        m_controller,
        &TargetController::updated,
        this,
        [this](const auto& targetId)
        {
            auto it = m_controller->find(targetId);
            if (it != m_controller->end())
            {
                emit notify(targetUpdatedMsg, json::toValue(*it).toObject());
            }
            else
            {
                LOG_WARNING << "Target notifier: missing target";
            }
        });

    connect(
        m_controller,
        &TargetController::lost,
        this,
        [this](const auto& targetId)
        {
            auto data = QJsonObject{};
            data[targetIdTag] = types::weak_cast(targetId);
            emit notify(targetLostMsg, data);
        });
}

} // namespace mpk::dss::core
