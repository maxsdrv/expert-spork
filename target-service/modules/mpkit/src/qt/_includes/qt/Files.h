/** @file
 * @brief QFile helper functions
 *
 *
 * $Id: $
 */

#pragma once

#include <QString>

namespace qt
{

/**
 * @return all content from text file with specified filename.
 */
QString loadUtf8File(const QString& fileName);

} // namespace qt
