/** @file
 * @brief Test suit for testing ptree rw methods
 *
 * $Id: $
 */

#include "config/PTreeRW.h"

#include <gtest/gtest.h>

#include <boost/property_tree/ptree.hpp>

namespace appkit
{

constexpr auto ASCII = "ascii";
constexpr auto ARRAY = "array";
constexpr auto ARRAY_VALUE("array_value_");
constexpr auto ASCII_VALUE("ascii_value");
const int arrayValues(5);

template <typename T>
class ConverterTest : public ::testing::Test
{
public:
    ConverterTest()
    {
        m_expected.add(ASCII, ASCII_VALUE);

        boost::property_tree::ptree nodes;
        for (int i = 0; i < arrayValues; ++i)
        {
            boost::property_tree::ptree node;
            node.put("", std::string(ARRAY_VALUE) + std::to_string(i));
            nodes.push_back(std::make_pair("", node));
        }
        m_expected.add_child(ARRAY, nodes);
    }

    boost::property_tree::ptree converted() const
    {
        std::string str = config::toString<T>(m_expected);
        std::cout << str;
        return config::fromString<T>(str);
    }

protected:
    boost::property_tree::ptree m_expected;
};

TYPED_TEST_SUITE_P(ConverterTest);

TYPED_TEST_P(ConverterTest, RwTest)
{
    boost::property_tree::ptree const actual = this->converted();

    EXPECT_EQ(ASCII_VALUE, actual.get<std::string>(ASCII));

    int i(0);
    for (auto const& v: actual.get_child("array."))
    {
        EXPECT_EQ(
            std::string(ARRAY_VALUE) + std::to_string(i++),
            v.second.data());
    }
}

REGISTER_TYPED_TEST_SUITE_P(ConverterTest, RwTest);

// format::XML save ptree array as empty node that leads to invalid xml
// format::INI can't add duplicate keys (so no array is available in INI)
// format::JSON has a bug with Unicode encoding/decoding in boost before 1.60
typedef ::testing::Types<
    config::format::INFO /*, format::XML, format::JSON, format::INI*/>
    Types;
INSTANTIATE_TYPED_TEST_SUITE_P(Rw, ConverterTest, Types);

} // namespace appkit
