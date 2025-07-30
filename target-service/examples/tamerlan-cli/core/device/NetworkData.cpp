#include "NetworkData.h"

namespace mpk::drone::core
{

bool operator==(const NetworkData& left, const NetworkData& right)
{
    return std::tie(
               left.host,
               left.netmask,
               left.gateway,
               left.dns,
               left.useDhcp,
               left.useDnsFromDhcp)
           == std::tie(
               right.host,
               right.netmask,
               right.gateway,
               right.dns,
               right.useDhcp,
               right.useDnsFromDhcp);
}
bool operator!=(const NetworkData& left, const NetworkData& right)
{
    return !(left == right);
}

NetworkDataHolder::NetworkDataHolder(
    NetworkData data, int port, QObject* parent) :
  QObject{parent}, m_data{std::move(data)}, m_port{port}
{
}

void NetworkDataHolder::setData(const NetworkData& data)
{
    if (data != m_data)
    {
        m_data = data;
        emit dataChanged(m_data);
    }
}

NetworkData NetworkDataHolder::data() const
{
    return m_data;
}

void NetworkDataHolder::setPort(int port)
{
    if (port != m_port)
    {
        m_port = port;
        emit portChanged(m_port);
    }
}

int NetworkDataHolder::port() const
{
    return m_port;
}

} // namespace mpk::drone::core
