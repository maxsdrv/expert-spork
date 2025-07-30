#include "httpws/AuthType.h"

#include "exception/InvalidValue.h"

#include <gtest/gtest.h>

namespace http_test_AuthType
{

using namespace http;

TEST(TestAuthType, value)
{
    EXPECT_EQ(AuthType::BASIC, AuthType::fromString("Basic"));
    EXPECT_EQ(AuthType::BEARER, AuthType::fromString("Bearer"));
    EXPECT_THROW(AuthType::fromString("Barr"), exception::InvalidValue);
}

TEST(TestAuthType, name)
{
    EXPECT_EQ(QStringLiteral("Basic"), AuthType::toString(AuthType::BASIC));
    EXPECT_EQ(QStringLiteral("Bearer"), AuthType::toString(AuthType::BEARER));
}

} // namespace http_test_AuthType
