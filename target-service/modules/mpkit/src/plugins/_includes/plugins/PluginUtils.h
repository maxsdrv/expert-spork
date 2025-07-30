/**
 * @file
 * @brief      Plugins helper functions
 *
 * $Id: $
 */

#pragma once

#include <QObject>

namespace appkit
{

namespace plugins
{

/**
 * This function is trying to cast @a objectPLugin to interface @a I,
 * then call fn for it.
 */
template <typename I, typename Predicate>
bool initPlugin(QObject* objectPlugin, Predicate fn)
{
    I* plugin = qobject_cast<I*>(objectPlugin);

    // TODO: strange workaround. It seems some plugins does not implement certain interface
    if (!plugin)
    {
        plugin = dynamic_cast<I*>(objectPlugin); // clazy:exclude=unneeded-cast
    }

    if (plugin)
    {
        fn(plugin);
        return true;
    }
    return false;
}

} // namespace plugins

} // namespace appkit
