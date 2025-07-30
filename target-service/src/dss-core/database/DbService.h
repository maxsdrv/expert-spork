#pragma once

#include "QueryCursor.h"

#include <QMutex>
#include <QObject>
#include <QSqlDatabase>

#include <memory>
#include <set>

class QSqlQuery;

namespace mpk::dss::core
{

struct PostgreSQLSettings
{
    QString host;
    int port;
    QString dbName;
    QString user;
    QString password;
};

QSqlDatabase openDatabase(
    const QString& name, const PostgreSQLSettings& settings);

class DbService : public QObject
{
    Q_OBJECT

public:
    explicit DbService(
        const PostgreSQLSettings& settings, QObject* parent = nullptr);
    ~DbService() override;

    /**
     * Prepare query to execute
     */
    std::unique_ptr<QueryCursor> prepare(QString const& queryString);

    /**
     * Execute query. Throw if error occurs
     */
    static void exec(QueryCursor& query);

    /**
     * Executes query without preparation
     */
    std::unique_ptr<QueryCursor> exec(QString const& queryString);

signals:
    void newConnection(QString);

private:
    [[nodiscard]] QSqlQuery createQuery();

private:
    std::unique_ptr<QSqlDatabase> m_db;
};
} // namespace mpk::dss::core
