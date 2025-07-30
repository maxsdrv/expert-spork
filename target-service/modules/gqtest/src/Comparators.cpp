/** @file
 * @brief This file contains a set of comparators for testing purposes
 *
 * $Id: $
 */

#include "Comparators.h"

#include <QMetaProperty>
#include <QObject>
#include <QSet>
#include <QSharedPointer>
#include <QStringList>
#include <QUuid>

#include <memory>

Q_DECLARE_METATYPE(QUuid)

namespace gqtest
{

testing::AssertionResult fuzzyStringCompare(
    const QString& expected,
    const QString& actual,
    const QString& separator,
    bool ignoreWhitespace)
{
    QSet<QString> expectedSet;
    QSet<QString> actualSet;

    const auto& expectedValues = expected.split(separator);
    for (const auto& value: expectedValues)
    {
        expectedSet.insert(ignoreWhitespace ? value.trimmed() : value);
    }

    const auto& actualValues = actual.split(separator);
    for (const auto& value: actualValues)
    {
        actualSet.insert(ignoreWhitespace ? value.trimmed() : value);
    }

    if (actualSet == expectedSet)
    {
        return ::testing::AssertionSuccess();
    }

    return ::testing::AssertionFailure()
           << QStringLiteral("Expected '%1' does not match actual '%2'")
                  .arg(expected, actual)
                  .toStdString();
}

namespace
{

class Comparator
{
public:
    Comparator() = default;
    Comparator(const Comparator&) = delete;
    Comparator& operator=(const Comparator&) = delete;

    virtual testing::AssertionResult compare(const QVariant& expected, const QVariant& actual) = 0;
    virtual ~Comparator() = default;
};

using ComparatorPtr = std::shared_ptr<Comparator>;

template <class T>
class CustomComparator : public Comparator
{
public:
    testing::AssertionResult compare(const QVariant& expected, const QVariant& actual) override
    {
        if (expected.value<T>() != actual.value<T>())
        {
            return ::testing::AssertionFailure() << QStringLiteral("Expected='%1'', actual='%2'")
                                                        .arg(expected.value<T>().toString())
                                                        .arg(actual.value<T>().toString())
                                                        .toStdString();
        }
        return ::testing::AssertionSuccess();
    }
};

ComparatorPtr getComparator(int userType)
{
    static QMap<int, ComparatorPtr> comparators;
    if (comparators.isEmpty())
    {
        comparators.insert(
            QVariant::fromValue(QUuid()).userType(), std::make_shared<CustomComparator<QUuid>>());
    }
    return comparators.value(userType, ComparatorPtr());
}

testing::AssertionResult compareProperties(
    const QObject& expected, const QObject& actual, const char* name)
{
    QVariant expectedProperty = expected.property(name);
    QVariant actualProperty = actual.property(name);
#if QT_MAJOR < 6
    if (expectedProperty.type() < QVariant::UserType)
#else
    if (expectedProperty.metaType().id() < QMetaType::Type::User)
#endif
    {
        if (actualProperty != expectedProperty)
        {
            return ::testing::AssertionFailure()
                   << QStringLiteral("Property '%1' mismatch. Expected='%2'', actual='%3'")
                          .arg(name, expectedProperty.toString(), actualProperty.toString())
                          .toStdString();
        }
    }
    else
    {
        if (actualProperty.userType() == expectedProperty.userType())
        {
            ComparatorPtr comparator = getComparator(actualProperty.userType());
            if (!comparator)
            {
                return ::testing::AssertionFailure()
                       << QStringLiteral("No comparator for user type '%1'")
                              .arg(actualProperty.userType())
                              .toStdString();
            }

            testing::AssertionResult result =
                comparator->compare(expectedProperty, actualProperty);
            if (!result)
            {
                return ::testing::AssertionFailure()
                       << QStringLiteral("Property '%1' mismatch. %2")
                              .arg(name, result.failure_message())
                              .toStdString();
            }
        }
        else
        {
            return ::testing::AssertionFailure()
                   << QStringLiteral("Property '%1' mismatch. User types are differ")
                          .arg(name)
                          .toStdString();
        }
    }
    return ::testing::AssertionSuccess();
}

testing::AssertionResult compareOwnProperties(const QObject& expected, const QObject& actual)
{
    const QMetaObject* metaObject(expected.metaObject());
    for (int i = 0; i < metaObject->propertyCount(); ++i)
    {
        QMetaProperty metaProperty(metaObject->property(i));

        testing::AssertionResult result =
            compareProperties(expected, actual, metaProperty.name());
        if (!result)
        {
            return result;
        }
    }

    return ::testing::AssertionSuccess();
}

testing::AssertionResult compareDynamicProperties(const QObject& expected, const QObject& actual)
{
    const auto& names = expected.dynamicPropertyNames();
    for (const auto& name: names)
    {
        testing::AssertionResult result = compareProperties(expected, actual, name);
        if (!result)
        {
            return result;
        }
    }

    return testing::AssertionSuccess();
}

} // namespace

testing::AssertionResult isEqual(const QObject& expected, const QObject& actual)
{
    testing::AssertionResult compareOwn = compareOwnProperties(expected, actual);
    return compareOwn ? compareDynamicProperties(expected, actual) : compareOwn;
}

testing::AssertionResult isEqual(int expected, int actual)
{
    if (expected != actual)
    {
        return testing::AssertionFailure()
               << QStringLiteral("Expected=%1, actual=%2").arg(expected).arg(actual).toStdString();
    }

    return testing::AssertionSuccess();
}

} // namespace gqtest
