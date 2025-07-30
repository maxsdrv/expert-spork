/** @file
 * @brief      Service plugin for testing.
 *
 *
 * $Id: $
 */

#pragma once

#include "plugins/ServicePluginInterface.h"

#include "../DummyPluginInterface.h"

#include <QObject>
#include <QtPlugin>

namespace appkit
{

namespace plugins
{

/**
 * Dummy widget for testing
 */

class ServicePlugin : public QObject,
                      public ServicePluginInterface,
                      public DummyPluginInterface
{
    Q_OBJECT

    Q_PLUGIN_METADATA(IID "com.pkb-appkit.dev.ServicePluginInterface")
    Q_INTERFACES(appkit::plugins::ServicePluginInterface
                     appkit::plugins::DummyPluginInterface)

public:
    ServicePlugin()
    {
        m_text = "service";
    }

    QString text() const override
    {
        return m_text;
    }
};

} // namespace plugins

} // namespace appkit
