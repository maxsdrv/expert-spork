/**
 *  @file
 *  @brief Dummy plugin interface
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
 * Dummy plugin interface for testing.
 */
class DummyPluginInterface
{
public:
    virtual QString text() const = 0;
    virtual void setText(const QString& text)
    {
        m_text = text;
    }
    virtual ~DummyPluginInterface() {}

protected:
    QString m_text;
};

} // namespace plugins

} // namespace appkit

Q_DECLARE_INTERFACE(
    appkit::plugins::DummyPluginInterface,
    "com.pkb-appkit.dev.DummyPluginInterface")
