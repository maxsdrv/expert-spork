#pragma once

#include "core/modbus/ModbusServerController.h"
#include "core/proto/FileChunk.h"

#include "exception/General.h"

#include <QtSerialBus/QModbusServer>

#include "gsl/assert"
#include "gsl/pointers"

#include <array>
#include <cstdint>
#include <map>
#include <string>

namespace mpk::drone::core
{

namespace exception
{

class UnalignedAccess : public ::exception::General
{
};

class AddressOverlap : public ::exception::General
{
};

} // namespace exception

class ModbusData : public QObject
{
    Q_OBJECT

public:
    explicit ModbusData(
        gsl::not_null<ModbusServerController*> controller,
        QObject* parent = nullptr);

    template <typename T>
    void emplace(
        const std::string& id,
        int address,
        QModbusDataUnit::RegisterType type,
        const T& data = T())
    {
        static_assert(sizeof(T) <= maxModbusWriteBytes, "Oversized structure");
        static_assert(sizeof(T) % 2 == 0, "Unaligned structure");

        auto record = Record{address, static_cast<int>(sizeof(T) / 2)};

        auto it = std::find_if(
            m_records.begin(),
            m_records.end(),
            [&record](const auto& pair)
            { return hasIntersection(record, pair.second); });
        if (it == m_records.end())
        {
            m_records.insert({id, record});
        }
        else
        {
            BOOST_THROW_EXCEPTION(exception::AddressOverlap());
        }

        write<T>(id, data, type);
    }

    template <typename T>
    void write(
        const std::string& id,
        const T& value,
        QModbusDataUnit::RegisterType type)
    {
        auto it = findById<T>(id);
        QVector<std::uint16_t> regs;
        T::template toRegs<>(value, std::back_inserter(regs));

        auto dataUnit = QModbusDataUnit{type, it->second.address, regs};
        m_controller->server()->setData(dataUnit);
    }

    // GCC bug workaround https://gcc.gnu.org/bugzilla/show_bug.cgi?id=85282
    template <std::same_as<uint16_t> T>
    void write(
        const std::string& id,
        const T& value,
        QModbusDataUnit::RegisterType type)
    {
        auto it = findById(id, 1);
        auto dataUnit =
            QModbusDataUnit{type, it->second.address, QVector{value}};
        m_controller->server()->setData(dataUnit);
    }

    template <typename T>
    void read(
        const std::string& id,
        T& value,
        QModbusDataUnit::RegisterType type) const
    {
        auto it = findById<T>(id);
        auto regCount = static_cast<quint16>(sizeof(T) / 2);
        auto dataUnit = QModbusDataUnit{type, it->second.address, regCount};
        m_controller->server()->data(&dataUnit);

        auto regs = dataUnit.values();
        value = T::template fromRegs<>(regs.begin(), regs.end());
    }

    // GCC bug workaround https://gcc.gnu.org/bugzilla/show_bug.cgi?id=85282
    template <std::same_as<uint16_t> T>
    void read(
        const std::string& id,
        T& value,
        QModbusDataUnit::RegisterType type) const
    {
        auto it = findById(id, 1);
        auto dataUnit = QModbusDataUnit{type, it->second.address, 1};
        m_controller->server()->data(&dataUnit);

        auto regs = dataUnit.values();
        value = regs.first();
    }

signals:
    void modified(std::string);

private:
    struct Record
    {
        int address;
        int size;
    };

    using Records = std::map<std::string, Record>;

    template <typename T>
    [[nodiscard]] Records::const_iterator findById(const std::string& id) const
    {
        return findById(id, static_cast<int>(sizeof(T) / 2));
    }

    [[nodiscard]] Records::const_iterator findById(
        const std::string& id, int size) const
    {
        auto it = m_records.find(id);
        if (it == m_records.end() || it->second.size != size)
        {
            BOOST_THROW_EXCEPTION(exception::UnalignedAccess());
        }
        return it;
    }

    static bool hasIntersection(const Record& left, const Record& right)
    {
        return left.address + left.size < right.address
               && right.address + right.size < left.address;
    }

private:
    gsl::not_null<ModbusServerController*> m_controller;
    Records m_records;
};

} // namespace mpk::drone::core
