#include "httpws/HtpasswdAuthStorage.h"

#include "exception/FileError.h"
#include "qttypes/Filesystem.h"

#include <QFile>

#include <unistd.h>

namespace http
{

namespace
{

QString findLoginHash(const std::filesystem::path& path, const QString& login)
{
    QFile htpasswd(qt::fromFilesystemPath(path));
    if (!htpasswd.open(QIODevice::ReadOnly))
    {
        BOOST_THROW_EXCEPTION(exception::FileError() << exception::FileInfo(path));
    }

    for (auto line = htpasswd.readLine(); !line.isEmpty(); line = htpasswd.readLine())
    {
        auto lineStr = QString::fromUtf8(line).trimmed();
        auto userData = lineStr.split(":");
        if (userData.size() != 2)
        {
            BOOST_THROW_EXCEPTION(exception::FileError() << exception::FileInfo(path));
        }

        if (userData.at(0) == login)
        {
            return userData.at(1);
        }
    }

    return {};
}

} // namespace

HtpasswdAuthStorage::HtpasswdAuthStorage(std::filesystem::path path) : m_path(std::move(path))
{
}

bool HtpasswdAuthStorage::validate(const QString& login, [[maybe_unused]] const QString& password)
{
    if (auto hash = findLoginHash(m_path, login); !hash.isEmpty())
    {
        return hash ==
#ifdef __USE_MISC
               ::crypt(password.toUtf8().data(), hash.section("$", 0, 2).toUtf8().data());
#else
               // TODO: This is mingw stub. Result is not relevant, only compilation
               "--";
#endif
    }
    return false;
}

} // namespace http
