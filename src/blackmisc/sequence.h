/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*!
    \file
*/

#ifndef BLACKMISC_SEQUENCE_H
#define BLACKMISC_SEQUENCE_H

#include "iterator.h"
#include "containerbase.h"
#include <QScopedPointer>
#include <algorithm>
#include <type_traits>
#include <iterator>
#include <utility>

namespace BlackMisc
{

    /*!
     * \brief Generic type-erased sequential container with value semantics.
     * \tparam T the type of elements contained.
     *
     * Can take any suitable container class as its implementation at runtime.
     */
    template <class T>
    class CSequence : public CContainerBase<CSequence, T>
    {
    public:
        //! \brief STL compatibility
        //! @{
        typedef T key_type;
        typedef T value_type;
        typedef T &reference;
        typedef const T &const_reference;
        typedef T *pointer;
        typedef const T *const_pointer;
        typedef typename Iterators::ConstBidirectionalIterator<T> const_iterator;
        typedef typename Iterators::BidirectionalIterator<T> iterator;
        typedef ptrdiff_t difference_type;
        typedef intptr_t size_type;
        //! @}

        /*!
         * \brief Default constructor.
         */
        CSequence() : m_pimpl(new Pimpl<QList<T>>(QList<T>())) {}

        /*!
         * \brief Copy constructor.
         */
        CSequence(const CSequence &other) : m_pimpl(other.pimpl() ? other.pimpl()->clone() : nullptr) {}

        /*!
         * \brief Move constructor.
         */
        CSequence(CSequence &&other) : m_pimpl(other.m_pimpl.take()) {}

        /*!
         * \brief Copy assignment.
         */
        CSequence &operator =(const CSequence &other) { m_pimpl.reset(other.pimpl() ? other.pimpl()->clone() : nullptr); return *this; }

        /*!
         * \brief Move assignment.
         */
        CSequence &operator =(CSequence &&other) { m_pimpl.reset(other.m_pimpl.take()); return *this; }

        /*!
         * \brief Create a new sequence with a specific implementation type.
         * \tparam C Becomes the sequence's implementation type.
         * \param c Initial value for the sequence; default is empty, but it could contain elements if desired. The value is copied.
         */
        template <class C> static CSequence fromImpl(C c = C()) { return CSequence(new Pimpl<C>(std::move(c))); }

        /*!
         * \brief Change the implementation type but keep all the same elements, by moving them into the new implementation.
         * \tparam C Becomes the sequence's new implementation type.
         */
        template <class C> void changeImpl(C = C()) { auto c = fromImpl(C()); std::move(begin(), end(), std::inserter(c, c.begin())); *this = std::move(c); }

        /*!
         * \brief Like changeImpl, but uses the implementation type of another sequence.
         * \pre The other sequence must be initialized.
         */
        void useImplOf(const CSequence &other) { PimplPtr p = other.pimpl()->cloneEmpty(); std::move(begin(), end(), std::inserter(*p, p->begin())); m_pimpl.reset(p.take()); }

        /*!
         * \brief Returns iterator at the beginning of the sequence.
         */
        iterator begin() { return pimpl() ? pimpl()->begin() : iterator(); }

        /*!
         * \brief Returns const iterator at the beginning of the sequence.
         */
        const_iterator begin() const { return pimpl() ? pimpl()->begin() : const_iterator(); }

        /*!
         * \brief Returns const iterator at the beginning of the sequence.
         */
        const_iterator cbegin() const { return pimpl() ? pimpl()->cbegin() : const_iterator(); }

        /*!
         * \brief Returns iterator one past the end of the sequence.
         */
        iterator end() { return pimpl() ? pimpl()->end() : iterator(); }

        /*!
         * \brief Returns const iterator one past the end of the sequence.
         */
        const_iterator end() const { return pimpl() ? pimpl()->end() : const_iterator(); }

        /*!
         * \brief Returns const iterator one past the end of the sequence.
         */
        const_iterator cend() const { return pimpl() ? pimpl()->cend() : const_iterator(); }

        /*!
         * \brief Swap this sequence with another.
         */
        void swap(CSequence &other) { m_pimpl.swap(other.m_pimpl); }

        /*!
         * \brief Access an element by its index.
         * \pre The sequence must be initialized and the index in bounds.
         */
        reference operator [](size_type index) { Q_ASSERT(pimpl()); return pimpl()->operator [](index); }

