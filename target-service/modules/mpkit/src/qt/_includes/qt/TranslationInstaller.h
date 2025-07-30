/** @file
 * @brief      Helper function to install translations to application
 *
 *
 * $Id: $
 */

#pragma once

#include <QLocale>
#include <QStringList>

class QApplication;

namespace qt
{

/**
 * Install user translation with @a name from @a path for chosen @a locale
 *
 * @return List of installed translations
 */
QStringList installUserTranslation(
    const QString& name, const QString& path, const QLocale& locale = QLocale());

/**
 * Install system translation with @a name for chosen @a locale
 *
 * @return List of installed translations
 */
QStringList installSystemTranslation(const QString& name, const QLocale& locale = QLocale());

/**
 * Install all translations from @a path for chosen locale
 *
 * @return List of installed translations
 */
QStringList installUserTranslations(const QString& path, const QLocale& locale = QLocale());

} // namespace qt
