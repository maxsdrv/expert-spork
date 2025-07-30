#pragma once

#include "gqtest"

#include "json/FromJson.h"
#include "json/ToJson.h"

namespace json::tests
{

template <typename T>
void testSerialization(T const& expected)
{
    const auto json = json::toValue(expected);
    auto actual = json::fromValue<T>(json);
    EXPECT_EQ(expected, actual);
}

} // namespace json::tests
