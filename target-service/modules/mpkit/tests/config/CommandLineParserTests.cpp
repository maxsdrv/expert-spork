/*
 * @file
 * @brief Command line parser tests
 *
 *
 * $Id: $
 */

#include "config/PTreeRW.h"

#include <gtest/gtest.h>

namespace appkit
{

using namespace boost::property_tree;

typedef std::set<std::string> Keys;

// Command line parser tests
TEST(ParseCommandLineTest, noParameters)
{
    char arg0[] = "programName";
    char* argv[] = {&arg0[0], NULL};
    int argc = (int)(sizeof(argv) / sizeof(argv[0])) - 1;

    ptree settings = config::fromCommandLine(argc, argv, Keys(), true);

    EXPECT_TRUE(settings.empty());
}

TEST(ParseCommandLineTest, oneParameter)
{
    char arg0[] = "programName";
    char arg1[] = "--test";
    char arg2[] = "TEST";
    char* argv[] = {&arg0[0], &arg1[0], &arg2[0], NULL};
    int argc = (int)(sizeof(argv) / sizeof(argv[0])) - 1;

    Keys keys;
    keys.insert("test");
    ptree settings = config::fromCommandLine(argc, argv, keys, true);

    ASSERT_FALSE(settings.empty());

    ASSERT_NO_THROW(settings.get<std::string>("test"));

    EXPECT_EQ("TEST", settings.get<std::string>("test"));
}

TEST(ParseCommandLineTest, someParameters)
{
    char arg0[] = "programName";
    char arg1[] = "--master.server.host";
    char arg2[] = "remote";
    char arg3[] = "--server.port";
    char arg4[] = "8888";
    char arg5[] = "--token";
    char arg6[] = "hklmn";
    char* argv[] = {&arg0[0],
                    &arg1[0],
                    &arg2[0],
                    &arg3[0],
                    &arg4[0],
                    &arg5[0],
                    &arg6[0],
                    NULL};
    int argc = (int)(sizeof(argv) / sizeof(argv[0])) - 1;

    Keys keys;
    keys.insert("master.server.host");
    keys.insert("server.port");
    keys.insert("token");

    ptree settings = config::fromCommandLine(argc, argv, keys, true);

    ASSERT_NO_THROW(settings.get<std::string>("token"));
    EXPECT_EQ("hklmn", settings.get<std::string>("token"));

    ASSERT_NO_THROW(settings.get<int>("server.port"));
    EXPECT_EQ(8888, settings.get<int>("server.port"));

    ASSERT_NO_THROW(settings.get<std::string>("master.server.host"));
    EXPECT_EQ("remote", settings.get<std::string>("master.server.host"));
}

TEST(ParseCommandLineTest, noValueParameter)
{
    char arg0[] = "programName";
    char arg1[] = "--master.server.host";
    char* argv[] = {&arg0[0], &arg1[0], NULL};
    int argc = (int)(sizeof(argv) / sizeof(argv[0])) - 1;

    Keys keys;
    keys.insert("master.server.host");

    EXPECT_THROW(
        config::fromCommandLine(argc, argv, keys, true), std::exception);
}

TEST(ParseCommandLineTest, invalidParameter)
{
    char arg0[] = "programName";
    char arg1[] = "--master.server.host";
    char* argv[] = {&arg0[0], &arg1[0], NULL};
    int argc = (int)(sizeof(argv) / sizeof(argv[0])) - 1;

    Keys keys;
    keys.insert("host");

    EXPECT_THROW(
        config::fromCommandLine(argc, argv, keys, true), std::exception);
    EXPECT_NO_THROW(config::fromCommandLine(argc, argv, keys, false));
}

TEST(ParseCommandLineTest, twoEqualParameters)
{
    char arg0[] = "programName";
    char arg1[] = "--master.server.host";
    char arg2[] = "remote";
    char arg3[] = "--master.server.host";
    char arg4[] = "local";

    Keys keys;
    keys.insert("master.server.host");

    char* argv[] = {&arg0[0], &arg1[0], &arg2[0], &arg3[0], &arg4[0], NULL};
    int argc = (int)(sizeof(argv) / sizeof(argv[0])) - 1;

    ASSERT_THROW(
        config::fromCommandLine(argc, argv, keys, true), std::exception);
}

} // namespace appkit
