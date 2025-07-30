#pragma once

#include "Common.h"

#include "gsl/assert"

#include <array>
#include <span>
#include <tuple>

namespace mpk::dss::backend::tamerlan
{

#pragma pack(push, 2)

struct JammerData
{
    static constexpr auto jammersCount = 16;
    std::array<uint16_t, jammersCount> addresses = {};
    uint16_t mode = 0;
    uint16_t timeout = 0;

    static void toRegs(const JammerData& data, U16BackInsertIterator auto begin)
    {
        auto it = begin;
        for (auto i = 0; i < jammersCount; i++)
        {
            *it = data.addresses.at(i);
        }
        *it = data.mode;
        *it = data.timeout;
    }

    template <typename ConstIterator>
    static JammerData fromRegs(ConstIterator begin, ConstIterator end)
    {
        Expects(std::distance(begin, end) == sizeof(JammerData) / 2);

        JammerData result;
        auto it = begin;
        for (auto i = 0; i < jammersCount; i++)
        {
            // NOLINTBEGIN
            // (cppcoreguidelines-pro-bounds-constant-array-index)
            result.addresses[i] = *(it++);
            // NOLINTEND
            // (cppcoreguidelines-pro-bounds-constant-array-index)
        }
        result.mode = *(it++);
        result.timeout = *(it++);
        return result;
    }
};

#pragma pack(pop)

// NOLINTBEGIN (cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
static_assert(
    sizeof(JammerData) == 36,
    "Size of JammerData is different than expected size");
// NOLINTEND (cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

} // namespace mpk::dss::backend::tamerlan
