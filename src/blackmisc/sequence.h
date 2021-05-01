/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SEQUENCE_H
#define BLACKMISC_SEQUENCE_H

#include "blackmisc/containerbase.h"
#include "blackmisc/mixin/mixinicon.h"
#include "blackmisc/mixin/mixindatastream.h"
#include <QVector>
#include <algorithm>
#include <type_traits>
#include <iterator>
#include <utility>
#include <initializer_list>

namespace BlackMisc
{
    /*!
     * Generic sequential container with value semantics.
     * \tparam T the type of elements contained.
     */
    template <class T>
    class CSequence :
        public CContainerBase<CSequence<T>>,
        public Mixin::DataStreamOperators<CSequence<T>>,
        public Mixin::Icon<CSequence<T>>
    {
    public:
        //! STL compatibility
        //! @{
        typedef T key_type;
        typedef T value_type;
        typedef T &reference;
        typedef const T &const_reference;
        typedef T *pointer;
        typedef const T *const_pointer;
        typedef typename QVector<T>::const_iterator const_iterator;
        typedef typename QVector<T>::iterator iterator;
        typedef typename QVector<T>::const_reverse_iterator const_reverse_iterator;
        typedef typename QVector<T>::reverse_iterator reverse_iterator;
        typedef ptrdiff_t difference_type;
        typedef int size_type;
        //! @}

        //! Default constructor.
        CSequence() {}

        //! Initializer list constructor.
        CSequence(std::initializer_list<T> il) : m_impl(il) {}

        //! By QVector of type T.
        //! @{
        CSequence(const QVector<T> &vector) : m_impl(vector) {}
        CSequence(QVector<T> &&vector) : m_impl(std::move(vector)) {}
        //! @}

        //! By QList of type T.
        CSequence(const QList<T> &list) : m_impl(list.toVector()) {}

        //! Copy constructor.
        CSequence(const CSequence &other) = default;

        //! Move constructor.
        CSequence(CSequence &&other) = default;

        //! Copy assignment.
        CSequence &operator =(const CSequence &other) = default;

        //! Move assignment.
        CSequence &operator =(CSequence &&other) = default;

        //! Destructor.
        ~CSequence() = default;

        //! Copy of internal vector.
        //! @{
        QVector<T> toVector() const & { return m_impl; }
        QVector<T> toVector() && { return std::move(m_impl); }
        //! @}

        //! Returns iterator at the beginning of the sequence.
        iterator begin() { return m_impl.begin(); }

        //! Returns const iterator at the beginning of the sequence.
        const_iterator begin() const { return m_impl.begin(); }

        //! Returns const iterator at the beginning of the sequence.
        const_iterator cbegin() const { return m_impl.cbegin(); }

        //! Returns iterator one past the end of the sequence.
        iterator end() { return m_impl.end(); }

        //! Returns const iterator one past the end of the sequence.
        const_iterator end() const { return m_impl.end(); }

        //! Returns const iterator one past the end of the sequence.
        const_iterator cend() const { return m_impl.cend(); }

        //! Returns iterator at the beginning of the reversed sequence.
        reverse_iterator rbegin() { return m_impl.rbegin(); }

        //! Returns const iterator at the beginning of the reversed sequence.
        const_reverse_iterator rbegin() const { return m_impl.rbegin(); }

        //! Returns const iterator at the beginning of the reversed sequence.
        const_reverse_iterator crbegin() const { return m_impl.crbegin(); }

        //! Returns iterator at the end of the reversed sequence.
        reverse_iterator rend() { return m_impl.rend(); }

        //! Returns const iterator at the end of the reversed sequence.
        const_reverse_iterator rend() const { return m_impl.rend(); }

        //! Returns const iterator at the end of the reversed sequence.
        const_reverse_iterator crend() const { return m_impl.crend(); }

        //! Swap this sequence with another.
        void swap(CSequence &other) noexcept { m_impl.swap(other.m_impl); }

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-overflow"
#endif

        //! Access an element by its index.
        reference operator [](size_type index) { Q_ASSERT(index >= 0 && index < m_impl.size()); return m_impl[index]; }

        //! Access an element by its index.
        const_reference operator [](size_type index) const { Q_ASSERT(index >= 0 && index < m_impl.size()); return m_impl[index]; }

#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

        //! Access the first element.
        reference front() { Q_ASSERT(!empty()); return m_impl.front(); }

        //! Access the first element.
        const_reference front() const { Q_ASSERT(!empty()); return m_impl.front(); }

        //! Access the first element, or a default-initialized value if the sequence is empty.
        const_reference frontOrDefault() const { static const value_type def {}; return empty() ? def : m_impl.front(); }

        //! Access the first element, or a default-initialized value if the sequence is empty.
        value_type frontOrDefault(value_type def) const { return empty() ? def : m_impl.front(); }

        //! Access the last element.
        reference back() { Q_ASSERT(!empty()); return m_impl.back(); }

        //! Access the last element.
        const_reference back() const { Q_ASSERT(!empty()); return m_impl.back(); }

        //! Access the last element, or a default value if the sequence is empty.
        const_reference backOrDefault() const { static const value_type def {}; return empty() ? def : m_impl.back(); }

        //! Access the last element, or a default value if the sequence is empty.
        value_type backOrDefault(value_type def) const { return empty() ? def : m_impl.back(); }

        //! Returns number of elements in the sequence.
        size_type size() const { return m_impl.size(); }

        //! Avoid compiler warnings when using with int
        int sizeInt() const { return static_cast<int>(this->size()); }

        //! Convenience function
        QString sizeString() const { return QString::number(m_impl.size()); }

        //! Returns true if the sequence is empty.
        bool empty() const { return m_impl.isEmpty(); }

        //! Synonym for empty.
        bool isEmpty() const { return empty(); }

        //! Removes all elements in the sequence.
        void clear() { m_impl.clear(); }

        //! Changes the size of the sequence, if it is bigger than the given size.
        void truncate(size_type maxSize) { if (size() > maxSize) { erase(begin() + maxSize, end()); } }

        //! Inserts an element into the sequence.
        //! \return An iterator to the position where value was inserted.
        iterator insert(iterator before, const T &value) { return m_impl.insert(before, value); }

        //! Moves an element into the sequence.
        //! \return An iterator to the position where value was inserted.
        iterator insert(iterator before, T &&value) { return m_impl.insert(before, std::move(value)); }

        //! Appends an element at the end of the sequence.
        void push_back(const T &value) { m_impl.push_back(value); }

        //! Insert as first element.
        void push_front(const T &value) { insert(begin(), value); }

        //! Inserts all elements from another sequence at the beginning of this sequence.
        void push_front(const CSequence &other) { std::copy(other.begin(), other.end(), std::front_inserter(*this)); }

        //! Insert as first element by keep maxElements
        void push_frontMaxElements(const T &value, int maxElements)
        {
            Q_ASSERT(maxElements > 1);
            if (this->size() >= (maxElements - 1)) { this->truncate(maxElements - 1); }
            this->push_front(value);
        }

        //! Insert as last element by keep maxElements
        void push_backMaxElements(const T &value, int maxElements)
        {
            Q_ASSERT(maxElements > 1);
            while (this->size() >= (maxElements - 1)) { this->pop_front(); }
            this->push_back(value);
        }

        //! Move-appends an element at the end of the sequence.
        void push_back(T &&value) { m_impl.push_back(std::move(value)); }

        //! Move-insert as first element.
        void push_front(T &&value) { insert(begin(), std::move(value)); }

        //! Appends all elements from another sequence at the end of this sequence.
        void push_back(const CSequence &other) { std::copy(other.begin(), other.end(), std::back_inserter(*this)); }

        //! Appends all elements from another sequence at the end of this sequence.
        //! This version moves elements instead of copying.
        void push_back(CSequence &&other) { std::move(other.begin(), other.end(), std::back_inserter(*this)); }

        //! Appends all elements from a range at the end of this sequence.
        template <typename I>
        void push_back(const CRange<I> &range) { std::copy(range.begin(), range.end(), std::back_inserter(*this)); }

        //! Concatenates two sequences and returns the result.
        CSequence join(const CSequence &other) const { CSequence copy(*this); copy.push_back(other); return copy; }

        //! Concatenates a sequence and a range and returns the result.
        template <typename I>
        CSequence join(const CRange<I> &range) const { CSequence copy(*this); copy.push_back(range); return copy; }

        //! Removes an element at the end of the sequence.
        void pop_back() { Q_ASSERT(!empty()); m_impl.pop_back(); }

        //! Removes an element at the front of the sequence.
        void pop_front() { Q_ASSERT(!empty()); erase(begin()); }

        //! Remove the element pointed to by the given iterator.
        //! \return An iterator to the position of the next element after the one removed.
        iterator erase(iterator pos) { return m_impl.erase(pos); }

        //! Remove the range of elements between two iterators.
        //! \return An iterator to the position of the next element after the one removed.
        iterator erase(iterator it1, iterator it2) { return m_impl.erase(it1, it2); }

        //! Return an iterator to the first element equal to the given object, or the end iterator if not found. O(n).
        iterator find(const T &object) { return std::find(begin(), end(), object); }

        //! Return an iterator to the first element equal to the given object, or the end iterator if not found. O(n).
        const_iterator find(const T &object) const { return std::find(cbegin(), cend(), object); }

        //! Modify by applying a value map to each element for which a given predicate returns true.
        //! \return The number of elements modified.
        template <class Predicate, class VariantMap>
        int applyIf(Predicate p, const VariantMap &newValues, bool skipEqualValues = false)
        {
            int count = 0;
            for (auto &value : *this)
            {
                if (p(value) && ! value.apply(newValues, skipEqualValues).isEmpty()) { count++; }
            }
            return count;
        }

        //! Modify by applying a value map to each element matching a particular key/value pair.
        //! \param key1 A pointer to a member function of T.
        //! \param value1 Will be compared to the return value of key1.
        //! \param newValues Values from this map will be put into each matching element.
        //! \param skipEqualValues Equal values will not be updated
        //! \return The number of elements modified.
        template <class K1, class V1, class VariantMap>
        int applyIf(K1 key1, V1 value1, const VariantMap &newValues, bool skipEqualValues = false)
        {
            return applyIf(BlackMisc::Predicates::MemberEqual(key1, value1), newValues, skipEqualValues);
        }

        //! Remove all elements equal to the given object, if it is contained.
        //! \return The number of elements removed.
        int remove(const T &object)
        {
            const auto newEnd = std::remove(begin(), end(), object);
            const auto count = std::distance(newEnd, end());
            erase(newEnd, end());
            return count;
        }

        //! Remove elements for which a given predicate returns true.
        //! \return The number of elements removed.
        template <class Predicate>
        int removeIf(Predicate p)
        {
            const auto newEnd = std::remove_if(begin(), end(), p);
            const auto count = std::distance(newEnd, end());
            erase(newEnd, end());
            return count;
        }

        //! \copydoc BlackMisc::CContainerBase::removeIf
        template <class K0, class V0, class... KeysValues>
        int removeIf(K0 k0, V0 v0, KeysValues... keysValues)
        {
            // using-declaration doesn't play nicely with injected template names
            return CSequence::CContainerBase::removeIf(k0, v0, keysValues...);
        }

        //! Remove all elements if they are in other
        //! \return The number of elements removed.
        int removeIfIn(const CSequence &other)
        {
            return removeIf([&other](const T &v) { return other.contains(v); });
        }

        //! Remove all elements if they are in other
        //! \pre All elements of other must be present in the same order in this.
        void removeIfInSubset(const CSequence &other)
        {
            erase(BlackMisc::removeIfIn(begin(), end(), other.begin(), other.end()), end());
        }

        //! Replace elements matching the given element with a replacement.
        //! \return The number of elements replaced.
        int replace(const T &original, const T &replacement)
        {
            int count = 0;
            for (auto &element : *this)
            {
                if (element == original) { element = replacement; count++; }
            }
            return count;
        }

        //! Replace elements for which a given predicate returns true.
        //! \return The number of elements replaced.
        template <class Predicate>
        int replaceIf(Predicate p, const T &replacement)
        {
            int count = 0;
            for (auto &element : *this)
            {
                if (p(element)) { element = replacement; count++; }
            }
            return count;
        }

        //! Replace elements matching a particular key/value pair.
        //! \param key1 A pointer to a member function of T.
        //! \param value1 Will be compared to the return value of key1.
        //! \param replacement All matching elements will be replaced by copies of this one.
        //! \return The number of elements replaced.
        template <class K1, class V1>
        int replaceIf(K1 key1, V1 value1, const T &replacement)
        {
            return replaceIf(BlackMisc::Predicates::MemberEqual(key1, value1), replacement);
        }

        //! Replace elements for which a given predicate returns true. If there is no match, push the new element on the end.
        template <class Predicate>
        void replaceOrAdd(Predicate p, const T &replacement)
        {
            if (this->contains(p)) { replaceIf(p, replacement); }
            else { push_back(replacement); }
        }

        //! Replace elements matching the given element. If there is no match, push the new element on the end.
        void replaceOrAdd(const T &original, const T &replacement)
        {
            if (this->contains(original)) { replace(original, replacement); }
            else { push_back(replacement); }
        }

        //! Replace elements matching the given element. If there is no match, push the new element on the end.
        void replaceOrAdd(const T &replacement)
        {
            this->replaceOrAdd(replacement, replacement);
        }

        //! Replace or add given elements
        void replaceOrAdd(const CSequence<T> &replacements)
        {
            for (const T &replacement : replacements)
            {
                this->replaceOrAdd(replacement, replacement);
            }
        }

        //! Replace elements matching a particular key/value pair. If there is no match, push the new element on the end.
        //! \param key1 A pointer to a member function of T.
        //! \param value1 Will be compared to the return value of key1.
        //! \param replacement All matching elements will be replaced by copies of this one, or a copy will be added.
        template <class K1, class V1>
        void replaceOrAdd(K1 key1, V1 value1, const T &replacement)
        {
            if (this->contains(key1, value1)) { replaceIf(key1, value1, replacement); }
            else { push_back(replacement); }
        }

        //! In-place reverse
        void reverse()
        {
            std::reverse(begin(), end());
        }

        //! Reversed order
        CSequence reversed() const
        {
            CSequence result = *this;
            result.reverse();
            return result;
        }

        //! In-place sort by a given comparator predicate.
        template <class Predicate> void sort(Predicate p)
        {
            std::sort(begin(), end(), p);
        }

        //! In-place sort by some particular key(s).
        //! \param key1 A pointer to a member function of T.
        //! \param keys Zero or more additional pointers to member functions of T.
        template <class K1, class... Keys> void sortBy(K1 key1, Keys... keys)
        {
            sort(BlackMisc::Predicates::MemberLess(key1, keys...));
        }

        //! Return a copy sorted by a given comparator predicate.
        template <class Predicate>
        CSequence sorted(Predicate p) const
        {
            CSequence result = *this;
            result.sort(p);
            return result;
        }

        //! Return a copy sorted by some particular key(s).
        //! \param key1 A pointer to a member function of T.
        //! \param keys Zero or more additional pointers to member functions of T.
        template <class K1, class... Keys>
        CSequence sortedBy(K1 key1, Keys... keys) const
        {
            return sorted(BlackMisc::Predicates::MemberLess(key1, keys...));
        }

        //! In-place move the smallest n elements to the beginning and sort them.
        template <class Predicate> void partiallySort(size_type n, Predicate p)
        {
            std::partial_sort(begin(), begin() + std::min(n, size()), end(), p);
        }

        //! In-place partially sort by some particular key(s).
        //! \param n    size.
        //! \param key1 A pointer to a member function of T.
        //! \param keys Zero or more additional pointers to member functions of T.
        template <class K1, class... Keys> void partiallySortBy(size_type n, K1 key1, Keys... keys)
        {
            partiallySort(n, BlackMisc::Predicates::MemberLess(key1, keys...));
        }

        //! Return a copy with the smallest n elements at the beginning and sorted.
        template <class Predicate>
        CSequence partiallySorted(size_type n, Predicate p) const
        {
            CSequence result = *this;
            result.partiallySort(n, p);
            return result;
        }

        //! Return a copy partially sorted by some particular key(s).
        //! \param n    size
        //! \param key1 A pointer to a member function of T.
        //! \param keys Zero or more additional pointers to member functions of T.
        template <class K1, class... Keys>
        CSequence partiallySortedBy(size_type n, K1 key1, Keys... keys) const
        {
            return partiallySorted(n, BlackMisc::Predicates::MemberLess(key1, keys...));
        }

        //! Return true if this container equals another container, considering only the given element members.
        //! Order of elements is not considered; this is implemented using a transient sort, so don't overuse.
        template <class U, class Key0, class... Keys>
        bool unorderedEqualsByKeys(const U &other, Key0 k0, Keys... keys) const
        {
            if (equalPointers(this, &other)) { return true; }
            if (size() != other.size()) { return false; }
            return sorted(k0, keys...).equalsByKeys(other.sorted(k0, keys...));
        }

        //! Split up the sequence into subsequences for which the given predicate returns the same value.
        template <class Predicate>
        auto separate(Predicate p) const -> QMap<decltype(p(std::declval<T>())), CSequence>
        {
            QMap<decltype(p(std::declval<T>())), CSequence> result;
            auto copy = *this;
            std::stable_sort(copy.begin(), copy.end(), [p](const T &a, const T &b) { return p(a) < p(b); });
            for (auto it = copy.cbegin(); it != copy.cend(); )
            {
                auto mid = std::adjacent_find(it, copy.cend(), [p](const T &a, const T &b) { return p(a) != p(b); });
                result.insert(p(*it), makeRange(it, mid));
                it = mid;
            }
            return result;
        }

        //! Split up the sequence into subsequences of elements having the same value for the given key.
        template <class Key>
        auto separateBy(Key k) const -> QMap<decltype(std::invoke(k, std::declval<T>())), CSequence>
        {
            return separateBy([k](const T &v) { return std::invoke(k, v); });
        }

        //! \private Calls detach on the internal QVector.
        void detach() { m_impl.detach(); }

        //! Equals operator.
        friend bool operator ==(const CSequence &a, const CSequence &b) { return a.m_impl == b.m_impl; }

        //! Not equals operator.
        friend bool operator !=(const CSequence &a, const CSequence &b) { return a.m_impl != b.m_impl; }

        //! Less than operator.
        friend bool operator <(const CSequence &a, const CSequence &b) { return a.m_impl < b.m_impl; }

        //! Greater than operator.
        friend bool operator >(const CSequence &a, const CSequence &b) { return a.m_impl > b.m_impl; }

        //! Less or equal than operator.
        friend bool operator <=(const CSequence &a, const CSequence &b) { return a.m_impl <= b.m_impl; }

        //! Greater or equal operator.
        friend bool operator >=(const CSequence &a, const CSequence &b) { return a.m_impl >= b.m_impl; }

        //! \copydoc BlackMisc::Mixin::DataStreamByMetaClass::marshalToDataStream
        void marshalToDataStream(QDataStream &stream) const { stream << m_impl; }

        //! \copydoc BlackMisc::Mixin::DataStreamByMetaClass::unmarshalFromDataStream
        void unmarshalFromDataStream(QDataStream &stream) { stream >> m_impl; }

    private:
        QVector<T> m_impl;
    };
} //namespace BlackMisc

Q_DECLARE_SEQUENTIAL_CONTAINER_METATYPE(BlackMisc::CSequence)

#endif // guard
