#include "rpc/SimpleTimer.h"

#include <QTimer>

namespace rpc
{

SimpleTimer::SimpleTimer(int timeout)
{
    // NOLINTNEXTLINE(clang-analyzer-cplusplus.NewDeleteLeaks)
    QTimer::singleShot(timeout, &m_context, [this] { m_Triggered(); });
}

} // namespace rpc
