/**
 * @file
 * @brief      Plugins loader
 *
 * $Id: $
 */

#include "plugins/PluginsLoader.h"

#include "plugins/ServicePluginInterface.h"

#include "PluginException.h"

#include "exception/InvalidPath.h"
#include "qt/Strings.h"

#include <QDir>
#include <QLibrary>

#include <boost/algorithm/cxx11/any_of.hpp>
#include <boost/bind.hpp>

namespace appkit
{

namespace plugins
{

void PluginsLoader::load(bool serviceMode, const QString& baseDir, const QString& subDir)
{
    QDir pluginDir(baseDir);
    if (!(pluginDir.exists()) || !pluginDir.cd(subDir))
    {
        BOOST_THROW_EXCEPTION(exception::InvalidPath(strings::toUtf8(pluginDir.absolutePath())));
    }

    QStringList pluginList = pluginDir.entryList(QStringList() << "*", QDir::Files);

    for (const auto& pluginName: pluginList)
    {
        m_Probing(pluginName);
        bool pluginNameHasBeenLoaded = boost::algorithm::any_of(
            m_plugins,
            // NOLINTNEXTLINE
            boost::bind(&PluginWrapper::libraryName, _1) == pluginName);
        if (pluginNameHasBeenLoaded || !QLibrary::isLibrary(pluginName))
        {
            m_Skipped(pluginName);
            continue;
        }

        try
        {
            auto wrapper = std::make_shared<PluginWrapper>(pluginName, pluginDir);
            if (serviceMode || !wrapper->isInterfaceSupported<ServicePluginInterface>())
            {
                bool pluginClassHasBeenLoaded = boost::algorithm::any_of(
                    m_plugins,
                    // NOLINTNEXTLINE
                    boost::bind(&PluginWrapper::className, _1) == wrapper->className());

                // Skip loaded classes
                if (!pluginClassHasBeenLoaded)
                {
                    m_plugins.push_back(wrapper);
                    m_Loaded(pluginName);
                }
                else
                {
                    m_Skipped(pluginName);
                }
            }
            else
            {
                m_Skipped(pluginName);
            }
        }
        catch (const plugins::PluginException& ex)
        {
            // NOLINTNEXTLINE
            auto info = boost::get_error_info<plugins::PluginError>(ex);
            m_Failed(pluginName, info != nullptr ? *info : QObject::tr("Unknown error"));
        }
    }
}

size_t PluginsLoader::size() const
{
    return m_plugins.size();
}

void PluginsLoader::add(const QString& name, QObject* object)
{
    bool pluginClassHasBeenLoaded = boost::algorithm::any_of(
        m_plugins,
        boost::bind(&PluginWrapper::className, _1) == name); // NOLINT(modernize-avoid-bind)

    if (!pluginClassHasBeenLoaded)
    {
        m_plugins.push_back(std::make_shared<PluginWrapper>(name, object));
    }
}

PluginsLoader::iterator PluginsLoader::begin() const
{
    return iterator(m_plugins.begin());
}

PluginsLoader::iterator PluginsLoader::end() const
{
    return iterator(m_plugins.end());
}

} // namespace plugins

} // namespace appkit
