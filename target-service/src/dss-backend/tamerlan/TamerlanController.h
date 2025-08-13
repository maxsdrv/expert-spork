#pragma once

#include "dss-backend/BackendControllerBase.h"

#include <yaml-cpp/yaml.h>

namespace mpk::dss::backend::tamerlan
{

constexpr auto nodeKey = "tamerlan";

class TamerlanSensor;
class TamerlanJammer;

class TamerlanController : public BackendControllerBase
{

public:
    static BackendControllerPtr fromDescription(
        const YAML::Node& description, const BackendComponents& components);

private:
    struct SensorJammersConnection
    {
        gsl::not_null<TamerlanSensor*> sensor;
        std::vector<gsl::not_null<TamerlanJammer*>> jammers;
    };

    explicit TamerlanController(const BackendComponents& components);

    void setSensorsJammersConnection(
        const std::vector<SensorJammersConnection>& sensorsJammersConnection);

    void connectSensor(const core::DeviceId& deviceId);
    void connectJammer(const core::DeviceId& deviceId);

private:
    std::vector<SensorJammersConnection> m_sensorsJammersConnection;
};

} // namespace mpk::dss::backend::tamerlan
