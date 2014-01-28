/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*!
    \file
*/

#ifndef BLACKMISC_COLLECTION_H
#define BLACKMISC_COLLECTION_H

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
     * \brief Generic type-erased unsequenced container with value semantics.
     * \tparam T the type of elements contained.
     *
     * Can take any suitable container class as its implementation at runtime.
     */
    template <class T>
    class CCollection : public CContainerBase<CCollection, T>
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
        typedef typename Iterators::ConstForwardIterator<T> const_iterator;
        typedef const_iterator iterator; // can't modify elements in-place
        typedef ptrdiff_t difference_type;
        typedef int size_type;
        //! @}

        /*!
         * \brief Default constructor.
         */
        CCollection() : m_pimpl(new Pimpl<QSet<T>>(QSet<T>())) {}

        /*!
         * \brief Copy constructor.
         */
        CCollection(const CCollection &other) : m_pimpl(other.pimpl() ? other.pimpl()->clone() : nullptr) {}

        /*!
         * \brief Move constructor.
         */
        CCollection(CCollection &&other) : m_pimpl(other.m_pimpl.take()) {}

        /*!
         * \brief Copy assignment.
         */
        CCollection &operator =(const CCollection &other) { m_pimpl.reset(other.pimpl() ? other.pimpl()->clone() : nullptr); return *this; }

        /*!
         * \brief Move assignment.
         */
        CCollection &operator =(CCollection && other) { m_pimpl.reset(other.m_pimpl.take()); return *this; }

        /*!
         * \brief Create a new collection with a specific implementation type.
         * \tparam C Becomes the collection's implementation type.
         * \param c Initial value for the collection; default is empty, but it could contain elements if desired. The value is copied.
         */
        template <class C> static CCollection fromImpl(C c = C()) { return CCollection(new Pimpl<C>(std::move(c))); }

        /*!
         * \brief Change the implementation type but keep all the same elements, by copying them into the new implementation.
         * \tparam C Becomes the collection's new implementation type.
         */
        template <class C> void changeImpl(C = C()) { auto c = fromImpl(C()); for (auto i = cbegin(); i != cend(); ++i) c.insert(*i); *this = std::move(c); }

        /*!
         * \brief Like changeImpl, but uses the implementation type of another collection.
         * \pre The other collection must be initialized.
         */
        void useImplOf(const CCollection &other) { PimplPtr p = other.pimpl()->cloneEmpty(); for (auto i = cbegin(); i != cend(); ++i) p->insert(*i); m_pimpl.reset(p.take()); }

        /*!
         * \brief Returns iterator at the beginning of the collection.
         */
        iterator begin() { return pimpl() ? pimpl()->begin() : iterator(); }

        /*!
         * \brief Returns iterator at the beginning of the collection.
         */
        const_iterator begin() const { return pimpl() ? pimpl()->begin() : const_iterator(); }

        /*!
         * \brief Returns iterator at the beginning of the collection.
         */
        const_iterator cbegin() const { return pimpl() ? pimpl()->cbegin() : const_iterator(); }

        /*!
         * \brief Returns iterator one past the end of the collection.
         */
        iterator end() { return pimpl() ? pimpl()->end() : iterator(); }

        /*!
         * \brief Returns iterator one past the end of the collection.
         */
        const_iterator end() const { return pimpl() ? pimpl()->end() : const_iterator(); }

        /*!
         * \brief Returns iterator one past the end of the collection.
         */
        const_iterator cend() const { return pimpl() ? pimpl()->cend() : const_iterator(); }

        /*!
         * \brief Swap this collection with another.
         */
        void swap(CCollection &other) { m_pimpl.swap(other.m_pimpl); }

        /*!
         * \brief Returns number of elements in the collection.
         */
        size_type size() const { return pimpl() ? pimpl()->size() : 0; }

        /*!
         * \brief Returns true if the collection is empty.
         */
        bool empty() const { return pimpl() ? pimpl()->empty() : true; }

        /*!
         * \brief Synonym for empty.
         */
        bool isEmpty() const { return empty(); }

        /*!
         * \brief Removes all elements in the collection.
         */
        void clear() { if (pimpl()) pimpl()->clear(); }

        /*!
         * \brief Inserts an element into the collection.
         * \return An iterator to the position where value was inserted.
         * \pre The collection must be initialized.
         */
        iterator insert(const T &value) { Q_ASSERT(pimpl()); return pimpl()->insert(value); }

        /*!
         * \brief Synonym for insert.
         * \return An iterator to the position where value was inserted.
         * \pre The collection must be initialized.
         */
        iterator push_back(const T &value) { return insert(value); }

        /*!
         * \brief Remove the element pointed to by the given iterator.
         * \return An iterator to the position of the next element after the one removed.
         * \pre The collection must be initialized.
         */
        iterator erase(iterator pos) { Q_ASSERT(pimpl()); return pimpl()->erase(pos); }

        /*!
         * \brief Remove the range of elements between two iterators.
         * \return An iterator to the position of the next element after the one removed.
         * \pre The sequence must be initialized.
         */
        iterator erase(iterator it1, iterator it2) { Q_ASSERT(pimpl()); return pimpl()->erase(it1, it2); }

        /*!
         * \brief Efficient find method using the find of the implementation container. Typically O(log n).
         * \return An iterator to the position of the found element, or the end iterator if not found.
         * \pre The sequence must be initialized.
         */
        iterator find(const T &value) { Q_ASSERT(pimpl()); return pimpl()->find(value); }

        /*!
         * \brief Efficient find method using the find of the implementation container. Typically O(log n).
         * \return An iterator to the position of the found element, or the end iterator if not found.
         * \pre The sequence must be initialized.
         */
        const_iterator find(const T &value) const { Q_ASSERT(pimpl()); return pimpl()->find(value); }

        /*!
         * \brief Efficient remove using the find and erase of the implementation container. Typically O(log n).
         * \pre The sequence must be initialized.
         */
        void remove(const T &object) { auto it = find(object); if (it != end()) { erase(pos); } }

        /*!
         * \brief Test for equality.
         * \todo Improve inefficient implementation.
         */
        bool operator ==(const CCollection &other) const { return (empty() && other.empty()) ? true : (size() != other.size() ? false : *pimpl() == *other.pimpl()); }

        /*!
         * \brief Test for inequality.
         * \todo Improve inefficient implementation.
         */
        bool operator !=(const CCollection &other) const { return !(*this == other); }

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
            virtual size_type size() const = 0;
            virtual bool empty() const = 0;
            virtual void clear() = 0;
            virtual iterator insert(const T &value) = 0;
            virtual iterator erase(iterator pos) = 0;
            virtual iterator erase(iterator it1, iterator it2) = 0;
            virtual iterator find(const T &value) = 0;
            virtual const_iterator find(const T &value) const = 0;
            virtual bool operator ==(const PimplBase &other) const = 0;
        };

        template <class C> class Pimpl : public PimplBase
        {
        public:
            static_assert(std::is_same<T, typename C::value_type>::value, "CCollection must be initialized from a container with the same value_type.");
            Pimpl(C &&c) : m_impl(std::move(c)) {}
            PimplBase *clone() const { return new Pimpl(*this); }
            PimplBase *cloneEmpty() const { return new Pimpl(C()); }
            iterator begin() { return iterator::fromImpl(m_impl.begin()); }
            const_iterator begin() const { return const_iterator::fromImpl(m_impl.cbegin()); }
            const_iterator cbegin() const { return const_iterator::fromImpl(m_impl.cbegin()); }
            iterator end() { return iterator::fromImpl(m_impl.end()); }
            const_iterator end() const { return const_iterator::fromImpl(m_impl.cend()); }
            const_iterator cend() const { return const_iterator::fromImpl(m_impl.cend()); }
            size_type size() const { return m_impl.size(); }
            bool empty() const { return m_impl.empty(); }
            void clear() { m_impl.clear(); }
            iterator insert(const T &value) { return iterator::fromImpl(insertHelper(m_impl.insert(value))); }
            iterator erase(iterator pos) { return iterator::fromImpl(m_impl.erase(*static_cast<const typename C::iterator *>(pos.getImpl()))); }
            //iterator erase(iterator it1, iterator it2) { return iterator::fromImpl(m_impl.erase(*static_cast<const typename C::iterator *>(it1.getImpl()), *static_cast<const typename C::iterator*>(it2.getImpl()))); }
            iterator erase(iterator it1, iterator it2) { while (it1 != it2) { it1 = iterator::fromImpl(m_impl.erase(*static_cast<const typename C::iterator *>(it1.getImpl()))); } return it1; }
            iterator find(const T &value) { return iterator::fromImpl(m_impl.find(value)); }
            const_iterator find(const T &value) const { return const_iterator::fromImpl(m_impl.find(value)); }
            bool operator ==(const PimplBase &other) const { Pimpl copy = C(); for (auto i = other.cbegin(); i != other.cend(); ++i) copy.insert(*i); return m_impl == copy.m_impl; }
        private:
            C m_impl;
            // insertHelper: QSet::insert returns an iterator, but std::set::insert returns a std::pair<interator, bool>
            template <class I> static I insertHelper(I i) { return i; }
            template <class I> static I insertHelper(std::pair<I, bool> p) { return p.first; }
        };

        typedef QScopedPointer<PimplBase> PimplPtr;
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
// CCollection<double> not instantiated because QSet<double> is not supported due to hashing constraints

#endif // guard
