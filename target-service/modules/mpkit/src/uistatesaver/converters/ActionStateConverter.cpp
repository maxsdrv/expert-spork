/** @file
 * @brief     Action state converter implementation
 *
 * $Id: $
 */

#include "uistatesaver/converters/ActionStateConverter.h"

#include <QAction>

namespace uistatesaver
{

ActionStateConverterInjection::ActionStateConverterInjection(QAction* action, QString id) :
  StateConverterInjection(std::move(id), action), m_action(action)
{
    connect(m_action, &QAction::toggled, this, &ActionStateConverterInjection::requestSave);

    // We should restore state, but nobody is connected to restoreRequested,
    // so we do it via QueuedConnection invocation
    QMetaObject::invokeMethod(this, "requestRestore", Qt::QueuedConnection);
}

QByteArray ActionStateConverterInjection::state() const
{
    return QByteArray(1, m_action->isChecked() ? 1 : 0);
}

void ActionStateConverterInjection::restore(const QByteArray& state)
{
    if (!state.isEmpty())
    {
        m_action->setChecked(state[0] != 0);
    }
    StateConverterInjection::restore(state);
}

} // namespace uistatesaver
