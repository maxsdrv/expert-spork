#include "LicenseController.h"

#include "log/Log2.h"

#ifndef AVOID_LICENSECC
#include "licensecc/licensecc.h"
#endif

#include <QFile>
#include <QTemporaryDir>

#include <filesystem>
#include <unordered_map>

namespace mpk::dss::core
{

LicenseController::LicenseController(
    QString filePath,
    [[maybe_unused]] int updateInterval,
    [[maybe_unused]] bool disableLicense,
    QObject* parent) :
  QObject(parent), m_filePath(std::move(filePath))
{
    qRegisterMetaType<OAService::OAServiceLicense_status>(
        "OAService::OAServiceLicense_status");
    m_status.setRValid(false);
    m_status.setSignature({});
    m_status.setDescription({});
    m_status.setExpiryDate({});

#ifndef AVOID_LICENSECC
    if (!disableLicense)
    {
        QObject::connect(
            &m_timer, &QTimer::timeout, this, &LicenseController::validate);
        m_timer.start(updateInterval);
        validate();
    }
    else
#endif
    {
        m_status.setRValid(true);
    }
}

OAService::OAServiceLicense_status LicenseController::status() const
{
    return m_status;
}

bool LicenseController::assign(const QByteArray& data)
{
    QTemporaryDir tempDir;
    auto tempFilePath = tempDir.path() + "license.tmp";
    QFile tempFile(tempFilePath);
    tempFile.open(QIODevice::WriteOnly);
    tempFile.write(data);
    tempFile.close();

    auto validationResult = licenseStatus(tempFilePath);
    tempFile.remove();

    if (validationResult.isRValid())
    {
        QFile licenseFile(m_filePath);
        licenseFile.open(QIODevice::WriteOnly);
        licenseFile.write(data);
        licenseFile.close();

        LOG_INFO << "Uploaded license is valid";
        m_status = validationResult;
        emit statusUpdated(m_status);
        return true;
    }
    return false;
}

void LicenseController::validate()
{
#ifndef AVOID_LICENSECC
    auto validTo = QDate::fromString(m_status.getExpiryDate(), "yyyy-MM-dd");
    if (!m_status.isRValid() || !validTo.isValid()
        || QDate::currentDate() > validTo)
    {
        auto validationStatus = licenseStatus(m_filePath);
        if (validationStatus.isRValid())
        {
            LOG_INFO << "License is valid";
        }
        else
        {
            LOG_ERROR << "License validation error: "
                      << validationStatus.getDescription();
        }
        if (auto signature = licenseSignature())
        {
            LOG_INFO << "License signature: " << *signature;
        }
        if (m_status != validationStatus)
        {
            m_status = validationStatus;
            emit statusUpdated(m_status);
        }
        if (!m_status.isRValid())
        {
            emit licenseError(m_status);
        }
    }
#endif
}

std::optional<std::string> LicenseController::licenseSignature()
{
#ifndef AVOID_LICENSECC
    try
    {
        size_t pc_id_sz = LCC_API_PC_IDENTIFIER_SIZE + 1;
        std::array<char, LCC_API_PC_IDENTIFIER_SIZE + 1> pc_identifier = {};
        if (identify_pc(
                STRATEGY_ETHERNET, pc_identifier.data(), &pc_id_sz, nullptr))
        {
            return std::string(pc_identifier.data());
        }

        LOG_FATAL << "PC identification error";
    }
    catch (const std::exception& exception)
    {
        LOG_FATAL << "PC identification error: " << exception.what();
    }
#endif
    return std::nullopt;
}

OAService::OAServiceLicense_status LicenseController::licenseStatus(
#ifndef AVOID_LICENSECC
    const QString& filePath)
{
    std::unordered_map<LCC_EVENT_TYPE, std::string> stringByEventType = {
        {LICENSE_OK, "OK"},
        {LICENSE_FILE_NOT_FOUND,
         "license file not found: " + filePath.toStdString()},
        {LICENSE_SERVER_NOT_FOUND, "license server can't be contacted"},
        {ENVIRONMENT_VARIABLE_NOT_DEFINED, "environment variable not defined"},
        {FILE_FORMAT_NOT_RECOGNIZED,
         "license file has invalid format (not .ini file)"},
        {LICENSE_MALFORMED,
         "some mandatory field are missing, or data can't be fully read."},
        {PRODUCT_NOT_LICENSED, "this product was not licensed"},
        {PRODUCT_EXPIRED, "license expired"},
        {LICENSE_CORRUPTED,
         "license signature didn't match with current license"},
        {IDENTIFIERS_MISMATCH,
         "calculated identifier and the one provided in license didn't match"}};

    OAService::OAServiceLicense_status status;

    try
    {
        LicenseInfo licenseInfo{};

        LicenseLocation licenseLocation{};
        licenseLocation.license_data_type = LICENSE_PATH;
        std::filesystem::path path(filePath.toStdString());
        const auto absolutePath = std::filesystem::absolute(path).string();
        std::copy(
            absolutePath.begin(),
            absolutePath.end(),
            licenseLocation.licenseData);

        LCC_EVENT_TYPE result =
            acquire_license(nullptr, &licenseLocation, &licenseInfo);

        auto expiryDateStr = QString::fromUtf8(licenseInfo.expiry_date);
        auto expiryDate = QDate::fromString(expiryDateStr, "yyyy-MM-dd");

        if (!expiryDateStr.isEmpty() && !expiryDate.isValid())
        {
            status.setRValid(false);
            status.setDescription(QString("Invalid date: ") + expiryDateStr);
            status.setExpiryDate({});
        }
        else
        {
            status.setRValid(result == LICENSE_OK);
            status.setDescription(
                QString::fromStdString(stringByEventType[result]));
            status.setExpiryDate(QString::fromUtf8(licenseInfo.expiry_date));
        }
        auto signature = licenseSignature();
        status.setSignature(
            signature ? QString::fromStdString(*signature) : QString());
        return status;
    }
    catch (const std::exception& exception)
    {
        QString errorStr =
            QString("License read error: %1").arg(exception.what());
        LOG_ERROR << "License read error: " << exception.what();
        status.setRValid(false);
        status.setDescription(errorStr);
        status.setExpiryDate({});
        status.setSignature({});
    }
#else
    const QString& /*filePath*/)
{
    OAService::OAServiceLicense_status status;
#endif
    return status;
}

} // namespace mpk::dss::core
