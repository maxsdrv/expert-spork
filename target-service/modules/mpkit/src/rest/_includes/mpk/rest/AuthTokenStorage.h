#pragma once

#include "httpws/AuthenticateMethod.h"
#include "httpws/AuthorizeMethod.h"

#include "mpk/rest/UserPermissionsProvider.h"

#include "gsl/pointers"

#include <QFile>

#include <filesystem>

namespace http
{

class AuthService;

} // namespace http

namespace mpk::rest
{

class UserPermissionsProvider;
class AuthTokenStorage : public http::AuthenticateMethod, public http::AuthorizeMethod
{
public:
    AuthTokenStorage(
        gsl::not_null<http::AuthService*> authService,
        gsl::not_null<UserPermissionsProvider*> userPermissions,
        const std::filesystem::path& cacheFilePath);

    http::AuthTokens getTokens(const http::Credentials& credentials) override;
    http::AuthTokens getTokens(const QString& refreshTokenValue) override;
    http::Permissions validate(const QString& accessTokenValue) const override;

private:
    gsl::not_null<http::AuthService*> m_authService;
    gsl::not_null<UserPermissionsProvider*> m_userPermissions;
    http::AuthTokensMap m_tokens;
    QString m_cacheFilePath;
};

} // namespace mpk::rest
