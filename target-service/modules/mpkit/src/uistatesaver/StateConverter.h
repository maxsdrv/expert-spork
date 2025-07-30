/** @file
 * @brief     State converter base class specification
 *
 * $Id: $
 */

#pragma once

#include "uistatesaver/KeyValueStorage.h"
#include "uistatesaver/StateConverterFactory.h"

#include <QObject>
#include <QVariant>

#include <memory>

namespace uistatesaver
{

using StateStorage = KeyValueStorage<QString, QByteArray>;

/**
 * Base class for state converters. That implements conversion from widgets
 * states (ex: widget size, table columns size) to format that can be save (byte
 * array)
 */
class StateConverter : public QObject
{
    Q_OBJECT

public:
    explicit StateConverter(std::shared_ptr<StateStorage> storage, QObject* parent = nullptr);

    /**
     * Inject specific converter to recipient
     */
    bool inject(QObject* recipient, ConverterTraits traits);

public slots:
    /**
     * Save state from injection
     */
    void save(StateConverterInjection* injection);

    /**
     * Restore state for specified
     */
    void restore(StateConverterInjection* injection);

private:
    std::shared_ptr<StateStorage> m_storage;
};

} // namespace uistatesaver
