#include "httpws/HttpMethod.h"

#include "utils/EnumNamesMapper.h"

#include <unordered_map>

namespace http
{

namespace
{

auto methods()
{
    static const auto data = std::unordered_map<QString, HttpMethod::Value>{
        {"DELETE", HttpMethod::DELETE},
        {"GET", HttpMethod::GET},
        {"POST", HttpMethod::POST},
        {"PUT", HttpMethod::PUT},
    };
    return data;
}

} // namespace

QString HttpMethod::toString(Value value)
{
    return enums::toString(methods(), value);
}

HttpMethod::Value HttpMethod::fromString(const QString& string)
{
    return enums::fromString(methods(), string);
}

} // namespace http
