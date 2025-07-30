/** @file
 * @brief QFile helper functions
 *
 * $Id: $
 */

#include "qt/Files.h"

#include "exception/InvalidPath.h"
#include "qt/Strings.h"

#include <QFile>
#include <QTextStream>

namespace qt
{

QString loadUtf8File(const QString& fileName)
{
    QFile file(fileName);
    bool opened = file.open(QIODevice::ReadOnly | QIODevice::Text);
    if (!opened)
    {
        BOOST_THROW_EXCEPTION(exception::InvalidPath(strings::toUtf8(fileName)));
    }

    QTextStream stream(&file);
#if QT_MAJOR < 6
    stream.setCodec("UTF-8");
#else
    stream.setEncoding(QStringConverter::Encoding::Utf8);
#endif
    stream.setAutoDetectUnicode(true);

    return stream.readAll();
}

} // namespace qt
