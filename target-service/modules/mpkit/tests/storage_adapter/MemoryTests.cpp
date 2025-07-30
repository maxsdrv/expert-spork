#include "storage_adapter/MemoryStorage.h"
#include "storage_adapter/Provider.h"
#include "storage_adapter/StorageModifier.h"

#include <gmock/gmock.h>

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

using DataStorage = MemoryStorage<Data>;
using DataStorageTraits = MemoryStorageTraits<Data>;
using DataProvider = Provider<DataStorage, DataStorageTraits>;

class ZeroMemoryTest : public ::testing::Test
{
public:
    ZeroMemoryTest() = default;

protected:
    DataStorage m_provider;
};

TEST_F(ZeroMemoryTest, count)
{
    EXPECT_EQ(0, m_provider.count());
}

TEST_F(ZeroMemoryTest, empty)
{
    EXPECT_TRUE(m_provider.empty());
}

TEST_F(ZeroMemoryTest, beginEnd)
{
    EXPECT_EQ(m_provider.begin(), m_provider.end());
}

const auto dataSet = {Data{42, "John"}, Data{77, "Jack"}, Data{14, "James"}};
class MemoryTest : public ::testing::Test
{
public:
    MemoryTest()
    {
        m_provider.modifier()->append(dataSet.begin(), dataSet.end());
    }

protected:
    DataStorage m_provider;
};

TEST_F(MemoryTest, count)
{
    EXPECT_EQ(3, m_provider.count());
}

TEST_F(MemoryTest, at)
{
    EXPECT_EQ(77, m_provider.at(1).value);
    EXPECT_EQ("James", m_provider.at(2).string);
}

TEST_F(MemoryTest, empty)
{
    EXPECT_FALSE(m_provider.empty());
}

TEST_F(MemoryTest, begin)
{
    auto it = m_provider.begin();
    ASSERT_NE(m_provider.end(), it);
    EXPECT_EQ(42, it->value);
    EXPECT_EQ("John", it->string);
}

TEST_F(MemoryTest, end)
{
    auto it = std::next(m_provider.begin(), m_provider.count());
    EXPECT_EQ(m_provider.end(), it);
}

TEST_F(MemoryTest, findTrue)
{
    auto it = m_provider.find([](const auto& data) { return data.string == "James"; });
    ASSERT_NE(m_provider.end(), it);
    EXPECT_EQ(14, it->value);
}

TEST_F(MemoryTest, findFalse)
{
    auto it = m_provider.find([](const auto& data) { return data.string == "Robert"; });
    EXPECT_EQ(m_provider.end(), it);
}

TEST_F(MemoryTest, modifierAppend)
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

TEST_F(MemoryTest, modifierMultiAppend)
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

TEST_F(MemoryTest, modifierReplaceBegin)
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

TEST_F(MemoryTest, modifierReplaceEnd)
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

TEST_F(MemoryTest, modifierRemoveBegin)
{
    auto modifier = m_provider.modifier();
    ASSERT_NE(nullptr, modifier);
    auto it = modifier->remove(m_provider.begin());
    EXPECT_EQ(2, m_provider.count());
    EXPECT_EQ(
        m_provider.end(),
        m_provider.find([](const auto& data) { return data.string == "John"; }));
    EXPECT_EQ(*m_provider.begin(), *it);
}

TEST_F(MemoryTest, modifierRemoveIfNonEmpty)
{
    auto modifier = m_provider.modifier();
    ASSERT_NE(nullptr, modifier);
    modifier->removeIf([](const auto& data) { return data.string[1] == 'a'; });
    EXPECT_EQ(1, m_provider.count());
    EXPECT_EQ("John", m_provider.begin()->string);
}

TEST_F(MemoryTest, modifierRemoveIfEmpty)
{
    auto modifier = m_provider.modifier();
    ASSERT_NE(nullptr, modifier);
    modifier->removeIf([](const auto& data) { return data.value > 10000; });
    EXPECT_EQ(3, m_provider.count());
}

TEST_F(MemoryTest, modifierClear)
{
    auto modifier = m_provider.modifier();
    ASSERT_NE(nullptr, modifier);
    modifier->clear();
    EXPECT_TRUE(m_provider.empty());
}

struct NonCopyableData
{
    int value;
    std::string string;

    NonCopyableData(int _value, std::string _string) : value{_value}, string{_string}
    {
    }
    NonCopyableData(const NonCopyableData& data) = delete;
    NonCopyableData& operator=(const NonCopyableData& data) = delete;
    NonCopyableData(NonCopyableData&& data) = default;
    NonCopyableData& operator=(NonCopyableData&& data) = default;
};

using NonCopyableStorage = MemoryStorage<NonCopyableData>;
using NonCopyableStorageTraits = MemoryStorageTraits<NonCopyableData>;
using NonCopyableProvider = Provider<NonCopyableStorage, NonCopyableStorageTraits>;

class NonCopyableTest : public ::testing::Test
{
public:
    NonCopyableTest()
    {
        auto nonCopyableDataSet = std::vector<NonCopyableData>{};
        nonCopyableDataSet.emplace_back(42, "John");
        nonCopyableDataSet.emplace_back(77, "Jack");
        nonCopyableDataSet.emplace_back(14, "James");

        m_provider.modifier()->append(
            std::make_move_iterator(nonCopyableDataSet.begin()),
            std::make_move_iterator(nonCopyableDataSet.end()));
    }

protected:
    NonCopyableStorage m_provider;
};

TEST_F(NonCopyableTest, count)
{
    EXPECT_EQ(3, m_provider.count());
}

TEST_F(NonCopyableTest, at)
{
    EXPECT_EQ(77, m_provider.at(1).value);
    EXPECT_EQ("James", m_provider.at(2).string);
}

TEST_F(NonCopyableTest, modifierMultiAppend)
{
    auto modifier = m_provider.modifier();
    ASSERT_NE(nullptr, modifier);
    auto values = std::vector<NonCopyableData>{};
    values.emplace_back(152, "Robert");
    values.emplace_back(1099, "Brady");
    modifier->append(
        std::make_move_iterator(values.begin()), std::make_move_iterator(values.end()));
    EXPECT_EQ(5, m_provider.count());
    auto it = m_provider.find([](const auto& data) { return data.string == "Brady"; });
    ASSERT_NE(m_provider.end(), it);
    EXPECT_EQ(1099, it->value);
    EXPECT_EQ("Brady", it->string);
}
