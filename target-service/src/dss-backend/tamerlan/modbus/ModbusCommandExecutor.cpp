#include "ModbusCommandExecutor.h"

#include "ModbusConnection.h"

namespace mpk::dss::backend::tamerlan
{

ModbusCommandExecutor::ModbusCommandExecutor(
    std::unique_ptr<ModbusConnection> connection, QObject* parent) :
  QObject{parent}, m_connection{std::move(connection)}
{
}

void ModbusCommandExecutor::setDisabled(bool disabled)
{
    m_connection->setDisabled(disabled);
}

ModbusCommandExecutor::~ModbusCommandExecutor() = default;

gsl::not_null<const ModbusConnection*> ModbusCommandExecutor::constConnection()
    const
{
    return m_connection.get();
}

gsl::not_null<ModbusConnection*> ModbusCommandExecutor::connection()
{
    return m_connection.get();
}

} // namespace mpk::dss::backend::tamerlan
