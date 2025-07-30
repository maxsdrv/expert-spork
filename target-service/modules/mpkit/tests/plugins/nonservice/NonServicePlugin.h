/** @file
 * @brief      NonService plugin for testing.
 *
 *
 * $Id: $
 */

#pragma once

#include "../DummyPluginInterface.h"

#include <QObject>

namespace appkit
{

namespace plugins
{

/**
 * Dummy widget for testing
 */
class NonServicePlugin : public QObject, public DummyPluginInterface
{
    Q_OBJECT

    Q_PLUGIN_METADATA(IID "com.pkb-appkit.dev.DummyPluginInterface")
    Q_INTERFACES(appkit::plugins::DummyPluginInterface)

public:
    QString text() const override
    {
        return m_text.isEmpty() ? "non service" : m_text;
    }
};

} // namespace plugins

} // namespace appkit
