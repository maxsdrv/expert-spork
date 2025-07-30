#pragma once

#include <QString>

namespace http
{

class AuthService
{
public:
    virtual ~AuthService() = default;

    virtual bool validate(const QString& login, const QString& password) = 0;
};

} /* namespace http */
