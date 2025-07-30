#include "mpk/rest/ApiDescriptionJsonFile.h"

#include "httpws/HttpRoute.h"

#include "appkit/Paths.h"

#include "qt/Strings.h"

#include <gtest/gtest.h>

using namespace mpk::rest;

class TestApiDescriptionJsonFile : public ::testing::Test
{
public:
    TestApiDescriptionJsonFile() : m_apiDescription(appkit::configFile("api_description.json"))
    {
    }

protected:
    const ApiDescription& apiDescription() const
    {
        return m_apiDescription;
    }

    const ApiDescriptionJsonFile m_apiDescription;
};

TEST_F(TestApiDescriptionJsonFile, nonExistent)
{
    EXPECT_THROW(                                     // NOLINT
        apiDescription().route("get_something_else"), // NOLINT
        std::exception);                              // NOLINT
}

TEST_F(TestApiDescriptionJsonFile, getSomething)
{
    auto route = apiDescription().route("get_something");
    EXPECT_EQ(http::HttpMethod::GET, route.method);
    EXPECT_EQ("get_something", route.path);
    ASSERT_EQ(2, route.permissions.groups.size());
    EXPECT_TRUE(route.permissions.unauthorized());
    EXPECT_TRUE(route.permissions.groups.contains("ADMIN"));
    EXPECT_FALSE(route.permissions.groups.contains("LAMER"));
}

TEST_F(TestApiDescriptionJsonFile, setSomething)
{
    auto route = apiDescription().route("set_something");
    EXPECT_EQ(http::HttpMethod::POST, route.method);
    EXPECT_EQ("set_something", route.path);
    ASSERT_EQ(1, route.permissions.groups.size());
    EXPECT_TRUE(route.permissions.groups.contains("ADMIN"));
}

// TODO: Add tests for PUT and DELETE methods.
// TODO: Add tests for the {id} in the path.
