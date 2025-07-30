#pragma once

#include "json/JsonExceptions.h"
#include "json/ToJson.h"

#include "exception/FileError.h"
#include "qttypes/Filesystem.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

namespace json
{

template <typename T>
void putUniqueValue(QJsonObject& object, const QString& key, T value)
{
    if (object.contains(key))
    {
        BOOST_THROW_EXCEPTION(exception::json::DuplicateField(key));
    }

    object.insert(key, json::toValue(value));
}

/**
 * Read json from file (in JSON format)
 */
[[nodiscard]] inline QJsonObject fromFile(const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly))
    {
        BOOST_THROW_EXCEPTION(
            exception::FileError()
            << exception::json::File(filename) << exception::json::Error(file.errorString()));
    }

    auto fileContent = file.readAll();

    QJsonParseError error{};
    QJsonDocument document(QJsonDocument::fromJson(fileContent, &error));
    if (document.isNull())
    {
        BOOST_THROW_EXCEPTION(
            exception::json::InvalidJson()
            << exception::json::File(filename) << exception::json::Error(error.errorString()));
    }

    return document.object();
}

[[nodiscard]] inline QJsonObject fromFile(const std::filesystem::path& filename)
{
    return fromFile(qt::fromFilesystemPath(filename));
}

/**
 * Write json to file (in JSON format)
 */
inline void toFile(const QJsonObject& object, const QString& filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly))
    {
        BOOST_THROW_EXCEPTION(
            exception::FileError()
            << exception::json::File(filename) << exception::json::Error(file.errorString()));
    }

    auto data = QJsonDocument(object).toJson();
    if (file.write(data) != data.size())
    {
        BOOST_THROW_EXCEPTION(
            exception::FileError()
            << exception::json::File(filename) << exception::json::Error(file.errorString()));
    }
}

/**
 * Convert JSON to byte array
 */
[[nodiscard]] inline QByteArray toByteArray(
    const QJsonObject& object,
    QJsonDocument::JsonFormat format = QJsonDocument::JsonFormat::Indented)
{
    return QJsonDocument(object).toJson(format);
}

/**
 * Read json from byte array (in JSON format)
 */
[[nodiscard]] inline QJsonObject fromByteArray(const QByteArray& array)
{
    QJsonParseError error{};
    QJsonDocument document(QJsonDocument::fromJson(array, &error));
    if (document.isNull())
    {
        BOOST_THROW_EXCEPTION(
            exception::json::InvalidJson() << exception::json::Error(error.errorString()));
    }

    return document.object();
}

/**
 * Convert JSON to std::string array
 */
[[nodiscard]] inline std::string toStdString(
    const QJsonObject& object,
    QJsonDocument::JsonFormat format = QJsonDocument::JsonFormat::Indented)
{
    return toByteArray(object, format).toStdString();
}

/**
 * Read json from std::string (in JSON format)
 */
[[nodiscard]] inline QJsonObject fromStdString(const std::string& string)
{
    return json::fromByteArray(QByteArray::fromStdString(string));
}

/**
 * Add all nodes from @a fromObject to @a toObject
 */
inline void merge(QJsonObject& toObject, const QJsonObject& fromObject)
{
    for (auto it = fromObject.begin(); it != fromObject.end(); ++it)
    {
        toObject[it.key()] = it.value();
    }
}

// NOLINTBEGIN(misc-no-recursion)
// Replace all values with key matches regexPattern to newValue
// May be useful to mask passwords for logging
template <typename T>
QJsonObject replaceValue(
    const QJsonObject& jsonObject, const QRegularExpression& regex, const T& newValue)
{
    auto result = QJsonObject{};
    const auto keys = jsonObject.keys();
    for (const auto& key: keys)
    {
        auto value = jsonObject.value(key);
        if (regex.match(key).hasMatch())
        {
            result[key] = newValue;
        }
        else if (value.isObject())
        {
            result[key] = replaceValue(value.toObject(), regex, newValue);
        }
        else if (value.isArray())
        {
            auto array = value.toArray();
            for (auto&& arrayElement: array)
            {
                if (arrayElement.isObject())
                {
                    arrayElement = replaceValue(arrayElement.toObject(), regex, newValue);
                }
            }
            result[key] = array;
        }
        else
        {
            result[key] = value;
        }
    }

    return result;
}
// NOLINTEND(misc-no-recursion)

} // namespace json
