#pragma once

#include <QString>

#include <array>
#include <cstdlib>

namespace mpk
{

namespace compile_time
{

template <char... chars>
struct String
{
    static auto value()
    {
        constexpr char string[sizeof...(chars) + 1] = {chars..., '\0'};
        return QString{string};
    }
};

template <size_t N, typename T, char...>
struct StringBuilderDetails;

template <size_t N, char... chars>
struct StringBuilderDetails<N, String<chars...>>
{
    static constexpr auto make()
    {
        return String<chars...>{};
    };
};

template <size_t N, char... chars, char head, char... tail>
struct StringBuilderDetails<N, String<chars...>, head, tail...>
{
    static constexpr auto make()
    {
        return StringBuilderDetails<N, String<chars..., head>, tail...>::make();
    };
};

template <size_t N, char... chars>
constexpr auto makeString()
{
    return StringBuilderDetails<N, String<>, chars...>::make();
};

template <size_t size>
constexpr size_t stringSize(const char (&)[size])
{
    return size;
}

} // namespace compile_time

} // namespace mpk

#define CLAMP(index, count) (index) >= (count) ? ((count)-1) : (index)
#define STR_TO_CHARS(str) STR_TO_CHARS_(str, mpk::compile_time::stringSize(str))
#define STR_TO_CHARS_(str, N)                                                                 \
    str[CLAMP(0, N)], str[CLAMP(1, N)], str[CLAMP(2, N)], str[CLAMP(3, N)], str[CLAMP(4, N)], \
        str[CLAMP(5, N)], str[CLAMP(6, N)], str[CLAMP(7, N)], str[CLAMP(8, N)],               \
        str[CLAMP(9, N)], str[CLAMP(10, N)], str[CLAMP(11, N)], str[CLAMP(12, N)],            \
        str[CLAMP(13, N)], str[CLAMP(14, N)], str[CLAMP(15, N)], str[CLAMP(16, N)],           \
        str[CLAMP(17, N)], str[CLAMP(18, N)], str[CLAMP(19, N)], str[CLAMP(20, N)]

#define TPL_STR(str)             \
    decltype(mpk::compile_time:: \
                 makeString<mpk::compile_time::stringSize(str), STR_TO_CHARS(str)>())
