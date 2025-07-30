#pragma once

#include "PluginsHolder.h"

#include "exception/General.h"

#include <algorithm>

namespace exception
{

class MissingPlugin : public virtual General
{
};

} // namespace exception

namespace appkit::plugins
{

template <typename PluginInterface>
class PluginLocator
{
public:
    using Holder = PluginsHolder<PluginInterface>;

    explicit PluginLocator(Holder holder) : m_plugins(std::move(holder))
    {
    }
    /**
     * Return plugin by its type, throw if no plugin is found
     */
    PluginInterface* plugin(const QString& type) const
    {
        auto it = std::find_if(
            m_plugins.begin(),
            m_plugins.end(),
            [type](auto wrapper) { return wrapper()->type() == type; });

        if (it == m_plugins.end())
        {
            BOOST_THROW_EXCEPTION(exception::MissingPlugin());
        }

        return (*it)();
    }

    const Holder& holder() const
    {
        return m_plugins;
    }

private:
    Holder m_plugins;
};

} // namespace appkit::plugins
