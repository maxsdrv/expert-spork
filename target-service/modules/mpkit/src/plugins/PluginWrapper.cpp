/** @file
 * @brief  Wraps Qt plugin functionality
 *
 * $Id: $
 */

#include "plugins/PluginWrapper.h"

#include "PluginException.h"

#include <QDir>
#include <QPluginLoader>

namespace appkit::plugins
{

PluginWrapper::PluginWrapper(const QString& fileName, const QDir& path) :
  m_libraryName(fileName),
  m_loader(std::make_unique<QPluginLoader>(path.absoluteFilePath(fileName))),
  m_instance(m_loader->instance())
{
    if (m_instance != nullptr)
    {
        m_className = m_instance->metaObject()->className();
        m_instance->setObjectName(m_className.split("::").back());
    }
    else
    {
        BOOST_THROW_EXCEPTION(PluginException() << PluginError(m_loader->errorString()));
    }
}

PluginWrapper::PluginWrapper(const QString& name, QObject* object) :
  m_libraryName(name), m_instance(object), m_className(name)
{
}

PluginWrapper::~PluginWrapper()
{
    delete m_instance;
    if (m_loader)
    {
        m_loader->unload();
    }
}

QObject* PluginWrapper::instance() const
{
    return m_instance;
}

QString PluginWrapper::className() const
{
    return m_className;
}

QString PluginWrapper::libraryName() const
{
    return m_libraryName;
}

} // namespace appkit::plugins
