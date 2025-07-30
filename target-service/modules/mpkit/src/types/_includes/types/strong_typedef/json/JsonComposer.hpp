#pragma once

#include "Traits.hpp"

#include "json/ToJson.h"

template <typename StrongType>
struct ToJsonConverter<
    StrongType,
    typename std::enable_if_t<
        std::is_base_of_v<mpk::types::SerializableAsJson<StrongType>, StrongType>,
        StrongType>>
{
    static QJsonValue convert(StrongType const& value)
    {
        return StrongType::toJsonValue(value);
    }
};
