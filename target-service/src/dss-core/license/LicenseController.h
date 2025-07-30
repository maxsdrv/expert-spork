#pragma once

#include "OAServiceLicense_status.h"

#include "json/JsonComposer.h"

#include <QTimer>

#include <optional>
#include <string>

namespace mpk::dss::core
{

class LicenseController : public QObject
{
    Q_OBJECT

public:
    LicenseController(
        QString filePath,
        int updateInterval,
        bool disableLicense = false,
        QObject* parent = nullptr);

    [[nodiscard]] OAService::OAServiceLicense_status status() const;
    bool assign(const QByteArray& data);

signals:
    void statusUpdated(OAService::OAServiceLicense_status status);
    void licenseError(OAService::OAServiceLicense_status status);

private:
    [[nodiscard]] static std::optional<std::string> licenseSignature();
    [[nodiscard]] static OAService::OAServiceLicense_status licenseStatus(
        const QString& filePath);

private slots:
    void validate();

private:
    QString m_filePath;
    QTimer m_timer;
    QDate m_validTo;
    OAService::OAServiceLicense_status m_status;
};

} // namespace mpk::dss::core
