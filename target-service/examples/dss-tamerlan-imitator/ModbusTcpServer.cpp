#include "ModbusTcpServer.h"

#include "log/Log2.h"

#include <QModbusPdu>

#include "gsl/assert"

#include <sys/select.h>
#include <sys/socket.h>

#include <map>

namespace mpk::dss::imitator::tamerlan
{

constexpr auto serverAddressOffset = 6;
constexpr auto functionCodeOffset = 7;
constexpr auto startRegisterOffset = 8;
constexpr auto numRegistersOffset = 10;

quint16 valuesToUint16(uint8_t const* values)
{
    constexpr auto eightBit = 8U;
    // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    quint16 result = values[0] << eightBit;
    result |= values[1];
    // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    return result;
}

namespace {
std::string registerType2Str(QModbusDataUnit::RegisterType registerType)
{
    static std::map<QModbusDataUnit::RegisterType, std::string> registerStr = {
        {QModbusDataUnit::RegisterType::Invalid, "Invalid"},
        {QModbusDataUnit::RegisterType::DiscreteInputs, "DiscreteInputs"},
        {QModbusDataUnit::RegisterType::Coils, "Coils"},
        {QModbusDataUnit::RegisterType::InputRegisters, "InputRegisters"},
        {QModbusDataUnit::RegisterType::HoldingRegisters, "HoldingRegisters"}};

    return registerStr.at(registerType);
}
} // namespace

ModbusTcpServer::ModbusTcpServer(
    const QString& host, int port, QObject* parent) :
  ModbusServer(parent)
{
    m_server = modbus_new_tcp(host.toStdString().c_str(), port);
    if (!m_server)
    {
        throw std::runtime_error(
            std::string("Failed to create modbus server: ")
            + modbus_strerror(errno));
    }
}

void ModbusTcpServer::registerModbusDataUnits(
    int serverAddress, ServiceRole role, const ModbusDataUnits& units)
{
    auto it = m_dataMap.find(serverAddress);
    if (it == m_dataMap.end())
    {
        const int registersCount = 0xFFFF;
        auto* mapping = modbus_mapping_new(
            registersCount, 0, registersCount, registersCount);
        it = m_dataMap.insert(
            it,
            std::make_pair(
                serverAddress,
                SingleDeviceData{.role = role, .mapping = mapping}));
    }

    for (const auto& unit: units)
    {
        setData(serverAddress, unit);
    }
}

// NOLINTNEXTLINE (readability-function-cognitive-complexity)
void ModbusTcpServer::start()
{
    constexpr auto maxConnections = 10;
    int serverSocket = modbus_tcp_listen(m_server, maxConnections);

    /* Maximum file descriptor number */
    int fileDescriptorMax = serverSocket;

    fd_set referenceSet;
    /* Clear the reference set of socket */
    FD_ZERO(&referenceSet);
    /* Add the server socket */
    FD_SET(serverSocket, &referenceSet);

    while (!m_stopFlag)
    {
        fd_set rdSet = referenceSet;
        if (select(fileDescriptorMax + 1, &rdSet, nullptr, nullptr, nullptr)
            == -1)
        {
            perror("Server select() failure.");
        }
        for (int masterSocket = 0; masterSocket <= fileDescriptorMax;
             masterSocket++)
        {

            if (!FD_ISSET(masterSocket, &rdSet))
            {
                continue;
            }

            if (masterSocket == serverSocket)
            {
                /* A client is asking a new connection */
                int newfd = modbus_tcp_accept(m_server, &serverSocket);
                if (newfd == -1)
                {
                    LOG_DEBUG << "Modbus TCP accept error";
                }
                else
                {
                    FD_SET(newfd, &referenceSet);
                    if (newfd > fileDescriptorMax)
                    {
                        /* Keep track of the maximum */
                        fileDescriptorMax = newfd;
                    }
                    LOG_DEBUG << "New connection on socket " << newfd;
                }
            }
            else
            {
                modbus_set_socket(m_server, masterSocket);
                uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH]; // NOLINT

                int rc = modbus_receive(m_server, query);
                if (rc > numRegistersOffset + 1)
                {
                    auto serverAddress =
                        static_cast<int>(query[serverAddressOffset]);
                    auto functionCode = static_cast<QModbusPdu::FunctionCode>(
                        query[functionCodeOffset]);
                    auto it = m_dataMap.find(serverAddress);
                    if (it != m_dataMap.end())
                    {
                        if (functionCode == QModbusPdu::WriteSingleCoil
                            || functionCode == QModbusPdu::WriteSingleRegister
                            || functionCode == QModbusPdu::WriteMultipleCoils
                            || functionCode
                                   == QModbusPdu::WriteMultipleRegisters)
                        {
                            QModbusDataUnit unit;
                            if (functionCode == QModbusPdu::WriteSingleCoil
                                || functionCode
                                       == QModbusPdu::WriteMultipleCoils)
                            {
                                unit.setRegisterType(QModbusDataUnit::Coils);
                            }
                            else
                            {
                                unit.setRegisterType(
                                    QModbusDataUnit::HoldingRegisters);
                            }

                            // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
                            unit.setStartAddress(
                                valuesToUint16(query + startRegisterOffset));

                            if (functionCode == QModbusPdu::WriteSingleCoil
                                || functionCode
                                       == QModbusPdu::WriteSingleRegister)
                            {
                                unit.setValues({valuesToUint16(
                                    query + numRegistersOffset)});
                            }
                            else
                            {
                                auto numRegisters =
                                    valuesToUint16(query + numRegistersOffset);
                                QVector<quint16> values;
                                for (auto i = 0U; i < numRegisters; i++)
                                {
                                    values.append(valuesToUint16(
                                        query
                                        + static_cast<int>(
                                            numRegistersOffset + 3 + i * 2)));
                                }
                                unit.setValues(values);
                            }
                            // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)

                            setData(serverAddress, unit);
                        }
                        modbus_reply(m_server, query, rc, it->second.mapping);
                    }
                }
                else if (rc == -1)
                {
                    LOG_DEBUG << "Connection closed on socket " << masterSocket;
                    close(masterSocket);
                    FD_CLR(masterSocket, &referenceSet);
                    if (masterSocket == fileDescriptorMax)
                    {
                        fileDescriptorMax--;
                    }
                }
            }
        }
    }

