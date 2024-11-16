// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_COLLECTION_H
#define SWIFT_MISC_COLLECTION_H

#include <algorithm>
#include <initializer_list>
#include <iterator>
#include <type_traits>
#include <typeindex>
#include <utility>

#include <QMap>

#include "misc/containerbase.h"
#include "misc/mixin/mixindatastream.h"
#include "misc/mixin/mixinicon.h"

//! \cond
#define SWIFT_TEMPLATE_COLLECTION_MIXINS(NS, T, Set, Extern)                                                           \
    namespace NS                                                                                                       \
    {                                                                                                                  \
        class Set;                                                                                                     \
    }                                                                                                                  \
    namespace swift::misc::private_ns                                                                                  \
    {                                                                                                                  \
        Extern template struct CValueObjectMetaInfo<NS::Set>;                                                          \
        Extern template struct CValueObjectMetaInfo<CCollection<NS::T>>;                                               \
        Extern template struct MetaTypeHelper<NS::Set>;                                                                \
        Extern template struct MetaTypeHelper<CCollection<NS::T>>;                                                     \
    }                                                                                                                  \
    namespace swift::misc::mixin                                                                                       \
    {                                                                                                                  \
        Extern template class MetaType<NS::Set>;                                                                       \
        Extern template class MetaType<CCollection<NS::T>>;                                                            \
        Extern template class DBusOperators<CCollection<NS::T>>;                                                       \
        Extern template class JsonOperators<CCollection<NS::T>>;                                                       \
        Extern template class String<CCollection<NS::T>>;                                                              \
        Extern template class DataStreamOperators<CCollection<NS::T>>;                                                 \
        Extern template class Icon<CCollection<NS::T>>;                                                                \
    }
//! \endcond

/*!
 * \def SWIFT_DECLARE_COLLECTION_MIXINS
 * Explicit template declaration of mixins for a CCollection subclass
 * to be placed near the top of the header that defines the class
 */

/*!
 * \def SWIFT_DEFINE_COLLECTION_MIXINS
 * Explicit template definition of mixins for a CCollection subclass
 */
#if defined(Q_OS_WIN) && defined(Q_CC_GNU)
#    define SWIFT_DECLARE_COLLECTION_MIXINS(Namespace, T, Set)
#    define SWIFT_DEFINE_COLLECTION_MIXINS(Namespace, T, Set)
#else
#    define SWIFT_DECLARE_COLLECTION_MIXINS(Namespace, T, Set)                                                         \
        SWIFT_TEMPLATE_COLLECTION_MIXINS(Namespace, T, Set, extern)
#    define SWIFT_DEFINE_COLLECTION_MIXINS(Namespace, T, Set) SWIFT_TEMPLATE_COLLECTION_MIXINS(Namespace, T, Set, )
#endif

namespace swift::misc
{

    /*!
     * Needed for compatibility with C++ standard algorithms which expect ordered sets.
     */
    template <class T>
    class QOrderedSet : public QMap<T, T>
    {
    public:
        //! Type of values stored in the set.
        using value_type = T;

        //! Insert a new value into the set.
        typename QMap<T, T>::iterator insert(const T &value) { return QMap<T, T>::insert(value, value); }

        //! Default constructor.
        QOrderedSet() = default;

        //! Initializer list constructor.
        QOrderedSet(std::initializer_list<T> il)
        {
            for (const auto &v : il) { insert(v); }
        }

        //! Constructor from QList
        QOrderedSet(const QList<T> &list)
        {
            for (const auto &v : list) { insert(v); }
        }
    };

