/**
 * @file
 * @brief Tests for property tree utils
 *
 *
 * $Id: $
 */

#include "appkit/Paths.h"

#include "config/PTreeRW.h"
#include "config/PTreeUtils.h"

#include "exception/InvalidConfigEntry.h"
#include "exception/KeyInfo.h"

#include <gtest/gtest.h>

namespace appkit
{

using namespace boost::property_tree;

// Declare valid config options
constexpr auto HOST = "test_node1.host";
constexpr auto PORT = "test_node1.port";
constexpr auto ANY = "test_node3";
constexpr auto ANY1 = "test_node2.any.any_node1";
constexpr auto ARRAY = "test_array";

typedef std::set<std::string> Keys;

TEST(pullLeavesTest, allLeaves)
{
    boost::property_tree::ptree ptree;
    ptree.put(HOST, "local");
    ptree.put(PORT, 7777);
    ptree.put(ANY, "aaa");
    ptree.put(ANY1, "aaa");

    boost::property_tree::ptree child1, child2;
    child1.put("", 1);
    child2.put("", 2);

    boost::property_tree::ptree children;
    children.push_back(std::make_pair("", child1));
    children.push_back(std::make_pair("", child2));

    ptree.add_child(ARRAY, children);

    Keys leaves;
    config::pullLeaves(leaves, ptree);

    ASSERT_FALSE(leaves.empty());

    EXPECT_TRUE(leaves.count(HOST));

    EXPECT_TRUE(leaves.count(PORT));

    EXPECT_TRUE(leaves.count(ANY));

    EXPECT_TRUE(leaves.count(ANY1));

    EXPECT_TRUE(leaves.count(ARRAY));
}

TEST(mergeTest, merge)
{
    ptree ptree1 = config::fromFile<config::format::JSON>(
        appkit::configFile("tree1.json"));
    ptree ptree2 = config::fromFile<config::format::JSON>(
        appkit::configFile("tree2.json"));

    boost::property_tree::ptree ptree3 = config::merge(ptree1, ptree2);

    ASSERT_FALSE(ptree3.empty());

    // Nodes from first tree
    EXPECT_EQ("local", ptree3.get<std::string>("test_node1.host"));
    EXPECT_EQ(7777, ptree3.get<int>("test_node1.port"));
    EXPECT_EQ("nedog", ptree3.get<std::string>("test_node2.nekot"));
    EXPECT_EQ("dron", ptree3.get<std::string>("test_node3.0.kron"));

    // Nodes from second tree
    EXPECT_EQ("abcdef", ptree3.get<std::string>("test_node2.token"));
    EXPECT_EQ("rome", ptree3.get<std::string>("test_node3.0.chrome"));
    EXPECT_EQ("sodom", ptree3.get<std::string>("test_node3.1.chrome"));
    EXPECT_EQ("slon", ptree3.get<std::string>("test_node3.1.kron"));
    EXPECT_EQ(0, ptree3.get<int>("test_node2.any.any_node1"));
    EXPECT_EQ("any_value", ptree3.get<std::string>("test_node2.any.any_node3"));

    // Test array merging
    std::set<std::string> values;
    for (auto& v: ptree3.get_child("array_node"))
    {
        values.insert(v.second.data());
    }
    std::set<std::string> expectedValues;
    expectedValues.insert("one");
    expectedValues.insert("two");
    expectedValues.insert("three");
    EXPECT_EQ(expectedValues, values);
}

TEST(validateTest, valid)
{
    boost::property_tree::ptree ptree;
    ptree.put(HOST, "local");

    Keys keys;
    keys.insert(HOST);
    keys.insert(PORT);

    ASSERT_NO_THROW(config::validate(ptree, keys));
}

TEST(validateTest, notValidCheckData)
{
    boost::property_tree::ptree ptree;
    ptree.put(HOST, "local");
    ptree.put(PORT, 7777);

    Keys keys;
    keys.insert(HOST);

    try
    {
        config::validate(ptree, keys);
        // check throw
        EXPECT_TRUE(false);
    }
    catch (const exception::InvalidConfigEntry& ex)
    {
        const std::vector<std::string>* info
            = boost::get_error_info<exception::KeysInfo>(ex);
        ASSERT_TRUE(info);

        ASSERT_FALSE(info->empty());

        EXPECT_EQ(1U, info->size());
        EXPECT_EQ(PORT, info->back());
    }
}

} // namespace appkit
