#include "httpws/JsonAuthStorage.h"

#include "gqtest"

#include "appkit/Paths.h"

#include "qt/Strings.h"
#include "utils/RandomString.h"

#include <QJsonDocument>

#include <filesystem>

#ifndef GTEST_OS_WINDOWS

using namespace http;

QString fileCopy(const std::string& filename)
{
    auto newFileName = std::filesystem::temp_directory_path();
    newFileName /= utils::randomLatinString(8) + ".json";
    std::filesystem::copy_file(
        filename, newFileName, std::filesystem::copy_options::overwrite_existing);
    return strings::fromUtf8(newFileName.string());
}

class TestJsonAuthStorage: public ::testing::Test
{
public:
    TestJsonAuthStorage():
        m_storage(fileCopy(appkit::configFile("auth.json")))
    {
        // m_storage contains one record with
        // login: "admin" and password: "admin"
    }

protected:
    JsonAuthStorage m_storage;
};

TEST_F(TestJsonAuthStorage, validateCorrect)
{
    EXPECT_TRUE(m_storage.validate("admin", "admin"));
}

TEST_F(TestJsonAuthStorage, validateWrongLogin)
{
    EXPECT_FALSE(m_storage.validate("nimda", "admin"));
}

TEST_F(TestJsonAuthStorage, validateWrongPassword)
{
    EXPECT_FALSE(m_storage.validate("admin", "nimda"));
}

#endif
