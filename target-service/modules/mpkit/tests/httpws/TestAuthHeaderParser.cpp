#include "httpws/AuthType.h"
#include "httpws/AuthUtils.h"
#include "httpws/Definitions.h"
#include "httpws/HttpException.h"

#include <gtest/gtest.h>

#include <string>

namespace http_test_AuthHeaderParser
{
using namespace http;

class TestAuthHeaderParser : public testing::Test
{
protected:
    std::string authHeader;

    void setBasicAuth(const char* userPasswdBase64)
    {
        authHeader = std::string("Basic ") + std::string(userPasswdBase64);
    }

    void setBearerAuth(const char* token)
    {
        authHeader = std::string("Bearer ") + std::string(token);
    }

    void setUnknownAuth(const char* token)
    {
        authHeader = std::string("Unknown ") + std::string(token);
    }
};

TEST_F(TestAuthHeaderParser, parse_empty_header)
{
    auto result = AuthHeaderData{};
    try
    {
        result = parseAuthHeaderParams(std::string());
        FAIL() << "exception::http::Exception must throw";
    }
    catch (exception::http::Exception& ex)
    {
        auto actualResponseStatus =
            static_cast<int>(exception::http::responseErrorStatus(ex));
        ASSERT_EQ(
            static_cast<int>(::http::HttpCode::BadRequest),
            actualResponseStatus);
    }
}

TEST_F(TestAuthHeaderParser, parse_not_complete)
{
    auto result = AuthHeaderData{};
    try
    {
        setBasicAuth("");
        result = parseAuthHeaderParams(authHeader);
        FAIL() << "exception::http::Exception must throw";
    }
    catch (exception::http::Exception& ex)
    {
        auto actualResponseStatus =
            static_cast<int>(exception::http::responseErrorStatus(ex));
        ASSERT_EQ(
            static_cast<int>(::http::HttpCode::BadRequest),
            actualResponseStatus);
    }
}

TEST_F(TestAuthHeaderParser, parse_basic_auth)
{
    auto credentials = QByteArray("guest:guest");
    setBasicAuth(credentials.toBase64());
    auto result = AuthHeaderData{};
    ASSERT_NO_THROW(result = parseAuthHeaderParams(authHeader)); // NOLINT

    auto basicParams = parseBasicAuthData(result.data);

    const int EXPECTED_AUTH_TYPE(AuthType::BASIC);
    ASSERT_EQ(EXPECTED_AUTH_TYPE, result.type);

    auto actualUser(basicParams.login.toStdString());
    ASSERT_STREQ("guest", actualUser.c_str());

    auto actualPasswd(basicParams.password.toStdString());
    ASSERT_STREQ("guest", actualPasswd.c_str());
}

TEST_F(TestAuthHeaderParser, parse_basic_auth_not_complete)
{
    auto result = AuthHeaderData{};
    try
    {
        setBasicAuth("Z3Vlc3Q6");
        result = parseAuthHeaderParams(authHeader);
        auto basicResult = parseBasicAuthData(result.data);
        FAIL() << "exception::http::Exception must throw";
    }
    catch (exception::http::Exception& ex)
    {
        auto actualResponseStatus =
            static_cast<int>(exception::http::responseErrorStatus(ex));
        ASSERT_EQ(
            static_cast<int>(::http::HttpCode::BadRequest),
            actualResponseStatus);
    }
}

TEST_F(TestAuthHeaderParser, parse_bearer)
{
    const char* EXPECTED_TOKEN = "dbbb9f7596fe1db48b14c8bf70a83a6e7800fbd4";
    setBearerAuth(EXPECTED_TOKEN);
    auto result = AuthHeaderData{};
    ASSERT_NO_THROW(result = parseAuthHeaderParams(authHeader)); // NOLINT

    const int EXPECTED_AUTH_TYPE(AuthType::BEARER);
    ASSERT_EQ(EXPECTED_AUTH_TYPE, result.type);

    auto actualToken = result.data.toLocal8Bit();
    ASSERT_STREQ(EXPECTED_TOKEN, actualToken.data());
}

TEST_F(TestAuthHeaderParser, parse_unknown_auth)
{
    auto result = AuthHeaderData{};
    try
    {
        setUnknownAuth("Z3Vlc3Q6");
        result = parseAuthHeaderParams(authHeader);
        FAIL() << "exception::http::Exception must throw";
    }
    catch (exception::http::Exception& ex)
    {
        auto actualResponseStatus =
            static_cast<int>(exception::http::responseErrorStatus(ex));
        ASSERT_EQ(
            static_cast<int>(::http::HttpCode::NotAcceptable),
            actualResponseStatus);
    }
}

} // namespace http_test_AuthHeaderParser
