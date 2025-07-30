/**
 * @file
 * @brief  PropertyTreeUtils. Implementation
 *
 *
 * $Id: $
 *
 **/

#include "config/PTreeUtils.h"

#include "config/PTreeRW.h"

#include "exception/InvalidConfigEntry.h"
#include "exception/KeyInfo.h"

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <boost/program_options/cmdline.hpp>
#include <boost/program_options/config.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>
#include <boost/property_tree/ptree.hpp>

#include <queue>

namespace appkit
{

namespace config
{

void validate(const boost::property_tree::ptree& ptree, const std::set<std::string>& keys)
{
    using namespace boost::property_tree;
    using namespace exception;

    std::set<std::string> leaves;
    pullLeaves(leaves, ptree);

    std::vector<std::string> invalidKeys;

    for (const auto& leaf: leaves)
    {
        if (keys.count(leaf) == 0)
        {
            invalidKeys.push_back(leaf);
        }
    }
    if (!invalidKeys.empty())
    {
        BOOST_THROW_EXCEPTION(InvalidConfigEntry() << KeysInfo(invalidKeys));
    }
}

// NOLINTNEXTLINE(misc-no-recursion)
void pullLeaves(
    std::set<std::string>& leaves,
    const boost::property_tree::ptree& ptree,
    const std::string& parent)
{
    using namespace boost::property_tree;

    for (ptree::const_iterator ptreeIt = ptree.begin(); ptreeIt != ptree.end(); ++ptreeIt)
    {
        if (ptreeIt->first.empty() && // array
            !parent.empty())
        {
            leaves.insert(parent);
            return;
        }

        std::string newPath = parent.empty() ? ptreeIt->first : parent + '.' + ptreeIt->first;

        if (ptreeIt->second.empty())
        {
            leaves.insert(newPath);
        }
        else
        {
            pullLeaves(leaves, ptreeIt->second, newPath);
        }
    }
}

// NOLINTNEXTLINE(readability-function-cognitive-complexity)
auto merge(boost::property_tree::ptree dest, const boost::property_tree::ptree& src)
    -> boost::property_tree::ptree
{
    // Keep track of keys and values (subtrees) in second property tree
    std::queue<std::string> keys;
    std::queue<boost::property_tree::ptree> values;
    values.push(src);

    // Iterate over second property tree
    while (!values.empty())
    {
        // Setup keys and corresponding values
        boost::property_tree::ptree ptree = values.front();
        values.pop();
        std::string keychain;
        if (!keys.empty())
        {
            keychain = keys.front();
            keys.pop();
        }

        boost::property_tree::ptree array;

        // Iterate over keys level-wise
        for (const auto& child: ptree)
        {
            // Leaf
            if (child.second.empty())
            {
                // No "." for first level entries
                std::string s;
                if (!keychain.empty())
                {
                    if (!child.first.empty())
                    {
                        s = keychain + "." + child.first;
                    }
                    else
                    {
                        s = keychain;
                        array.push_back(
                            std::make_pair("", boost::property_tree::ptree(child.second.data())));
                        continue;
                    }
                }
                else
                {
                    s = child.first;
                }

                // Put into combined property tree
                dest.put(s, child.second.data());
            }
            // Subtree
            else
            {
                // Put keys (identifiers of subtrees) and all of its parents
                // (where present) aside for later iteration. Keys on first
                // level have no parents
                if (!keychain.empty())
                {
                    keys.push(keychain + "." + child.first);
                }
                else
                {
                    keys.push(child.first);
                }
                // Put values (the subtrees) aside, too
                values.push(child.second);
            }
        }

        if (!array.empty())
        {
            dest.add_child(keychain, array);
            array.clear();
        }
    }

    return dest;
}

} // namespace config

} // namespace appkit
