#pragma once

#include "ModbusServerType.h"

#include <QtSerialBus/QModbusServer>

#include "gsl/pointers"

#include <yaml-cpp/yaml.h>

#include <memory>

namespace mpk::drone::core
{

class ModbusServerController
{
public:
    explicit ModbusServerController(const YAML::Node& description);

    void start();
    void stop();
    void setPort(int port);

    [[nodiscard]] gsl::not_null<QModbusServer*> server() const;

private:
    using Creator =
        std::function<std::unique_ptr<QModbusServer>(const YAML::Node&)>;
    std::map<ModbusServerType, Creator> m_creators;

    ModbusServerType m_type = ModbusServerType::TCP;
    std::unique_ptr<QModbusServer> m_server;
};

} // namespace mpk::drone::core