        /*!
         * \brief Access an element by its index.
         * \pre The sequence must be initialized and the index in bounds.
         */
        const_reference operator [](size_type index) const { Q_ASSERT(pimpl()); return pimpl()->operator [](index); }

        /*!
         * \brief Access the first element.
         * \pre The sequence must not be empty.
         */
        reference front() { Q_ASSERT(!empty()); return pimpl()->front(); }

        /*!
         * \brief Access the first element.
         * \pre The sequence must not be empty.
         */
        const_reference front() const { Q_ASSERT(!empty()); return pimpl()->front(); }

        /*!
         * \brief Access the first element, or a default-initialized value if the sequence is empty.
         */
        const_reference frontOrDefault() const { static const value_type def; return empty() ? def : front(); }

        /*!
         * \brief Access the last element.
         * \pre The sequence must not be empty.
         */
        reference back() { Q_ASSERT(!empty()); return pimpl()->back(); }

        /*!
         * \brief Access the last element.
         * \pre The sequence must not be empty.
         */
        const_reference back() const { Q_ASSERT(!empty()); return pimpl()->back(); }

        /*!
        * \brief Access the last element, or a default-initialized value if the sequence is empty.
        */
        const_reference backOrDefault() const { static const value_type def; return empty() ? def : back(); }

        /*!
         * \brief Returns number of elements in the sequence.
         */
        size_type size() const { return pimpl() ? pimpl()->size() : 0; }

        /*!
         * \brief Returns true if the sequence is empty.
         */
        bool empty() const { return pimpl() ? pimpl()->empty() : true; }

        /*!
         * \brief Synonym for empty.
         */
        bool isEmpty() const { return empty(); }

        /*!
         * \brief Removes all elements in the sequence.
         */
        void clear() { if (pimpl()) pimpl()->clear(); }

        /*!
         * \brief Inserts an element into the sequence.
         * \return An iterator to the position where value was inserted.
         * \pre The sequence must be initialized.
         */
        iterator insert(iterator before, const T &value) { Q_ASSERT(pimpl()); return pimpl()->insert(before, value); }

        /*!
         * \brief Moves an element into the sequence.
         * \return An iterator to the position where value was inserted.
         * \pre The sequence must be initialized.
         */
        iterator insert(iterator before, T &&value) { Q_ASSERT(pimpl()); return pimpl()->insert(before, std::move(value)); }

        /*!
         * \brief Appends an element at the end of the sequence.
         * \pre The sequence must be initialized.
         */
        void push_back(const T &value) { Q_ASSERT(pimpl()); pimpl()->push_back(value); }

        /*!
         * \brief Move-appends an element at the end of the sequence.
         * \pre The sequence must be initialized.
         */
        void push_back(T &&value) { Q_ASSERT(pimpl()); pimpl()->push_back(std::move(value)); }

        /*!
         * \brief Appends all elements from another sequence at the end of this sequence.
         * \pre This sequence must be initialized.
         */
        void push_back(const CSequence &other) { std::copy(other.begin(), other.end(), std::back_inserter(*this)); }

        /*!
         * \brief Appends all elements from another sequence at the end of this sequence.
         * This version moves elements instead of copying.
         * \pre This sequence must be initialized.
         */
        void push_back(CSequence &&other) { std::move(other.begin(), other.end(), std::back_inserter(*this)); }

        /*!
         * \brief Synonym for push_back.
         * \pre The sequence must be initialized.
         */
        void insert(const T &value) { push_back(value); }

        /*!
         * \brief Synonym for push_back.
         * \pre The sequence must be initialized.
         */
        void insert(T &&value) { push_back(std::move(value)); }

        /*!
         * \brief Synonym for push_back.
         * \pre The sequence must be initialized.
         */
        void insert(const CSequence &other) { push_back(other); }

        /*!
         * \brief Synonym for push_back.
         * \pre The sequence must be initialized.
         */
        void insert(CSequence &&other) { push_back(std::move(other)); }

        /*!
         * \brief Concatenates two sequences and returns the result.
         * \pre This sequence must be initialized.
         */
        CSequence join(const CSequence &other) const { CSequence copy(*this); copy.push_back(other); return copy; }

        /*!
         * \brief Removes an element at the end of the sequence.
         * \pre The sequence must contain at least one element.
         */
        void pop_back() { Q_ASSERT(!empty()); pimpl()->pop_back(); }

