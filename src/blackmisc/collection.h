/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_COLLECTION_H
#define BLACKMISC_COLLECTION_H

#include "iterator.h"
#include "containerbase.h"
#include "icon.h"

#include <QScopedPointer>
#include <algorithm>
#include <type_traits>
#include <typeindex>
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
        QOrderedSet() {}

        //! Initializer list constructor.
        QOrderedSet(std::initializer_list<T> il) { for (const auto &v : il) { insert(v); } }

        //! Constructor from QList
        QOrderedSet(const QList<T> &list) { for (const auto &v : list) { insert(v); }}
    };

    /*!
     * Generic type-erased ordered container with value semantics.
     * \tparam T the type of elements contained.
     *
     * Can take any suitable container class as its implementation at runtime.
     */
    template <class T>
    class CCollection :
        public CContainerBase<CCollection<T>>,
        public Mixin::Icon<CCollection<T>>
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
        typedef typename Iterators::ConstForwardIterator<T> const_iterator;
        typedef const_iterator iterator; // can't modify elements in-place
        typedef ptrdiff_t difference_type;
        typedef int size_type;
        //! @}

        //! Default constructor.
        CCollection() : m_pimpl(new Pimpl<QOrderedSet<T>>(QOrderedSet<T>())) {}

        //! Initializer list constructor.
        CCollection(std::initializer_list<T> il) : m_pimpl(new Pimpl<QOrderedSet<T>>(QOrderedSet<T>(il))) {}

        //! Copy constructor.
        CCollection(const CCollection &other) : m_pimpl(other.pimpl() ? other.pimpl()->clone() : nullptr) {}

        //! Constructor from QList.
        CCollection(const QList<T> &list) : m_pimpl(new Pimpl<QOrderedSet<T>>(QOrderedSet<T>(list))) {}

        //! Move constructor.
        CCollection(CCollection &&other) noexcept(std::is_nothrow_move_constructible<T>::value) : m_pimpl(other.m_pimpl.take()) {}

        //! Copy assignment.
        CCollection &operator =(const CCollection &other) { m_pimpl.reset(other.pimpl() ? other.pimpl()->clone() : nullptr); return *this; }

        //! Move assignment.
        CCollection &operator =(CCollection && other) noexcept(std::is_nothrow_move_assignable<T>::value) { m_pimpl.reset(other.m_pimpl.take()); return *this; }

        //! Destructor.
        ~CCollection() = default;

        //! Create a new collection with a specific implementation type.
        //! \tparam C Becomes the collection's implementation type.
        //! \param c Initial value for the collection; default is empty, but it could contain elements if desired. The value is copied.
        template <class C> static CCollection fromImpl(C c = C()) { return CCollection(new Pimpl<C>(std::move(c))); }

        //! Change the implementation type but keep all the same elements, by moving them into the new implementation.
        //! \tparam C Becomes the collection's new implementation type.
        template <class C> void changeImpl(C = C()) { auto c = fromImpl(C()); std::move(begin(), end(), std::inserter(c, c.begin())); *this = std::move(c); }

        //! Like changeImpl, but uses the implementation type of another collection.
        //! \pre The other collection must be initialized.
        void useImplOf(const CCollection &other) { CCollection c(other.pimpl()->cloneEmpty()); std::move(begin(), end(), std::inserter(c, c.begin())); *this = std::move(c); }

        //! Returns iterator at the beginning of the collection.
        iterator begin() { return pimpl() ? pimpl()->begin() : iterator(); }

        //! Returns iterator at the beginning of the collection.
        const_iterator begin() const { return pimpl() ? pimpl()->begin() : const_iterator(); }

        //! Returns iterator at the beginning of the collection.
        const_iterator cbegin() const { return pimpl() ? pimpl()->cbegin() : const_iterator(); }

        //! Returns iterator one past the end of the collection.
        iterator end() { return pimpl() ? pimpl()->end() : iterator(); }

        //! Returns const iterator one past the end of the collection.
        const_iterator end() const { return pimpl() ? pimpl()->end() : const_iterator(); }

        //! Returns const iterator one past the end of the collection.
        const_iterator cend() const { return pimpl() ? pimpl()->cend() : const_iterator(); }

        //! Swap this collection with another.
        void swap(CCollection &other) noexcept { m_pimpl.swap(other.m_pimpl); }

        //! Returns number of elements in the collection.
        size_type size() const { return pimpl() ? pimpl()->size() : 0; }

        //! Returns true if the collection is empty.
        bool empty() const { return pimpl() ? pimpl()->empty() : true; }

        //! Synonym for empty.
        bool isEmpty() const { return empty(); }

        //! Removes all elements in the collection.
        void clear() { if (pimpl()) pimpl()->clear(); }

        //! For compatibility with std::inserter.
        //! \param hint Ignored.
        //! \param value The value to insert.
        //! \pre The collection must be initialized.
        iterator insert(const_iterator hint, const T &value) { Q_UNUSED(hint); return insert(value); }

        //! For compatibility with std::inserter.
        //! \param hint Ignored.
        //! \param value The value to move in.
        //! \pre The collection must be initialized.
        iterator insert(const_iterator hint, T &&value) { Q_UNUSED(hint); return insert(std::move(value)); }

        //! Inserts an element into the collection.
        //! \return An iterator to the position where value was inserted.
        //! \pre The collection must be initialized.
        iterator insert(const T &value) { Q_ASSERT(pimpl()); return pimpl()->insert(value); }

        //! Moves an element into the collection.
        //! \return An iterator to the position where value was inserted.
        //! \pre The collection must be initialized.
        iterator insert(T &&value) { Q_ASSERT(pimpl()); return pimpl()->insert(std::move(value)); }

        //! Inserts all elements from another collection into this collection.
        //! \pre This collection must be initialized.
        void insert(const CCollection &other) { std::copy(other.begin(), other.end(), std::inserter(*this, begin())); }

        //! Inserts all elements from another collection into this collection.
        //! This version moves elements instead of copying.
        //! \pre This collection must be initialized.
        void insert(CCollection &&other) { std::move(other.begin(), other.end(), std::inserter(*this, begin())); }

        //! Appends all elements from a range at the end of this collection.
        //! \pre This collection must be initialized.
        template <typename I>
        void insert(const CRange<I> &range) { std::copy(range.begin(), range.end(), std::back_inserter(*this)); }

        //! Synonym for insert.
        //! \return An iterator to the position where value was inserted.
        //! \pre The collection must be initialized.
        iterator push_back(const T &value) { return insert(value); }

        //! Synonym for insert.
        //! \return An iterator to the position where value was inserted.
        //! \pre The collection must be initialized.
        iterator push_back(T &&value) { return insert(std::move(value)); }

        //! Synonym for insert.
        //! \pre This collection must be initialized.
        void push_back(const CCollection &other) { insert(other); }

        //! Synonym for insert.
        //! \pre This collection must be initialized.
        void push_back(CCollection &&other) { insert(std::move(other)); }

        //! Synonym for insert.
        //! \pre This collection must be initialized.
        template <typename I>
        void push_back(const CRange<I> &range) { std::copy(range.begin(), range.end(), std::back_inserter(*this)); }

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

        //! Returns a collection which contains all the elements from this collection which are not in the other collection.
        template <class C>
        CCollection difference(const C &other) const
        {
            CCollection result;
            std::set_difference(begin(), end(), other.begin(), other.end(), std::inserter(result, result.begin()));
            return result;
        }

        //! Remove the element pointed to by the given iterator.
        //! \return An iterator to the position of the next element after the one removed.
        //! \pre The collection must be initialized.
        iterator erase(iterator pos) { Q_ASSERT(pimpl()); return pimpl()->erase(pos); }

        //! Remove the range of elements between two iterators.
        //! \return An iterator to the position of the next element after the one removed.
        //! \pre The sequence must be initialized.
        iterator erase(iterator it1, iterator it2) { Q_ASSERT(pimpl()); return pimpl()->erase(it1, it2); }

        //! Efficient find method using the find of the implementation container. Typically O(log n).
        //! \return An iterator to the position of the found element, or the end iterator if not found.
        //! \pre The sequence must be initialized.
        //! \warning Take care that the returned non-const iterator is not compared with a const iterator.
        iterator find(const T &value) { Q_ASSERT(pimpl()); return pimpl()->find(value); }

        //! Efficient find method using the find of the implementation container. Typically O(log n).
        //! \return An iterator to the position of the found element, or the end iterator if not found.
        //! \pre The sequence must be initialized.
        const_iterator find(const T &value) const { Q_ASSERT(pimpl()); return pimpl()->find(value); }

        //! Efficient remove using the find and erase of the implementation container. Typically O(log n).
        //! \pre The sequence must be initialized.
        void remove(const T &object) { auto it = find(object); if (it != end()) { erase(it); } }

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
                if (p(*it)) { it = erase(it); count++; }
                else { ++it; }
            }
            return count;
        }

        //! \copydoc BlackMisc::CContainerBase::removeIf
        template <class K0, class V0, class... KeysValues>
        int removeIf(K0 k0, V0 v0, KeysValues... keysValues)
        {
            // using-declaration doesn't play nicely with injected template names
            return CCollection::CContainerBase::removeIf(k0, v0, keysValues...);
        }

        //! Convert to a QList
        QList<T> toQList() const
        {
            return this->to(QList<T>());
        }

        //! Test for equality.
        bool operator ==(const CCollection &other) const { return *pimpl() == *other.pimpl(); }

        //! Test for inequality.
        bool operator !=(const CCollection &other) const { return !(*this == other); }

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
            virtual size_type size() const = 0;
            virtual bool empty() const = 0;
            virtual void clear() = 0;
            virtual iterator insert(const T &value) = 0;
            virtual iterator insert(T &&value) = 0;
            virtual iterator erase(iterator pos) = 0;
            virtual iterator erase(iterator it1, iterator it2) = 0;
            virtual iterator find(const T &value) = 0;
            virtual const_iterator find(const T &value) const = 0;
            virtual bool operator ==(const PimplBase &other) const = 0;
            virtual void *impl() = 0;
            virtual const void *impl() const = 0;
            virtual std::type_index implType() const = 0;
        };

        template <class C> class Pimpl : public PimplBase
        {
        public:
            static_assert(std::is_same<T, typename C::value_type>::value, "CCollection must be initialized from a container with the same value_type.");
            Pimpl(C &&c) : m_impl(std::move(c)) {}
            PimplBase *clone() const override { return new Pimpl(*this); }
            PimplBase *cloneEmpty() const override { return new Pimpl(C()); }
            iterator begin() override { return iterator::fromImpl(m_impl.begin()); }
            const_iterator begin() const override { return const_iterator::fromImpl(m_impl.cbegin()); }
            const_iterator cbegin() const override { return const_iterator::fromImpl(m_impl.cbegin()); }
            iterator end() override { return iterator::fromImpl(m_impl.end()); }
            const_iterator end() const override { return const_iterator::fromImpl(m_impl.cend()); }
            const_iterator cend() const override { return const_iterator::fromImpl(m_impl.cend()); }
            size_type size() const override { return static_cast<size_type>(m_impl.size()); }
            bool empty() const override { return m_impl.empty(); }
            void clear() override { m_impl.clear(); }
            iterator insert(const T &value) override { return iterator::fromImpl(insertHelper(m_impl.insert(value))); }
            iterator insert(T &&value) override { return iterator::fromImpl(insertHelper(m_impl.insert(std::move(value)))); }
            iterator erase(iterator pos) override { return iterator::fromImpl(m_impl.erase(pos.template getImpl<const typename C::iterator>())); }
            //iterator erase(iterator it1, iterator it2) override { return iterator::fromImpl(m_impl.erase(it1.template getImpl<const typename C::iterator>(), it2.template getImpl<const typename C::iterator>())); }
            iterator erase(iterator it1, iterator it2) override { while (it1 != it2) { it1 = iterator::fromImpl(m_impl.erase(it1.template getImpl<const typename C::iterator>())); } return it1; }
            iterator find(const T &value) override { return iterator::fromImpl(m_impl.find(value)); }
            const_iterator find(const T &value) const override { return const_iterator::fromImpl(m_impl.find(value)); }
            bool operator ==(const PimplBase &other) const override { return implType() == other.implType() ? m_impl == *static_cast<const C *>(other.impl()) : size() == other.size() && std::equal(begin(), end(), other.begin()); }
            void *impl() override { return &m_impl; }
            const void *impl() const override { return &m_impl; }
            std::type_index implType() const override { return typeid(C); }
        private:
            C m_impl;
            bool implEquals(const PimplBase &other) const { return m_impl == *static_cast<const C *>(other.impl()); }
            bool equals(const PimplBase &other) const { return size() == other.size() && std::equal(begin(), end(), other.begin()); }
            // insertHelper: QOrderedSet::insert returns an iterator, but std::set::insert returns a std::pair<interator, bool>
            template <class I> static I insertHelper(I i) { return i; }
            template <class I> static I insertHelper(std::pair<I, bool> p) { return p.first; }
        };

        using PimplPtr = QScopedPointer<PimplBase>;
        PimplPtr m_pimpl;

        CCollection(PimplBase *pimpl) : m_pimpl(pimpl) {} // private ctor used by fromImpl()

        // using these methods to access m_pimpl.data() eases the cognitive burden of correctly forwarding const
        PimplBase *pimpl() { return m_pimpl.data(); }
        const PimplBase *pimpl() const { return m_pimpl.data(); }
    };

} //namespace BlackMisc

Q_DECLARE_METATYPE(BlackMisc::CCollection<int>)
Q_DECLARE_METATYPE(BlackMisc::CCollection<uint>)
Q_DECLARE_METATYPE(BlackMisc::CCollection<qlonglong>)
Q_DECLARE_METATYPE(BlackMisc::CCollection<qulonglong>)
// CCollection<double> not instantiated due to it being a dumb idea because of rounding issues

// conditions matched with pop pragmas at bottom of file
#if defined(QT_CC_CLANG)
#pragma clang diagnostic pop
#elif defined(Q_CC_MSVC) && defined(Q_OS_WIN64) && defined(QT_CC_WARNINGS)
#pragma warning(pop)
#endif

#endif // guard
