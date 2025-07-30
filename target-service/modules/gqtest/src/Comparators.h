/** @file
 * @brief A set of comparators for common objects
 *
 * $Id: $
 */

#pragma once

#include "Printers.h"

#include <gtest/gtest.h>

class QObject;

namespace gqtest
{

/**
 * Compares two QStrings for equality
 **/
template <typename T>
testing::AssertionResult isEqual(const T& expected, const T& actual)
{
    if (expected != actual)
    {
        return ::testing::AssertionFailure()
               << QString("Expected=%1, actual=%2")
                      .arg(expected)
                      .arg(actual)
                      .toStdString();
    }

    return ::testing::AssertionSuccess();
}

/**
 * Compares two QMap's
 **/
template <typename K, typename V>
testing::AssertionResult
isEqual(const QMap<K, V>& expected, const QMap<K, V>& actual)
{
    if (expected.size() != actual.size())
    {
        return ::testing::AssertionFailure()
               << QString("Expected size \"%1\" does not match actual \"%2\"")
                      .arg(expected.size())
                      .arg(actual.size())
                      .toStdString();
    }

    typename QMap<K, V>::const_iterator expectedFirst = expected.constBegin();
    typename QMap<K, V>::const_iterator expectedLast = expected.constEnd();
    typename QMap<K, V>::const_iterator actualFirst = actual.constBegin();

    for (; expectedFirst != expectedLast; ++expectedFirst, ++actualFirst)
    {
        if (expectedFirst.key() != actualFirst.key())
        {
            return ::testing::AssertionFailure()
                   << QString("Expected key \"%1\" does not match actual key "
                              "\"%2\"")
                          .arg(expectedFirst.key())
                          .arg(actualFirst.key())
                          .toStdString();
        }

        ::testing::AssertionResult result
            = isEqual(expectedFirst.value(), actualFirst.value());
        if (!result)
        {
            return ::testing::AssertionFailure()
                   << result.message()
                   << QString(" for key %1")
                          .arg(expectedFirst.key())
                          .toStdString();
        }
    }

    return ::testing::AssertionSuccess();
}

/**
 * Compares two QList's
 **/
template <typename V>
testing::AssertionResult
isEqual(const QList<V>& expected, const QList<V>& actual)
{
    if (expected.size() != actual.size())
    {
        return ::testing::AssertionFailure()
               << QString("Expected size \"%1\" does not match actual \"%2\"")
                      .arg(expected.size())
                      .arg(actual.size())
                      .toStdString();
    }

    for (int i = 0; i < expected.size(); ++i)
    {
        ::testing::AssertionResult result(
            isEqual(expected.at(i), actual.at(i)));
        if (!result)
        {
            return ::testing::AssertionFailure()
                   << QString(" for i=%1").arg(i).toStdString();
        }
    }

    return ::testing::AssertionSuccess();
}

/**
 * Compares two QSet's
 **/
template <typename V>
testing::AssertionResult isEqual(const QSet<V>& expected, const QSet<V>& actual)
{
    if (expected.size() != actual.size())
    {
        return ::testing::AssertionFailure()
               << QString("Expected size \"%1\" does not match actual \"%2\"")
                      .arg(expected.size())
                      .arg(actual.size())
                      .toStdString();
    }

    for (typename QSet<V>::const_iterator expectedIt = expected.constBegin(),
                                          actualIt = actual.constBegin();
         expectedIt != expected.constEnd() && actualIt != actual.constEnd();
         ++expectedIt, ++actualIt)
    {
        ::testing::AssertionResult result(isEqual(*expectedIt, *actualIt));
        if (!result)
        {
            return ::testing::AssertionFailure()
                   << QString("Expected: %1, actual: %2")
                          .arg(*expectedIt)
                          .arg(*actualIt)
                          .toStdString();
        }
    }

    return ::testing::AssertionSuccess();
}

/**
 * Compares two strings that are comma-separated list whit
 **/
testing::AssertionResult fuzzyStringCompare(
    const QString& expected,
    const QString& actual,
    const QString& separator = ",",
    bool ignoreWhitespace = true);

/**
 * Compares two QObjects for equality. Objects are equal if all properties are
 *equals
 **/
testing::AssertionResult
isEqual(const QObject& expected, const QObject& actual);

} // namespace gqtest
