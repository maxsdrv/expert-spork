/** @file
 * @brief string <-> QString converters
 *
 * $Id: $
 */

#pragma once

#include <QString>

#include <iostream>
#include <string>

namespace strings
{
/**
 * Convert utf8 std::string @a str to QString.
 */
inline QString fromUtf8(const std::string& str)
{
    return QString::fromUtf8(str.c_str());
}

/**
 * Convert QString @a str to utf8-encoded std::string.
 */
inline std::string toUtf8(const QString& str)
{
    auto utf8 = str.toUtf8();
    return {utf8.begin(), utf8.end()};
}

} // namespace strings

inline std::ostream& operator<<(std::ostream& output, const QString& str)
{
    output << strings::toUtf8(str);
    return output;
}

inline std::istream& operator>>(std::istream& input, QString& str)
{
    // We should read the whole input buffer
    std::istreambuf_iterator<char> eos;
    std::string string(std::istreambuf_iterator<char>(input), eos);

    str = strings::fromUtf8(string);
    return input;
}
