#include "DeviceIdMapping.h"

#include "gsl/assert"

namespace mpk::dss::core
{

constexpr auto separator = ";";

QString DeviceIdMappingKey::toString(const DeviceIdMappingKey& key)
{
    return QString("%1%2%3").arg(
        key.fingerprint, separator, DeviceClass::toString(key.type));
}

DeviceIdMappingKey DeviceIdMappingKey::fromString(const QString& str)
{
    auto strParts = str.split(separator);
    Ensures(strParts.size() == 2);
    return DeviceIdMappingKey{
        .fingerprint = strParts.at(0),
        .type = DeviceClass::fromString(strParts.at(1))};
}

bool operator==(const DeviceIdMapping& left, const DeviceIdMapping& right)
{
    return std::tie(left.key, left.id) == std::tie(right.key, right.id);
}

bool operator!=(const DeviceIdMapping& left, const DeviceIdMapping& right)
{
    return !(left == right);
}

} // namespace mpk::dss::core
