#include "mpk/rest/UserPermissionsJsonFile.h"
#include "mpk/rest/UserPermissionsStatic.h"

#include "appkit/Paths.h"
#include "qt/Strings.h"

#include <gtest/gtest.h>

using namespace mpk::rest;

static const UserPermissionsStatic::GroupsMap defaultGroups = {
    {"ADMIN", {"vasya", "petya", "kolya"}}, {"USER", {"masha", "katya", "vasya"}}};

class TestAccessRolesStatic : public ::testing::Test
{
public:
    TestAccessRolesStatic() : m_userPermissions(defaultGroups)
    {
    }

protected:
    UserPermissionsStatic m_userPermissions;
};

TEST_F(TestAccessRolesStatic, userRolesEmpty)
{
    auto permissions = m_userPermissions.list();
    ASSERT_EQ(2, permissions.groups.count());
    EXPECT_TRUE(permissions.groups.contains("ADMIN"));
    EXPECT_TRUE(permissions.groups.contains("USER"));
    EXPECT_FALSE(permissions.groups.contains("LAMER"));
}

TEST_F(TestAccessRolesStatic, userRolesInvalid)
{
    auto fedyaPermissions = m_userPermissions.list("fedya");
    EXPECT_TRUE(fedyaPermissions.groups.isEmpty());
}

TEST_F(TestAccessRolesStatic, userRolesSingle)
{
    auto petyaPermissions = m_userPermissions.list("petya");
    ASSERT_EQ(1, petyaPermissions.groups.count());
    EXPECT_TRUE(petyaPermissions.groups.contains("ADMIN"));
}

TEST_F(TestAccessRolesStatic, userRolesMulti)
{
    auto vasyaPermissions = m_userPermissions.list("vasya");
    ASSERT_EQ(2, vasyaPermissions.groups.count());
    EXPECT_TRUE(vasyaPermissions.groups.contains("ADMIN"));
    EXPECT_TRUE(vasyaPermissions.groups.contains("USER"));
    EXPECT_FALSE(vasyaPermissions.groups.contains("LAMER"));
}

class TestAccessRolesJson : public ::testing::Test
{
public:
    TestAccessRolesJson() : m_userPermissions(appkit::configFile("permission_groups.json"))
    {
    }

protected:
    UserPermissionsJsonFile m_userPermissions;
};

TEST_F(TestAccessRolesJson, userRolesEmpty)
{
    auto permissions = m_userPermissions.list();
    ASSERT_EQ(2, permissions.groups.count());
    EXPECT_TRUE(permissions.groups.contains("ADMIN"));
    EXPECT_TRUE(permissions.groups.contains("USER"));
    EXPECT_FALSE(permissions.groups.contains("LAMER"));
}

TEST_F(TestAccessRolesJson, userRolesInvalid)
{
    auto fedyaPermissions = m_userPermissions.list("fedya");
    EXPECT_TRUE(fedyaPermissions.groups.isEmpty());
}

TEST_F(TestAccessRolesJson, userRolesSingle)
{
    auto petyaPermissions = m_userPermissions.list("petya");
    ASSERT_EQ(1, petyaPermissions.groups.count());
    EXPECT_TRUE(petyaPermissions.groups.contains("ADMIN"));
}

TEST_F(TestAccessRolesJson, userRolesMulti)
{
    auto vasyaPermissions = m_userPermissions.list("vasya");
    ASSERT_EQ(2, vasyaPermissions.groups.count());
    EXPECT_TRUE(vasyaPermissions.groups.contains("ADMIN"));
    EXPECT_TRUE(vasyaPermissions.groups.contains("USER"));
    EXPECT_FALSE(vasyaPermissions.groups.contains("LAMER"));
}
