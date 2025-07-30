#include "qt/SignalHandler.h"

#include <QtGlobal>

#if defined(Q_OS_UNIX)
#include "qt/UnixSignalHandler.h"
#endif

namespace appkit
{

void installSignalHandler([[maybe_unused]] QCoreApplication* app)
{
#if defined(Q_OS_UNIX)
    installUnixSignalHandler(app);
#endif // Q_OS_UNIX
}

} // namespace appkit
