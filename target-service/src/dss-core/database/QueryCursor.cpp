#include "QueryCursor.h"

#include "json/JsonUtilities.h"

#include <QDateTime>
#include <QMutex>

namespace mpk::dss::core
{

QueryCursor::QueryCursor(const QSqlQuery& query) : m_query(query)
{
}

bool QueryCursor::next()
{
    m_columnIndex = 0;
    return m_query.next();
}

bool QueryCursor::seek(int index)
{
    m_columnIndex = 0;
    return m_query.seek(index);
}

int QueryCursor::size() const
{
    if (m_query.isActive() && m_query.isSelect())
    {
        auto size = m_query.size();

        // Some DB not support size() method. Try calc it manually
        if (size == -1)
        {
            size = 0;
            if (m_query.first())
            {
                size = 1;
            }
            while (m_query.next())
            {
                ++size;
            }
            return size;
        }
        return size;
    }
    return 0;
}

void QueryCursor::bindValue(const QString& placeholder, const QVariant& val)
{
    m_query.bindValue(placeholder, val);
}

void QueryCursor::bindValue(
    const QString& placeholder, const QJsonObject& object)
{
    m_query.bindValue(placeholder, json::toByteArray(object));
}

QJsonObject QueryValueParser<QJsonObject>::getNextValue(
    QSqlQuery& query, int& index)
{
    return json::fromByteArray(query.value(index++).toString().toUtf8());
}

} // namespace mpk::dss::core
