#include "DeviceState.h"

#include "qttypes/MetaEnum.h"

namespace mpk::dss::core
{

auto DeviceState::fromString(const QString& string) -> Value
{
    return qt::metaEnumFromString<Value>(string);
}

QString DeviceState::toString(Value state)
{
    return qt::metaEnumToString(state);
}

} // namespace mpk::dss::core
