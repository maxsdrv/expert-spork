/** @file
 * @brief     State saver implementation
 *
 * $Id: $
 */

#include "uistatesaver/UiStateSaver.h"

#include "DeferredStorage.h"
#include "QSettingsStorage.h"
#include "StateConverter.h"
#include "uistatesaver/StateConverterFactory.h"

#include <QEvent>

namespace uistatesaver
{

UiStateSaver::UiStateSaver(
    ConverterTraitsVector converters,
    std::unique_ptr<StateConverter> stateConverter,
    QObject* parent) :
  QObject(parent),
  m_enabledConverters(std::move(converters)),
  m_stateConverter(std::move(stateConverter))
{
}

void UiStateSaver::install(QObject* recipient)
{
    install(recipient, INJECT_CHILDREN);
}

void UiStateSaver::install(QObject* recipient, ChildrenPolicy policy)
{
    if (recipient != nullptr)
    {
        for (const auto& converter: m_enabledConverters)
        {
            if (m_stateConverter->inject(recipient, converter))
            {
                break;
            }
        }

        if (policy == INJECT_CHILDREN)
        {
            recipient->installEventFilter(this);
            for (QObject* child: recipient->children())
            {
                install(child);
            }
        }
    }
}

// Forward declaration
UiStateSaver::~UiStateSaver() = default;

bool UiStateSaver::eventFilter(QObject* object, QEvent* event)
{
    if (event->type() == QEvent::ChildAdded)
    {
        auto childEvent = static_cast<QChildEvent*>(event); // NOLINT
        QObject* child = childEvent->child();
        if (child->isWidgetType())
        {
            install(child);
        }
    }

    return QObject::eventFilter(object, event);
}

std::unique_ptr<UiStateSaver> createUiStateSaver(
    const QString& fileName, const std::vector<std::string>& converterIds, int saveTimeout)
{
    auto settingsStorage = std::make_shared<QSettingsStorage>(fileName);

    std::shared_ptr<uistatesaver::StateStorage> stateStorage =
        std::make_shared<uistatesaver::DeferredStorage<
            uistatesaver::StateStorage::key_type,
            uistatesaver::StateStorage::value_type>>(saveTimeout, settingsStorage);

    UiStateSaver::ConverterTraitsVector converters;

    // The critical part of algorithm. Because QSplitter, QHeaderView etc
    // are subclasses of QWidget, WidgetTraits must be the last one
    // we try to create the injection.
    for (const auto& trait: AVAILABLE_TRAITS)
    {
        if (std::any_of(
                converterIds.begin(),
                converterIds.end(),
                [trait](const auto& id) { return trait.first == id; }))
        {
            converters.push_back(trait.second);
        }
    }

    return std::make_unique<UiStateSaver>(
        converters, std::make_unique<StateConverter>(stateStorage));
}

std::unique_ptr<UiStateSaver> createUiStateSaver(const QString& fileName, int saveTimeout)
{
    auto settingsStorage = std::make_shared<QSettingsStorage>(fileName);

    std::shared_ptr<uistatesaver::StateStorage> stateStorage =
        std::make_shared<uistatesaver::DeferredStorage<
            uistatesaver::StateStorage::key_type,
            uistatesaver::StateStorage::value_type>>(saveTimeout, settingsStorage);

    UiStateSaver::ConverterTraitsVector converters;
    for (const auto& pair: AVAILABLE_TRAITS)
    {
        converters.push_back(pair.second);
    }

    return std::make_unique<UiStateSaver>(
        converters, std::make_unique<StateConverter>(stateStorage));
}

} // namespace uistatesaver
