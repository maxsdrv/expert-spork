#pragma once

#include <QString>

#include <filesystem>

namespace qt
{

// Convert native filesystem path to QString
[[nodiscard]] inline QString fromFilesystemPath(const std::filesystem::path& path)
{
#ifdef Q_OS_WIN
    return QString::fromStdWString(path.native());
#else
    return QString::fromStdString(path.native());
#endif
}

} // namespace qt
