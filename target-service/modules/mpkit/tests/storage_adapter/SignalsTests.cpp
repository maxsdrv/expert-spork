#include "storage_adapter/MemoryStorage.h"
#include "storage_adapter/Provider.h"
#include "storage_adapter/StorageModifier.h"

#include "ApplicationFixture.h"

#include <gmock/gmock.h>

using namespace mpk::storage_adapter;

class PinMock : public QObject
{
    Q_OBJECT

public:
    explicit PinMock(QObject* parent = nullptr) : QObject(parent)
    {
    }

public:
    void setup(gsl::not_null<const ProviderSignals*> pin)
    {
        connect(pin, &ProviderSignals::aboutToAppend, this, &PinMock::aboutToAppend);
        connect(pin, &ProviderSignals::appendCompleted, this, &PinMock::appendCompleted);
        connect(pin, &ProviderSignals::aboutToRemove, this, &PinMock::aboutToRemove);
        connect(pin, &ProviderSignals::removeCompleted, this, &PinMock::removeCompleted);
        connect(pin, &ProviderSignals::aboutToReset, this, &PinMock::aboutToReset);
        connect(pin, &ProviderSignals::resetCompleted, this, &PinMock::resetCompleted);
        connect(pin, &ProviderSignals::changed, this, &PinMock::changed);
        connect(pin, &ProviderSignals::editingFinished, this, &PinMock::editingFinished);
    }

public:
    MOCK_METHOD(void, aboutToAppend, (), ());
    MOCK_METHOD(void, appendCompleted, (int), ());
    MOCK_METHOD(void, aboutToRemove, (int), ());
    MOCK_METHOD(void, removeCompleted, (), ());
    MOCK_METHOD(void, aboutToReset, (), ());
    MOCK_METHOD(void, resetCompleted, (), ());
    MOCK_METHOD(void, changed, (int), ());
    MOCK_METHOD(void, editingFinished, (), ());
};

#include "SignalsTests.moc"

struct Data
{
    int value;
    std::string string;
};

using DataStorage = MemoryStorage<Data>;
using DataStorageTraits = MemoryStorageTraits<Data>;
using DataProvider = Provider<DataStorage, DataStorageTraits>;

const auto dataSet = {Data{42, "John"}, Data{77, "Jack"}, Data{14, "James"}};

class MemoryPinTest : public ::testing::Test
{
public:
    MemoryPinTest()
    {
        m_provider.modifier()->append(dataSet.begin(), dataSet.end());
    }

    void SetUp() override
    {
        m_mock.setup(m_provider.pin());
    }

protected:
    DataStorage m_provider;
    PinMock m_mock;
    gqtest::ApplicationFixture m_app;
};

using ::testing::Eq;
using ::testing::Exactly;

TEST_F(MemoryPinTest, pinAppend)
{
    auto modifier = m_provider.modifier();
    ASSERT_NE(nullptr, modifier);
    auto value = Data{152, "Robert"};
    EXPECT_CALL(m_mock, aboutToAppend()).Times(Exactly(1));
    EXPECT_CALL(m_mock, appendCompleted(Eq(3))).Times(Exactly(1));
    EXPECT_CALL(m_mock, editingFinished()).Times(Exactly(1));
    modifier->append(value);
}

TEST_F(MemoryPinTest, pinRemove)
{
    auto modifier = m_provider.modifier();
    ASSERT_NE(nullptr, modifier);
    EXPECT_CALL(m_mock, aboutToRemove(Eq(2))).Times(Exactly(1));
    EXPECT_CALL(m_mock, removeCompleted()).Times(Exactly(1));
    EXPECT_CALL(m_mock, editingFinished()).Times(Exactly(1));
    auto it = m_provider.find([](const auto& data) { return data.string == "James"; });
    ASSERT_NE(m_provider.end(), it);
    modifier->remove(it);
}

TEST_F(MemoryPinTest, pinReset)
{
    auto modifier = m_provider.modifier();
    ASSERT_NE(nullptr, modifier);
    EXPECT_CALL(m_mock, aboutToReset()).Times(Exactly(1));
    EXPECT_CALL(m_mock, resetCompleted()).Times(Exactly(1));
    EXPECT_CALL(m_mock, editingFinished()).Times(Exactly(1));
    modifier->clear();
}

TEST_F(MemoryPinTest, pinChanged)
{
    auto modifier = m_provider.modifier();
    ASSERT_NE(nullptr, modifier);
    EXPECT_CALL(m_mock, changed(Eq(2))).Times(Exactly(1));
    EXPECT_CALL(m_mock, editingFinished()).Times(Exactly(1));
    auto value = Data{152, "Robert"};
    auto it = m_provider.find([](const auto& data) { return data.string == "James"; });
    ASSERT_NE(m_provider.end(), it);
    modifier->replace(it, value);
}
