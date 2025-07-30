#pragma once

#include <yaml-cpp/yaml.h>

#include <chrono>
#include <optional>

#include <QString>

namespace YAML
{

template <class Rep, class Period>
struct convert<std::chrono::duration<Rep, Period>>
{
    using Duration = std::chrono::duration<Rep, Period>;

    static Node encode(const Duration& duration)
    {
        return Node(duration.count());
    }

    static bool decode(const Node& node, Duration& duration)
    {
        duration = Duration(node.as<typename Duration::rep>());
        return true;
    }
};

template <class T>
struct convert<std::optional<T>>
{
    static Node encode(const std::optional<T>& optional)
    {
        return optional ? Node(*optional) : Node();
    }

    static bool decode(const Node& node, std::optional<T>& optional)
    {
        if (!node.IsNull())
        {
            optional = node.as<T>();
        }
        else
        {
            optional.reset();
        }
        return true;
    }
};

template <>
struct convert<QString>
{
    static Node encode(const QString& rhs)
    {
        return Node(rhs.toStdString());
    }

    static bool decode(const Node& node, QString& rhs)
    {
        rhs = QString::fromStdString(node.as<std::string>());
        return true;
    }
};

} // namespace YAML
