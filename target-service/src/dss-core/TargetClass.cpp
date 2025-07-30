#include "TargetClass.h"

#include "qt/MetaEnum.h"

namespace mpk::dss::core
{

auto TargetClass::fromString(const QString& string) -> Value
{
    return qt::metaEnumFromString<TargetClass::Value>(string);
}

QString TargetClass::toString(Value targetClass)
{
    return qt::metaEnumToString(targetClass);
}

} // namespace mpk::dss::core
