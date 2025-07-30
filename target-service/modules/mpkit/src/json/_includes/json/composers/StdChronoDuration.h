#pragma once

// Add 'add_mpkit_item(date)' in project CMakeLists.txt to enable std::chrono::duration conversion

#if __cpp_lib_format >= 201907L || __has_include("date/date.h")

#include "json/ToJson.h"

#include <chrono>
#if __cpp_lib_format >= 201907L
#include <format>
#else
// TODO: Remove when no longer relevant
// This is a kludge for gcc error:
// 'QTextStream& QTextStreamFunctions::ws(QTextStream&)' is deprecated
// ----
#include <istream>
namespace date
{
using std::ws;
}
// ----

#include "date/date.h"
#endif

template <typename Rep, typename Period>
struct ToJsonConverter<std::chrono::duration<Rep, Period>>
{
    static QJsonValue convert(const std::chrono::duration<Rep, Period>& duration)
    {
#if __cpp_lib_format >= 201907L
        return QJsonValue{QString::fromStdString(std::format("{:%T}", duration))};
#else
        return json::toValueWithFormat(duration, "%T");
#endif
    }
};

template <typename Rep, typename Period, typename Format>
struct ToJsonConverterWithFormat<std::chrono::duration<Rep, Period>, Format>
{
    static QJsonValue convert(
        const std::chrono::duration<Rep, Period>& duration, const Format& format)
    {
#if __cpp_lib_format >= 201907L
        return QJsonValue{QString::fromStdString(std::vformat(
            std::string_view{std::format("{{:{}}}", format)}, std::make_format_args(duration)))};
#else
        return QJsonValue{QString::fromStdString(date::format(format, duration))};
#endif
    }
};

#endif // #if __cpp_lib_format >= 201907L || __has_include("date/date.h")
