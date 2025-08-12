#include "ModbusParams.h"

namespace mpk::drone::core
{

inline bool operator==(const ModbusParams& left, const ModbusParams& right)
{
    return left.address == right.address;
}

inline bool operator!=(const ModbusParams& left, const ModbusParams& right)
{
    return !(left == right);
}

ModbusParamsHolder::ModbusParamsHolder(
    const ModbusParams& params, QObject* parent) :
  QObject(parent), m_params(params)
{
}

void ModbusParamsHolder::setParams(const ModbusParams& params)
{
    if (m_params != params)
    {
        m_params = params;
        emit changed(m_params);
    }
}

ModbusParams ModbusParamsHolder::params() const
{
    return m_params;
}

} // namespace mpk::drone::core
