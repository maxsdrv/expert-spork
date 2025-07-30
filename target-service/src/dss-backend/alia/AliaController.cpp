#include "AliaController.h"

#include "dss-backend/BackendComponents.h"
#include "dss-backend/BackendSignals.h"
#include "dss-backend/alia/AliaJammer.h"

#include "dss-common/config/ConnectionSettings.h"
#include "dss-common/config/Option.h"
#include "dss-core/YamlConvertHelper.h"

#include "yaml/yamlConverter.h"

#include "exceptions/InvalidConfig.h"

#include <QString>
#include <QUuid>

namespace mpk::dss::backend::alia
{

namespace
{

constexpr auto modelTag = "model";
constexpr auto distanceTag = "distance";
constexpr auto controllersTag = "controllers";
constexpr auto connTag = "moxa";
constexpr auto deviceIdTag = "device_id";
constexpr auto coilsTag = "coils";
constexpr auto coilTag = "coil";
constexpr auto signTag = "sign";
constexpr auto addrTag = "address";
constexpr auto updateIntervalTag = "update_interval";
constexpr auto groupIdTag = "group_id";

} // namespace

BackendControllerPtr AliaController::fromDescription(
    const YAML::Node& description, const BackendComponents& components)
{
    // TODO: Single jammer only
    AliaJammer::Config aliaConfig;

    auto modelNode = description[modelTag];
    if (modelNode.IsDefined() && !modelNode.IsNull())
    {
        aliaConfig.model = QString::fromStdString(modelNode.as<std::string>());
    }
    aliaConfig.distance =
        core::convertYAML<config::Option<double>>(description[distanceTag])
            .value;
    aliaConfig.updateInterval =
        core::convertYAML<config::Option<int>>(description[updateIntervalTag])
            .value;
    auto groupIdNode = description[groupIdTag];
    if (groupIdNode.IsDefined() && !groupIdNode.IsNull())
    {
        aliaConfig.groupId =
            QString::fromStdString(groupIdNode.as<std::string>());
        if (!components.jammerGroupProvider->contains(*aliaConfig.groupId))
        {
            BOOST_THROW_EXCEPTION(
                exception::InvalidConfig() << ::exception::ExceptionInfo(
                    std::string("Invalid jammer group id: ")
                    + aliaConfig.groupId->toStdString()));
        }
    }
    auto controllersNode = description[controllersTag];
    if (!controllersNode.IsNull() && controllersNode.size() != 0)
    {
        auto controllersNodes = controllersNode.as<std::vector<YAML::Node>>();
        aliaConfig.items.resize(controllersNodes.size());
        auto controllerParamsIt = aliaConfig.items.begin();

        for (const auto& controllerNode: controllersNodes)
        {
            auto tcpParams = core::convertYAML<config::ConnectionSettings>(
                controllerNode[connTag]);
            controllerParamsIt->connectionParams.TcpParams = {
                .ServicePort = static_cast<quint16>(tcpParams.port),
                .IPAddress = tcpParams.host};
            controllerParamsIt->deviceId =
                core::convertYAML<config::Option<int>>(
                    controllerNode[deviceIdTag])
                    .value;

            auto coilsNodes = controllerNode[coilsTag];
            if (!coilsNodes.IsNull() && coilsNodes.size() != 0)
            {
                for (const auto& coilNode: coilsNodes)
                {
                    auto coil = coilNode[coilTag];
                    auto sign = core::convertYAML<config::Option<std::string>>(
                        coil[signTag]);
                    auto address =
                        core::convertYAML<config::Option<std::string>>(
                            coil[addrTag]);

                    const int hexBase = 16;
                    controllerParamsIt->coilAddresses.insert(std::make_pair(
                        QString::fromStdString(sign.value),
                        QString::fromStdString(address.value)
                            .toUShort(nullptr, hexBase)));
                }
            }
            controllerParamsIt++;
        }
    }

    auto jammer = std::make_unique<AliaJammer>(aliaConfig);
    auto controller =
        std::unique_ptr<AliaController>(new AliaController{components});
    controller->appendJammer(std::move(jammer));

    return controller;
}

AliaController::AliaController(const BackendComponents& components) :
  BackendControllerBase(std::make_unique<BackendSignals>(), components)
{
}

} // namespace mpk::dss::backend::alia
