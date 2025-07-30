#include "NetworkDataModbus.h"

#include "core/modbus/ModbusData.h"
#include "core/proto/NetworkFlags.h"
#include "core/proto/Protocol.h"

#ifdef OS_UNIX
#include <arpa/inet.h>
#include <netinet/in.h>
#endif

#ifdef _WIN32
#include <winsock2.h>
#endif

namespace mpk::drone::core
{

NetworkData convert(const NetworkDataModbus& data)
{
    return {
        QString(inet_ntoa({data.networkAddress})),
        QString(inet_ntoa({data.netmask})),
        QString(inet_ntoa({data.gateway})),
        QString(inet_ntoa({data.dns})),
        static_cast<bool>(data.flags & useDhcpFlag),
        static_cast<bool>(data.flags & useDnsFromDhcpFlag)};
}

NetworkDataModbus convert(const NetworkData& data)
{
    uint16_t flags = 0U;
    if (data.useDhcp)
    {
        flags |= useDhcpFlag;
    }
    if (data.useDnsFromDhcp)
    {
        flags |= useDnsFromDhcpFlag;
    }
    return {
        inet_addr(data.host.toStdString().c_str()),
        inet_addr(data.netmask.toStdString().c_str()),
        inet_addr(data.gateway.toStdString().c_str()),
        inet_addr(data.dns.toStdString().c_str()),
        flags};
}

void connectNetworkDataWithModbusData(
    gsl::not_null<ModbusData*> modbusData,
    gsl::not_null<NetworkDataHolder*> holder)
{
    modbusData->emplace<NetworkDataModbus>(
        proto::NetworkData::id,
        proto::NetworkData::address,
        proto::NetworkData::type,
        convert(holder->data()));
    modbusData->emplace<uint16_t>(
        proto::ServicePort::id,
        proto::ServicePort::address,
        proto::ServicePort::type,
        static_cast<uint16_t>(holder->port()));
    QObject::connect(
        modbusData,
        &ModbusData::modified,
        holder,
        [modbusData, holder](const auto& id)
        {
            if (id == proto::NetworkData::id)
            {
                NetworkDataModbus networkDataModbus;
                modbusData->read(
                    proto::NetworkData::id,
                    networkDataModbus,
                    proto::NetworkData::type);
                holder->setData(convert(networkDataModbus));
            }
            if (id == proto::ServicePort::id)
            {
                uint16_t port = 0U;
                modbusData->read(
                    proto::ServicePort::id, port, proto::ServicePort::type);
                holder->setPort(port);
            }
        });
    QObject::connect(
        holder,
        &NetworkDataHolder::dataChanged,
        modbusData,
        [modbusData](const auto& data)
        {
            modbusData->write(
                proto::NetworkData::id,
                convert(data),
                proto::NetworkData::type);
        });
    QObject::connect(
        holder,
        &NetworkDataHolder::portChanged,
        modbusData,
        [modbusData](int port)
        {
            modbusData->write(
                proto::ServicePort::id,
                static_cast<uint16_t>(port),
                proto::ServicePort::type);
        });
}

} // namespace mpk::drone::core
