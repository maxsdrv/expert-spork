#include "storage_adapter/Provider.h"
#include "storage_adapter/ProxyStorage.h"
#include "storage_adapter/StorageModifier.h"

#include <gmock/gmock.h>

#include <vector>

struct Data
{
    int value;
    std::string string;
};

inline bool operator==(const Data& left, const Data& right)
{
    return std::tie(left.value, left.string) == std::tie(right.value, right.string);
}

using namespace mpk::storage_adapter;

template <>
class mpk::storage_adapter::ProxyObject<Data>
{
public:
    ProxyObject();

    // read interface
    void update();
    const Data& value(int index) const;
    int size() const;

    // write interface
    void append(const Data& value);
    void replace(int index, const Data& value);
    void remove(int index);
    void clear();

private:
    std::vector<Data> m_data;
};

ProxyObject<Data>::ProxyObject() : m_data{{12, "Good"}, {134, "Bad"}, {-23, "Ugly"}}
{
}

void ProxyObject<Data>::update()
{
    // Nothing to do
}

const Data& ProxyObject<Data>::value(int index) const
{
    return m_data.at(index);
}

int ProxyObject<Data>::size() const
{
    return static_cast<int>(m_data.size());
}

void ProxyObject<Data>::append(const Data& value)
{
    m_data.push_back(value);
}

void ProxyObject<Data>::replace(int index, const Data& value)
{
    m_data[index] = value;
}

void ProxyObject<Data>::remove(int index)
{
    m_data.erase(std::next(m_data.begin(), index));
}

void ProxyObject<Data>::clear()
{
    m_data.clear();
}

using TestProxyStorage = ProxyStorage<Data>;

class ProxyTest : public ::testing::Test
{
public:
    ProxyTest() : m_provider{m_proxy}
    {
    }

protected:
    ProxyObject<Data> m_proxy;
    TestProxyStorage m_provider;
};

TEST_F(ProxyTest, count)
{
    EXPECT_EQ(3, m_provider.count());
}

TEST_F(ProxyTest, at)
{
    EXPECT_EQ(134, m_provider.at(1).value);
    EXPECT_EQ("Ugly", m_provider.at(2).string);
}

TEST_F(ProxyTest, empty)
{
    EXPECT_FALSE(m_provider.empty());
}

TEST_F(ProxyTest, begin)
{
    auto it = m_provider.begin();
    ASSERT_NE(m_provider.end(), it);
    EXPECT_EQ(12, it->value);
    EXPECT_EQ("Good", it->string);
}

TEST_F(ProxyTest, end)
{
    auto it = std::next(m_provider.begin(), m_provider.count());
    EXPECT_EQ(m_provider.end(), it);
}

TEST_F(ProxyTest, findTrue)
{
    auto it = m_provider.find([](const auto& data) { return data.value < 0; });
    ASSERT_NE(m_provider.end(), it);
    EXPECT_EQ("Ugly", it->string);
}

TEST_F(ProxyTest, findFalse)
{
    auto it = m_provider.find([](const auto& data) { return data.value == 43; });
    EXPECT_EQ(m_provider.end(), it);
}

TEST_F(ProxyTest, modifierAppend)
{
    auto modifier = m_provider.modifier();
    ASSERT_NE(nullptr, modifier);
    auto value = Data{152, "Robert"};
    modifier->append(value);
    EXPECT_EQ(4, m_provider.count());
    auto it = m_provider.find([](const auto& data) { return data.string == "Robert"; });
    ASSERT_NE(m_provider.end(), it);
    EXPECT_EQ(value, *it);
}

TEST_F(ProxyTest, modifierMultiAppend)
{
    auto modifier = m_provider.modifier();
    ASSERT_NE(nullptr, modifier);
    auto value1 = Data{152, "Robert"};
    auto value2 = Data{1099, "Brady"};
    auto values = std::vector{value1, value2};
    modifier->append(values.begin(), values.end());
    EXPECT_EQ(5, m_provider.count());
    auto it = m_provider.find([](const auto& data) { return data.string == "Brady"; });
    ASSERT_NE(m_provider.end(), it);
    EXPECT_EQ(value2, *it);
}

TEST_F(ProxyTest, modifierReplaceBegin)
{
    auto modifier = m_provider.modifier();
    ASSERT_NE(nullptr, modifier);
    auto value = Data{152, "Robert"};
    modifier->replace(m_provider.begin(), value);
    EXPECT_EQ(3, m_provider.count());
    auto it = m_provider.begin();
    ASSERT_NE(m_provider.end(), it);
    EXPECT_EQ(value, *it);
}

TEST_F(ProxyTest, modifierReplaceEnd)
{
    auto modifier = m_provider.modifier();
    ASSERT_NE(nullptr, modifier);
    auto value = Data{152, "Robert"};
    modifier->replace(m_provider.end(), value);
    EXPECT_EQ(4, m_provider.count());
    auto it = m_provider.find([](const auto& data) { return data.string == "Robert"; });
    ASSERT_NE(m_provider.end(), it);
    EXPECT_EQ(value, *it);
}

TEST_F(ProxyTest, modifierRemoveBegin)
{
    auto modifier = m_provider.modifier();
    ASSERT_NE(nullptr, modifier);
    auto it = modifier->remove(m_provider.begin());
    EXPECT_EQ(2, m_provider.count());
    EXPECT_EQ(
        m_provider.end(),
        m_provider.find([](const auto& data) { return data.string == "Good"; }));
    EXPECT_EQ(*m_provider.begin(), *it);
}

TEST_F(ProxyTest, modifierRemoveIfNonEmpty)
{
    auto modifier = m_provider.modifier();
    ASSERT_NE(nullptr, modifier);
    modifier->removeIf([](const auto& data) { return data.string[1] == 'a'; });
    EXPECT_EQ(2, m_provider.count());
    EXPECT_EQ("Good", m_provider.begin()->string);
}

TEST_F(ProxyTest, modifierRemoveIfEmpty)
{
    auto modifier = m_provider.modifier();
    ASSERT_NE(nullptr, modifier);
    modifier->removeIf([](const auto& data) { return data.value > 10000; });
    EXPECT_EQ(3, m_provider.count());
}

TEST_F(ProxyTest, modifierClear)
{
    auto modifier = m_provider.modifier();
    ASSERT_NE(nullptr, modifier);
    modifier->clear();
    EXPECT_TRUE(m_provider.empty());
}
