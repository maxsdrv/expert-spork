#pragma once

#include "AuthService.h"

#include <QJsonObject>

namespace http
{

class JsonAuthStorage : public AuthService
{
public:
    explicit JsonAuthStorage(const QString& path);

    bool validate(const QString& login, const QString& password) override;

private:
    QString m_path;
    QJsonObject m_json;
};

} // namespace http
