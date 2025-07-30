/** @file
 * @brief Qt types printers
 *
 * $Id: $
 */

#include "Printers.h"

#include <QByteArray>
#include <QDateTime>
#include <QRegularExpression>
#include <QUuid>

QT_BEGIN_NAMESPACE

namespace
{

constexpr auto hexmap = "0123456789ABCDEF";

std::string hexString(const QByteArray& array)
{
    std::string s(array.size() * 2, ' ');
    for (int i = 0; i < array.size(); ++i)
    {
        const unsigned char c = array.at(i);
        constexpr auto hiByte = 0xF0U;
        s[2 * i] = hexmap[(c & hiByte) >> 4U];
        constexpr auto lowByte = 0x0FU;
        s[2 * i + 1] = hexmap[c & lowByte];
    }
    return s;
}

} // namespace

void PrintTo(const QString& string, ::std::ostream* stream)
{
    *stream << string.toStdString();
}

void PrintTo(const QByteArray& array, ::std::ostream* stream)
{
    *stream << "QByteArray(" << hexString(array) << ")";
}

void PrintTo(const QRegularExpression& regexp, std::ostream* stream)
{
    PrintTo(regexp.pattern(), stream);
}

void PrintTo(const QList<int>::const_iterator& iterator, ::std::ostream* stream)
{
    *stream << *iterator;
}

void PrintTo(const QList<int>::iterator& iterator, ::std::ostream* stream)
{
    testing::internal::UniversalPrint(QList<int>::const_iterator(iterator), stream);
}

void PrintTo(QMap<int, int>::const_iterator iterator, ::std::ostream* stream)
{
    *stream << *iterator;
}

void PrintTo(QUuid uid, std::ostream* stream)
{
    PrintTo(uid.toString(), stream);
}

void PrintTo(const QDateTime& dateTime, ::std::ostream* stream)
{
    PrintTo(dateTime.toString(), stream);
}

void PrintTo(const QModelIndex& index, ::std::ostream* stream)
{
    *stream << "(" << index.row() << ", " << index.column() << ", " << index.internalPointer()
            << ")";
}

void PrintTo(QMap<int, int>::iterator iterator, ::std::ostream* stream)
{
    testing::internal::UniversalPrint(QMap<int, int>::const_iterator(iterator), stream);
}

QT_END_NAMESPACE