    close(serverSocket);
    modbus_close(m_server);
    modbus_free(m_server);
}

void ModbusTcpServer::stop()
{
    m_stopFlag = true;
}

std::vector<int> ModbusTcpServer::serverAddresses() const
{
    std::vector<int> addresses;
    std::transform(
        m_dataMap.begin(),
        m_dataMap.end(),
        std::back_inserter(addresses),
        [](const auto& data) { return data.first; });
    return addresses;
}

ServiceRole ModbusTcpServer::serviceRole(int serverAddress) const
{
    return m_dataMap.at(serverAddress).role;
}

bool ModbusTcpServer::data(int serverAddress, QModbusDataUnit* newData) const
{
    LOG_TRACE << __FUNCTION__ << " " << serverAddress << " "
              << registerType2Str(newData->registerType()) << " " << std::hex
              << "0x" << newData->startAddress() << " " << std::dec
              << newData->valueCount();

    if (!newData)
    {
        return false;
    }
    auto it = m_dataMap.find(serverAddress);
    if (it == m_dataMap.end())
    {
        return false;
    }
    for (int i = 0; i < static_cast<int>(newData->valueCount()); i++)
    {
        // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        if (newData->registerType() == QModbusDataUnit::Coils)
        {
            newData->setValue(
                i, it->second.mapping->tab_bits[newData->startAddress() + i]);
        }
        else if (newData->registerType() == QModbusDataUnit::InputRegisters)
        {
            newData->setValue(
                i,
                it->second.mapping
                    ->tab_input_registers[newData->startAddress() + i]);
        }
        else if (newData->registerType() == QModbusDataUnit::HoldingRegisters)
        {
            newData->setValue(
                i,
                it->second.mapping->tab_registers[newData->startAddress() + i]);
        }
        // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    }
    return true;
}

bool ModbusTcpServer::data(
    int serverAddress,
    QModbusDataUnit::RegisterType table,
    quint16 address,
    quint16* dataPtr) const
{
    QModbusDataUnit unit{table, address, QVector<quint16>{0}};
    auto result = data(serverAddress, &unit);
    if (result)
    {
        *dataPtr = unit.value(0);
    }
    return result;
}

bool ModbusTcpServer::setData(int serverAddress, const QModbusDataUnit& newData)
{
    LOG_TRACE << __FUNCTION__ << " " << serverAddress << " "
              << registerType2Str(newData.registerType()) << " " << std::hex
              << "0x" << newData.startAddress() << " " << std::dec
              << newData.valueCount();

    auto it = m_dataMap.find(serverAddress);
    if (it == m_dataMap.end())
    {
        return false;
    }
    for (int i = 0; i < static_cast<int>(newData.valueCount()); i++)
    {
        // NOLINTBEGIN(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        if (newData.registerType() == QModbusDataUnit::Coils)
        {
            it->second.mapping->tab_bits[newData.startAddress() + i] =
                static_cast<uint8_t>(newData.value(i));
        }
        else if (newData.registerType() == QModbusDataUnit::InputRegisters)
        {
            it->second.mapping->tab_input_registers[newData.startAddress() + i] =
                newData.value(i);
        }
        else if (newData.registerType() == QModbusDataUnit::HoldingRegisters)
        {
            it->second.mapping->tab_registers[newData.startAddress() + i] =
                newData.value(i);
        }
        // NOLINTEND(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    }
    return true;
}

bool ModbusTcpServer::setData(
    int serverAddress,
    QModbusDataUnit::RegisterType table,
    quint16 address,
    quint16 newData)
{
    QModbusDataUnit unit{table, address, QVector<quint16>{newData}};
    return setData(serverAddress, unit);
}

} // namespace mpk::dss::imitator::tamerlan
