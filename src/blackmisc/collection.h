/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_COLLECTION_H
#define BLACKMISC_COLLECTION_H

#include "blackmisc/containerbase.h"
#include "blackmisc/mixin/mixindatastream.h"
#include "blackmisc/mixin/mixinicon.h"
#include <QMap>
#include <algorithm>
#include <type_traits>
#include <typeindex>
#include <iterator>
#include <utility>
#include <initializer_list>

//! \cond
#define BLACK_TEMPLATE_COLLECTION_MIXINS(NS, T, Set, Extern)                \
    namespace NS { class Set; }                                             \
    namespace BlackMisc::Private                                            \
    {                                                                       \
        Extern template struct CValueObjectMetaInfo<NS::Set>;               \
        Extern template struct CValueObjectMetaInfo<CCollection<NS::T>>;    \
        Extern template struct MetaTypeHelper<NS::Set>;                     \
        Extern template struct MetaTypeHelper<CCollection<NS::T>>;          \
    }                                                                       \
    namespace BlackMisc::Mixin                                              \
    {                                                                       \
        Extern template class MetaType<NS::Set>;                            \
        Extern template class MetaType<CCollection<NS::T>>;                 \
        Extern template class DBusOperators<CCollection<NS::T>>;            \
        Extern template class JsonOperators<CCollection<NS::T>>;            \
        Extern template class String<CCollection<NS::T>>;                   \
        Extern template class DataStreamOperators<CCollection<NS::T>>;      \
        Extern template class Icon<CCollection<NS::T>>;                     \
    }
//! \endcond

/*!
 * \def BLACK_DECLARE_COLLECTION_MIXINS
 * Explicit template declaration of mixins for a CCollection subclass
 * to be placed near the top of the header that defines the class
 */

/*!
 * \def BLACK_DEFINE_COLLECTION_MIXINS
 * Explicit template definition of mixins for a CCollection subclass
 */
#if defined(Q_OS_WIN) && defined(Q_CC_GNU)
#  define BLACK_DECLARE_COLLECTION_MIXINS(Namespace, T, Set)
#  define BLACK_DEFINE_COLLECTION_MIXINS(Namespace, T, Set)
#else
#  define BLACK_DECLARE_COLLECTION_MIXINS(Namespace, T, Set) BLACK_TEMPLATE_COLLECTION_MIXINS(Namespace, T, Set, extern)
#  define BLACK_DEFINE_COLLECTION_MIXINS(Namespace, T, Set)  BLACK_TEMPLATE_COLLECTION_MIXINS(Namespace, T, Set, )
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
     * Generic ordered container with value semantics.
     * \tparam T the type of elements contained.
     */
    template <class T>
    class CCollection :
        public CContainerBase<CCollection<T>>,
        public Mixin::DataStreamOperators<CCollection<T>>,
        public Mixin::Icon<CCollection<T>>
    {
    public:
        //! STL compatibility
        typedef T key_type;

        //! STL compatibility
        typedef T value_type;

        //! STL compatibility
        typedef T &reference;

        //! STL compatibility
        typedef const T &const_reference;

        //! STL compatibility
        typedef T *pointer;

        //! STL compatibility
        typedef const T *const_pointer;

        //! STL compatibility
        typedef typename QOrderedSet<T>::const_iterator const_iterator;

        //! STL compatibility
        typedef const_iterator iterator; // can't modify elements in-place

        //! STL compatibility
        typedef ptrdiff_t difference_type;

        //! STL compatibility
        typedef int size_type;

        //! Default constructor.
        CCollection() {}

        //! Initializer list constructor.
        CCollection(std::initializer_list<T> il) : m_impl(il) {}

        //! Copy constructor.
        CCollection(const CCollection &other) = default;

        //! Constructor from QList.
        CCollection(const QList<T> &list) : m_impl(list) {}

        //! Move constructor.
        CCollection(CCollection &&other) = default;

        //! Copy assignment.
        CCollection &operator =(const CCollection &other) = default;

        //! Move assignment.
        CCollection &operator =(CCollection &&other) = default;

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
        iterator insert(const_iterator hint, const T &value) { Q_UNUSED(hint); return insert(value); }

        //! For compatibility with std::inserter.
        //! \param hint Ignored.
        //! \param value The value to move in.
        iterator insert(const_iterator hint, T &&value) { Q_UNUSED(hint); return insert(std::move(value)); }

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
        void insert(const CRange<I> &range) { std::copy(range.begin(), range.end(), std::back_inserter(*this)); }

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
        //! \fixme Relying on implementation detail of QMap to reinterpret_cast to the necessary iterator type.
        iterator erase(iterator pos)
        {
            return m_impl.erase(const_cast<QMapNode<T, T> *&>(reinterpret_cast<const QMapNode<T, T> *&>(pos)));
        }

        //! Remove the range of elements between two iterators.
        //! \return An iterator to the position of the next element after the one removed.
        //! \fixme Relying on implementation detail of QMap to reinterpret_cast to the necessary iterator type.
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

        //! \private Calls detach on the internal QMap.
        void detach() { m_impl.detach(); }

        //! Test for equality.
        friend bool operator ==(const CCollection &a, const CCollection &b) { return a.m_impl == b.m_impl; }

        //! Test for inequality.
        friend bool operator !=(const CCollection &a, const CCollection &b) { return a.m_impl != b.m_impl; }

        //! \copydoc BlackMisc::Mixin::DataStreamByMetaClass::marshalToDataStream
        void marshalToDataStream(QDataStream &stream) const { stream << m_impl; }

        //! \copydoc BlackMisc::Mixin::DataStreamByMetaClass::unmarshalFromDataStream
        void unmarshalFromDataStream(QDataStream &stream) { stream >> m_impl; }

    private:
        QOrderedSet<T> m_impl;
    };

} //namespace BlackMisc

Q_DECLARE_METATYPE(BlackMisc::CCollection<int>)
Q_DECLARE_METATYPE(BlackMisc::CCollection<uint>)
Q_DECLARE_METATYPE(BlackMisc::CCollection<qlonglong>)
Q_DECLARE_METATYPE(BlackMisc::CCollection<qulonglong>)
// CCollection<double> not instantiated due to it being a dumb idea because of rounding issues

#endif // guard
