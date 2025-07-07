#include "DeviceClass.h"

#include "qttypes/MetaEnum.h"

namespace mpk::dss::core
{

auto DeviceClass::fromString(const QString& string) -> Value
{
    return qt::metaEnumFromString<Value>(string);
}

QString DeviceClass::toString(Value state)
{
    return qt::metaEnumToString(state);
}

} // namespace mpk::dss::core
