/** @file
 * @brief  Wraps Qt plugin functionality
 *
 * @ingroup
 *
 *
 * $Id: $
 */

#pragma once

#include "mpkplugins_export.h"

#include <boost/noncopyable.hpp>

#include <QObject>

#include <memory>

class QPluginLoader;
class QDir;

namespace appkit::plugins
{

/**
 * This class wraps Qt plugin functionality
 */
class MPKPLUGINS_EXPORT PluginWrapper : private boost::noncopyable
{
public:
    /**
     * Load plugin with @a fileName from @a path
     */
    PluginWrapper(const QString& fileName, const QDir& path);

    /**
     * Add plugin @a object with @a name. Wrapper takes ownership over object
     */
    PluginWrapper(const QString& name, QObject* object);
    ~PluginWrapper();

    /**
     * Return true if plugin support interface
     */
    template <typename I>
    bool isInterfaceSupported() const
    {
        return instance<I>() != nullptr;
    }

    /**
     * Return plugin instance converted to I*
     */
    template <class I>
    I* instance() const
    {
        return qobject_cast<I*>(instance());
    }

    /**
     * Return raw plugin instance
     */
    QObject* instance() const;

    /**
     * Return plugin class name
     */
    QString className() const;

    /**
     * Return plugin library name
     */
    QString libraryName() const;

private:
    QString m_libraryName;
    std::unique_ptr<QPluginLoader> m_loader;

    QObject* m_instance;
    QString m_className;
};

} // namespace appkit::plugins
