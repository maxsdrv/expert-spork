/** @file
 * @brief     State converter base class implementation
 *
 * $Id: $
 */

#include "StateConverter.h"

#include <QAction>
#include <QHeaderView>
#include <QSplitter>
#include <QWidget>

namespace uistatesaver
{

namespace
{

class CreateInjection : public boost::static_visitor<>
{
public:
    CreateInjection(StateConverter* stateConverter, QObject* recipient) :
      m_stateConverter(stateConverter), m_recipient(recipient)
    {
    }

    template <typename Traits>
    void operator()(Traits traits) const
    {
        Q_UNUSED(traits);

        auto object = qobject_cast<typename Traits::ObjectType*>(m_recipient);
        if (object != nullptr)
        {
            QVariant id = object->property(statePropertyKey(object).toLatin1().constData());
            if (id.isValid())
            {
                auto injection = new typename Traits::InjectionType(object, id.toString());

                QObject::connect(
                    injection,
                    &StateConverterInjection::saveRequested,
                    m_stateConverter,
                    &StateConverter::save);

                QObject::connect(
                    injection,
                    &StateConverterInjection::restoreRequested,
                    m_stateConverter,
                    &StateConverter::restore);

                m_created = true;
            }
        }
    }

    bool created() const
    {
        return m_created;
    }

private:
    StateConverter* m_stateConverter;
    QObject* m_recipient;
    mutable bool m_created = false;
};

} // namespace

StateConverter::StateConverter(std::shared_ptr<StateStorage> storage, QObject* parent) :
  QObject(parent), m_storage(std::move(storage))
{
    Q_ASSERT(m_storage != nullptr);
}

bool StateConverter::inject(QObject* recipient, ConverterTraits traits)
{
    CreateInjection injectionCreator(this, recipient);
    boost::apply_visitor(injectionCreator, traits);
    return injectionCreator.created();
}

void StateConverter::save(StateConverterInjection* injection)
{
    m_storage->set(injection->id(), injection->state());
}

void StateConverter::restore(StateConverterInjection* injection)
{
    injection->restore(m_storage->get(injection->id()));
}

} // namespace uistatesaver
