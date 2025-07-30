/** @file
 * @brief     Header view state converter implementation
 *
 * $Id: $
 */

#include "uistatesaver/converters/HeaderViewStateConverter.h"

#include <QEvent>
#include <QHeaderView>

namespace uistatesaver
{

HeaderViewStateConverterInjection::HeaderViewStateConverterInjection(
    QHeaderView* header, QString id) :
  StateConverterInjection(std::move(id), header), m_header(header)
{
    connect(
        m_header,
        &QHeaderView::sectionCountChanged,
        this,
        &HeaderViewStateConverterInjection::requestSave);
    connect(
        m_header,
        &QHeaderView::sectionMoved,
        this,
        &HeaderViewStateConverterInjection::requestSave);
    connect(
        m_header,
        &QHeaderView::sectionResized,
        this,
        &HeaderViewStateConverterInjection::requestSave);
    connect(
        m_header,
        &QHeaderView::sortIndicatorChanged,
        this,
        &HeaderViewStateConverterInjection::requestSave);

    m_header->installEventFilter(this);
}

bool HeaderViewStateConverterInjection::eventFilter(QObject* object, QEvent* event)
{
    Q_UNUSED(object);
    if (event->type() == QEvent::Show)
    {
        requestRestore();
    }

    return StateConverterInjection::eventFilter(object, event);
}

QByteArray HeaderViewStateConverterInjection::state() const
{
    return m_header->saveState();
}

void HeaderViewStateConverterInjection::restore(const QByteArray& state)
{
    m_header->restoreState(state);
    m_header->reset();
    StateConverterInjection::restore(state);
}

} // namespace uistatesaver
