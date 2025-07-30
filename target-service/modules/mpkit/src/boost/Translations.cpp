#include "boost/translations/Translations.h"

#include <filesystem>
#include <regex>

namespace mpkit::translate
{

void installBoostTranslation(const std::string& path, const std::vector<std::string>& domains)
{
    boost::locale::generator gen;
    gen.add_messages_path(path);

    for (const std::string& domain: domains)
    {
        gen.add_messages_domain(domain);
    }

    // Create an object with default locale
    std::locale locale = std::locale::global(gen(""));
    UNUSED(locale);
}

bool matchTranslationFiles(const std::filesystem::path& path)
{
    const std::regex mo_filter(".*\\.mo");
    std::smatch what;
    auto pathString = path.filename().string();
    return std::regex_match(pathString, what, mo_filter);
}

std::vector<std::string> installBoostTranslation(const std::string& path)
{
    std::vector<std::string> domains;
    std::filesystem::recursive_directory_iterator first(path);
    std::filesystem::recursive_directory_iterator last;

    for (; first != last; ++first)
    {
        if (std::filesystem::is_regular_file(*first) && matchTranslationFiles(*first))
        {
            domains.push_back(first->path().stem().string());
        }
    }

    installBoostTranslation(path, domains);
    return domains;
}

} // namespace mpkit::translate
