/**
 * @file
 * @brief File parser tests
 *
 *
 * $Id: $
 */

#include "appkit/Paths.h"

#include "config/PTreeRW.h"

#include <gtest/gtest.h>

#include <boost/format.hpp>

namespace appkit
{

using namespace boost::property_tree;

// Declare valid config options
constexpr auto HOST = "test_node1.host";
constexpr auto PORT = "test_node1.port";
constexpr auto TOKEN = "test_node2.token";
constexpr auto ANY = "test_node3";
constexpr auto ANY1 = "test_node2.any.any_node1";
constexpr auto ANY2 = "test_node2.any.any_node2";
constexpr auto ANY3 = "test_node2.any.any_node3";

typedef std::set<std::string> Keys;

// File parser tests
TEST(ParseFileTest, notFile)
{
    ASSERT_THROW(config::fromFile<config::format::JSON>("X"), std::exception);
}

TEST(ParseFileTest, parseAllFilePapameters)
{
    ptree settings = config::fromFile<config::format::JSON>(
        appkit::configFile("testconfig.json"));

    ASSERT_EQ(std::string(), settings.get<std::string>("test_node1"));

    ASSERT_EQ(std::string(), settings.get<std::string>("test_node2"));

    ASSERT_NO_THROW(settings.get<std::string>(HOST));

    boost::optional<std::string> host
        = settings.get_optional<std::string>(HOST);

    EXPECT_TRUE(host.is_initialized());

    EXPECT_EQ("local", host.get());

    ASSERT_NO_THROW(settings.get<int>((PORT)));

    boost::optional<int> port = settings.get_optional<int>((PORT));

    ASSERT_TRUE(port.is_initialized());

    EXPECT_EQ(7777, port.get());

    EXPECT_THROW(settings.get<std::string>("test"), std::exception);

    ASSERT_NO_THROW(settings.get<std::string>(TOKEN));

    boost::optional<std::string> token
        = settings.get_optional<std::string>(TOKEN);

    ASSERT_TRUE(token.is_initialized());

    EXPECT_EQ("abcdef", token.get());

    ASSERT_NO_THROW(settings.get<std::string>(ANY));

    EXPECT_EQ("hklmn", settings.get<std::string>(ANY));

    ASSERT_NO_THROW(settings.get<int>(ANY1));

    EXPECT_EQ(0, settings.get<int>(ANY1));

    boost::optional<std::string> any2
        = settings.get_optional<std::string>(ANY2);

    ASSERT_TRUE(any2.is_initialized());

    EXPECT_EQ("null", any2.get());

    boost::optional<int> _any2 = settings.get_optional<int>(ANY2);

    ASSERT_FALSE(_any2.is_initialized());

    ASSERT_NO_THROW(settings.get<std::string>(ANY3));

    EXPECT_EQ("any_value", settings.get<std::string>(ANY3));
}

} // namespace appkit
