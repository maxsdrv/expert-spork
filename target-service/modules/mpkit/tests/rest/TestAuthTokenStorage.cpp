#include "httpws/AuthService.h"
#include "httpws/AuthTokens.h"
#include "httpws/AuthType.h"

#include "json/JsonUtilities.h"

#include "mpk/rest/AuthTokenStorage.h"

#include "gqtest"

#include "qttypes/QStringConverters.h"

#include "utils/RandomString.h"

#include <filesystem>

#ifndef GTEST_OS_WINDOWS

using namespace mpk::rest;

std::filesystem::path getTmpFile()
{
    auto newFileName = std::filesystem::temp_directory_path();
    newFileName /= "AuthToken_" + utils::randomLatinString(8) + ".json";
    return newFileName;
}

http::AuthTokensMap getAuthTokensMapFromFile(const std::filesystem::path& filePath)
{
    constexpr auto rootObjectTag = "users";
    return json::fromObject<http::AuthTokensMap>(json::fromFile(filePath), rootObjectTag);
}

namespace http
{

bool operator==(const http::AuthTokens& lhs, const http::AuthTokens& rhs)
{
    return lhs.accessToken.value == rhs.accessToken.value
           && lhs.refreshToken.value == rhs.refreshToken.value;
}

} // namespace http

class AuthServiceMock : public http::AuthService
{
    bool validate(const QString& login, const QString& password) override
    {
        return login == "login" && password == "password";
    }
};

class UserPermissionsMock : public UserPermissionsProvider
{
    http::Permissions list(const QString& username) const override
    {
        return username == "login" ? http::Permissions{{"LADS"}} : http::Permissions{{}};
    }
};

class TestAuthTokenStorage : public ::testing::Test
{
public:
    TestAuthTokenStorage() :
      m_tmpFilePath(getTmpFile()), m_storage(&m_authService, &m_userPermissions, m_tmpFilePath)
    {
    }

protected:
    AuthServiceMock m_authService;
    UserPermissionsMock m_userPermissions;
    std::filesystem::path m_tmpFilePath;
    AuthTokenStorage m_storage;
};

TEST_F(TestAuthTokenStorage, getTokensByCredentialsCorrect)
{
    http::Credentials credentials{"login", "password"};
    http::AuthTokens tokens;
    ASSERT_NO_THROW(tokens = m_storage.getTokens(credentials)); // NOLINT
    EXPECT_TRUE(testToken(tokens.accessToken));
    EXPECT_TRUE(testToken(tokens.refreshToken));
}

TEST_F(TestAuthTokenStorage, getTokensByCredentialsIncorrect)
{
    http::Credentials credentials{"logan", "renault"};
    EXPECT_THROW(m_storage.getTokens(credentials), std::exception); // NOLINT
}

TEST_F(TestAuthTokenStorage, getTokensByRefreshTokenCorrect)
{
    http::Credentials credentials{"login", "password"};
    http::AuthTokens tokens;
    ASSERT_NO_THROW(tokens = m_storage.getTokens(credentials)); // NOLINT
    auto refreshToken = tokens.refreshToken;
    ASSERT_NO_THROW(tokens = m_storage.getTokens(refreshToken.value)); // NOLINT
    EXPECT_EQ(refreshToken.value, tokens.refreshToken.value);
    EXPECT_TRUE(testToken(tokens.accessToken));
}

TEST_F(TestAuthTokenStorage, getTokensByRefreshTokenIncorrect)
{
    EXPECT_THROW(m_storage.getTokens("not_a_token"), std::exception); // NOLINT
}

TEST_F(TestAuthTokenStorage, validateCorrect)
{
    http::Credentials credentials{"login", "password"};
    http::AuthTokens tokens;
    ASSERT_NO_THROW(tokens = m_storage.getTokens(credentials)); // NOLINT
    auto permissions = m_storage.validate(tokens.accessToken.value);
    ASSERT_FALSE(permissions.groups.isEmpty());
    EXPECT_TRUE(permissions.groups.contains("LADS"));
}

