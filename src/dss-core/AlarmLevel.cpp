#include "AlarmLevel.h"

#include "qttypes/MetaEnum.h"

namespace mpk::dss::core
{

auto AlarmLevel::fromString(const QString& string) -> Value
{
    return qt::metaEnumFromString<Value>(string);
}

QString AlarmLevel::toString(Value state)
{
    return qt::metaEnumToString(state);
}

} // namespace mpk::dss::core
