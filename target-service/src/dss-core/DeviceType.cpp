#include "DeviceType.h"

#include "qttypes/MetaEnum.h"

namespace mpk::dss::core
{

auto DeviceType::fromString(const QString& string) -> Value
{
    return qt::metaEnumFromString<Value>(string);
}

QString DeviceType::toString(Value state)
{
    return qt::metaEnumToString(state);
}

} // namespace mpk::dss::core
