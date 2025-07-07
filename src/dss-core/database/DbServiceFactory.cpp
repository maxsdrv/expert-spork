#include "DbServiceFactory.h"

#include "dss-backend/exceptions/InvalidConfig.h"
#include "dss-backend/exceptions/InvalidConfigValue.h"

namespace mpk::dss::core
{

constexpr auto dbNameVarName = "DB_PG_NAME";
constexpr auto dbUserVarName = "DB_PG_USER";
constexpr auto dbPasswordVarName = "DB_PG_PASS";
constexpr auto dbHostVarName = "DB_PG_HOST";
constexpr auto dbPortVarName = "DB_PG_PORT";

std::shared_ptr<DbService> createDbService()
{
    auto* dbHostValue = std::getenv(dbHostVarName);
    if (dbHostValue == nullptr)
    {
        BOOST_THROW_EXCEPTION(
            dss::backend::exception::InvalidConfigValue{}
            << exception::ExceptionInfo(dbHostVarName));
    }
    auto* dbPortValue = std::getenv(dbPortVarName);
    if (dbPortValue == nullptr)
    {
        BOOST_THROW_EXCEPTION(
            dss::backend::exception::InvalidConfigValue{}
            << exception::ExceptionInfo(dbPortVarName));
    }
    auto* dbNameValue = std::getenv(dbNameVarName);
    if (dbNameValue == nullptr)
    {
        BOOST_THROW_EXCEPTION(
            dss::backend::exception::InvalidConfigValue{}
            << exception::ExceptionInfo(dbNameVarName));
    }
    auto* dbUserValue = std::getenv(dbUserVarName);
    if (dbUserValue == nullptr)
    {
        BOOST_THROW_EXCEPTION(
            dss::backend::exception::InvalidConfigValue{}
            << exception::ExceptionInfo(dbUserVarName));
    }
    auto* dbPasswordValue = std::getenv(dbPasswordVarName);
    if (dbPasswordValue == nullptr)
    {
        BOOST_THROW_EXCEPTION(
            dss::backend::exception::InvalidConfigValue{}
            << exception::ExceptionInfo(dbPasswordVarName));
    }
    return std::make_shared<DbService>(PostgreSQLSettings{
        .host = QString::fromUtf8(dbHostValue),
        .port = atoi(dbPortValue),
        .dbName = QString::fromUtf8(dbNameValue),
        .user = QString::fromUtf8(dbUserValue),
        .password = QString::fromUtf8(dbPasswordValue)});
}

} // namespace mpk::dss::core
