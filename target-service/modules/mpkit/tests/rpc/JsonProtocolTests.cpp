#include "rpc/proto/JsonProtocol.h"

#include "gqtest"

namespace rpc
{

constexpr auto valueTag = "value";
constexpr auto arrayTag = "array";

struct Foo
{
    bool boolValue = true;
    int intValue = 0;
    std::string stringValue;
};

template <typename T>
T packAndUnpackValue(T value)
{
    Protocol protocol(DataFormat::Json);
    auto buffer = protocol.serializer().packValue(valueTag, value).buffer();

    decltype(value) unpacked;
    protocol.deserializer(buffer).unpackValue(valueTag, unpacked);
    return unpacked;
}

TEST(JsonProtocolTest, qString)
{
    QString value{"Everyone has left you"};
    EXPECT_EQ(value, packAndUnpackValue(value));
}

TEST(JsonProtocolTest, signedInteger)
{
    int value{1844};
    EXPECT_EQ(value, packAndUnpackValue(value));
}

TEST(JsonProtocolTest, unsignedInteger)
{
    int32_t value{1844};
    EXPECT_EQ(value, packAndUnpackValue(value));
}

TEST(JsonProtocolTest, stdString)
{
    std::string value("Is this a dream or is it real?");
    EXPECT_EQ(value, packAndUnpackValue(value));
}

TEST(JsonProtocolTest, bool)
{
    bool value{true};
    EXPECT_EQ(value, packAndUnpackValue(value));
}

TEST(JsonProtocolTest, double)
{
    double value{16.77};
    EXPECT_NEAR(value, packAndUnpackValue(value), 0.0001);
}

TEST(JsonProtocolTest, packUserDefinedValue)
{
    Foo value{true, 17, "I free you from the space"};
    auto unpacked = packAndUnpackValue(value);

    EXPECT_EQ(
        std::tie(value.boolValue, value.intValue, value.stringValue),
        std::tie(unpacked.boolValue, unpacked.intValue, unpacked.stringValue));
}

TEST(JsonProtocolTest, unpackMismatchedType)
{
    QString value{"Strange things happend"};
    Protocol protocol(DataFormat::Json);
    auto buffer = protocol.serializer().packValue(valueTag, value).buffer();

    int unpacked;

    EXPECT_THROW(
        protocol.deserializer(buffer).unpackValue(valueTag, unpacked),
        exception::json::UnexpectedType);
}

TEST(JsonProtocolTest, unpackMissingValue)
{
    QString value{"Sad Ignat"};
    Protocol protocol(DataFormat::Json);
    auto buffer = protocol.serializer().packValue(valueTag, value).buffer();

    decltype(value) unpacked;
    EXPECT_THROW(
        protocol.deserializer(buffer).unpackValue("missingTag", unpacked),
        exception::json::MissingNode);
}

TEST(JsonProtocolTest, packDuplicateValue)
{
    QString value{"Ignat is not so sad"};
    Protocol protocol(DataFormat::Json);
    EXPECT_THROW(
        protocol.serializer()
            .packValue(valueTag, value)
            .packValue(valueTag, value),
        exception::json::DuplicateField);
}

TEST(JsonProtocolTest, packArray)
{
    QString stringValue{"Lead me to places unknown"};
    Foo fooValue{true, 71, "The die is cast"};

    Protocol protocol(DataFormat::Json);
    auto buffer = protocol.serializer()
                      .packArray(arrayTag, stringValue, fooValue)
                      .buffer();

    std::tuple<QString, Foo> unpacked;
    protocol.deserializer(buffer).unpackArray(arrayTag, unpacked);

    EXPECT_EQ(stringValue, std::get<QString>(unpacked));

    auto unpackedFoo = std::get<Foo>(unpacked);
    EXPECT_EQ(
        std::tie(fooValue.boolValue, fooValue.intValue, fooValue.stringValue),
        std::tie(
            unpackedFoo.boolValue,
            unpackedFoo.intValue,
            unpackedFoo.stringValue));
}

TEST(JsonProtocolTest, packArrayAndValue)
{
    QString stringValue{"Fly me across the sky"};
    Foo fooValue{true, 17, "Reveal to me secrets of life"};
    Protocol protocol(DataFormat::Json);

    auto buffer = protocol.serializer()
                      .packArray(arrayTag, fooValue)
                      .packValue(valueTag, stringValue)
                      .buffer();

    QString unpackedValue;
    std::tuple<Foo> unpackedArray;
    auto deserializer = protocol.deserializer(buffer);
    deserializer.unpackValue(valueTag, unpackedValue);
    deserializer.unpackArray(arrayTag, unpackedArray);

    EXPECT_EQ(stringValue, unpackedValue);

    auto unpackedFoo = std::get<Foo>(unpackedArray);
    EXPECT_EQ(
        std::tie(fooValue.boolValue, fooValue.intValue, fooValue.stringValue),
        std::tie(
            unpackedFoo.boolValue,
            unpackedFoo.intValue,
            unpackedFoo.stringValue));
}

TEST(JsonProtocolTest, unpackArrayInvalidOrder)
{
    QString stringValue{"Lead me to places unknown"};
    Foo fooValue{true, 71, "The die is cast"};

    Protocol protocol(DataFormat::Json);
    auto buffer = protocol.serializer()
                      .packArray(arrayTag, stringValue, fooValue)
                      .buffer();

    std::tuple<Foo, QString> unpacked;
    EXPECT_THROW(
        protocol.deserializer(buffer).unpackArray(arrayTag, unpacked),
        exception::json::UnexpectedType);
}

TEST(JsonProtocolTest, packEmptyArray)
{
    Protocol protocol(DataFormat::Json);
    auto buffer = protocol.serializer().packArray(arrayTag).buffer();
    std::tuple<> unpacked;
    protocol.deserializer(buffer).unpackArray(arrayTag, unpacked);
    // Nothing to compare here. Just empty value, check we not throw
}

TEST(JsonProtocolTest, chainedPackAndSequantalUnpack)
{
    QString valueOne{"Hola, amigos!"};
    QString valueTwo{"Como esta?"};

    constexpr auto valueOneTag = "valueOne";
    constexpr auto valueTwoTag = "valueTwo";

    Protocol protocol(DataFormat::Json);
    auto buffer = protocol.serializer()
                      .packValue(valueOneTag, valueOne)
                      .packValue(valueTwoTag, valueTwo)
                      .buffer();

    auto deserializer = protocol.deserializer(buffer);

    decltype(valueOne) unpackedOne;
    decltype(valueTwo) unpackedTwo;

    // Check if we can change order of named args
    deserializer.unpackValue(valueTwoTag, unpackedTwo);
    deserializer.unpackValue(valueOneTag, unpackedOne);

    EXPECT_EQ(valueOne, unpackedOne);
    EXPECT_EQ(valueTwo, unpackedTwo);
}

TEST(JsonProtocolTest, sequantalPackAndChainedUnpack)
{
    QString valueOne{"Hola, amigos!"};
    QString valueTwo{"Como esta?"};

    constexpr auto valueOneTag = "valueOne";
    constexpr auto valueTwoTag = "valueTwo";

    Protocol protocol(DataFormat::Json);
    auto serializer = protocol.serializer();
    serializer.packValue(valueOneTag, valueOne);
    serializer.packValue(valueTwoTag, valueTwo);

    auto deserializer = protocol.deserializer(serializer.buffer());

    decltype(valueOne) unpackedOne;
    decltype(valueTwo) unpackedTwo;

    // Check if we can change order of named args
    deserializer.unpackValue(valueTwoTag, unpackedTwo)
        .unpackValue(valueOneTag, unpackedOne);

    EXPECT_EQ(valueOne, unpackedOne);
    EXPECT_EQ(valueTwo, unpackedTwo);
}

} // namespace rpc

constexpr auto boolTag = "bool";
constexpr auto intTag = "int";
constexpr auto stringTag = "string";

template <>
struct FromJsonConverter<rpc::Foo>
{
    static rpc::Foo get(const QJsonValue& value)
    {
        if (!value.isObject())
        {
            BOOST_THROW_EXCEPTION(exception::json::UnexpectedType());
        }

        auto object = value.toObject();
        return rpc::Foo{
            json::fromObject<bool>(object, boolTag),
            json::fromObject<int>(object, intTag),
            json::fromObject<std::string>(object, stringTag),
        };
    }
};

template <>
struct ToJsonConverter<rpc::Foo>
{
    static QJsonValue convert(const rpc::Foo& value)
    {
        QJsonObject object;
        object[boolTag] = value.boolValue;
        object[intTag] = value.intValue;
        object[stringTag] = strings::fromUtf8(value.stringValue);
        return object;
    }
};
