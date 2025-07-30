/**
 * @file
 * @brief      Class for simplifying ptree parsing
 *
 * $Id: $
 */

#pragma once

#include "mpkconfig_export.h"

#include "ArrayOption.h"
#include "IndexedOption.h"
#include "SubtreeOption.h"
#include "ValueOption.h"

#include <boost/make_shared.hpp>
#include <boost/optional.hpp>

#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/property_tree/ptree.hpp>

#include <iostream>
#include <map>
#include <set>

namespace appkit::config
{

/**
 * Use this class to parse your configs from boost::ptree
 */
class MPKCONFIG_EXPORT ConfigParser
{
public:
    /** General predefined options for every config */
    enum ProgramOption
    {
        HELP,    /**< --help, -h */
        VERSION, /**< --version, -v */
        CONFIG   /**< --config, -c */
    };

    using ProgramOptions = std::set<ProgramOption>;

public:
    /**
     * Add @a ptree to ConfigParser.
     * The @a ptree source is indicated by @a source
     * The later tree merges to the former one.
     */
    ConfigParser& addPtree(const boost::property_tree::ptree& ptree, const std::string& source);

    /**
     * Reads all options from ptree recursively
     */
    void readAll() const;

    /**
     * Get all known keys including subtree keys
     */
    std::set<std::string> keys() const;

    /**
     * Get all known keys including subtree keys and program option keys
     */
    std::set<std::string> allKeys() const;

    /**
     * Get informational options
     * If this options supplied, program print something useful and exit
     */
    static ProgramOptions infoOptions(int argc, char** argv);

    /**
     * Add option (value + setter function)
     */
    template <class Type>
    ValueOption<Type>& addOption(
        const std::string& key,
        const std::string& description,
        boost::function<void(const Type&)> setter)
    {
        return addOptionImpl<ValueOption, Type>(key, description, setter);
    }

    /**
     * Add option (value + value ref)
     */
    template <class Type>
    ValueOption<Type>& addOption(
        const std::string& key, const std::string& description, Type& value)
    {
        return addOptionImpl<ValueOption, Type>(
            key, description, boost::lambda::var(value) = boost::lambda::_1);
    }

    /**
     * Add option (array + setter function)
     */
    template <class Type>
    void addArrayOption(
        const std::string& key,
        const std::string& description,
        boost::function<void(const Type&)> setter)
    {
        addOptionImpl<ArrayOption, Type>(key, description, setter);
    }

    /**
     * Add option (subtree + parser)
     */
    void addSubtreeOption(
        const std::string& key, const std::string& description, const ConfigParser& subtreeParser)
    {
        verifyKey(key);

        std::shared_ptr<SubtreeOption> option{new SubtreeOption(key, description, subtreeParser)};
        m_options[key] = option;
    }

    /**
     * Add indexed option
     */
    template <class Type>
    IndexedOption<Type>& addIndexedOption(
        const std::string& key,
        const std::string& description,
        boost::function<void(int, const Type&)> setter)
    {
        return addOptionImpl<IndexedOption, Type>(key, description, setter);
    }

    /**
     * Print usage
     */
    void printUsage() const;

    /**
     * Print program options text
     */
    static void printProgramOptionsUsage(std::ostream& stream, int indent);

    /**
     * Print config options usage
     */
    void printConfigOptionsUsage(std::ostream& stream, int indent) const;

    /**
     * Write whole config to JSON-formatted string
     */
    std::string toJson() const;

private:
    /**
     * Add option (internal magic)
     */
    template <template <typename> class O, typename T, typename Setter>
    O<T>& addOptionImpl(const std::string& key, const std::string& description, Setter setter)
    {
        verifyKey(key);

        std::shared_ptr<O<T>> option{new O<T>(setter, key, description)};
        m_options[key] = option;
        return *option;
    }

    /**
     * Check key for duplicate one
     */
    void verifyKey(const std::string& key) const;

private:
    using Options = std::map<std::string, std::shared_ptr<Option>>;
    Options m_options;
    boost::property_tree::ptree m_config;

    using KeySourceMap = std::map<std::string, std::string>;
    KeySourceMap m_keySources;
};

/**
 * @brief configFileName
 * @param argc
 * @param argv
 * @param defaultConfigFile
 * @return actual config file name
 */
std::string MPKCONFIG_EXPORT
configFileName(int argc, char** argv, const std::string& defaultConfigFile);

/**
 * @brief cmdLineToString
 * @param argc
 * @param argv
 * @return command line as string
 */
std::string MPKCONFIG_EXPORT cmdLineToString(int argc, char** argv);

} // namespace appkit::config
