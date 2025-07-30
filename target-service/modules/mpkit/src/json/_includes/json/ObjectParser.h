#pragma once

#include "json/FromJson.h"
#include "json/parsers/QJsonTypes.h"

namespace json
{

class ObjectParser
{
public:
    // Constructor is templated to support both QJsonValue and QJsonValueRef
    // The latter along with implicit constructor is used to iterate QJsonArray
    // via range-based for
    template <typename T>
    ObjectParser( // NOLINT (hicpp-explicit-conversions)
        const T& value,
        [[maybe_unused]] std::enable_if_t<
            std::is_same_v<T, QJsonValue> || std::is_same_v<T, QJsonValueRef>,
            T>* _ = nullptr) :
      m_object(json::fromValue<QJsonObject>(value))
    {
    }

#if QT_MAJOR > 5
    // NOLINTNEXTLINE (hicpp-explicit-conversions)
    ObjectParser(QJsonValueConstRef value) :
      m_object{[](auto value)
               {
                   if (!value.isObject())
                   {
                       BOOST_THROW_EXCEPTION(
                           exception::json::UnexpectedType(QJsonValue::Object, value.type()));
                   }
                   return value.toObject();
               }(value)}
    {
    }
#endif

    explicit ObjectParser(QJsonObject object) : m_object(std::move(object))
    {
    }

    template <typename T>
    [[nodiscard]] T get(const QString& key) const
    {
        return json::fromObject<T>(m_object, key);
    }

    template <typename T>
    [[nodiscard]] T get(const QString& key, const T& defaultValue) const
    {
        return json::fromObjectWithDefault<T>(m_object, key, defaultValue);
    }

    template <typename T>
    const ObjectParser& to(T& value, const QString& key) const
    {
        value = json::fromObject<T>(m_object, key);
        return *this;
    }

    template <typename T>
    const ObjectParser& to(T& value, const QString& key, const T& defaultValue) const
    {
        value = json::fromObjectWithDefault<T>(m_object, key, defaultValue);
        return *this;
    }

private:
    QJsonObject m_object;
};

} // namespace json
