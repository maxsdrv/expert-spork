/** @file
 * @brief Qt types printers
 *
 * $Id: $
 */

#pragma once

#include <gtest/gtest-printers.h>

#include <QMap>
#include <QModelIndex>
#include <QPair>
#include <QString>
#include <QUuid>

class QByteArray;
class QDateTime;

/*
 * PrintTo functions must be implemented in namespace of printed objects
 */
QT_BEGIN_NAMESPACE

void PrintTo(const QString& string, ::std::ostream* stream);
void PrintTo(const QByteArray& array, ::std::ostream* stream);
void PrintTo(const QUuid& uid, ::std::ostream* stream);
void PrintTo(const QRegularExpression& regexp, ::std::ostream* stream);
void PrintTo(const QDateTime& dateTime, ::std::ostream* stream);

template <typename FirstValueType, typename SecondValueType>
void PrintTo(const QPair<FirstValueType, SecondValueType>& pair, ::std::ostream* stream)
{
    *stream << "QPair(";
    ::testing::internal::UniversalPrint(pair.first, stream);
    *stream << ", ";
    ::testing::internal::UniversalPrint(pair.second, stream);
    *stream << ")";
}

template <typename ItemsType>
void PrintTo(const QList<ItemsType>& container, ::std::ostream* stream)
{
    *stream << "QList(";
    for (auto it = container.constBegin(); it != container.constEnd(); ++it)
    {
        ::testing::internal::UniversalPrint(*it, stream);

        auto end = it;
        ++end;
        if (end != container.constEnd())
        {
            *stream << ", ";
        }
    }
    *stream << ")";
}

template <typename KeyType, typename ValueType, typename Iterator>
void PrintTo(const QKeyValueIterator<KeyType, ValueType, Iterator>& it, ::std::ostream* stream)
{
    *stream << "(";
    ::testing::internal::UniversalPrint((*it).first, stream);
    *stream << ", ";
    ::testing::internal::UniversalPrint((*it).second, stream);
    *stream << ")";
}

template <typename KeyType, typename ValueType>
void PrintTo(const QMap<KeyType, ValueType>& container, ::std::ostream* stream)
{
    *stream << "QMap(";
    for (auto it = container.constKeyValueBegin(); it != container.constKeyValueEnd(); ++it)
    {
        ::testing::internal::UniversalPrint(it, stream);

        auto end = it;
        ++end;
        if (end != container.constKeyValueEnd())
        {
            *stream << ", ";
        }
    }
    *stream << ")";
}

void PrintTo(const QModelIndex& index, ::std::ostream* stream);

QT_END_NAMESPACE
