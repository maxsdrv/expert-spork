#pragma once

#include <QSet>
#include <QStringList>

#include <deque>
#include <list>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace concepts
{

/** We can either check for begin/end/iterator or explicitly
 * define containers. The former is more general but the latter
 * gives more control over different types */
template <typename Container>
struct IsSequenceContainer : std::false_type
{
};

template <typename Container>
struct IsAssociativeContainer : std::false_type
{
};

template <typename... Args>
struct IsSequenceContainer<std::set<Args...>> : std::true_type
{
    static auto makeInserter(std::set<Args...>& container)
    {
        return std::inserter(container, container.end());
    }
};

template <typename... Args>
struct IsSequenceContainer<std::unordered_set<Args...>> : std::true_type
{
    static auto makeInserter(std::unordered_set<Args...>& container)
    {
        return std::inserter(container, container.end());
    }
};

template <typename... Args>
struct IsSequenceContainer<std::map<Args...>> : std::true_type
{
    static auto makeInserter(std::map<Args...>& container)
    {
        return std::inserter(container, container.end());
    }
};

template <typename... Args>
struct IsSequenceContainer<std::unordered_map<Args...>> : std::true_type
{
    static auto makeInserter(std::unordered_map<Args...>& container)
    {
        return std::inserter(container, container.end());
    }
};

template <typename... Args>
struct IsAssociativeContainer<std::multimap<Args...>> : std::true_type
{
    static auto makeInserter(std::multimap<Args...>& container)
    {
        return std::inserter(container, container.end());
    }
};

template <typename... Args>
struct IsAssociativeContainer<std::unordered_multimap<Args...>> : std::true_type
{
    static auto makeInserter(std::unordered_multimap<Args...>& container)
    {
        return std::inserter(container, container.end());
    }
};

template <typename... Args>
struct IsSequenceContainer<std::vector<Args...>> : std::true_type
{
    static auto makeInserter(std::vector<Args...>& container)
    {
        return std::back_inserter(container);
    }
};

template <typename... Args>
struct IsSequenceContainer<std::deque<Args...>> : std::true_type
{
    static auto makeInserter(std::deque<Args...>& container)
    {
        return std::back_inserter(container);
    }
};

template <typename... Args>
struct IsSequenceContainer<std::list<Args...>> : std::true_type
{
    static auto makeInserter(std::list<Args...>& container)
    {
        return std::back_inserter(container);
    }
};

template <typename... Args>
struct IsSequenceContainer<QList<Args...>> : std::true_type
{
    static auto makeInserter(QList<Args...>& container)
    {
        return std::back_inserter(container);
    }
};

#if QT_MAJOR < 6
// For Qt6 QVector is just an alias to QList
template <typename... Args>
struct IsSequenceContainer<QVector<Args...>> : std::true_type
{
    static auto makeInserter(QVector<Args...>& container)
    {
        return std::back_inserter(container);
    }
};

template <>
struct IsSequenceContainer<QStringList> : std::true_type
{
    static auto makeInserter(QStringList& container)
    {
        return IsSequenceContainer<QList<QString>>::makeInserter(container);
    }
};
#endif

// Insert iterator that use insert(value). Applicable for QSet and QHashSet
template <typename Container>
class QtInsertIterator
{
public:
    /// One of the @link iterator_tags tag types@endlink.
    using iterator_category = std::output_iterator_tag;
    /// The type "pointed to" by the iterator.
    using value_type = void;
    /// Distance between iterators is represented as this type.
    using difference_type = void;
    /// This type represents a pointer-to-value_type.
    using pointer = void;
    /// This type represents a reference-to-value_type.
    using reference = void;

private:
    Container* mContainer;

public:
    using container_type = Container;

    explicit QtInsertIterator(Container& container) : mContainer(std::addressof(container))
    {
    }

    QtInsertIterator& operator=(const typename Container::value_type& value)
    {
        mContainer->insert(value);
        return *this;
    }

    QtInsertIterator& operator=(typename Container::value_type&& value)
    {
        mContainer->insert(std::move(value));
        return *this;
    }

    // Iterator does nothing more
    QtInsertIterator& operator*()
    {
        return *this;
    }

    QtInsertIterator& operator++()
    {
        return *this;
    }

    QtInsertIterator operator++(int)
    {
        return *this;
    }
};

template <typename... Args>
struct IsSequenceContainer<QSet<Args...>> : std::true_type
{
    static auto makeInserter(QSet<Args...>& container)
    {
        return QtInsertIterator(container);
    }
};

} // namespace concepts