        /*!
         * \brief Remove the element pointed to by the given iterator.
         * \return An iterator to the position of the next element after the one removed.
         * \pre The sequence must be initialized.
         */
        iterator erase(iterator pos) { Q_ASSERT(pimpl()); return pimpl()->erase(pos); }

        /*!
         * \brief Remove the range of elements between two iterators.
         * \return An iterator to the position of the next element after the one removed.
         * \pre The sequence must be initialized.
         */
        iterator erase(iterator it1, iterator it2) { Q_ASSERT(pimpl()); return pimpl()->erase(it1, it2); }

        /*!
         * \brief Return an iterator to the first element equal to the given object, or the end iterator if not found. O(n).
         * \warning Take care that the returned non-const iterator is not compared with a const iterator.
         */
        iterator find(const T &object) { return std::find(begin(), end(), object); }

        /*!
         * \brief Return an iterator to the first element equal to the given object, or the end iterator if not found. O(n).
         */
        const_iterator find(const T &object) const { return std::find(cbegin(), cend(), object); }

        /*!
         * \brief Modify by applying a value map to each element for which a given predicate returns true.
         */
        template <class Predicate>
        void applyIf(Predicate p, const CIndexVariantMap &newValues)
        {
            std::for_each(begin(), end(), [ &, p ](T &value) { if (p(value)) { value.apply(newValues); } });
        }

        /*!
         * \brief Modify by applying a value map to each element matching a particular key/value pair.
         * \param key1 A pointer to a member function of T.
         * \param value1 Will be compared to the return value of key1.
         * \param newValues Values from this map will be put into each matching element.
         */
        template <class K1, class V1>
        void applyIf(K1 key1, V1 value1, const CIndexVariantMap &newValues)
        {
            applyIf(BlackMisc::Predicates::MemberEqual<T>(key1, value1), newValues);
        }

        /*!
         * \brief Modify by applying a value map to each element matching a given value map.
         */
        void applyIf(const CIndexVariantMap &pattern, const CIndexVariantMap &newValues)
        {
            applyIf([ & ](const T &value) { return value == pattern; }, newValues);
        }

        /*!
         * \brief Remove the given object, if it is contained.
         * \pre The sequence must be initialized.
         */
        void remove(const T &object)
        {
            erase(std::remove(begin(), end(), object), end());
        }

        /*!
         * \brief Replace elements matching the given element with a replacement.
         */
        void replace(const T &original, const T &replacement)
        {
            std::replace(begin(), end(), original, replacement);
        }

        /*!
         * \brief Replace elements for which a given predicate returns true.
         */
        template <class Predicate>
        void replaceIf(Predicate p, const T &replacement)
        {
            std::replace_if(begin(), end(), p, replacement);
        }

        /*!
         * \brief Replace elements matching a particular key/value pair.
         * \param key1 A pointer to a member function of T.
         * \param value1 Will be compared to the return value of key1.
         * \param replacement All matching elements will be replaced by copies of this one.
         */
        template <class K1, class V1>
        void replaceIf(K1 key1, V1 value1, const T &replacement)
        {
            replaceIf(BlackMisc::Predicates::MemberEqual<T>(key1, value1), replacement);
        }

        /*!
         * \brief Replace elements for which a given predicate returns true. If there is no match, push the new element on the end.
         * \pre The sequence must be initialized.
         */
        template <class Predicate>
        void replaceOrAdd(Predicate p, const T &replacement)
        {
            if (this->contains(p)) { replaceIf(p, replacement); }
            else { push_back(replacement); }
        }

        /*!
         * \brief Replace elements matching the given element. If there is no match, push the new element on the end.
         * \pre The sequence must be initialized.
         */
        void replaceOrAdd(const T &original, const T &replacement)
        {
            if (this->contains(original)) { replace(original, replacement); }
            else { push_back(replacement); }
        }

        /*!
         * \brief Replace elements matching a particular key/value pair. If there is no match, push the new element on the end.
         * \param key1 A pointer to a member function of T.
         * \param value1 Will be compared to the return value of key1.
         * \param replacement All matching elements will be replaced by copies of this one, or a copy will be added.
         * \pre The sequence must be initialized.
         */
        template <class K1, class V1>
        void replaceOrAdd(K1 key1, V1 value1, const T &replacement)
        {
            if (this->contains(key1, value1)) { replaceIf(key1, value1, replacement); }
            else { push_back(replacement); }
        }

        /*!
         * \brief In-place sort by a given comparator predicate.
         */
        template <class Predicate> void sort(Predicate p)
        {
            std::sort(begin(), end(), p);
        }

