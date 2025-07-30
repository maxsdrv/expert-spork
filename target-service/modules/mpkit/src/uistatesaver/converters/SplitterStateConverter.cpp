/** @file
 * @brief     Header view state converter implementation
 *
 * $Id: $
 */

#include "uistatesaver/converters/SplitterStateConverter.h"

#include <QEvent>
#include <QSplitter>
#include <QVariant>

namespace uistatesaver
{

SplitterStateConverterInjection::SplitterStateConverterInjection(QSplitter* splitter, QString id) :
  StateConverterInjection(std::move(id), splitter), m_splitter(splitter)
{
    connect(
        splitter, &QSplitter::splitterMoved, this, &SplitterStateConverterInjection::requestSave);

    m_splitter->installEventFilter(this);
}

QByteArray SplitterStateConverterInjection::state() const
{
    return m_splitter->saveState();
}

void SplitterStateConverterInjection::restore(const QByteArray& state)
{
    m_splitter->restoreState(state);
    StateConverterInjection::restore(state);
}

bool SplitterStateConverterInjection::eventFilter(QObject* object, QEvent* event)
{
    Q_UNUSED(object);

    if (event->type() == QEvent::Show)
    {
        requestRestore();
    }

    return StateConverterInjection::eventFilter(object, event);
}

} // namespace uistatesaver
