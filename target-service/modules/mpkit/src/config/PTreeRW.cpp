/**
 * @file
 * @brief PTree read/write routines. Implementation.
 *
 *
 * $Id: $
 *
 **/

#include "config/PTreeRW.h"

#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/config.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

namespace appkit
{

namespace config
{

boost::property_tree::ptree fromCommandLine(
    int argc, char** argv, const std::set<std::string>& keys, bool strict)
{
    using namespace boost::program_options;

    boost::property_tree::ptree ptree;
    // Set the supported options.
    options_description desc;

    for (const auto& key: keys)
    {
        desc.add_options()(key.c_str(), value<std::string>());
    }

    variables_map vm;
    command_line_parser parser(argc, argv);
    parser.options(desc);
    if (!strict)
    {
        parser.allow_unregistered();
    }

    parsed_options parsed = parser.run();

    store(parsed, vm);
    notify(vm);

    for (auto vmIt = vm.begin(); vmIt != vm.end(); ++vmIt)
    {
        {
            const std::string optionValue = vm[vmIt->first].as<std::string>();
            ptree.put(vmIt->first, optionValue);
        }
    }

    return ptree;
}

} // namespace config

} // namespace appkit
