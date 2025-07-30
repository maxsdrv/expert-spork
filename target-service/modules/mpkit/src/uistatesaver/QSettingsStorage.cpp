/** @file
 * @brief   Implementation of KeyValueStorage for saving QByteArray to
 *          QSettings
 *
 * $Id: $
 */

#include "QSettingsStorage.h"

#include <QCoreApplication>

namespace uistatesaver
{

QSettingsStorage::QSettingsStorage(const QString& filename) :
  m_settings(
      QSettings::IniFormat, QSettings::UserScope, QCoreApplication::organizationName(), filename)
{
}

void QSettingsStorage::set(const QString& id, const QByteArray& state)
{
    m_settings.setValue(id, state.toBase64());
}

QByteArray QSettingsStorage::get(const QString& id)
{
    return QByteArray::fromBase64(m_settings.value(id).toByteArray());
}

} // namespace uistatesaver
