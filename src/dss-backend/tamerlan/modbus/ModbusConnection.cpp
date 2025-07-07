#include "ModbusConnection.h"

#include "exception/General.h"
#include "log/Log2.h"

#include <QStringList>

#include <set>

// NOLINTBEGIN (hicpp-signed-bitwise)

template <>
struct std::hash<mpk::dss::config::ConnectionSettings>
{
    std::size_t operator()(
        const mpk::dss::config::ConnectionSettings& settings) const noexcept
    {
        auto h1 = std::hash<std::string>{}(settings.host.toStdString());
        auto h2 = std::hash<int>{}(settings.port);
        return h1 ^ (h2 << 1);
    }
};

template <>
struct std::hash<mpk::dss::backend::tamerlan::ModbusRtuSettings>
{
    std::size_t operator()(const mpk::dss::backend::tamerlan::ModbusRtuSettings&
                               settings) const noexcept
    {
        auto h1 = std::hash<std::string>{}(settings.name);
        auto h2 = std::hash<int>{}(settings.baudRate);
        auto h3 = std::hash<char>{}(settings.parity);
        auto h4 = std::hash<int>{}(settings.dataBits);
        auto h5 = std::hash<int>{}(settings.stopBits);
        return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3) ^ (h5 << 4);
    }
};

// NOLINTEND (hicpp-signed-bitwise)

