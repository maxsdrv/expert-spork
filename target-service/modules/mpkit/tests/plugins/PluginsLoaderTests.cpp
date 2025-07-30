/*
 * @file
 * @brief Test for PluginsLoader
 *
 *
 * $Id: $
 */

#include "DummyPluginInterface.h"

#include "exception/InvalidPath.h"
#include "plugins/PluginsHolder.h"
#include "plugins/PluginsLoader.h"
#include "plugins/ServicePluginInterface.h"

#include "boost/SignalTracker.h"

#include "gqtest"

#include <gmock/gmock.h>

#include <boost/bind.hpp>

#include <algorithm>

namespace appkit
{

namespace plugins
{

class SignalsSpy
{
public:
    MOCK_CONST_METHOD1(probing, void(const QString&));
    MOCK_CONST_METHOD1(loaded, void(const QString&));
    MOCK_CONST_METHOD1(skipped, void(const QString&));
    MOCK_CONST_METHOD2(failed, void(const QString&, const QString&));
};

class PluginsLoaderTest : public testing::Test
{
public:
    PluginsLoaderTest()
    {
        pluginLoader.onProbing(
            boost::bind(&SignalsSpy::probing, boost::cref(spy), _1), tracker());
        pluginLoader.onLoaded(
            boost::bind(&SignalsSpy::loaded, boost::cref(spy), _1), tracker());
        pluginLoader.onSkipped(
            boost::bind(&SignalsSpy::skipped, boost::cref(spy), _1), tracker());
        pluginLoader.onFailed(
            boost::bind(&SignalsSpy::failed, boost::cref(spy), _1, _2),
            tracker());
    }

protected:
    PluginsLoader pluginLoader;
    SignalsSpy spy;

    boostsignals::SignalTracker tracker;
};

TEST_F(PluginsLoaderTest, loadAllInNonserviceMode)
{
    EXPECT_CALL(spy, probing(testing::_)).Times(testing::Exactly(6));
    EXPECT_CALL(spy, loaded(testing::_)).Times(testing::Exactly(1));
    EXPECT_CALL(spy, skipped(testing::_)).Times(testing::Exactly(5));
    EXPECT_CALL(spy, failed(testing::_, testing::_)).Times(testing::Exactly(0));

    pluginLoader.load(false, PLUGIN_PATH, "");
    pluginLoader.load(false, "", PLUGIN_PATH);

    EXPECT_EQ(1UL, pluginLoader.size());

    auto plugins = pluginLoader.plugins<DummyPluginInterface>();
    ASSERT_EQ(1UL, plugins.size());
    EXPECT_EQ(
        "non service",
        plugins.front()->instance<DummyPluginInterface>()->text());
}

TEST_F(PluginsLoaderTest, loadAllInServiceMode)
{
    EXPECT_CALL(spy, probing(testing::_)).Times(testing::Exactly(6));
    EXPECT_CALL(spy, loaded(testing::_)).Times(testing::Exactly(2));
    EXPECT_CALL(spy, skipped(testing::_)).Times(testing::Exactly(4));
    EXPECT_CALL(spy, failed(testing::_, testing::_)).Times(testing::Exactly(0));

    pluginLoader.load(true, PLUGIN_PATH);
    pluginLoader.load(true, "", PLUGIN_PATH);

    auto plugins = pluginLoader.plugins<DummyPluginInterface>();
    EXPECT_EQ(2UL, plugins.size());
}

TEST_F(PluginsLoaderTest, testPluginsLoaderIterator)
{
    pluginLoader.load(true, PLUGIN_PATH);
    EXPECT_EQ(2, std::distance(pluginLoader.begin(), pluginLoader.end()));
}

TEST_F(PluginsLoaderTest, testPluginsLoaderRange)
{
    pluginLoader.load(true, PLUGIN_PATH);

    bool nonServiceFound = std::any_of(
        pluginLoader.begin(),
        pluginLoader.end(),
        boost::bind(&PluginWrapper::className, _1)
            == "appkit::plugins::NonServicePlugin");

    EXPECT_TRUE(nonServiceFound);

    bool nonExistNotFound = std::any_of(
        pluginLoader.begin(),
        pluginLoader.end(),
        boost::bind(&PluginWrapper::className, _1) == "bla-bla-bla");
    EXPECT_FALSE(nonExistNotFound);
}

TEST_F(PluginsLoaderTest, loadFromNonexistingPath)
{
    EXPECT_THROW(
        pluginLoader.load(true, "The path never exists"),
        exception::InvalidPath);
}

TEST_F(PluginsLoaderTest, manualLoadAndSkipByClassName)
{

    EXPECT_CALL(spy, probing(testing::_)).Times(testing::Exactly(3));
    EXPECT_CALL(spy, loaded(testing::_)).Times(testing::Exactly(0));
    EXPECT_CALL(spy, skipped(testing::_)).Times(testing::Exactly(3));
    EXPECT_CALL(spy, failed(testing::_, testing::_)).Times(testing::Exactly(0));

    QObject* nonServicePlugin(new QObject);
    pluginLoader.add("appkit::plugins::NonServicePlugin", nonServicePlugin);

    pluginLoader.load(false, PLUGIN_PATH, "");

    auto plugins = pluginLoader.plugins<DummyPluginInterface>();
    EXPECT_EQ(0UL, plugins.size());
}

TEST_F(PluginsLoaderTest, testBasicPluginsHolderUsage)
{
    PluginsLoader pluginLoader;
    pluginLoader.load(true, PLUGIN_PATH);
    EXPECT_EQ(2UL, pluginLoader.size());

    PluginsHolder<ServicePluginInterface> servicePluginsHolder(
        pluginLoader, true);
    EXPECT_EQ(1UL, pluginLoader.size());

    EXPECT_EQ(
        1,
        std::distance(servicePluginsHolder.begin(), servicePluginsHolder.end()));

    auto it = servicePluginsHolder.begin();

    EXPECT_EQ("appkit::plugins::ServicePlugin", it->wrapper->className());

    ++it;
    EXPECT_EQ(it, servicePluginsHolder.end());
}

TEST_F(PluginsLoaderTest, testPluginsHolderAsRange)
{
    PluginsLoader pluginLoader;
    pluginLoader.load(true, PLUGIN_PATH);
    EXPECT_EQ(2UL, pluginLoader.size());

    typedef PluginsHolder<DummyPluginInterface> DummyInterfaceHolder;
    DummyInterfaceHolder servicePluginsHolder(pluginLoader, true);
    EXPECT_EQ(0UL, pluginLoader.size());

    // Just check if it compiles
    for (const auto& iface: servicePluginsHolder)
    {
        iface()->setText("dummy");
    }

    for (const auto& iface: servicePluginsHolder)
    {
        EXPECT_EQ("dummy", iface()->text());
    }
}

} // namespace plugins

} // namespace appkit
