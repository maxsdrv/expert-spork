/** @file
 * @brief     Interface for storage with arbitrary key and value types
 *
 * $Id: $
 */

#pragma once

namespace uistatesaver
{

template <class Key, class Value>
class KeyValueStorage
{
public:
    using key_type = Key;
    using value_type = Value;

    virtual ~KeyValueStorage() = default;

    /**
     * Save state for specified id
     */
    virtual void set(const Key& key, const Value& value) = 0;

    /**
     * Return state for specified id
     */
    virtual Value get(const Key& key) = 0;
};

} // namespace uistatesaver
