/** @file
 * @brief     State saver specification
 *
 * $Id: $
 */

#pragma once

#include "uistatesaver/KeyValueStorage.h"
#include "uistatesaver/StateConverterFactory.h"

#include <QByteArray>
#include <QObject>
#include <QString>

#include <memory>
#include <string>
#include <vector>

namespace uistatesaver
{

class StateConverter;

/**
 * Save state from state converters to ini settings
 */
class UiStateSaver : public QObject
{
    Q_OBJECT

public:
    using ConverterTraitsVector = std::vector<ConverterTraits>;

public:
    UiStateSaver(
        ConverterTraitsVector converters,
        std::unique_ptr<StateConverter> stateConverter,
        QObject* parent = nullptr);

    enum ChildrenPolicy
    {
        INJECT_CHILDREN,
        OMIT_CHILDREN
    };

    /**
     * Installs this object to specified object on ChildAdded event
     */
    bool eventFilter(QObject* object, QEvent* event) override;

    /**
     * Sets itself as event filter to specified object and it's children
     * This function is the same as install with INJECT_CHILDREN policy
     * left for backward compatibility reasons
     */
    void install(QObject* recipient);

    /**
     * Sets itself as event filter to specified object and it's children
     */
    void install(QObject* recipient, ChildrenPolicy policy);

    ~UiStateSaver() override;

private:
    const ConverterTraitsVector m_enabledConverters;
    std::unique_ptr<StateConverter> m_stateConverter;
};

/**
 * Create UiStateSaver object which stores @a converterIds
 * settings to @a filename every @a saveTimeout msecs
 */
std::unique_ptr<UiStateSaver> createUiStateSaver(
    const QString& fileName, const std::vector<std::string>& converterIds, int saveTimeout);

/**
 * Create UiStateSaver object which stores all available
 * settings to @a filename every @a saveTimeout msecs
 */
std::unique_ptr<UiStateSaver> createUiStateSaver(const QString& fileName, int saveTimeout);

} // namespace uistatesaver
