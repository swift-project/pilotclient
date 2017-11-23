/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SEQUENCE_H
#define BLACKMISC_SEQUENCE_H

#include "iterator.h"
#include "containerbase.h"
#include "propertyindex.h"
#include "icon.h"
#include <QScopedPointer>
#include <algorithm>
#include <type_traits>
#include <iterator>
#include <utility>
#include <initializer_list>

// conditions matched with pop pragmas at bottom of file
#if defined(QT_CC_CLANG)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wshorten-64-to-32"
#elif defined(Q_CC_MSVC) && defined(Q_OS_WIN64) && defined(QT_CC_WARNINGS)
#pragma warning(push)
#pragma warning(disable:4244)
#endif

namespace BlackMisc
{

    /*!
     * Generic type-erased sequential container with value semantics.
     * \tparam T the type of elements contained.
     *
     * Can take any suitable container class as its implementation at runtime.
     */
    template <class T>
    class CSequence :
        public CContainerBase<CSequence<T>>,
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
        typedef typename Iterators::ConstRandomAccessIterator<T> const_iterator;
        typedef typename Iterators::RandomAccessIterator<T> iterator;
        typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
        typedef std::reverse_iterator<iterator> reverse_iterator;
        typedef ptrdiff_t difference_type;
        typedef int size_type;
        //! @}

        //! Default constructor.
        CSequence() : m_pimpl(new Pimpl<QList<T>>(QList<T>())) {}

        //! Initializer list constructor.
        CSequence(std::initializer_list<T> il) : m_pimpl(new Pimpl<QList<T>>(QList<T>(il))) {}

        //! By QList of type T.
        CSequence(const QList<T> &list) : m_pimpl(new Pimpl<QList<T>>(QList<T>(list))) {}

        //! Copy constructor.
        CSequence(const CSequence &other) : m_pimpl(other.pimpl() ? other.pimpl()->clone() : nullptr) {}

        //! Move constructor.
        CSequence(CSequence &&other) noexcept(std::is_nothrow_move_constructible<T>::value) : m_pimpl(other.m_pimpl.take()) {}

        //! Copy assignment.
        CSequence &operator =(const CSequence &other) { m_pimpl.reset(other.pimpl() ? other.pimpl()->clone() : nullptr); return *this; }

        //! Move assignment.
        CSequence &operator =(CSequence && other) noexcept(std::is_nothrow_move_assignable<T>::value) { m_pimpl.reset(other.m_pimpl.take()); return *this; }

        //! Destructor.
        ~CSequence() = default;

        //! Create a new sequence with a specific implementation type.
        //! \tparam C Becomes the sequence's implementation type.
        //! \param c Initial value for the sequence; default is empty, but it could contain elements if desired. The value is copied.
        template <class C> static CSequence fromImpl(C c = C()) { return CSequence(new Pimpl<C>(std::move(c))); }

        //! Change the implementation type but keep all the same elements, by moving them into the new implementation.
        //! \tparam C Becomes the sequence's new implementation type.
        template <class C> void changeImpl(C = C()) { auto c = fromImpl(C()); std::move(begin(), end(), std::inserter(c, c.begin())); *this = std::move(c); }

        //! Like changeImpl, but uses the implementation type of another sequence.
        //! \pre The other sequence must be initialized.
        void useImplOf(const CSequence &other) { CSequence c(other.pimpl()->cloneEmpty()); std::move(begin(), end(), std::inserter(c, c.begin())); *this = std::move(c); }

        //! Returns iterator at the beginning of the sequence.
        iterator begin() { return pimpl() ? pimpl()->begin() : iterator(); }

        //! Returns const iterator at the beginning of the sequence.
        const_iterator begin() const { return pimpl() ? pimpl()->begin() : const_iterator(); }

        //! Returns const iterator at the beginning of the sequence.
        const_iterator cbegin() const { return pimpl() ? pimpl()->cbegin() : const_iterator(); }

        //! Returns iterator one past the end of the sequence.
        iterator end() { return pimpl() ? pimpl()->end() : iterator(); }

