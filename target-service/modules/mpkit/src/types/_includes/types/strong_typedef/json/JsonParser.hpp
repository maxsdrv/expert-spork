#pragma once

#include "Traits.hpp"

#include "json/FromJson.h"

/// Converts json value to serializable StrongTypedef
template <typename StrongType>
struct FromJsonConverter<
    StrongType,
    typename std::enable_if_t<
        std::is_base_of_v<mpk::types::SerializableAsJson<StrongType>, StrongType>,
        StrongType>>
{
    static StrongType get(const QJsonValue& jsonValue)
    {
        return StrongType::fromJson(jsonValue);
    }
};