TEST_F(TestAuthTokenStorage, validateIncorrect)
{
    EXPECT_TRUE(m_storage.validate("not_a_token").empty());
}

TEST_F(TestAuthTokenStorage, validateFileWriting)
{
    http::Credentials credentials{"login", "password"};
    http::AuthTokens tokens;
    ASSERT_NO_THROW(tokens = m_storage.getTokens(credentials)); // NOLINT
    auto refreshToken = tokens.refreshToken;

    auto authTokensMap = getAuthTokensMapFromFile(m_tmpFilePath);
    auto res = authTokensMap["login"];

    EXPECT_EQ(res.front().accessToken.value, tokens.accessToken.value);
    EXPECT_EQ(res.front().refreshToken.value, tokens.refreshToken.value);
}

TEST_F(TestAuthTokenStorage, accessTokenChangeAfterGetTokensByRefreshToken)
{
    http::Credentials credentials{"login", "password"};
    http::AuthTokens tokens;
    ASSERT_NO_THROW(tokens = m_storage.getTokens(credentials)); // NOLINT
    auto refreshToken = tokens.refreshToken;
    auto initialAuthTokensMap = getAuthTokensMapFromFile(m_tmpFilePath);

    http::AuthTokens newTokens = m_storage.getTokens(refreshToken.value);
    EXPECT_NE(newTokens.accessToken.value, tokens.accessToken.value);
    EXPECT_EQ(newTokens.refreshToken.value, tokens.refreshToken.value);

    auto actualAuthTokensMap = getAuthTokensMapFromFile(m_tmpFilePath);

    auto actualAuthTokens = actualAuthTokensMap["login"];
    auto initialAuthTokens = initialAuthTokensMap["login"];

    EXPECT_EQ(actualAuthTokens.size(), 1);
    EXPECT_EQ(initialAuthTokens.size(), 1);

    EXPECT_NE(
        actualAuthTokens.front().accessToken.value, initialAuthTokens.front().accessToken.value);
    EXPECT_EQ(
        actualAuthTokens.front().refreshToken.value,
        initialAuthTokens.front().refreshToken.value);
}

TEST_F(TestAuthTokenStorage, validateAfterGetTokensByRefreshToken)
{
    http::Credentials credentials{"login", "password"};
    http::AuthTokens tokens;
    ASSERT_NO_THROW(tokens = m_storage.getTokens(credentials)); // NOLINT

    auto permissions = m_storage.validate(tokens.accessToken.value); // NOLINT
    ASSERT_FALSE(permissions.groups.isEmpty());
    EXPECT_TRUE(permissions.groups.contains("LADS"));

    http::AuthTokens newTokens = m_storage.getTokens(tokens.refreshToken.value);

    auto newPermissions = m_storage.validate(newTokens.accessToken.value);
    ASSERT_FALSE(newPermissions.groups.isEmpty());
    EXPECT_TRUE(newPermissions.groups.contains("LADS"));
}

TEST_F(TestAuthTokenStorage, simultaneuslyAuthentication)
{
    http::Credentials credentials{"login", "password"};

    auto firstUserTokens = m_storage.getTokens(credentials);
    auto firstUserPermissions = m_storage.validate(firstUserTokens.accessToken.value);
    ASSERT_FALSE(firstUserPermissions.groups.isEmpty());

    auto secondUserTokens = m_storage.getTokens(credentials);
    auto secondUserPermissions = m_storage.validate(secondUserTokens.accessToken.value);
    ASSERT_FALSE(secondUserPermissions.groups.isEmpty());

    EXPECT_TRUE(firstUserPermissions.match(secondUserPermissions));

    auto firstUserNewPermissions = m_storage.validate(firstUserTokens.accessToken.value);
    EXPECT_TRUE(firstUserNewPermissions.match(firstUserPermissions));
}

#endif
