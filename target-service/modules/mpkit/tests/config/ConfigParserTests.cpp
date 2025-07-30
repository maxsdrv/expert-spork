/**
 * @file
 * @brief Test suite for ConfigParser
 *
 *
 * $Id: $
 */

#include "appkit/Paths.h"

#include "config/ConfigParser.h"
#include "config/PTreeRW.h"

#include "exception/DuplicateEntity.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace appkit
{

struct Config
{
    Config() : port(0) {}

    std::string host;
    int port;

    void setHost(const std::string& host_) { host = host_; }
};

class ConfigParserTest : public ::testing::Test
{
    public:
    ConfigParserTest()
        : m_settings(
            config::fromFile<config::format::JSON>(appkit::configFile("testconf"
                                                                      "ig."
                                                                      "json")))
    {
    }

    protected:
    boost::property_tree::ptree m_settings;
    Config m_config;
};

TEST_F(ConfigParserTest, addDuplicateOption)
{
    config::ConfigParser parser;

    EXPECT_NO_THROW(parser.addOption("test_node1", "Test node", m_config.host));
    EXPECT_THROW(
        parser.addOption("test_node1", "Another test node", m_config.port),
        exception::DuplicateEntity);
}

TEST_F(ConfigParserTest, testKeys)
{
    const std::string optionOne("test_node1.option1");
    const std::string optionTwo("test_node1.option2");
    config::ConfigParser parser;

    parser.addOption(optionOne, "Option one", m_config.host);
    parser.addOption(optionTwo, "Option two", m_config.port);

    std::set<std::string> keys;
    keys.insert(optionOne);
    keys.insert(optionTwo);

    EXPECT_EQ(keys, parser.keys());
}

TEST_F(ConfigParserTest, addDuplicateOptionFromSubtree)
{
    config::ConfigParser parser;
    config::ConfigParser subtreeParser;

    EXPECT_NO_THROW(parser.addOption("test_node1", "Main node", m_config.host));
    EXPECT_NO_THROW(
        subtreeParser.addOption("subnode", "Subnode", m_config.port));

    EXPECT_THROW(
        parser.addSubtreeOption("test_node1", "Some subtree", subtreeParser),
        exception::DuplicateEntity);
}

TEST_F(ConfigParserTest, testKeysFromSubtree)
{
    config::ConfigParser parser;
    config::ConfigParser subtreeParser;

    EXPECT_NO_THROW(parser.addOption("test_node1", "Main node", m_config.host));
    EXPECT_NO_THROW(
        subtreeParser.addOption("subnode", "Subnode", m_config.port));
    EXPECT_NO_THROW(
        parser.addSubtreeOption("subtree", "Some subtree", subtreeParser));

    std::set<std::string> keys;
    keys.insert("test_node1");
    keys.insert("subtree.subnode");
    EXPECT_EQ(keys, parser.keys());
}

TEST_F(ConfigParserTest, parseSubtree)
{
    /* Test part.
     "test_node2":
    {
        "token": "abcdef",
        "any" :
        {
            "any_node3" : "any_value"
        }
    }
    */

    std::string token;
    std::string value;

    config::ConfigParser parser;
    parser.addOption("test_node2.token", "Token", token);
    config::ConfigParser subtreeParser;
    subtreeParser.addOption("any_node3", "Not a token", value);

    parser.addSubtreeOption("test_node2.any", "Subtree node", subtreeParser);

    parser.addPtree(m_settings, "settings").readAll();

    EXPECT_EQ("abcdef", token);
    EXPECT_EQ("any_value", value);
}

TEST_F(ConfigParserTest, printUsage)
{
    // Actually this test verify nothing. Just want to see usage string

    std::string token;
    std::string value;

    config::ConfigParser parser;
    parser.addOption("test_node2.token", "Description string", token);
    config::ConfigParser subtreeParser;
    subtreeParser.addOption("any_node3", "Some node", value);
    subtreeParser.addOption(
        "some_node_with_long_value", "Long value option", value);

    parser.addSubtreeOption("test_node2.any", "Subtree option", subtreeParser);

    std::cout << "===========================PARSER "
                 "USAGE============================\n";
    parser.printProgramOptionsUsage(std::cout, 1);
    std::cout << "============================================================="
                 "======\n";
    parser.printConfigOptionsUsage(std::cout, 1);
    std::cout << "============================================================="
                 "======\n";
}

TEST_F(ConfigParserTest, parseMandatoryPapametersWithValues)
{
    config::ConfigParser parser;
    parser.addOption("test_node1.host", "Test node one", m_config.host);
    parser.addOption("test_node1.port", "Test port", m_config.port);

    parser.addPtree(m_settings, "settings").readAll();

    EXPECT_EQ("local", m_config.host);
    EXPECT_EQ(7777, m_config.port);
}

TEST_F(ConfigParserTest, parseMandatoryPapametersWithBind)
{
    config::ConfigParser parser;
    parser.addOption<std::string>(
        "test_node1.host",
        "Host",
        boost::bind(&Config::setHost, boost::ref(m_config), _1));

    parser.addPtree(m_settings, "settings").readAll();

    EXPECT_EQ("local", m_config.host);
}

TEST_F(ConfigParserTest, parseOptionalPapametersWithValues)
{
    config::ConfigParser parser;
    parser.addOption<std::string>("optional", "Optional value", m_config.host)
        .setDefaultValue("Value");

    parser.addPtree(m_settings, "settings").readAll();

    EXPECT_EQ("Value", m_config.host);
}

TEST_F(ConfigParserTest, parseOptionalPapametersWithBind)
{
    config::ConfigParser parser;
    parser
        .addOption<std::string>(
            "optional",
            "Optional value",
            boost::bind(&Config::setHost, boost::ref(m_config), _1))
        .setDefaultValue("Value");

    parser.addPtree(m_settings, "settings").readAll();

    EXPECT_EQ("Value", m_config.host);
}

TEST_F(ConfigParserTest, parseMissingMandatoryPropertiesWithValues)
{
    config::ConfigParser parser;
    parser.addOption("mandatory", "Mandatory node", m_config.host);
    parser.addOption("optional", "Optional node", m_config.port)
        .setDefaultValue(-1);

    parser.addPtree(m_settings, "settings");
    EXPECT_THROW(parser.readAll(), boost::property_tree::ptree_error);
}

TEST_F(ConfigParserTest, readIncorrectValue)
{
    config::ConfigParser parser;
    parser.addOption<int>("test_node3", "Test", 0);

    parser.addPtree(m_settings, "settings");
    try
    {
        parser.readAll();
        FAIL() << "Expected exception::InvalidConfigEntry thrown";
    }
    catch (const exception::InvalidConfigEntry& ex)
    {
        std::vector<std::string> const* keys =
            boost::get_error_info<exception::KeysInfo>(ex);
        EXPECT_TRUE(keys);
        EXPECT_EQ(2U, keys->size());
        EXPECT_EQ("test_node3", keys->at(0));
        EXPECT_EQ("hklmn", keys->at(1));
    }
    catch (...)
    {
        FAIL() << "Expected exception::InvalidConfigEntry thrown";
    }
}

class TestStringArrayMock
{
    public:
    MOCK_METHOD1(add, void(const std::string&));
};

class TestIntArrayMock
{
    public:
    MOCK_METHOD1(add, void(int));
};

TEST(ConfigParserWithArrayTest, readSimpleArray)
{
    boost::property_tree::ptree m_settings =
        config::fromFile<config::format::JSON>(appkit::configFile("arrayconfig."
                                                                  "json"));

    TestStringArrayMock testStringArrayMock;
    TestIntArrayMock testIntArrayMock;

    EXPECT_CALL(testStringArrayMock, add("one")).Times(testing::Exactly((1)));
    EXPECT_CALL(testStringArrayMock, add("two")).Times(testing::Exactly((1)));
    EXPECT_CALL(testStringArrayMock, add("three")).Times(testing::Exactly((1)));

    EXPECT_CALL(testIntArrayMock, add(1)).Times(testing::Exactly((1)));
    EXPECT_CALL(testIntArrayMock, add(2)).Times(testing::Exactly((1)));
    EXPECT_CALL(testIntArrayMock, add(3)).Times(testing::Exactly((1)));

    config::ConfigParser parser;
    parser.addArrayOption<std::string>(
        "string_array",
        "Array option",
        boost::bind(
            &TestStringArrayMock::add, boost::ref(testStringArrayMock), _1));
    parser.addArrayOption<int>(
        "int_array",
        "Array option",
        boost::bind(&TestIntArrayMock::add, boost::ref(testIntArrayMock), _1));

    parser.addPtree(m_settings, "settings");
    parser.readAll();
}

TEST(ConfigParserWithArrayTest, readIncorrectArray)
{
    boost::property_tree::ptree m_settings =
        config::fromFile<config::format::JSON>(appkit::configFile("arrayconfig."
                                                                  "json"));

    TestIntArrayMock testIntArrayMock;

    config::ConfigParser parser;
    parser.addArrayOption<int>(
        "incorrect_int_array",
        "Array option",
        boost::bind(&TestIntArrayMock::add, boost::ref(testIntArrayMock), _1));

    parser.addPtree(m_settings, "settings");

    try
    {
        parser.readAll();
        FAIL() << "Expected exception::InvalidConfigEntry thrown";
    }
    catch (const exception::InvalidConfigEntry& ex)
    {
        std::vector<std::string> const* keys =
            boost::get_error_info<exception::KeysInfo>(ex);
        EXPECT_TRUE(keys);
        EXPECT_EQ(2U, keys->size());
        EXPECT_EQ("incorrect_int_array", keys->at(0));
        EXPECT_EQ("one", keys->at(1));
    }
    catch (...)
    {
        FAIL() << "Expected exception::InvalidConfigEntry thrown";
    }
}

class TestIndexedArrayMock
{
    public:
    MOCK_METHOD2(addId, void(int index, int));
    MOCK_METHOD2(addValue, void(int index, std::string));
};

TEST(ConfigParserWithArrayTest, readComplexArray)
{
    boost::property_tree::ptree m_settings =
        config::fromFile<config::format::JSON>(appkit::configFile("arrayconfig."
                                                                  "json"));

    TestIndexedArrayMock testIndexedArrayMock;

    EXPECT_CALL(testIndexedArrayMock, addId(0, 22)).Times(testing::Exactly((1)));
    EXPECT_CALL(testIndexedArrayMock, addId(1, 33)).Times(testing::Exactly((1)));
    EXPECT_CALL(testIndexedArrayMock, addValue(0, "ololo"))
        .Times(testing::Exactly((1)));
    EXPECT_CALL(testIndexedArrayMock, addValue(1, "ufo driver"))
        .Times(testing::Exactly((1)));

    config::ConfigParser parser;
    parser.addIndexedOption<int>(
        "subtree_array.%.id",
        "Some array option",
        boost::bind(
            &TestIndexedArrayMock::addId,
            boost::ref(testIndexedArrayMock),
            _1,
            _2));

    parser
        .addIndexedOption<std::string>(
            "subtree_array.%.value",
            "Some array option with default",
            boost::bind(
                &TestIndexedArrayMock::addValue,
                boost::ref(testIndexedArrayMock),
                _1,
                _2))
        .setDefaultValue("ololo");

    parser.addPtree(m_settings, "settings");
    parser.readAll();
}

TEST(ConfigParserWithArrayTest, readIncorrectComplexArray)
{
    boost::property_tree::ptree m_settings =
        config::fromFile<config::format::JSON>(appkit::configFile("arrayconfig."
                                                                  "json"));

    TestIndexedArrayMock testIndexedArrayMock;

    config::ConfigParser parser;
    parser.addIndexedOption<int>(
        "subtree_array.%.number",
        "Some number array option",
        boost::bind(
            &TestIndexedArrayMock::addId,
            boost::ref(testIndexedArrayMock),
            _1,
            _2));

    parser.addPtree(m_settings, "settings");

    try
    {
        parser.readAll();
        FAIL() << "Expected exception::InvalidConfigEntry thrown";
    }
    catch (const exception::InvalidConfigEntry& ex)
    {
        std::vector<std::string> const* keys =
            boost::get_error_info<exception::KeysInfo>(ex);
        EXPECT_TRUE(keys);
        EXPECT_EQ(2U, keys->size());
        EXPECT_EQ("subtree_array.0.number", keys->at(0));
        EXPECT_EQ("one", keys->at(1));
    }
    catch (...)
    {
        FAIL() << "Expected exception::InvalidConfigEntry thrown";
    }
}

TEST(ConfigParserWithArrayTest, overrideIndexByCommandLine)
{
    boost::property_tree::ptree m_settings =
        config::fromFile<config::format::JSON>(appkit::configFile("arrayconfig."
                                                                  "json"));

    TestIndexedArrayMock testIndexedArrayMock;

    EXPECT_CALL(testIndexedArrayMock, addValue(0, "ololo"))
        .Times(testing::Exactly((1)));
    EXPECT_CALL(testIndexedArrayMock, addValue(1, "custom"))
        .Times(testing::Exactly((1)));

    config::ConfigParser parser;

    parser
        .addIndexedOption<std::string>(
            "subtree_array.$.value",
            "Some array option with default",
            boost::bind(
                &TestIndexedArrayMock::addValue,
                boost::ref(testIndexedArrayMock),
                _1,
                _2))
        .setDefaultValue("ololo")
        .setPlaceholder('$');

    char arg0[] = "programName";
    char arg1[] = "--subtree_array.1.value=custom";
    char* argv[] = {&arg0[0], &arg1[0], NULL};
    int argc = (int)(sizeof(argv) / sizeof(argv[0])) - 1;

    parser.addPtree(m_settings, "settings");
    parser.addPtree(
        config::fromCommandLine(argc, argv, parser.keys(), false),
        "command line");

    std::cout << parser.toJson() << std::endl;
    parser.readAll();
}

} // namespace appkit
