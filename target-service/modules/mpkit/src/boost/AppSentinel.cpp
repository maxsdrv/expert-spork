/** @file
 * @brief  Application sentinel class. Implementation.
 *
 *
 * $Id: $
 */

#include "boost/AppSentinel.h"

#include "Platform.h"

#include "exception/BadRange.h"

// This is workaround for MSVS operating system, that have no POSIX
// interprocess system calls implemented
#ifdef PLATFORM_MSVS
#define USE_BOOST_XSI_INTERPROCESS
#else
#define USE_BOOST_POSIX_INTERPROCESS
#endif

#if defined USE_BOOST_XSI_INTERPROCESS
#include <boost/interprocess/errors.hpp>
#include <boost/interprocess/xsi_key.hpp>
#include <boost/interprocess/xsi_shared_memory.hpp>
#elif defined USE_BOOST_POSIX_INTERPROCESS
#include <boost/interprocess/shared_memory_object.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#endif

#include <boost/interprocess/mapped_region.hpp>
#include <boost/interprocess/sync/file_lock.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>

#include <boost/filesystem/fstream.hpp>
#include <boost/filesystem/operations.hpp>

#include <cstring>

#if defined USE_BOOST_XSI_INTERPROCESS
#include <map>

#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
#endif

using namespace boost::interprocess;
using namespace boost::filesystem;

namespace appkit
{

namespace
{

const std::size_t SHM_STRING_SIZE = 256;

// Thats for XSI key only, can be any value
#if defined USE_BOOST_XSI_INTERPROCESS
const int xsi_id = 0x9D;

class XsiNamedMutex
{
public:
    explicit XsiNamedMutex(const char* name) : m_semid(-1)
    {
        key_t key;
        if (name)
        {
            path filepath = temp_directory_path();
            filepath /= name;

            key = ::ftok(filepath.string().c_str(), xsi_id);
            if (((key_t)-1) == key)
            {
                error_info err = system_error_code();
                throw interprocess_exception(err);
            }
        }

        int sem_id = ::semget(key, 1, 0x01FF | IPC_CREAT);
        if (sem_id < 0)
        {
            error_info err = system_error_code();
            throw interprocess_exception(err);
        }

        m_semid = sem_id;
        m_ids.insert(std::pair<const char*, int>(name, sem_id));
    }

    void lock()
    {
        const short int lock_op = 1;
        execOp(lock_op);
    }

    void unlock()
    {
        const short int unlock_op = -1;
        execOp(unlock_op);
    }

    static void remove(const char* name)
    {
        SemIds::iterator it = m_ids.find(name);
        if (it != m_ids.end())
        {
            ::semctl(it->second, IPC_RMID, 0);
            m_ids.erase(it);
        }
    }

private:
    void execOp(short int op)
    {
        ::sembuf sem_op[1];
        sem_op[0].sem_num = 0;
        sem_op[0].sem_op = op;
        sem_op[0].sem_flg = SEM_UNDO;

        if (::semop(m_semid, &sem_op[0], 1) < 0)
        {
            error_info err = system_error_code();
            throw interprocess_exception(err);
        }
    }

private:
    int m_semid;
    typedef std::map<const char*, int> SemIds;
    static SemIds m_ids;
};

XsiNamedMutex::SemIds XsiNamedMutex::m_ids;

#endif

std::string getString(const mapped_region& region)
{
    char* ptr = static_cast<char*>(region.get_address()); // NOLINT
    std::string str(ptr);
    ptr[0] = 0;
    return str;
}

void setString(const mapped_region& region, const std::string& str)
{
    std::strncpy(static_cast<char*>(region.get_address()), str.c_str(), SHM_STRING_SIZE);
}

} // namespace

AppSentinel::AppSentinel(std::string name) : m_name(std::move(name)), m_id(0), m_own(false)
{
#if defined USE_BOOST_XSI_INTERPROCESS
    XsiNamedMutex::remove(m_name.c_str());
#elif defined USE_BOOST_POSIX_INTERPROCESS
    (void)m_id;
    named_mutex::remove(m_name.c_str());
#endif
}

AppSentinel::~AppSentinel()
{
#if defined USE_BOOST_XSI_INTERPROCESS
    XsiNamedMutex::remove(m_name.c_str());
#elif defined USE_BOOST_POSIX_INTERPROCESS
    named_mutex::remove(m_name.c_str());
#endif
    if (m_own)
    {
#if defined USE_BOOST_XSI_INTERPROCESS
        xsi_shared_memory::remove(m_id);
#elif defined USE_BOOST_POSIX_INTERPROCESS
        shared_memory_object::remove(m_name.c_str());
#endif
    }
}

bool AppSentinel::lock(const std::string& message)
{
    // Global lock file
    path filepath = temp_directory_path();
    filepath /= m_name.c_str();
    if (!exists(filepath))
    {
        fstream file;
        file.open(filepath, std::ios_base::out);
    }

    // Global synchronization
#if defined USE_BOOST_XSI_INTERPROCESS
    XsiNamedMutex mtx(m_name.c_str());
    scoped_lock<XsiNamedMutex> locker(mtx);
#elif defined USE_BOOST_POSIX_INTERPROCESS
    named_mutex mtx(open_or_create, m_name.c_str());
    scoped_lock<named_mutex> locker(mtx);
#endif

    // Global locking
    m_lock = std::make_unique<file_lock>(filepath.string().c_str());

    if (m_lock->try_lock())
    {
        // Create memory object
#if defined USE_BOOST_XSI_INTERPROCESS
        xsi_key key(filepath.string().c_str(), xsi_id);
        xsi_shared_memory shm(open_or_create, key, SHM_STRING_SIZE);
        m_id = shm.get_shmid();
#elif defined USE_BOOST_POSIX_INTERPROCESS
        shared_memory_object shm(open_or_create, m_name.c_str(), read_write);
        shm.truncate(SHM_STRING_SIZE);
#endif
        m_region = std::make_unique<mapped_region>(shm, read_write);
        m_own = true;
    }
    else
    {
#if defined USE_BOOST_XSI_INTERPROCESS
        xsi_key key(filepath.string().c_str(), xsi_id);
        xsi_shared_memory shm(open_only, key);
#elif defined USE_BOOST_POSIX_INTERPROCESS
        shared_memory_object shm(open_only, m_name.c_str(), read_write);
#endif
        m_region = std::make_unique<mapped_region>(shm, read_write);

        if (!message.empty())
        {
            if (message.size() > SHM_STRING_SIZE - 1)
            {
                BOOST_THROW_EXCEPTION(
                    exception::BadRange()
                    << exception::ExceptionInfo("Message string is too large "));
            }

            setString(*m_region, message);
        }
    }

    return m_own;
}

std::string AppSentinel::readMessage()
{
    if (m_region)
    {
#if defined USE_BOOST_XSI_INTERPROCESS
        XsiNamedMutex mtx(m_name.c_str());
        scoped_lock<XsiNamedMutex> locker(mtx);
#elif defined USE_BOOST_POSIX_INTERPROCESS
        named_mutex mtx(open_or_create, m_name.c_str());
        scoped_lock<named_mutex> locker(mtx);
#endif

        return getString(*m_region);
    }

    return {};
}

} // namespace appkit
