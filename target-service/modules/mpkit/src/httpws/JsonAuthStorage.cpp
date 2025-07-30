#include "httpws/JsonAuthStorage.h"

#include "httpws/AuthType.h"
#include "httpws/HttpException.h"

#include "exception/DuplicateEntity.h"
#include "exception/ErrorInfo.h"
#include "exception/MissingEntity.h"

#include "json/JsonParser.h"
#include "json/JsonUtilities.h"

#include "log/Log2.h"
#include "qt/Strings.h"

#include <QCryptographicHash>
#include <QJsonObject>

namespace http
{

namespace
{

constexpr auto GLOBAL_SALT = "8ebe8de05e3b7e9475dc7d529f55cc57";

constexpr auto ROOT_OBJECT = "users";
constexpr auto LOGIN = "login";
constexpr auto PASSWORD = "password";
constexpr auto SALT = "salt";

auto toSha256String(const QString& value)
{
    QCryptographicHash encoder(QCryptographicHash::Sha256);
#if QT_MAJOR < 6
    encoder.addData(value.toLocal8Bit(), value.size());
#else
    encoder.addData(QByteArrayView{value.toLocal8Bit()});
#endif
    QByteArray encodeToken = encoder.result();
    auto hex = encodeToken.toHex();
    return hex;
}

auto hashPassword(const QString& password, const QString& salt)
{
    auto hashed = salt + password + GLOBAL_SALT;
    return toSha256String(hashed);
}

auto passwordIsValid(const QString& rawPass, const QString& hashedPass, const QString& salt)
{
    return hashedPass == hashPassword(rawPass, salt);
}

auto findUser(QJsonArray& users, const QString& login)
{
    return std::find_if(
        users.begin(),
        users.end(),
        [login](const auto& value)
        {
            auto user = value.toObject();
            return user.value(LOGIN) == login;
        });
}

} // namespace

JsonAuthStorage::JsonAuthStorage(const QString& path) : m_path(path)
{
    try
    {
        m_json = json::fromFile(path);
    }
    catch (const std::exception& ex)
    {
        LOG_WARNING << "Unable to load auth storage '" << m_path << "': " << ex.what();
    }
}

bool JsonAuthStorage::validate(const QString& login, const QString& password)
{
    auto users = json::fromObject<QJsonArray>(m_json, ROOT_OBJECT);
    auto it = findUser(users, login);
    if (it != users.end())
    {
        auto object = it->toObject();
        auto salt = json::fromObject<QString>(object, SALT);
        auto hash = json::fromObject<QByteArray>(object, PASSWORD);
        return passwordIsValid(password, hash, salt);
    }

    return false;
}

} // namespace http
