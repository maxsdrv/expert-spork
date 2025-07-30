#pragma once

#include "json/parsers/General.h"
#include "json/parsers/QJsonTypes.h"

#include <QHostAddress>

namespace rpc
{

struct SocketAddress
{
    QHostAddress host;
    quint16 port;
};

inline bool operator<(const SocketAddress& left, const SocketAddress& right)
{
    return left.host.toString() < right.host.toString() && left.port < right.port;
}

} // namespace rpc

template <>
struct FromJsonConverter<QHostAddress>
{
    static QHostAddress get(const QJsonValue& value)
    {
        auto hostString = json::fromValue<QString>(value);
        if (hostString == "localhost")
        {
            return QHostAddress::LocalHost;
        }

        if (hostString == "any")
        {
            return QHostAddress::Any;
        }

        return QHostAddress(hostString);
    }
};

template <>
struct FromJsonConverter<rpc::SocketAddress>
{
    static rpc::SocketAddress get(const QJsonValue& value)
    {
        constexpr auto hostTag = "host";
        constexpr auto portTag = "port";

        auto object = json::fromValue<QJsonObject>(value);
        return {
            json::fromObject<QHostAddress>(object, hostTag),
            json::fromObject<quint16>(object, portTag)};
    }
};
