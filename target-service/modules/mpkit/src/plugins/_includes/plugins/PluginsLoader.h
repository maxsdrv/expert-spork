/**
 * @file
 * @brief      Plugins loader
 *
 * $Id: $
 */

#pragma once

#include "mpkplugins_export.h"

#include "PluginWrapper.h"

#include "boost/signals2.h"
#ifndef Q_MOC_RUN // Protect from BOOST_JOIN error
#include <boost/iterator/iterator_facade.hpp>
#endif

#include <memory>
#include <vector>

namespace appkit::plugins
{

/**
 * Plugins iterator type (const)
 */
class MPKPLUGINS_EXPORT PluginsLoaderIterator : public boost::iterator_facade<
                                                    PluginsLoaderIterator,
                                                    std::shared_ptr<PluginWrapper> const,
                                                    boost::forward_traversal_tag>
{
public:
    using value_type = std::shared_ptr<PluginWrapper>;

    explicit PluginsLoaderIterator(const std::vector<value_type>::const_iterator& it) :
      m_iterator(it)
    {
    }

private:
    friend class boost::iterator_core_access;

    const value_type& dereference() const
    {
        return *m_iterator;
    }

    void increment()
    {
        ++m_iterator;
    }

    bool equal(const PluginsLoaderIterator& other) const
    {
        return m_iterator == other.m_iterator;
    }

private:
    std::vector<value_type>::const_iterator m_iterator;
};

/**
 * Class for loading plugins. Sending signals about loading progress
 */
class MPKPLUGINS_EXPORT PluginsLoader
{
    ADD_SIGNAL(Probing, void(QString))
    ADD_SIGNAL(Loaded, void(QString))
    ADD_SIGNAL(Skipped, void(QString))
    ADD_SIGNAL(Failed, void(QString, QString))

    template <class I>
    friend class PluginsHolder;

public:
    using iterator = PluginsLoaderIterator; // Type of iterator
    using value_type = iterator::value_type;

    /**
     * Loading plugins
     */
    void load(bool serviceMode, const QString& baseDir, const QString& subDir = QString(""));

    /**
     * Return plugin wrappers implementing certain @a Interface.
     */
    template <typename Interface>
    std::vector<value_type> plugins() const
    {
        std::vector<std::shared_ptr<PluginWrapper>> plugins;
        std::copy_if(
            begin(),
            end(),
            std::back_inserter(plugins),
            [](const std::shared_ptr<PluginWrapper>& plugin)
            { return plugin->isInterfaceSupported<Interface>(); });

        return plugins;
    }

    /**
     * Take loaded plugins.
     */
    template <typename Interface>
    std::vector<value_type> take()
    {
        std::vector<value_type> _plugins;

        auto it = m_plugins.begin();
        for (; it != m_plugins.end();)
        {
            if ((*it)->isInterfaceSupported<Interface>())
            {
                _plugins.push_back(*it);
                it = m_plugins.erase(it);
            }
            else
            {
                ++it;
            }
        }
        return _plugins;
    }

    /**
     * Return number of loaded plugins
     */
    size_t size() const;

    /**
     * Add custom plugin @a object (created manually). PluginsLoader takes
     * ownership over @a object
     */
    void add(const QString& name, QObject* object);

    /**
     * Return iterator points to begin
     */
    iterator begin() const;

    /**
     * Return iterator points to end
     */
    iterator end() const;

private:
    // Internal plugins storage
    using Plugins = std::vector<std::shared_ptr<PluginWrapper>>;
    Plugins m_plugins;
};

} // namespace appkit::plugins
