/** @file
 * @brief     Widget state converter implementation
 *
 * $Id: $
 */

#include "uistatesaver/converters/WidgetStateConverter.h"

#include <QChildEvent>
#include <QVariant>
#include <QWidget>

namespace uistatesaver
{

WidgetStateConverterInjection::WidgetStateConverterInjection(QWidget* widget, QString id) :
  StateConverterInjection(std::move(id), widget), m_widget(widget)
{
    m_widget->installEventFilter(this);
}

bool WidgetStateConverterInjection::eventFilter(QObject* object, QEvent* event)
{
    switch (event->type())
    {
        case QEvent::Move:
        case QEvent::Resize:
        {
            requestSave();
            break;
        }
        case QEvent::Show:
        {
            requestRestore();
            break;
        }

        default:
            // nothing to do here
            break;
    }

    return QObject::eventFilter(object, event);
}

QByteArray WidgetStateConverterInjection::state() const
{
    return m_widget->saveGeometry();
}

void WidgetStateConverterInjection::restore(const QByteArray& state)
{
    m_widget->restoreGeometry(state);
    StateConverterInjection::restore(state);
}

} // namespace uistatesaver
