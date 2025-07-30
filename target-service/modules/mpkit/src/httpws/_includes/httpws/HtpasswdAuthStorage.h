#pragma once

#include "httpws/AuthService.h"

#include <filesystem>

namespace http
{

class HtpasswdAuthStorage : public AuthService
{
public:
    explicit HtpasswdAuthStorage(std::filesystem::path path);

    bool validate(const QString& login, const QString& password) override;

private:
    std::filesystem::path m_path;
};

} // namespace http
