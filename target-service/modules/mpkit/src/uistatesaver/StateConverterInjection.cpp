/** @file
 * @brief     State converter injection
 *
 * $Id: $
 */

#include "uistatesaver/StateConverterInjection.h"

namespace uistatesaver
{

StateConverterInjection::StateConverterInjection(QString id, QObject* parent) :
  QObject(parent), m_id(std::move(id))
{
}

QString StateConverterInjection::id() const
{
    return m_id;
}

void StateConverterInjection::restore(const QByteArray& /*state*/)
{
    m_wasRestored = true;
}

void StateConverterInjection::requestSave()
{
    if (m_wasRestored)
    {
        emit saveRequested(this);
    }
}

void StateConverterInjection::requestRestore()
{
    if (!m_wasRestored)
    {
        emit restoreRequested(this);
    }
}

} // namespace uistatesaver
