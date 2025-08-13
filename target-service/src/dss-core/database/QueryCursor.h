#pragma once

#include "QueryCursorParser.h"

#include "json/JsonUtilities.h"

#include <QJsonObject>
#include <QMetaEnum>
#include <QSqlQuery>
#include <QVariant>

namespace mpk::dss::core
{

template <typename T, typename U = T>
struct QueryValueParser
{
    static T getNextValue(QSqlQuery& query, int& columnIndex)
    {
        return query.value(columnIndex++).value<T>();
    }
};

template <typename T>
struct QueryValueParser<boost::optional<T>>
{
    static boost::optional<T> getNextValue(QSqlQuery& query, int& index)
    {
        if (query.isNull(index))
        {
            ++index;
            return boost::none;
        }
        return QueryValueParser<T>::getNextValue(query, index);
    }
};

template <typename T>
struct QueryValueParser<T, std::enable_if_t<QtPrivate::IsQEnumHelper<T>::Value, T>>
{
    static T getNextValue(QSqlQuery& query, int& columnIndex)
    {
        return static_cast<T>(query.value(columnIndex++).toInt());
    }
};

template <>
struct QueryValueParser<QJsonObject>
{
    static QJsonObject getNextValue(QSqlQuery& query, int& columnIndex);
};

class QueryCursor
{
    friend class DbService;

public:
    explicit QueryCursor(QSqlQuery const& query);

    /**
     * Move cursor to next record in query. Current value index is set to 0
     */
    bool next();

    /**
     * Move cursor to index record in query. Current value index is set to 0
     */
    bool seek(int index);

    /**
     * Return record count
     */
    int size() const;

    /**
     * Return current value in record and move index to next position
     */
    template <typename T>
    T nextValue()
    {
        return QueryValueParser<T>::getNextValue(m_query, m_columnIndex);
    }

    /**
     * Return value from full row
     */
    template <typename T>
    T rowValue()
    {
        return fromQueryCursor<std::remove_const_t<T>>(this);
    }

    /**
     * Bind placeholder to inner value
     */
    void bindValue(const QString& placeholder, const QVariant& val);
    void bindValue(const QString& placeholder, const QJsonObject& object);

private:
    mutable QSqlQuery m_query;
    int m_columnIndex = 0;
};

} // namespace mpk::dss::core
