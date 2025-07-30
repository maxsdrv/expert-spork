#pragma once

// Add 'add_mpkit_item(date)' in project CMakeLists.txt to enable std::chrono::duration conversion

#if __cpp_lib_chrono >= 201907L || __has_include("date/date.h")

#include "json/FromJson.h"
#include "json/parsers/General.h"

#if __cpp_lib_chrono < 201907L // #if not defined std::chrono::from_stream
// TODO: Remove when no longer relevant
// This is a kludge for gcc error:
// 'QTextStream& QTextStreamFunctions::ws(QTextStream&)' is deprecated
// ----
#include <istream>
namespace date
{
using std::ws;
} // namespace date
// ----

#include "date/date.h"
#endif

#include <chrono>
#include <sstream>

template <typename Rep, typename Period>
struct FromJsonConverter<std::chrono::duration<Rep, Period>>
{
    using Duration = std::chrono::duration<Rep, Period>;

    static Duration get(const QJsonValue& value)
    {
        return json::fromValueWithFormat<Duration>(value, "%0H:%M:%S");
    }
};

template <typename Rep, typename Period, typename Format>
struct FromJsonConverterWithFormat<std::chrono::duration<Rep, Period>, Format>
{
    using Duration = std::chrono::duration<Rep, Period>;

    static Duration get(const QJsonValue& value, const Format& format)
    {
#if __cpp_lib_chrono >= 201907L // #if defined std::chrono::from_stream
        using std::chrono::from_stream;
#else
        using date::from_stream;
#endif
        auto string = json::fromValue<QString>(value);
        std::istringstream stream{string.toStdString()};
        Duration duration{};
        from_stream(stream, format, duration);
        if (!stream)
        {
            BOOST_THROW_EXCEPTION(exception::json::UnexpectedValue(string));
        }
        return duration;
    }
};

#endif // #if __cpp_lib_chrono >= 201907L || __has_include("date/date.h")
