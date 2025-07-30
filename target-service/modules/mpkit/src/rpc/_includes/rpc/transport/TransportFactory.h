#pragma once

#include "rpc/transport/Transport.h"

#include <QString>

#include <functional>
#include <map>
#include <memory>

class QJsonObject;

namespace rpc
{

class TransportFactory
{
public:
    TransportFactory();
    std::unique_ptr<Transport> create(const QJsonObject& description);

private:
    using Creator = std::function<std::unique_ptr<Transport>(const QJsonObject&)>;
    std::map<QString, Creator> m_creators;
};

} // namespace rpc
