#pragma once

#include <concepts>
#include <cstdint>
#include <iterator>
#include <utility>

namespace mpk::dss::backend::tamerlan
{

constexpr auto eightBit = 8U;
constexpr auto eightBitMask = 0xFFU;
constexpr auto sixteenBit = 16U;
constexpr auto sixteenBitMask = 0xFFFFU;

// clang-format off
template <typename Iterator>
concept U16Iterator = (
    std::same_as<typename Iterator::value_type, std::uint16_t>
 || std::same_as<Iterator, std::uint16_t*>);

template <typename Iterator>
concept U16BackInsertIterator = (
    std::same_as<Iterator, std::back_insert_iterator<typename Iterator::container_type>>
 && std::same_as<typename Iterator::container_type::value_type, std::uint16_t>
);

template <typename T>
concept UnsignedIntegerSameSizeAsFloat = (
    std::unsigned_integral<T> && sizeof(T) == sizeof(float));
// clang-format on

union FloatUInt32
{
    float f32;
    std::uint32_t ui32;
};

// NOLINTBEGIN (readability-identifier-naming,
// cppcoreguidelines-pro-type-union-access)
template <typename T>
requires UnsignedIntegerSameSizeAsFloat<T> T binary_cast(float value)
{
    return FloatUInt32{.f32 = value}.ui32;
}

template <typename T>
requires UnsignedIntegerSameSizeAsFloat<T>
float binary_cast(T value)
{
    return FloatUInt32{.ui32 = value}.f32;
}
// NOLINTEND (readability-identifier-naming,
// cppcoreguidelines-pro-type-union-access)

constexpr auto maxModbusWriteBytes = 246;

static U16BackInsertIterator auto int32ToRegs(
    int32_t value, U16BackInsertIterator auto it)
{
    *it = static_cast<unsigned int>(value) & sixteenBitMask;
    *it = static_cast<unsigned int>(value) >> sixteenBit;
    return it;
}

template <typename Iterator>
requires U16Iterator<Iterator>
static std::pair<int32_t, Iterator> regsToInt32(Iterator it)
{
    auto value = static_cast<unsigned int>(*(it++));
    value |= static_cast<unsigned int>(*(it++)) << sixteenBit;
    return std::make_pair(static_cast<int32_t>(value), it);
}

static U16BackInsertIterator auto uint32ToRegs(
    uint32_t value, U16BackInsertIterator auto it)
{
    *it = static_cast<unsigned int>(value) & sixteenBitMask;
    *it = static_cast<unsigned int>(value) >> sixteenBit;
    return it;
}

template <typename Iterator>
requires U16Iterator<Iterator>
static std::pair<uint32_t, Iterator> regsToUint32(Iterator it)
{
    auto value = static_cast<unsigned int>(*(it++));
    value |= static_cast<unsigned int>(*(it++)) << sixteenBit;
    return std::make_pair(value, it);
}

template <typename Iterator>
requires U16Iterator<Iterator>
static std::pair<int64_t, Iterator> regsToInt64(Iterator it)
{
    auto value = static_cast<uint64_t>(*(it++));
    value |= static_cast<uint64_t>(*(it++)) << sixteenBit;
    value |= static_cast<uint64_t>(*(it++)) << (2 * sixteenBit);
    value |= static_cast<uint64_t>(*(it++)) << (3 * sixteenBit);
    return std::make_pair(static_cast<int64_t>(value), it);
}

static U16BackInsertIterator auto int64ToRegs(
    int64_t value, U16BackInsertIterator auto it)
{
    *it = static_cast<uint64_t>(value) & sixteenBitMask;
    *it = (static_cast<uint64_t>(value) >> sixteenBit) & sixteenBitMask;
    *it = (static_cast<uint64_t>(value) >> (2 * sixteenBit)) & sixteenBitMask;
    *it = static_cast<uint64_t>(value) >> (3 * sixteenBit);
    return it;
}

template <typename Iterator>
requires U16Iterator<Iterator>
static std::pair<uint64_t, Iterator> regsToUint64(Iterator it)
{
    auto value = static_cast<uint64_t>(*(it++));
    value |= static_cast<uint64_t>(*(it++)) << sixteenBit;
    value |= static_cast<uint64_t>(*(it++)) << (2 * sixteenBit);
    value |= static_cast<uint64_t>(*(it++)) << (3 * sixteenBit);
    return std::make_pair(static_cast<uint64_t>(value), it);
}

static U16BackInsertIterator auto uint64ToRegs(
    uint64_t value, U16BackInsertIterator auto it)
{
    *it = value & sixteenBitMask;
    *it = (value >> sixteenBit) & sixteenBitMask;
    *it = (value >> (2 * sixteenBit)) & sixteenBitMask;
    *it = value >> (3 * sixteenBit);
    return it;
}

static U16BackInsertIterator auto floatToRegs(
    float value, U16BackInsertIterator auto it)
{
    auto u32value = binary_cast<unsigned int>(value);
    *it = u32value & sixteenBitMask;
    *it = u32value >> sixteenBit;
    return it;
}

template <typename Iterator>
requires U16Iterator<Iterator>
static std::pair<float, Iterator> regsToFloat(Iterator it)
{
    auto value = static_cast<unsigned int>(*(it++));
    value |= static_cast<unsigned int>(*(it++)) << sixteenBit;
    return std::make_pair(binary_cast(value), it);
}

static U16BackInsertIterator auto charArrayToRegs(
    const char* data, int size, U16BackInsertIterator auto it)
{
    auto odd = false;
    auto reg = 0U;
    for (int i = 0; i < size; i++)
    {
        // NOLINTBEGIN (cppcoreguidelines-pro-bounds-pointer-arithmetic)
        if (!odd)
        {
            reg =
                static_cast<unsigned int>(static_cast<unsigned char>(data[i]));
        }
        else
        {
            reg |=
                static_cast<unsigned int>(static_cast<unsigned char>(data[i]))
                << eightBit;
            *it = static_cast<std::uint16_t>(reg);
        }
        // NOLINTEND (cppcoreguidelines-pro-bounds-pointer-arithmetic)
        odd = !odd;
    }
    return it;
}

static U16Iterator auto regsToCharArray(
    char* data, int size, U16Iterator auto it)
{
    for (int i = 0; i < size - 1; i += 2)
    {
        // NOLINTBEGIN (cppcoreguidelines-pro-bounds-pointer-arithmetic)
        data[i] = *it & eightBitMask;
        data[i + 1] = *it >> eightBit;
        // NOLINTEND (cppcoreguidelines-pro-bounds-pointer-arithmetic)
        it++;
    }
    return it;
}

} // namespace mpk::dss::backend::tamerlan
