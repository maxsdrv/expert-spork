/** @file
 * @brief     Load plugins for factory
 *
 * $Id: $
 */

#pragma once

#include "plugins/PluginsHolder.h"

#include "boost/SignalTracker.h"

#include <functional>

namespace appkit
{

namespace plugins
{

/**
 * Wrapper for loading specific plugins from specific folder.
 * Also provides notification capability (for splash screen)
 */
class FactoryLoader : public QObject
{
    Q_OBJECT

public:
    using Log = std::function<void(const QString&)>;

    explicit FactoryLoader(Log logger, QObject* parent = nullptr);

    /**
     * Do actual plugins loading
     */
    template <typename PluginInterface>
    std::unique_ptr<plugins::PluginsHolder<PluginInterface>> load(
        const QString& path, const QString& subdir)
    {
        using namespace plugins;
        PluginsLoader pluginsLoader;
        loadPlugins(pluginsLoader, path, subdir);

        return std::make_unique<PluginsHolder<PluginInterface>>(pluginsLoader);
    }

private:
    void loadPlugins(PluginsLoader& pluginsLoader, const QString& path, const QString& subdir);

private:
    Log m_logger;

    boostsignals::SignalTracker m_tracker;
};

} // namespace plugins

} // namespace appkit
