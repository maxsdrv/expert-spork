/** @file
 * @brief     App test aux implementation
 *
 *
 * $Id: $
 */

#include "aux.h"

#include "app/AppSentinel.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp>

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        return INIT_ERROR;
    }

    const char* name = argv[1];
    const char* msg = argv[2];

    if (name == nullptr || msg == nullptr)
    {
        return INIT_ERROR;
    }

    using appkit::AppSentinel;
    AppSentinel sentinel(name);
    sentinel.lock();

    std::string readMsg;
    while (readMsg.empty())
    {
        boost::this_thread::sleep(boost::posix_time::milliseconds(10));
        readMsg = sentinel.readMessage();
    }

    if (msg != readMsg)
    {
        return MSG_ERROR;
    }

    return NO_ERROR;
}