    /*!
     * Generic ordered container with value semantics.
     * \tparam T the type of elements contained.
     */
    template <class T>
    class CCollection :
        public CContainerBase<CCollection<T>>,
        public mixin::DataStreamOperators<CCollection<T>>,
        public mixin::Icon<CCollection<T>>
    {
    public:
        //! STL compatibility
        using key_type = T;

        //! STL compatibility
        using value_type = T;

        //! STL compatibility
        using reference = T &;

        //! STL compatibility
        using const_reference = const T &;

        //! STL compatibility
        using pointer = T *;

        //! STL compatibility
        using const_pointer = const T *;

        //! STL compatibility
        using const_iterator = typename QOrderedSet<T>::const_iterator;

        //! STL compatibility
        using iterator = const_iterator; // can't modify elements in-place

        //! STL compatibility
        using difference_type = ptrdiff_t;

        //! STL compatibility
        using size_type = int;

        //! Default constructor.
        CCollection() = default;

        //! Initializer list constructor.
        CCollection(std::initializer_list<T> il) : m_impl(il) {}

        //! Copy constructor.
        CCollection(const CCollection &other) = default;

        //! Constructor from QList.
        CCollection(const QList<T> &list) : m_impl(list) {}

        //! Move constructor.
        CCollection(CCollection &&other) = default;

        //! Copy assignment.
        CCollection &operator=(const CCollection &other) = default;

        //! Move assignment.
        CCollection &operator=(CCollection &&other) = default;

        //! Destructor.
        ~CCollection() = default;

        //! Returns iterator at the beginning of the collection.
        iterator begin() { return m_impl.begin(); }

        //! Returns iterator at the beginning of the collection.
        const_iterator begin() const { return m_impl.begin(); }

        //! Returns iterator at the beginning of the collection.
        const_iterator cbegin() const { return m_impl.cbegin(); }

        //! Returns iterator one past the end of the collection.
        iterator end() { return m_impl.end(); }

        //! Returns const iterator one past the end of the collection.
        const_iterator end() const { return m_impl.end(); }

        //! Returns const iterator one past the end of the collection.
        const_iterator cend() const { return m_impl.cend(); }

        //! Swap this collection with another.
        void swap(CCollection &other) noexcept { m_impl.swap(other.m_impl); }

        //! Returns number of elements in the collection.
        size_type size() const { return m_impl.size(); }

        //! Returns true if the collection is empty.
        bool empty() const { return m_impl.isEmpty(); }

        //! Synonym for empty.
        bool isEmpty() const { return empty(); }

        //! Removes all elements in the collection.
        void clear() { m_impl.clear(); }

        //! For compatibility with std::inserter.
        //! \param hint Ignored.
        //! \param value The value to insert.
        iterator insert(const_iterator hint, const T &value)
        {
            Q_UNUSED(hint);
            return insert(value);
        }

        //! For compatibility with std::inserter.
        //! \param hint Ignored.
        //! \param value The value to move in.
        iterator insert(const_iterator hint, T &&value)
        {
            Q_UNUSED(hint);
            return insert(std::move(value));
        }

        //! Inserts an element into the collection.
        //! \return An iterator to the position where value was inserted.
        iterator insert(const T &value) { return m_impl.insert(value); }

        //! Moves an element into the collection.
        //! \return An iterator to the position where value was inserted.
        iterator insert(T &&value) { return m_impl.insert(std::move(value)); }

        //! Inserts all elements from another collection into this collection.
        void insert(const CCollection &other) { std::copy(other.begin(), other.end(), std::inserter(*this, begin())); }

        //! Inserts all elements from another collection into this collection.
        //! This version moves elements instead of copying.
        void insert(CCollection &&other) { std::move(other.begin(), other.end(), std::inserter(*this, begin())); }

        //! Appends all elements from a range at the end of this collection.
        template <typename I>
        void insert(const CRange<I> &range)
        {
            std::copy(range.begin(), range.end(), std::back_inserter(*this));
        }

        //! Synonym for insert.
        //! \return An iterator to the position where value was inserted.
        iterator push_back(const T &value) { return insert(value); }

        //! Synonym for insert.
        //! \return An iterator to the position where value was inserted.
        iterator push_back(T &&value) { return insert(std::move(value)); }

        //! Synonym for insert.
        void push_back(const CCollection &other) { insert(other); }

        //! Synonym for insert.
        void push_back(CCollection &&other) { insert(std::move(other)); }

        //! Synonym for insert.
        template <typename I>
        void push_back(const CRange<I> &range)
        {
            std::copy(range.begin(), range.end(), std::back_inserter(*this));
        }

        //! Returns a collection which is the union of this collection and another container.
        template <class C>
        CCollection makeUnion(const C &other) const
        {
            CCollection result;
            std::set_union(begin(), end(), other.begin(), other.end(), std::inserter(result, result.begin()));
            return result;
        }

        //! Returns a collection which is the intersection of this collection and another.
        template <class C>
        CCollection intersection(const C &other) const
        {
            CCollection result;
            std::set_intersection(begin(), end(), other.begin(), other.end(), std::inserter(result, result.begin()));
            return result;
        }

        //! Returns a collection which contains all the elements from this collection which are not in the other
        //! collection.
        template <class C>
        CCollection difference(const C &other) const
        {
            CCollection result;
            std::set_difference(begin(), end(), other.begin(), other.end(), std::inserter(result, result.begin()));
            return result;
        }

        //! Remove the element pointed to by the given iterator.
        //! \return An iterator to the position of the next element after the one removed.
        iterator erase(iterator pos) { return m_impl.erase(pos); }

        //! Remove the range of elements between two iterators.
        //! \return An iterator to the position of the next element after the one removed.
        iterator erase(iterator it1, iterator it2)
        {
            while (it1 != it2) { it1 = erase(it1); }
            return it1;
        }

        //! Efficient find method using the find of the implementation container. Typically O(log n).
        //! \return An iterator to the position of the found element, or the end iterator if not found.
        iterator find(const T &value) { return m_impl.find(value); }

        //! Efficient find method using the find of the implementation container. Typically O(log n).
        //! \return An iterator to the position of the found element, or the end iterator if not found.
        const_iterator find(const T &value) const { return m_impl.find(value); }

        //! Efficient remove using the find and erase of the implementation container. Typically O(log n).
        //! \pre The sequence must be initialized.
        void remove(const T &object)
        {
            auto it = find(object);
            if (it != end()) { erase(it); }
        }

        //! Removes from this collection all of the elements of another collection.
        //! \pre This sequence must be initialized.
        void remove(const CCollection &other) { *this = CCollection(*this).difference(other); }

        //! Remove elements for which a given predicate returns true.
        //! \pre The collection must be initialized.
        //! \return The number of elements removed.
        template <class Predicate>
        int removeIf(Predicate p)
        {
            int count = 0;
            for (auto it = begin(); it != end();)
            {
                if (p(*it))
                {
                    it = erase(it);
                    count++;
                }
                else { ++it; }
            }
            return count;
        }

        //! \copydoc swift::misc::CContainerBase::removeIf
        template <class K0, class V0, class... KeysValues>
        int removeIf(K0 k0, V0 v0, KeysValues... keysValues)
        {
            // using-declaration doesn't play nicely with injected template names
            return CCollection::CContainerBase::removeIf(k0, v0, keysValues...);
        }

        //! Convert to a QList
        QList<T> toQList() const { return this->to(QList<T>()); }

        //! \private Calls detach on the internal QMap.
        void detach() { m_impl.detach(); }

        //! Test for equality.
        friend bool operator==(const CCollection &a, const CCollection &b) { return a.m_impl == b.m_impl; }

        //! Test for inequality.
        friend bool operator!=(const CCollection &a, const CCollection &b) { return a.m_impl != b.m_impl; }

        //! \copydoc swift::misc::mixin::DataStreamByMetaClass::marshalToDataStream
        void marshalToDataStream(QDataStream &stream) const { stream << m_impl; }

        //! \copydoc swift::misc::mixin::DataStreamByMetaClass::unmarshalFromDataStream
        void unmarshalFromDataStream(QDataStream &stream) { stream >> m_impl; }

    private:
        QOrderedSet<T> m_impl;
    };

} // namespace swift::misc

Q_DECLARE_METATYPE(swift::misc::CCollection<int>)
Q_DECLARE_METATYPE(swift::misc::CCollection<uint>)
Q_DECLARE_METATYPE(swift::misc::CCollection<qlonglong>)
Q_DECLARE_METATYPE(swift::misc::CCollection<qulonglong>)
// CCollection<double> not instantiated due to it being a dumb idea because of rounding issues

#endif // guard
