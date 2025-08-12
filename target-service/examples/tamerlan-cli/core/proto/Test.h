#pragma once

#include "Common.h"

#include "gsl/assert"

#include <array>
#include <span>

namespace mpk::drone::core
{

struct TestData
{
    std::int32_t integer = 0;
    static constexpr auto stringSize = 16;
    std::array<char, stringSize> string = {};

    template <typename BackInsertIterator>
    static void toRegs(const TestData& data, BackInsertIterator begin)
    {
        static_assert(std::is_same_v<
                      typename BackInsertIterator::container_type::value_type,
                      std::uint16_t>);

        auto it = begin;
        *it = static_cast<unsigned int>(data.integer) & sixteenBitMask;
        *it = static_cast<unsigned int>(data.integer) >> sixteenBit;

        // TODO: works properly only on even number of chars
        auto odd = false;
        auto reg = 0U;
        for (const auto& ch: data.string)
        {
            if (!odd)
            {
                reg = static_cast<unsigned int>(static_cast<unsigned char>(ch));
            }
            else
            {
                reg |= static_cast<unsigned int>(static_cast<unsigned char>(ch))
                       << eightBit;
                *it = static_cast<std::uint16_t>(reg);
            }
            odd = !odd;
        }
    }

    template <typename ConstIterator>
    static TestData fromRegs(ConstIterator begin, ConstIterator end)
    {
        static_assert(
            std::is_same_v<typename ConstIterator::value_type, std::uint16_t>);

        Expects(std::distance(begin, end) >= 2);

        TestData data;

        auto it = begin;
        auto integer = static_cast<unsigned int>(*(it++));
        integer |= static_cast<unsigned int>(*(it++)) << sixteenBit;
        data.integer = static_cast<std::int32_t>(integer);

        auto i = 0;
        auto string = std::span<char>(data.string);
        while (it != end)
        {
            string[i++] += *it & eightBitMask;
            string[i++] += *it >> eightBit;
            it++;
        }
        return data;
    }
};

} // namespace mpk::drone::core
