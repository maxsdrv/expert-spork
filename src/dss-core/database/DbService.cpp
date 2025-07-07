#include "DbService.h"

#include "DatabaseError.h"

#include "log/Log2.h"

#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QThread>
#include <QUuid>

namespace mpk::dss::core
{

DbService::DbService(const PostgreSQLSettings& settings, QObject* parent) :
  QObject(parent)
{
    auto connectionName = QUuid::createUuid().toString(QUuid::WithoutBraces);
    LOG_DEBUG << "DbService.cpp: Creating connection " << connectionName;
    m_db =
        std::make_unique<QSqlDatabase>(openDatabase(connectionName, settings));
}

DbService::~DbService()
{
    LOG_DEBUG << "DbService.cpp: Releasing connection "
              << m_db->connectionName();
    auto connName = m_db->connectionName();
    m_db->close();
    m_db.reset();
    QSqlDatabase::removeDatabase(connName);
}

std::unique_ptr<QueryCursor> DbService::prepare(const QString& queryString)
{
    auto query = createQuery();

    query.prepare(queryString);
    if (!query.prepare(queryString))
    {
        LOG_ERROR << "DbService::prepare(): Query preparation failed:";
        LOG_ERROR << "DbService::prepare():" << query.lastError().text();
        LOG_ERROR << "DbService::prepare():" << queryString;
    }
    return std::make_unique<QueryCursor>(query);
}

void DbService::exec(QueryCursor& query)
{
    auto& sqlQuery = query.m_query;

    if (!sqlQuery.exec())
    {
        LOG_WARNING << "DbService::exec() " << sqlQuery.lastQuery();
        LOG_WARNING << "DbService::exec() " << sqlQuery.lastError().text();
        BOOST_THROW_EXCEPTION(
            exception::DatabaseError()
            << exception::DatabaseErrorText(sqlQuery.lastError().text()));
    }
    LOG_TRACE << sqlQuery.executedQuery();
}

std::unique_ptr<QueryCursor> DbService::exec(const QString& queryString)
{
    LOG_DEBUG << queryString;
    auto query = createQuery();

    if (!query.exec(queryString))
    {
        LOG_WARNING << "DbService::exec() " << query.lastQuery();
        LOG_WARNING << "DbService::exec() " << query.lastError().text();
        BOOST_THROW_EXCEPTION(
            exception::DatabaseError()
            << exception::DatabaseErrorText(query.lastError().text()));
    }
    return std::make_unique<QueryCursor>(query);
}

QSqlQuery DbService::createQuery()
{
    if (!m_db->isValid() || !m_db->open())
    {
        BOOST_THROW_EXCEPTION(
            exception::DatabaseError()
            << exception::DatabaseErrorText(QString("Invalid database")));
    }
    return QSqlQuery(*m_db);
}

QSqlDatabase openDatabase(
    const QString& name, const PostgreSQLSettings& settings)
{
    auto database = QSqlDatabase::addDatabase("QPSQL", name);
    database.setHostName(settings.host);
    database.setPort(settings.port);
    database.setDatabaseName(settings.dbName);
    database.setUserName(settings.user);
    database.setPassword(settings.password);

    bool ok = database.open();
    if (!ok)
    {
        BOOST_THROW_EXCEPTION(
            exception::DatabaseError()
            << exception::DatabaseErrorText(database.lastError().text()));
    }
    return database;
}
} // namespace mpk::dss::core
