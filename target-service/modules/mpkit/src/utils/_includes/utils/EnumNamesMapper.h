#pragma once

#include "exception/InvalidValue.h"

#include "gsl/assert"

namespace enums
{

template <typename C>
typename C::key_type toString(const C& mapping, typename C::mapped_type value)
{
    auto it = std::find_if(
        mapping.begin(),
        mapping.end(),
        [value](const auto& pair) { return pair.second == value; });
    Expects(it != mapping.end());

    return it->first;
}

template <typename C>
typename C::mapped_type fromString(const C& mapping, typename C::key_type key)
{
    auto it = mapping.find(key);
    if (it == mapping.end())
    {
        BOOST_THROW_EXCEPTION(exception::InvalidValue{});
    }

    return it->second;
}

} // namespace enums
