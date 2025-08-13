#include "JammerMode.h"

#include "qttypes/MetaEnum.h"

namespace mpk::dss::core
{

auto JammerMode::fromString(const QString& string) -> Value
{
    return qt::metaEnumFromString<Value>(string);
}

QString JammerMode::toString(Value mode)
{
    return qt::metaEnumToString(mode);
}

} // namespace mpk::dss::core