namespace mpk::dss::backend::tamerlan
{

constexpr auto hexBase = 16;
constexpr auto maxTryCount = 3;

// TODO: For debug
template <typename T>
QString regsToStr(const T* regs, int count)
{
    QStringList regsStrList;
    for (int i = 0; i < count; i++)
    {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        regsStrList.append(QString::number(regs[i], hexBase));
    }
    return regsStrList.join(", ");
}

ModbusConnection::ModbusConnection(
    const config::ConnectionSettings& settings,
    std::chrono::milliseconds timeout) :
  m_address(settings.host + ":" + QString::number(settings.port)),
  m_tcpConnectionSettings(settings),
  m_timeout(timeout)
{
    QHostInfo info = QHostInfo::fromName(settings.host);
    if (info.error() != QHostInfo::HostInfoError::NoError)
    {
        LOG_ERROR << "Failed to resolve host: " << info.errorString();
        return;
    }
    createTcpContext(info, settings.port);
    init();
}

ModbusConnection::ModbusConnection(
    const ModbusRtuSettings& settings, std::chrono::milliseconds timeout) :
  m_ctx(modbus_new_rtu(
      settings.name.data(),
      settings.baudRate,
      settings.parity,
      settings.dataBits,
      settings.stopBits)),
  m_address(QString::fromStdString(settings.name)),
  m_timeout(timeout)
{
    init();
}

void ModbusConnection::setServerAddress(ModbusServerAddress value)
{
    if (modbus_get_slave(m_ctx) != static_cast<int>(value))
    {
        modbus_set_slave(m_ctx, static_cast<int>(value));
    }
}

// NOLINTNEXTLINE(misc-no-recursion)
void ModbusConnection::tryConnectOrThrow()
{
    if (!m_ctx)
    {
        if (m_tcpConnectionSettings)
        {
            QHostInfo info = QHostInfo::fromName(m_tcpConnectionSettings->host);
            if (info.error() != QHostInfo::HostInfoError::NoError)
            {
                BOOST_THROW_EXCEPTION(
                    exception::General() << exception::ExceptionInfo(
                        std::string("Failed to resolve host: ")
                        + info.errorString().toStdString()));
            }
            createTcpContext(info, m_tcpConnectionSettings->port);
            init();
        }
        else
        {
            BOOST_THROW_EXCEPTION(
                exception::General() << exception::ExceptionInfo(
                    std::string(
                        "Failed to connect: modbus connection not initialized")
                    + modbus_strerror(errno)));
        }
    }
    if (!m_connected)
    {
        modbus_close(m_ctx);
        m_connected = (modbus_connect(m_ctx) == 0);
        if (!m_connected)
        {
            BOOST_THROW_EXCEPTION(
                exception::General() << exception::ExceptionInfo(
                    std::string("Failed to connect: ")
                    + modbus_strerror(errno)));
        }
    }
}

void ModbusConnection::processModbusError()
{
    static const std::set<int> connectionErrorCodes = {
        EIO,
        ENXIO,
        ENETDOWN,
        ENETUNREACH,
        ENETRESET,
        ECONNABORTED,
        ECONNRESET,
        ETIMEDOUT,
        ECONNREFUSED,
        EHOSTUNREACH,
        EMBXSFAIL,
        EMBXSBUSY,
        EMBXGPATH,
        EMBXGTAR};
    if (connectionErrorCodes.find(errno) != connectionErrorCodes.end())
    {
        m_connected = false;
    }
}

bool ModbusConnection::isErrorAvailableForRetry()
{
    static const std::set<int> errorCodes = {
        EMBXSBUSY, EMBXGTAR, EMBBADCRC, EMBBADDATA, EMBBADEXC, EMBMDATA};
    return errorCodes.find(errno) != errorCodes.end();
}

void ModbusConnection::createTcpContext(const QHostInfo& hostInfo, int port)
{
    auto addresses = hostInfo.addresses();
    m_ctx = modbus_new_tcp(addresses[0].toString().toStdString().c_str(), port);
}

// NOLINTNEXTLINE(misc-no-recursion)
void ModbusConnection::init()
{
    if (!m_ctx)
    {
        BOOST_THROW_EXCEPTION(
            exception::General() << exception::ExceptionInfo(
                std::string("Failed to create modbus: ")
                + modbus_strerror(errno)));
    }

    // NOLINTBEGIN(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
    modbus_set_response_timeout(
        m_ctx, m_timeout.count() / 1000, 1000 * (m_timeout.count() % 1000));
    // NOLINTEND(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)

    try
    {
        tryConnectOrThrow();
    }
    catch (const std::exception& ex)
    {
        LOG_ERROR << "Error while establishing modbus connection: "
                  << ex.what();
    }
}

void ModbusConnection::readInputRegisters(
    int address,
    int numRegs,
    uint16_t* destination,
    ModbusServerAddress serverAddress)
{
    if (m_disabled)
    {
        return;
    }

    tryConnectOrThrow();
    setServerAddress(serverAddress);
    bool success = false;
    for (int tryCount = 0; tryCount < maxTryCount; ++tryCount)
    {
        if (modbus_read_input_registers(m_ctx, address, numRegs, destination)
            < 0)
        {
            if (!isErrorAvailableForRetry())
            {
                break;
            }
        }
        else
        {
            success = true;
            break;
        }
    }
    if (!success)
    {
        processModbusError();
        BOOST_THROW_EXCEPTION(
            exception::General() << exception::ExceptionInfo(
                std::string("Failed to read input registers at <")
                + std::to_string(address) + ">: " + modbus_strerror(errno)));
    }
    LOG_TRACE << "Read " << numRegs << " input registers starting from "
              << QString::number(address, hexBase).toStdString() << ": {"
              << regsToStr(destination, numRegs).toStdString() << "}";
}

void ModbusConnection::readHoldingRegisters(
    int address,
    int numRegs,
    uint16_t* destination,
    ModbusServerAddress serverAddress)
{
    if (m_disabled)
    {
        return;
    }

    tryConnectOrThrow();
    setServerAddress(serverAddress);
    bool success = false;
    for (int tryCount = 0; tryCount < maxTryCount; ++tryCount)
    {
        if (modbus_read_registers(m_ctx, address, numRegs, destination) < 0)
        {
            if (!isErrorAvailableForRetry())
            {
                break;
            }
        }
        else
        {
            success = true;
            break;
        }
    }
    if (!success)
    {
        processModbusError();
        BOOST_THROW_EXCEPTION(
            exception::General() << exception::ExceptionInfo(
                std::string("Failed to read holding registers at <")
                + std::to_string(address) + ">: " + modbus_strerror(errno)));
    }
    LOG_TRACE << "Read " << numRegs << " holding registers starting from "
              << QString::number(address, hexBase).toStdString() << ": {"
              << regsToStr(destination, numRegs).toStdString() << "}";
}

void ModbusConnection::readCoils(
    int address,
    int numRegs,
    uint8_t* destination,
    ModbusServerAddress serverAddress)
{
    if (m_disabled)
    {
        return;
    }

    tryConnectOrThrow();
    setServerAddress(serverAddress);
    bool success = false;
    for (int tryCount = 0; tryCount < maxTryCount; ++tryCount)
    {
        if (modbus_read_bits(m_ctx, address, numRegs, destination) < 0)
        {
            if (!isErrorAvailableForRetry())
            {
                break;
            }
        }
        else
        {
            success = true;
            break;
        }
    }
    if (!success)
    {
        processModbusError();
        BOOST_THROW_EXCEPTION(
            exception::General() << exception::ExceptionInfo(
                std::string("Failed to read coils at <")
                + std::to_string(address) + ">: " + modbus_strerror(errno)));
    }
    LOG_TRACE << "Read " << numRegs << " coils starting from "
              << QString::number(address, hexBase).toStdString() << ": {"
              << regsToStr(destination, numRegs).toStdString() << "}";
}

void ModbusConnection::writeHoldingRegister(
    int address, uint16_t value, ModbusServerAddress serverAddress)
{
    if (m_disabled)
    {
        return;
    }

    tryConnectOrThrow();
    setServerAddress(serverAddress);
    bool success = false;
    for (int tryCount = 0; tryCount < maxTryCount; ++tryCount)
    {
        if (modbus_write_register(m_ctx, address, value) < 0)
        {
            if (!isErrorAvailableForRetry())
            {
                break;
            }
        }
        else
        {
            success = true;
            break;
        }
    }
    if (!success)
    {
        processModbusError();
        BOOST_THROW_EXCEPTION(
            exception::General() << exception::ExceptionInfo(
                std::string("Failed to write holding register at <")
                + std::to_string(address) + ">: " + modbus_strerror(errno)));
    }
}

void ModbusConnection::writeHoldingRegisters(
    int address,
    int numRegs,
    const uint16_t* data,
    ModbusServerAddress serverAddress)
{
    if (m_disabled)
    {
        return;
    }

    tryConnectOrThrow();
    setServerAddress(serverAddress);
    bool success = false;
    for (int tryCount = 0; tryCount < maxTryCount; ++tryCount)
    {
        if (modbus_write_registers(m_ctx, address, numRegs, data) < 0)
        {
            if (!isErrorAvailableForRetry())
            {
                break;
            }
        }
        else
        {
            success = true;
            break;
        }
    }
    if (!success)
    {
        processModbusError();
        BOOST_THROW_EXCEPTION(
            exception::General() << exception::ExceptionInfo(
                std::string("Failed to write holding registers at <")
                + std::to_string(address) + ">: " + modbus_strerror(errno)));
    }
    LOG_TRACE << "Written " << numRegs << " holding registers starting from "
              << QString::number(address, hexBase).toStdString() << ": {"
              << regsToStr(data, numRegs).toStdString() << "}";
}

void ModbusConnection::writeCoil(
    int address, int status, ModbusServerAddress serverAddress)
{
    if (m_disabled)
    {
        return;
    }

    tryConnectOrThrow();
    setServerAddress(serverAddress);
    bool success = false;
    for (int tryCount = 0; tryCount < maxTryCount; ++tryCount)
    {
        if (modbus_write_bit(m_ctx, address, status) < 0)
        {
            if (!isErrorAvailableForRetry())
            {
                break;
            }
        }
        else
        {
            success = true;
            break;
        }
    }
    if (!success)
    {
        processModbusError();
        BOOST_THROW_EXCEPTION(
            exception::General() << exception::ExceptionInfo(
                std::string("Failed to write coil at <")
                + std::to_string(address) + ">: " + modbus_strerror(errno)));
    }
}

void ModbusConnection::writeCoils(
    int address,
    int numRegs,
    const uint8_t* data,
    ModbusServerAddress serverAddress)
{
    if (m_disabled)
    {
        return;
    }

    tryConnectOrThrow();
    setServerAddress(serverAddress);
    bool success = false;
    for (int tryCount = 0; tryCount < maxTryCount; ++tryCount)
    {
        if (modbus_write_bits(m_ctx, address, numRegs, data) < 0)
        {
            if (!isErrorAvailableForRetry())
            {
                break;
            }
        }
        else
        {
            success = true;
            break;
        }
    }
    if (!success)
    {
        processModbusError();
        BOOST_THROW_EXCEPTION(
            exception::General() << exception::ExceptionInfo(
                std::string("Failed to write coils at <")
                + std::to_string(address) + ">: " + modbus_strerror(errno)));
    }
    LOG_TRACE << "Written " << numRegs << " coils starting from "
              << QString::number(address, hexBase).toStdString() << ": {"
              << regsToStr(data, numRegs).toStdString() << "}";
}

void ModbusConnection::setDisabled(bool disabled)
{
    if (disabled != m_disabled)
    {
        if (disabled)
        {
            modbus_close(m_ctx);
            m_connected = false;
        }
        else
        {
            try
            {
                tryConnectOrThrow();
            }
            catch (const std::exception& ex)
            {
                LOG_ERROR << "Error while establishing modbus connection: "
                          << ex.what();
            }
        }
        m_disabled = disabled;
    }
}

QString ModbusConnection::address() const
{
    return m_address;
}

} // namespace mpk::dss::backend::tamerlan
