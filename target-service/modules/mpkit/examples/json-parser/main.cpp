#include "json/JsonParser.h"
#include "json/JsonUtilities.h"

#include "json/Paths.h"

#include <iostream>

struct Parsed
{
    int id{-1};
    std::string description;
    bool active{false};
    std::vector<int> arrayOfObjects;
    std::vector<int> arrayOfInts;
};

/**
 * Simple object parser. Try to parse the following json
 * {
 *   "id": 100,
 *   "description": "description",
 *   "active": true,
 *   "objectsArray": [{"value": 100}, {"value": 200}, {"value": 300}],
 *   "intArray": [0, 1, 2, 3]
 * }
 */
auto parseWithObjectParser(std::string const& filename)
{
    auto const file = QString::fromStdString(filename);
    try
    {
        auto const parser = json::ObjectParser(json::fromFile(file));
        Parsed parsed;
        parser.to(parsed.id, "id")
            .to(parsed.description, "description")
            .to(parsed.active, "active")
            .to(parsed.arrayOfInts, "intArray");
        auto const array = parser.get<QJsonArray>("objectsArray");
        for (auto const& object: qAsConst(array))
        {
            parsed.arrayOfObjects.push_back(json::ObjectParser{object}.get<int>("value"));
        }
        return parsed;
    }
    catch (exception::json::InvalidJson& ex)
    {
        ex << exception::json::File(file);
        throw;
    }
}

// Define formatter to parse Parsed struct
template <>
struct FromJsonConverter<Parsed>
{
    static Parsed get(const QJsonValue& value)
    {
        Parsed parsed;
        auto const parser = json::ObjectParser(value);
        parser.to(parsed.id, "id")
            .to(parsed.description, "description")
            .to(parsed.active, "active")
            .to(parsed.arrayOfInts, "intArray");
        auto const array = parser.get<QJsonArray>("objectsArray");
        for (auto const& object: qAsConst(array))
        {
            parsed.arrayOfObjects.push_back(json::ObjectParser{object}.get<int>("value"));
        }
        return parsed;
    }
};

Parsed parseWithCustomFormatter(std::string const& filename)
{
    auto const file = QString::fromStdString(filename);
    try
    {
        return json::fromValue<Parsed>(json::fromFile(file));
    }
    catch (exception::json::InvalidJson& ex)
    {
        ex << exception::json::File(file);
        throw;
    }
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
    // Simple and valid json
    try
    {
        auto const parsed = parseWithObjectParser(appkit::configFile("valid.json"));
        std::cout << parsed.description << std::endl;
    }
    catch (std::exception const& ex)
    {
        std::cout << ex.what() << std::endl;
    }

    // Invalid json, should throw while reading file
    try
    {
        [[maybe_unused]] auto const parsed1 =
            parseWithObjectParser(appkit::configFile("invalid.json"));
    }
    catch (std::exception const& ex)
    {
        std::cout << ex.what() << std::endl;
    }

    // Parse the same json with custom formatter
    try
    {
        auto const parsed = parseWithCustomFormatter(appkit::configFile("valid.json"));
        std::cout << parsed.description << std::endl;
    }
    catch (std::exception const& ex)
    {
        std::cout << ex.what() << std::endl;
    }
}
