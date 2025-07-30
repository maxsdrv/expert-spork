/** @file
 * @brief     Load plugins for factory
 *
 * $Id: $
 */

#include "plugins/FactoryLoader.h"

#include "gsl/assert"

namespace appkit
{

namespace plugins
{

FactoryLoader::FactoryLoader(std::function<void(const QString&)> logger, QObject* parent) :
  QObject(parent), m_logger(std::move(logger))
{
    Expects(m_logger);
}

void FactoryLoader::loadPlugins(
    PluginsLoader& pluginsLoader, const QString& path, const QString& subdir)
{
    m_logger(tr("Loading plugins from %1/%2").arg(path, subdir));

    pluginsLoader.onProbing(
        [logger = m_logger](const QString& pluginName)
        { logger(QObject::tr("Loading %1").arg(pluginName)); },
        m_tracker());

    pluginsLoader.onSkipped(
        [logger = m_logger](const QString& pluginName)
        { logger(QObject::tr("Skipped %1").arg(pluginName)); },
        m_tracker());

    pluginsLoader.onLoaded(
        [logger = m_logger](const QString& pluginName)
        { logger(QObject::tr("Loaded %1").arg(pluginName)); },
        m_tracker());

    pluginsLoader.onFailed(
        [logger = m_logger](const QString& pluginName, const QString& error)
        { logger(QObject::tr("Failed to load %1: %2").arg(pluginName, error)); },
        m_tracker());

    pluginsLoader.load(false, path, subdir);
}

} // namespace plugins

} // namespace appkit