        //! Returns const iterator one past the end of the sequence.
        const_iterator end() const { return pimpl() ? pimpl()->end() : const_iterator(); }

        //! Returns const iterator one past the end of the sequence.
        const_iterator cend() const { return pimpl() ? pimpl()->cend() : const_iterator(); }

        //! Returns iterator at the beginning of the reversed sequence.
        reverse_iterator rbegin() { return reverse_iterator(end()); }

        //! Returns const iterator at the beginning of the reversed sequence.
        const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }

        //! Returns const iterator at the beginning of the reversed sequence.
        const_reverse_iterator crbegin() const { return const_reverse_iterator(cend()); }

        //! Returns iterator at the end of the reversed sequence.
        reverse_iterator rend() { return reverse_iterator(begin()); }

        //! Returns const iterator at the end of the reversed sequence.
        const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

        //! Returns const iterator at the end of the reversed sequence.
        const_reverse_iterator crend() const { return const_reverse_iterator(cbegin()); }

        //! Swap this sequence with another.
        void swap(CSequence &other) noexcept { m_pimpl.swap(other.m_pimpl); }

        //! Access an element by its index.
        //! \pre The sequence must be initialized and the index in bounds.
        reference operator [](size_type index) { Q_ASSERT(pimpl()); return pimpl()->operator [](index); }

        //! Access an element by its index.
        //! \pre The sequence must be initialized and the index in bounds.
        const_reference operator [](size_type index) const { Q_ASSERT(pimpl()); return pimpl()->operator [](index); }

        //! Access the first element.
        //! \pre The sequence must not be empty.
        reference front() { Q_ASSERT(!empty()); return pimpl()->front(); }

        //! Access the first element.
        //! \pre The sequence must not be empty.
        const_reference front() const { Q_ASSERT(!empty()); return pimpl()->front(); }

        //! Access the first element, or a default-initialized value if the sequence is empty.
        const_reference frontOrDefault() const { static const value_type def {}; return empty() ? def : front(); }

        //! Access the first element, or a default-initialized value if the sequence is empty.
        value_type frontOrDefault(value_type def) const { return empty() ? def : front(); }

        //! Access the last element.
        //! \pre The sequence must not be empty.
        reference back() { Q_ASSERT(!empty()); return pimpl()->back(); }

        //! Access the last element.
        //! \pre The sequence must not be empty.
        const_reference back() const { Q_ASSERT(!empty()); return pimpl()->back(); }

        //! Access the last element, or a default value if the sequence is empty.
        const_reference backOrDefault() const { static const value_type def {}; return empty() ? def : back(); }

        //! Access the last element, or a default value if the sequence is empty.
        value_type backOrDefault(value_type def) const { return empty() ? def : back(); }

        //! Returns number of elements in the sequence.
        size_type size() const { return pimpl() ? pimpl()->size() : 0; }

        //! Returns true if the sequence is empty.
        bool empty() const { return pimpl() ? pimpl()->empty() : true; }

        //! Synonym for empty.
        bool isEmpty() const { return empty(); }

        //! Removes all elements in the sequence.
        void clear() { if (pimpl()) pimpl()->clear(); }

        //! Changes the size of the sequence, if it is bigger than the given size.
        void truncate(size_type maxSize) { if (size() > maxSize) { erase(begin() + maxSize, end()); } }

        //! Inserts an element into the sequence.
        //! \return An iterator to the position where value was inserted.
        //! \pre The sequence must be initialized.
        iterator insert(iterator before, const T &value) { Q_ASSERT(pimpl()); return pimpl()->insert(before, value); }

        //! Moves an element into the sequence.
        //! \return An iterator to the position where value was inserted.
        //! \pre The sequence must be initialized.
        iterator insert(iterator before, T &&value) { Q_ASSERT(pimpl()); return pimpl()->insert(before, std::move(value)); }

        //! Appends an element at the end of the sequence.
        //! \pre The sequence must be initialized.
        void push_back(const T &value) { Q_ASSERT(pimpl()); pimpl()->push_back(value); }

        //! Insert as first element.
        //! \pre The sequence must be initialized.
        void push_front(const T &value) { insert(begin(), value); }

        //! Move-appends an element at the end of the sequence.
        //! \pre The sequence must be initialized.
        void push_back(T &&value) { Q_ASSERT(pimpl()); pimpl()->push_back(std::move(value)); }

        //! Move-insert as first element.
        //! \pre The sequence must be initialized.
        void push_front(T &&value) { insert(begin(), std::move(value)); }

        //! Appends all elements from another sequence at the end of this sequence.
        //! \pre This sequence must be initialized.
        void push_back(const CSequence &other) { std::copy(other.begin(), other.end(), std::back_inserter(*this)); }

        //! Appends all elements from another sequence at the end of this sequence.
        //! This version moves elements instead of copying.
        //! \pre This sequence must be initialized.
        void push_back(CSequence &&other) { std::move(other.begin(), other.end(), std::back_inserter(*this)); }

        //! Appends all elements from a range at the end of this sequence.
        //! \pre This sequence must be initialized.
        template <typename I>
        void push_back(const CRange<I> &range) { std::copy(range.begin(), range.end(), std::back_inserter(*this)); }

        //! Synonym for push_back.
        //! \pre The sequence must be initialized.
        void insert(const T &value) { push_back(value); }

        //! Synonym for push_back.
        //! \pre The sequence must be initialized.
        void insert(T &&value) { push_back(std::move(value)); }

        //! Synonym for push_back.
        //! \pre The sequence must be initialized.
        void insert(const CSequence &other) { push_back(other); }

        //! Synonym for push_back.
        //! \pre The sequence must be initialized.
        void insert(CSequence &&other) { push_back(std::move(other)); }

        //! Synonym for push_back.
        //! \pre This sequence must be initialized.
        template <typename I>
        void insert(const CRange<I> &range) { std::copy(range.begin(), range.end(), std::back_inserter(*this)); }

        //! Concatenates two sequences and returns the result.
        //! \pre This sequence must be initialized.
        CSequence join(const CSequence &other) const { CSequence copy(*this); copy.push_back(other); return copy; }

        //! Concatenates a sequence and a range and returns the result.
        //! \pre This sequence must be initialized.
        template <typename I>
        CSequence join(const CRange<I> &range) const { CSequence copy(*this); copy.push_back(range); return copy; }

        //! Removes an element at the end of the sequence.
        //! \pre The sequence must contain at least one element.
        void pop_back() { Q_ASSERT(!empty()); pimpl()->pop_back(); }

        //! Removes an element at the end of the sequence.
        //! \pre The sequence must contain at least one element.
        void pop_front() { erase(begin()); }

        //! Remove the element pointed to by the given iterator.
        //! \return An iterator to the position of the next element after the one removed.
        //! \pre The sequence must be initialized.
        iterator erase(iterator pos) { Q_ASSERT(pimpl()); return pimpl()->erase(pos); }

        //! Remove the range of elements between two iterators.
        //! \return An iterator to the position of the next element after the one removed.
        //! \pre The sequence must be initialized.
        iterator erase(iterator it1, iterator it2) { Q_ASSERT(pimpl()); return pimpl()->erase(it1, it2); }

        //! Return an iterator to the first element equal to the given object, or the end iterator if not found. O(n).
        //! \warning Take care that the returned non-const iterator is not compared with a const iterator.
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
        //! \pre The sequence must be initialized.
        //! \return The number of elements removed.
        int remove(const T &object)
        {
            const auto newEnd = std::remove(begin(), end(), object);
            int count = std::distance(newEnd, end());
            erase(newEnd, end());
            return count;
        }

        //! Remove elements for which a given predicate returns true.
        //! \pre The sequence must be initialized.
        //! \return The number of elements removed.
        template <class Predicate>
        int removeIf(Predicate p)
        {
            auto newEnd = std::remove_if(begin(), end(), p);
            int count = std::distance(newEnd, end());
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
        //! \pre The sequence must be initialized.
        //! \return The number of elements removed.
        int removeIfIn(const CSequence &other)
        {
            return removeIf([&other](const T &v) { return other.contains(v); });
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
        //! \pre The sequence must be initialized.
        template <class Predicate>
        void replaceOrAdd(Predicate p, const T &replacement)
        {
            if (this->contains(p)) { replaceIf(p, replacement); }
            else { push_back(replacement); }
        }

        //! Replace elements matching the given element. If there is no match, push the new element on the end.
        //! \pre The sequence must be initialized.
        void replaceOrAdd(const T &original, const T &replacement)
        {
            if (this->contains(original)) { replace(original, replacement); }
            else { push_back(replacement); }
        }

        //! Replace elements matching a particular key/value pair. If there is no match, push the new element on the end.
        //! \param key1 A pointer to a member function of T.
        //! \param value1 Will be compared to the return value of key1.
        //! \param replacement All matching elements will be replaced by copies of this one, or a copy will be added.
        //! \pre The sequence must be initialized.
        template <class K1, class V1>
        void replaceOrAdd(K1 key1, V1 value1, const T &replacement)
        {
            if (this->contains(key1, value1)) { replaceIf(key1, value1, replacement); }
            else { push_back(replacement); }
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

        //! In-place sort by some properties specified by a list of property indexes.
        void sortByProperty(const CSequence<CPropertyIndex> &indexes)
        {
            sort([&indexes](const T &a, const T &b)
            {
                for (const auto &index : indexes)
                {
                    int cmp = index.comparator()(a, b);
                    if (cmp < 0) { return true; }
                    if (cmp > 0) { return false; }
                }
                return false;
            });
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

        //! Return a copy sorted by some properties specified by a list of property indexes.
        CSequence sortedByProperty(const CSequence<CPropertyIndex> &indexes) const
        {
            CSequence result = *this;
            result.sortByProperty(indexes);
            return result;
        }

        //! In-place move the smallest n elements to the beginning and sort them.
        template <class Predicate> void partiallySort(size_type n, Predicate p)
        {
            std::partial_sort(begin(), begin() + n, end(), p);
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
        auto separateBy(Key k) const -> QMap<decltype(std::declval<T>().*k), CSequence>
        {
            return separateBy([k](const T &v) { return v.*k; });
        }

        //! Equals operator.
        friend bool operator ==(const CSequence &a, const CSequence &b)
        {
            if (a.size() != b.size()) { return false; }
            return std::equal(a.begin(), a.end(), b.begin());
        }

        //! Not equals operator.
        friend bool operator !=(const CSequence &a, const CSequence &b) { return !(a == b); }

        //! Less than operator.
        friend bool operator <(const CSequence &a, const CSequence &b)
        {
            return std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end());
        }

        //! Greater than operator.
        friend bool operator >(const CSequence &a, const CSequence &b) { return b < a; }

        //! Less or equal than operator.
        friend bool operator <=(const CSequence &a, const CSequence &b) { return !(b < a); }

        //! Greater or equal operator.
        friend bool operator >=(const CSequence &a, const CSequence &b) { return !(a < b); }

        //! Return an opaque pointer to the implementation container.
        //! \details Can be useful in unusual debugging situations.
        //! \warning Not for general use.
        void *getImpl() { return pimpl() ? pimpl()->impl() : nullptr; }

    private:
        class PimplBase
        {
        public:
            PimplBase() {}
            PimplBase(const PimplBase &) = default;
            PimplBase &operator =(const PimplBase &) = delete;
            virtual ~PimplBase() {}
            virtual PimplBase *clone() const = 0;
            virtual PimplBase *cloneEmpty() const = 0;
            virtual iterator begin() = 0;
            virtual const_iterator begin() const = 0;
            virtual const_iterator cbegin() const = 0;
            virtual iterator end() = 0;
            virtual const_iterator end() const = 0;
            virtual const_iterator cend() const = 0;
            virtual reference operator [](size_type index) = 0;
            virtual const_reference operator [](size_type index) const = 0;
            virtual reference front() = 0;
            virtual const_reference front() const = 0;
            virtual reference back() = 0;
            virtual const_reference back() const = 0;
            virtual size_type size() const = 0;
            virtual bool empty() const = 0;
            virtual void clear() = 0;
            virtual iterator insert(iterator pos, const T &value) = 0;
            virtual void push_back(const T &value) = 0;
            virtual void push_back(T &&value) = 0;
            virtual void pop_back() = 0;
            virtual iterator erase(iterator pos) = 0;
            virtual iterator erase(iterator it1, iterator it2) = 0;
            virtual void *impl() = 0;
        };

        template <class C> class Pimpl : public PimplBase
        {
        public:
            static_assert(std::is_same<T, typename C::value_type>::value, "CSequence must be initialized from a container with the same value_type.");
            Pimpl(C &&c) : m_impl(std::move(c)) {}
            PimplBase *clone() const override { return new Pimpl(*this); }
            PimplBase *cloneEmpty() const override { return new Pimpl(C()); }
            iterator begin() override { return iterator::fromImpl(m_impl.begin()); }
            const_iterator begin() const override { return const_iterator::fromImpl(m_impl.cbegin()); }
            const_iterator cbegin() const override { return const_iterator::fromImpl(m_impl.cbegin()); }
            iterator end() override { return iterator::fromImpl(m_impl.end()); }
            const_iterator end() const override { return const_iterator::fromImpl(m_impl.cend()); }
            const_iterator cend() const override { return const_iterator::fromImpl(m_impl.cend()); }
            reference operator [](size_type index) override { return m_impl[index]; }
            const_reference operator [](size_type index) const override { return m_impl[index]; }
            reference front() override { return m_impl.front(); }
            const_reference front() const override { return m_impl.front(); }
            reference back() override { return m_impl.back(); }
            const_reference back() const override { return m_impl.back(); }
            size_type size() const override { return static_cast<size_type>(m_impl.size()); }
            bool empty() const override { return m_impl.empty(); }
            void clear() override { m_impl.clear(); }
            iterator insert(iterator pos, const T &value) override { return iterator::fromImpl(m_impl.insert(pos.template getImpl<const typename C::iterator>(), value)); }
            void push_back(const T &value) override { m_impl.push_back(value); }
            void push_back(T &&value) override { m_impl.push_back(std::move(value)); }
            void pop_back() override { m_impl.pop_back(); }
            iterator erase(iterator pos) override { return iterator::fromImpl(m_impl.erase(pos.template getImpl<const typename C::iterator>())); }
            iterator erase(iterator it1, iterator it2) override { return iterator::fromImpl(m_impl.erase(it1.template getImpl<const typename C::iterator>(), it2.template getImpl<const typename C::iterator>())); }
            void *impl() override { return &m_impl; }
        private:
            C m_impl;
        };

        using PimplPtr = QScopedPointer<PimplBase>;
        PimplPtr m_pimpl;

        explicit CSequence(PimplBase *pimpl) : m_pimpl(pimpl) {} // private ctor used by fromImpl()

        // using these methods to access m_pimpl.data() eases the cognitive burden of correctly forwarding const
        PimplBase *pimpl() { return m_pimpl.data(); }
        const PimplBase *pimpl() const { return m_pimpl.data(); }
    };

} //namespace BlackMisc

Q_DECLARE_METATYPE(BlackMisc::CSequence<int>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<uint>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<qlonglong>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<qulonglong>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<double>)

// conditions matched with pop pragmas at bottom of file
#if defined(QT_CC_CLANG)
#pragma clang diagnostic pop
#elif defined(Q_CC_MSVC) && defined(Q_OS_WIN64) && defined(QT_CC_WARNINGS)
#pragma warning(pop)
#endif

#endif //BLACKMISC_SEQUENCE_H
