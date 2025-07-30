/** @file
 * @brief
 *
 * @ingroup
 *
 *
 * $Id: $
 */

#include "DelayedExecution.h"

#include <QTimer>

namespace gqtest
{

DelayedExecution::DelayedExecution()
{
    QTimer::singleShot(0, this, SLOT(execute()));
}

void DelayedExecution::addAction(DelayedExecution::Function action)
{
    m_actions.emplace_back(std::move(action));
}

void DelayedExecution::sendComplete()
{
    emit complete();
}

void DelayedExecution::execute()
{
    for (const auto& action: m_actions)
    {
        action();
    }
}

} // namespace gqtest
