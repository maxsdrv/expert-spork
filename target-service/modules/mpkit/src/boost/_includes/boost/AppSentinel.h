/** @file
 * @brief  Application sentinel class. Declaration.
 *
 *
 * $Id: $
 */

#pragma once

#include <boost/noncopyable.hpp>

#include <memory>
#include <string>

namespace boost
{

namespace interprocess
{

class file_lock;
class mapped_region;

} // namespace interprocess

} // namespace boost

namespace appkit
{

/**
 * Application sentinel keeps only one running application
 * instance per system. It is using system-global synchronization
 * mechanisms in order to prevent duplicate launches.
 * In case of duplicate launch already run instance can get a message
 * for proper response.
 *
 * Global ownership supported by file lock mechanism.
 * It has process lifetime, so any crash will automatically release ownership.
 * Global syncronization made with named mutex, created on-the-fly.
 * Message exchange created through shared memory mechanism.
 */
class AppSentinel : public boost::noncopyable
{

public:
    /**
     * Created sentinel with system-wide name
     *
     * @param name application specific name
     * WARNING This MUST be hardcoded or configured in caller code (not random)
     */
    explicit AppSentinel(std::string name);
    ~AppSentinel();

    /**
     * Try to obtain ownership
     *
     * @param message sent in case of duplicate application launch
     * @return true in case of success, false otherwise
     */
    bool lock(const std::string& message = std::string());

    /**
     * Read message sent in case of duplicate application launch
     */
    std::string readMessage();

private:
    std::string m_name;
    int m_id;
    bool m_own;
    std::unique_ptr<boost::interprocess::mapped_region> m_region;
    std::unique_ptr<boost::interprocess::file_lock> m_lock;
};

} // namespace appkit
