/**
 * @file
 * @brief      Plugins holder class.
 *
 * $Id: $
 */

#pragma once

#include "PluginsLoader.h"

#ifndef Q_MOC_RUN // Protect from BOOST_JOIN error
#include <boost/iterator/iterator_facade.hpp>
#endif

#include <vector>

namespace appkit
{

namespace plugins
{

class PluginsLoader;

template <typename PluginInterface>
struct PluginsHolderType
{
    std::shared_ptr<PluginWrapper> wrapper;

    PluginInterface* operator()() const
    {
        return wrapper->instance<PluginInterface>();
    }

    QObject* instance() const
    {
        return wrapper->instance();
    }
};

/**
 * Plugins iterator type (const)
 */
template <typename PluginInterface>
class PluginsHolderIterator : public boost::iterator_facade<
                                  PluginsHolderIterator<PluginInterface>,
                                  const PluginsHolderType<PluginInterface>,
                                  boost::forward_traversal_tag>
{
public:
    using PluginWrapperPtr = std::shared_ptr<PluginWrapper>;
    using value_type = PluginsHolderType<PluginInterface>;

    explicit PluginsHolderIterator(const std::vector<PluginWrapperPtr>::const_iterator& it) :
      m_iterator(it)
    {
    }

private:
    friend class boost::iterator_core_access;

    const value_type& dereference() const
    {
        m_instance.wrapper = *m_iterator;
        return m_instance;
    }

    void increment()
    {
        ++m_iterator;
    }

    bool equal(const PluginsHolderIterator& other) const
    {
        return m_iterator == other.m_iterator;
    }

private:
    mutable value_type m_instance;
    std::vector<PluginWrapperPtr>::const_iterator m_iterator;
};

/**
 * This class takes wrappers
 */
template <typename I>
class PluginsHolder
{
public:
    using iterator = PluginsHolderIterator<I>; // Type of iterator
    using value_type = typename iterator::value_type;

    /**
     * Create plugins holder from loader.
     * This function takes all plugins supports I from loader
     */
    explicit PluginsHolder(PluginsLoader& loader, bool eraseFromLoader = true) :
      m_wrappers(eraseFromLoader ? loader.take<I>() : loader.plugins<I>())
    {
    }

    /**
     * Create plugins holder from plugins vector.
     * This function takes all plugins supports I from loader
     */
    explicit PluginsHolder(std::vector<typename iterator::PluginWrapperPtr> wrappers) :
      m_wrappers(std::move(wrappers))
    {
    }

    /**
     * Return iterator points to begin
     */
    iterator begin() const
    {
        return iterator(m_wrappers.begin());
    }

    /**
     * Return iterator points to end
     */
    iterator end() const
    {
        return iterator(m_wrappers.end());
    }

private:
    std::vector<typename iterator::PluginWrapperPtr> m_wrappers;
};

} // namespace plugins

} // namespace appkit
