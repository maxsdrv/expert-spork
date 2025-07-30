/** @file
 * @brief
 *
 * @ingroup
 *
 * $Id: $
 */

#include "config/SubtreeOption.h"

#include "config/ConfigParser.h"

#include <boost/algorithm/string/trim.hpp>
#include <boost/format.hpp>

namespace appkit
{

namespace config
{

std::string SubtreeOption::description(int indent) const
{
    std::string str;
    std::ostringstream strstream(str);
    m_parser->printConfigOptionsUsage(strstream, indent + 1);

    return descriptionValue() + "/n" + strstream.str();
}

SubtreeOption::SubtreeOption(
    const std::string& key, const std::string& description, const ConfigParser& parser) :
  Option(key, description), m_parser(new ConfigParser(parser))
{
}

void SubtreeOption::read(const boost::property_tree::ptree& node)
{
    // Read subtree
    boost::property_tree::ptree subtree = node.get_child(key());

    ConfigParser parser(*m_parser);
    parser.addPtree(subtree, "");
    parser.readAll();
}

std::set<std::string> SubtreeOption::keys() const
{
    std::set<std::string> fullKeys;
    const std::string prefix = key() + ".";
    for (const auto& k: m_parser->keys())
    {
        fullKeys.insert(prefix + k);
    }

    return fullKeys;
}

} // namespace config

} // namespace appkit
