#pragma once

#include <QObject>

namespace mpk::drone::core
{

struct ModbusParams
{
    int address;
};

inline bool operator==(const ModbusParams& left, const ModbusParams& right);
inline bool operator!=(const ModbusParams& left, const ModbusParams& right);

class ModbusParamsHolder : public QObject
{
    Q_OBJECT

public:
    explicit ModbusParamsHolder(
        const ModbusParams& params, QObject* parent = nullptr);

    void setParams(const ModbusParams& params);
    [[nodiscard]] ModbusParams params() const;

signals:
    void changed(ModbusParams);

private:
    ModbusParams m_params;
};

} // namespace mpk::drone::core

Q_DECLARE_METATYPE(mpk::drone::core::ModbusParams);
