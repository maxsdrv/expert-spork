/**  @file
 *  @brief Service plugin interface
 *
 *
 *  $Id: $
 */

#pragma once

#include <QtPlugin>

namespace appkit
{

namespace plugins
{

/**
 * Interface for preventing plugins from loading in non-service mode
 */
class ServicePluginInterface
{
public:
    // No methods is here. All we need - to know that plugin implements this interface
    virtual ~ServicePluginInterface() = default;
};

} // namespace plugins

} // namespace appkit

// NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
Q_DECLARE_INTERFACE(
    appkit::plugins::ServicePluginInterface, "com.appkit.dev.ServicePluginInterface")
