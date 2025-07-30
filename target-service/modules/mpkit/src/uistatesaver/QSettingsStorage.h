/** @file
 * @brief   Implementation of KeyValueStorage for saving QByteArray to
 *          QSettings
 * $Id: $
 */

#pragma once

#include "uistatesaver/KeyValueStorage.h"

#include <QByteArray>
#include <QSettings>

namespace uistatesaver
{

class QSettingsStorage : public KeyValueStorage<QString, QByteArray>
{
public:
    explicit QSettingsStorage(const QString& filename);
    /**
     * Save state for specified id
     */
    void set(const QString& id, const QByteArray& state) override;

    /**
     * Return state for specified id
     */
    QByteArray get(const QString& id) override;

private:
    QSettings m_settings;
};

} // namespace uistatesaver