        /*!
         * \brief In-place sort by a particular key.
         * \param key1 A pointer to a member function of T.
         */
        template <class K1> void sortBy(K1 key1)
        {
            sort(BlackMisc::Predicates::MemberLess<T>(key1));
        }

        /*!
         * \brief In-place sort by some particular keys.
         * \param key1 A pointer to a member function of T.
         * \param key2 A pointer to a member function of T.
         */
        template <class K1, class K2> void sortBy(K1 key1, K2 key2)
        {
            sort(BlackMisc::Predicates::MemberLess<T>(key1, key2));
        }

        /*!
         * \brief In-place sort by some particular keys.
         * \param key1 A pointer to a member function of T.
         * \param key2 A pointer to a member function of T.
         * \param key3 A pointer to a member function of T.
         */
        template <class K1, class K2, class K3> void sortBy(K1 key1, K2 key2, K3 key3)
        {
            sort(BlackMisc::Predicates::MemberLess<T>(key1, key2, key3));
        }

        /*!
         * \brief Return a copy sorted by a given comparator predicate.
         */
        template <class Predicate>
        CSequence sorted(Predicate p) const
        {
            CSequence result = *this;
            result.sort(p);
            return result;
        }

        /*!
         * \brief Return a copy sorted by a particular key.
         * \param key1 A pointer to a member function of T.
         */
        template <class K1>
        CSequence sortedBy(K1 key1) const
        {
            return sorted(BlackMisc::Predicates::MemberLess<T>(key1));
        }

        /*!
         * \brief Return a copy sorted by some particular keys.
         * \param key1 A pointer to a member function of T.
         * \param key2 A pointer to a member function of T.
         */
        template <class K1, class K2>
        CSequence sortedBy(K1 key1, K2 key2) const
        {
            return sorted(BlackMisc::Predicates::MemberLess<T>(key1, key2));
        }

        /*!
         * \brief Return a copy sorted by some particular keys.
         * \param key1 A pointer to a member function of T.
         * \param key2 A pointer to a member function of T.
         * \param key3 A pointer to a member function of T.
         */
        template <class K1, class K2, class K3>
        CSequence sortedBy(K1 key1, K2 key2, K3 key3) const
        {
            return sorted(BlackMisc::Predicates::MemberLess<T>(key1, key2, key3));
        }

        /*!
         * \brief Test for equality.
         * \todo Improve inefficient implementation.
         */
        bool operator ==(const CSequence &other) const { return (empty() && other.empty()) ? true : (size() != other.size() ? false : *pimpl() == *other.pimpl()); }

        /*!
         * \brief Test for inequality.
         * \todo Improve inefficient implementation.
         */
        bool operator !=(const CSequence &other) const { return !(*this == other); }

        /*!
         * \brief Return an opaque pointer to the implementation container.
         * \details Can be useful in unusual debugging situations.
         * \warning Not for general use.
         */
        void *getImpl() { return pimpl() ? pimpl()->impl() : nullptr; }

    private:
        class PimplBase
        {
        public:
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
            virtual bool operator ==(const PimplBase &other) const = 0;
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
            size_type size() const override { return m_impl.size(); }
            bool empty() const override { return m_impl.empty(); }
            void clear() override { m_impl.clear(); }
            iterator insert(iterator pos, const T &value) override { return iterator::fromImpl(m_impl.insert(*static_cast<const typename C::iterator*>(pos.getImpl()), value)); }
            void push_back(const T &value) override { m_impl.push_back(value); }
            void push_back(T &&value) override { m_impl.push_back(std::move(value)); }
            void pop_back() override { m_impl.pop_back(); }
            iterator erase(iterator pos) override { return iterator::fromImpl(m_impl.erase(*static_cast<const typename C::iterator*>(pos.getImpl()))); }
            iterator erase(iterator it1, iterator it2) override { return iterator::fromImpl(m_impl.erase(*static_cast<const typename C::iterator*>(it1.getImpl()), *static_cast<const typename C::iterator*>(it2.getImpl()))); }
            bool operator ==(const PimplBase &other) const override { Pimpl copy = C(); for (auto i = other.cbegin(); i != other.cend(); ++i) copy.push_back(*i); return m_impl == copy.m_impl; }
            void *impl() override { return &m_impl; }
        private:
            C m_impl;
        };

        typedef QScopedPointer<PimplBase> PimplPtr;
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

#endif //BLACKMISC_SEQUENCE_H