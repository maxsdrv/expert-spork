#pragma once

#include "types/StrongTypedef.h"

#include "json/FromJson.h"
#include "json/ToJson.h"
#include "json/parsers/QJsonTypes.h"

namespace mpk
{

namespace types
{

/// Adds serialization from/to json
template <typename StrongType>
class SerializableAsJson
{
};

/// Adds serialization from/to json as JsonValue
template <typename StrongType>
class SerializableAsJsonValue : public SerializableAsJson<StrongType>
{
public:
    static StrongType fromJson(QJsonValue const& jsonValue)
    {
        return StrongType{json::fromValue<mpk::types::UnderlyingType<StrongType>>(jsonValue)};
    }

    static QJsonValue toJsonValue(StrongType const& value)
    {
        return json::toValue(weak_cast(value));
    }
};

/// Adds serialization from/to json as JsonObject
template <typename StrongType, typename Tag>
class SerializableAsJsonObject : public SerializableAsJson<StrongType>
{
public:
    static StrongType fromJson(QJsonValue const& jsonValue)
    {
        auto object = json::fromValue<QJsonObject>(jsonValue);
        return StrongType{
            json::fromObject<mpk::types::UnderlyingType<StrongType>>(object, Tag::value())};
    }

    static QJsonValue toJsonValue(StrongType const& value)
    {
        return QJsonObject{{Tag::value(), json::toValue(weak_cast(value))}};
    }
};

} // namespace types

} // namespace mpk
