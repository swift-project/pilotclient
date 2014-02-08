/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*!
    \file
*/

#ifndef BLACKMISC_ITERATOR_H
#define BLACKMISC_ITERATOR_H

#include <QScopedPointer>
#include <algorithm>
#include <type_traits>
#include <iterator>
#include <utility>

namespace BlackMisc
{
    namespace Iterators
    {

        /*!
         * \brief Generic type-erased const forward iterator with value semantics.
         * \tparam T the value_type of the container being iterated over.
         *
         * Can take any suitable iterator type as its implementation at runtime.
         */
        template <class T> class ConstForwardIterator
        {
        public:
            //! \brief STL compatibility
            //! @{
            typedef ptrdiff_t difference_type;
            typedef T value_type;
            typedef const T *pointer;
            typedef const T &reference;
            typedef const T *const_pointer;
            typedef const T &const_reference;
            typedef std::forward_iterator_tag iterator_category;
            //! @}

            //! \brief Default constructor.
            ConstForwardIterator() {}

            /*!
             * \brief Copy constructor.
             */
            ConstForwardIterator(const ConstForwardIterator &other) : m_pimpl(other.pimpl() ? other.pimpl()->clone() : nullptr) {}

            /*!
             * \brief Move constructor.
             */
            ConstForwardIterator(ConstForwardIterator &&other) : m_pimpl(other.m_pimpl.take()) {}

            /*!
             * \brief Copy assignment.
             */
            ConstForwardIterator &operator =(const ConstForwardIterator &other) { m_pimpl.reset(other.pimpl() ? other.pimpl()->clone() : nullptr); return *this; }

            /*!
             * \brief Move assignment.
             */
            ConstForwardIterator &operator =(ConstForwardIterator &&other) { m_pimpl.reset(other.m_pimpl.take()); return *this; }

            /*!
             * \brief Create a new iterator with a specific implementation type.
             * \tparam I Becomes the iterator's implementation type.
             * \param i Initial value for the iterator. The value is copied.
             */
            template <class I> static ConstForwardIterator fromImpl(I i) { return ConstForwardIterator(new Pimpl<I>(std::move(i))); }

            /*!
             * \brief Returns a reference to the object pointed to.
             * \pre The iterator must be initialized and valid.
             */
            const_reference operator *() const { Q_ASSERT(m_pimpl); return **pimpl(); }

            /*!
             * \brief Arrow operator provides access to members of the object pointed to.
             * \pre The iterator must be initialized and valid.
             */
            const_pointer operator ->() const { Q_ASSERT(m_pimpl); return &**pimpl(); }

            /*!
             * \brief Prefix increment operator advances the iterator.
             * \return Reference to the iterator at the new position.
             * \pre The iterator must be initialized and valid.
             */
            ConstForwardIterator &operator ++() { Q_ASSERT(m_pimpl); ++*pimpl(); return *this; }

            /*!
             * \brief Postfix increment operator advances the iterator.
             * \return Copy of the iterator in the old position.
             * \pre The iterator must be initialized and valid.
             */
            ConstForwardIterator operator ++(int) { Q_ASSERT(m_pimpl); auto copy = *this; ++*pimpl(); return copy; }

            /*!
             * \brief Advance the iterator by a certain amount.
             * \return Reference to the iterator at the new position.
             * \pre The iterator must be initialized and valid.
             */
            ConstForwardIterator operator +=(difference_type n) { Q_ASSERT(m_pimpl); *pimpl() += n; return *this; }

            /*!
             * \brief Advance the iterator by a certain amount.
             * \return Copy of the iterator in its new position.
             * \pre The iterator must be initialized and valid.
             */
            ConstForwardIterator operator +(difference_type n) const { auto copy = *this; return copy += n; }

            /*!
             * \brief Test for equality.
             * \pre Both iterators must originate from the same collection, and not mix begin/end with cbegin/cend.
             */
            bool operator ==(const ConstForwardIterator &other) const { return (pimpl() && other.pimpl()) ? *pimpl() == *other.pimpl() : pimpl() == other.pimpl(); }

            /*!
             * \brief Test for inequality.
             * \pre Both iterators must originate from the same collection, and not mix begin/end with cbegin/cend.
             */
            bool operator !=(const ConstForwardIterator &other) const { return !(*this == other); }

            /*!
             * \brief Return opaque pointer to underlying implementation iterator object.
             * \pre The iterator must have been initialized.
             * \todo Returning by void* is rotten, but GCC gives a very cryptic error if I make it a function template with a cast inside.
             */
            void *getImpl() { return pimpl()->impl(); }

        private:
            class PimplBase
            {
            public:
                virtual ~PimplBase() {}
                virtual PimplBase *clone() const = 0;
                virtual const_reference operator *() const = 0;
                virtual void operator ++() = 0;
                virtual void operator +=(difference_type) = 0;
                virtual bool operator ==(const PimplBase &) const = 0;
                virtual void *impl() = 0;
            };

            template <class I> class Pimpl : public PimplBase
            {
            public:
                static_assert(std::is_same<T, typename std::iterator_traits<I>::value_type>::value,
                    "ConstForwardIterator must be initialized from an iterator with the same value_type.");
                Pimpl(I &&i) : m_impl(std::move(i)) {}
                virtual PimplBase *clone() const override { return new Pimpl(*this); }
                virtual const_reference operator *() const override { return *m_impl; }
                virtual void operator ++() override { ++m_impl; }
                virtual void operator +=(difference_type n) override { std::advance(m_impl, n); }
                virtual bool operator ==(const PimplBase &other) const override { return m_impl == static_cast<const Pimpl&>(other).m_impl; }
                virtual void *impl() override { return &m_impl; }
            private:
                I m_impl;
            };

            typedef QScopedPointer<PimplBase> PimplPtr;
            PimplPtr m_pimpl;

            explicit ConstForwardIterator(PimplBase *pimpl) : m_pimpl(pimpl) {} // private ctor used by fromImpl()

            // using these methods to access m_pimpl.data() eases the cognitive burden of correctly forwarding const
            PimplBase *pimpl() { return m_pimpl.data(); }
            const PimplBase *pimpl() const { return m_pimpl.data(); }
        };

        /*!
         * \brief Generic type-erased const bidirectional iterator with value semantics.
         * \tparam T the value_type of the container being iterated over.
         *
         * Can take any suitable iterator type as its implementation at runtime.
         */
        template <class T> class ConstBidirectionalIterator
        {
        public:
            //! \brief STL compatibility
            //! @{
            typedef ptrdiff_t difference_type;
            typedef T value_type;
            typedef const T *pointer;
            typedef const T &reference;
            typedef const T *const_pointer;
            typedef const T &const_reference;
            typedef std::bidirectional_iterator_tag iterator_category;
            //! @}

            //! \brief Default constructor.
            ConstBidirectionalIterator() {}

            /*!
             * \brief Copy constructor.
             */
            ConstBidirectionalIterator(const ConstBidirectionalIterator &other) : m_pimpl(other.pimpl() ? other.pimpl()->clone() : nullptr) {}

            /*!
             * \brief Move constructor.
             */
            ConstBidirectionalIterator(ConstBidirectionalIterator &&other) : m_pimpl(other.m_pimpl.take()) {}

            /*!
             * \brief Copy assignment.
             */
            ConstBidirectionalIterator &operator =(const ConstBidirectionalIterator &other) { m_pimpl.reset(other.pimpl() ? other.pimpl()->clone() : nullptr); return *this; }

            /*!
             * \brief Move assignment.
             */
            ConstBidirectionalIterator &operator =(ConstBidirectionalIterator &&other) { m_pimpl.reset(other.m_pimpl.take()); return *this; }

            /*!
             * \brief Create a new iterator with a specific implementation type.
             * \tparam I Becomes the iterator's implementation type.
             * \param i Initial value for the iterator. The value is copied.
             */
            template <class I> static ConstBidirectionalIterator fromImpl(I i) { return ConstBidirectionalIterator(new Pimpl<I>(std::move(i))); }

            /*!
             * \brief Returns a reference to the object pointed to.
             * \pre The iterator must be initialized and valid.
             */
            const_reference operator *() const { Q_ASSERT(m_pimpl); return **pimpl(); }

            /*!
             * \brief Arrow operator provides access to members of the object pointed to.
             * \pre The iterator must be initialized and valid.
             */
            const_pointer operator ->() const { Q_ASSERT(m_pimpl); return &**pimpl(); }

            /*!
             * \brief Prefix increment operator advances the iterator.
             * \return Reference to the iterator at the new position.
             * \pre The iterator must be initialized and valid.
             */
            ConstBidirectionalIterator &operator ++() { Q_ASSERT(m_pimpl); ++*pimpl(); return *this; }

            /*!
             * \brief Postfix increment operator advances the iterator.
             * \return Copy of the iterator in the old position.
             * \pre The iterator must be initialized and valid.
             */
            ConstBidirectionalIterator operator ++(int) { Q_ASSERT(m_pimpl); auto copy = *this; ++*pimpl(); return copy; }

            /*!
             * \brief Prefix decrement operator backtracks the iterator.
             * \return Reference to the iterator at the new position.
             * \pre The iterator must be initialized and valid.
             */
            ConstBidirectionalIterator &operator --() { Q_ASSERT(m_pimpl); --*pimpl(); return *this; }

            /*!
             * \brief Postfix decrement operator backtracks the iterator.
             * \return Copy of the iterator at the old position.
             * \pre The iterator must be initialized and valid.
             */
            ConstBidirectionalIterator operator --(int) { Q_ASSERT(m_pimpl); auto copy = *this; --*pimpl(); return copy; }

            /*!
             * \brief Advance the iterator by a certain amount.
             * \return Reference to the iterator at the new position.
             * \pre The iterator must be initialized and valid.
             */
            ConstBidirectionalIterator operator +=(difference_type n) { Q_ASSERT(m_pimpl); *pimpl() += n; return *this; }

            /*!
             * \brief Advance the iterator by a certain amount.
             * \return Copy of the iterator in its new position.
             * \pre The iterator must be initialized and valid.
             */
            ConstBidirectionalIterator operator +(difference_type n) const { auto copy = *this; return copy += n; }

            /*!
             * \brief Backtrack the iterator by a certain amount.
             * \return Reference to the iterator at the new position.
             * \pre The iterator must be initialized and valid.
             */
            ConstBidirectionalIterator operator -=(difference_type n) { Q_ASSERT(m_pimpl); *pimpl() -= n; return *this; }

            /*!
             * \brief Backtrack the iterator by a certain amount.
             * \return Copy of the iterator in its new position.
             * \pre The iterator must be initialized and valid.
             */
            ConstBidirectionalIterator operator -(difference_type n) const { auto copy = *this; return copy -= n; }

            /*!
             * \brief Return the distance between two iterators.
             * \pre Both iterators must originate from the same collection, and not mix begin/end with cbegin/cend.
             */
            difference_type operator -(const ConstBidirectionalIterator &other) const { Q_ASSERT(m_pimpl && other.m_pimpl); return *pimpl() - *other.pimpl(); }

            /*!
             * \brief Test for equality.
             * \pre Both iterators must originate from the same collection, and not mix begin/end with cbegin/cend.
             */
            bool operator ==(const ConstBidirectionalIterator &other) const { return (pimpl() && other.pimpl()) ? *pimpl() == *other.pimpl() : pimpl() == other.pimpl(); }

            /*!
             * \brief Test for inequality.
             * \pre Both iterators must originate from the same collection, and not mix begin/end with cbegin/cend.
             */
            bool operator !=(const ConstBidirectionalIterator &other) const { return !(*this == other); }

            /*!
             * \brief For sorting.
             * \pre Both iterators must originate from the same collection, and not mix begin/end with cbegin/cend.
             */
            //! @{
            bool operator <(const ConstBidirectionalIterator &other) const { Q_ASSERT(m_pimpl && other.m_pimpl); return *pimpl() < *other.pimpl(); }
            bool operator >(const ConstBidirectionalIterator &other) const { Q_ASSERT(m_pimpl && other.m_pimpl); return *pimpl() > *other.pimpl(); }
            bool operator <=(const ConstBidirectionalIterator &other) const { Q_ASSERT(m_pimpl && other.m_pimpl); return *pimpl() <= *other.pimpl(); }
            bool operator >=(const ConstBidirectionalIterator &other) const { Q_ASSERT(m_pimpl && other.m_pimpl); return *pimpl() >= *other.pimpl(); }
            //! @}

            /*!
             * \brief Return opaque pointer to underlying implementation iterator object.
             * \pre The iterator must have been initialized.
             * \todo Returning by void* is rotten, but GCC gives a very cryptic error if I make it a function template with a cast inside.
             */
            void *getImpl() { return pimpl()->impl(); }

        private:
            class PimplBase
            {
            public:
                virtual ~PimplBase() {}
                virtual PimplBase *clone() const = 0;
                virtual const_reference operator *() const = 0;
                virtual void operator ++() = 0;
                virtual void operator --() = 0;
                virtual void operator +=(difference_type) = 0;
                virtual void operator -=(difference_type) = 0;
                virtual difference_type operator -(const PimplBase &) const = 0;
                virtual bool operator ==(const PimplBase &) const = 0;
                virtual bool operator <(const PimplBase &) const = 0;
                virtual bool operator >(const PimplBase &) const = 0;
                virtual bool operator <=(const PimplBase &) const = 0;
                virtual bool operator >=(const PimplBase &) const = 0;
                virtual void *impl() = 0;
            };

            template <class I> class Pimpl : public PimplBase
            {
            public:
                static_assert(std::is_same<T, typename std::iterator_traits<I>::value_type>::value,
                    "ConstBidirectionalIterator must be initialized from an iterator with the same value_type.");
                Pimpl(I &&i) : m_impl(std::move(i)) {}
                virtual PimplBase *clone() const override { return new Pimpl(*this); }
                virtual const_reference operator *() const override { return *m_impl; }
                virtual void operator ++() override { ++m_impl; }
                virtual void operator --() override { --m_impl; }
                virtual void operator +=(difference_type n) override { m_impl += n; }
                virtual void operator -=(difference_type n) override { m_impl -= n; }
                virtual difference_type operator -(const PimplBase &other) const override { return m_impl - static_cast<const Pimpl&>(other).m_impl; }
                virtual bool operator ==(const PimplBase &other) const override { return m_impl == static_cast<const Pimpl&>(other).m_impl; }
                virtual bool operator <(const PimplBase &other) const override { return m_impl < static_cast<const Pimpl&>(other).m_impl; }
                virtual bool operator >(const PimplBase &other) const override { return m_impl > static_cast<const Pimpl&>(other).m_impl; }
                virtual bool operator <=(const PimplBase &other) const override { return m_impl <= static_cast<const Pimpl&>(other).m_impl; }
                virtual bool operator >=(const PimplBase &other) const override { return m_impl >= static_cast<const Pimpl&>(other).m_impl; }
                virtual void *impl() override { return &m_impl; }
            private:
                I m_impl;
            };

            typedef QScopedPointer<PimplBase> PimplPtr;
            PimplPtr m_pimpl;

            explicit ConstBidirectionalIterator(PimplBase *pimpl) : m_pimpl(pimpl) {} // private ctor used by fromImpl()

            // using these methods to access m_pimpl.data() eases the cognitive burden of correctly forwarding const
            PimplBase *pimpl() { return m_pimpl.data(); }
            const PimplBase *pimpl() const { return m_pimpl.data(); }
        };

        /*!
         * \brief Generic type-erased non-const bidirectional iterator with value semantics.
         * \tparam T the value_type of the container being iterated over.
         *
         * Can take any suitable iterator type as its implementation at runtime.
         */
        template <class T> class BidirectionalIterator
        {
        public:
            //! \brief STL compatibility
            //! @{
            typedef ptrdiff_t difference_type;
            typedef T value_type;
            typedef T *pointer;
            typedef T &reference;
            typedef const T *const_pointer;
            typedef const T &const_reference;
            typedef std::bidirectional_iterator_tag iterator_category;
            //! @}

            //! \brief Default constructor.
            BidirectionalIterator() {}

            /*!
             * \brief Copy constructor.
             */
            BidirectionalIterator(const BidirectionalIterator &other) : m_pimpl(other.pimpl() ? other.pimpl()->clone() : nullptr) {}

            /*!
             * \brief Move constructor.
             */
            BidirectionalIterator(BidirectionalIterator &&other) : m_pimpl(other.m_pimpl.take()) {}

            /*!
             * \brief Copy assignment.
             */
            BidirectionalIterator &operator =(const BidirectionalIterator &other) { m_pimpl.reset(other.pimpl() ? other.pimpl()->clone() : nullptr); return *this; }

            /*!
             * \brief Move assignment.
             */
            BidirectionalIterator &operator =(BidirectionalIterator &&other) { m_pimpl.reset(other.m_pimpl.take()); return *this; }

            /*!
             * \brief Create a new iterator with a specific implementation type.
             * \tparam I Becomes the iterator's implementation type.
             * \param i Initial value for the iterator. The value is copied.
             */
            template <class I> static BidirectionalIterator fromImpl(I i) { return BidirectionalIterator(new Pimpl<I>(std::move(i))); }

            /*!
             * \brief Returns a reference to the object pointed to.
             * \pre The iterator must be initialized and valid.
             */
            const_reference operator *() const { Q_ASSERT(m_pimpl); return **pimpl(); }

            /*!
             * \brief Returns a reference to the object pointed to.
             * \pre The iterator must be initialized and valid.
             */
            reference operator *() { Q_ASSERT(m_pimpl); return **pimpl(); }

            /*!
             * \brief Arrow operator provides access to members of the object pointed to.
             * \pre The iterator must be initialized and valid.
             */
            const_pointer operator ->() const { Q_ASSERT(m_pimpl); return &**pimpl(); }

            /*!
             * \brief Arrow operator provides access to members of the object pointed to.
             * \pre The iterator must be initialized and valid.
             */
            pointer operator ->() { Q_ASSERT(m_pimpl); return &**pimpl(); }

            /*!
             * \brief Prefix increment operator advances the iterator.
             * \return Reference to the iterator at the new position.
             * \pre The iterator must be initialized and valid.
             */
            BidirectionalIterator &operator ++() { Q_ASSERT(m_pimpl); ++*pimpl(); return *this; }

            /*!
             * \brief Postfix increment operator advances the iterator.
             * \return Copy of the iterator in the old position.
             * \pre The iterator must be initialized and valid.
             */
            BidirectionalIterator operator ++(int) { Q_ASSERT(m_pimpl); auto copy = *this; ++*pimpl(); return copy; }

            /*!
             * \brief Prefix decrement operator backtracks the iterator.
             * \return Reference to the iterator at the new position.
             * \pre The iterator must be initialized and valid.
             */
            BidirectionalIterator &operator --() { Q_ASSERT(m_pimpl); --*pimpl(); return *this; }

            /*!
             * \brief Postfix decrement operator backtracks the iterator.
             * \return Copy of the iterator at the old position.
             * \pre The iterator must be initialized and valid.
             */
            BidirectionalIterator operator --(int) { Q_ASSERT(m_pimpl); auto copy = *this; --*pimpl(); return copy; }

            /*!
             * \brief Advance the iterator by a certain amount.
             * \return Reference to the iterator at the new position.
             * \pre The iterator must be initialized and valid.
             */
            BidirectionalIterator operator +=(difference_type n) { Q_ASSERT(m_pimpl); *pimpl() += n; return *this; }

            /*!
             * \brief Advance the iterator by a certain amount.
             * \return Copy of the iterator in its new position.
             * \pre The iterator must be initialized and valid.
             */
            BidirectionalIterator operator +(difference_type n) const { auto copy = *this; return copy += n; }

            /*!
             * \brief Backtrack the iterator by a certain amount.
             * \return Reference to the iterator at the new position.
             * \pre The iterator must be initialized and valid.
             */
            BidirectionalIterator operator -=(difference_type n) { Q_ASSERT(m_pimpl); *pimpl() -= n; return *this; }

            /*!
             * \brief Backtrack the iterator by a certain amount.
             * \return Copy of the iterator in its new position.
             * \pre The iterator must be initialized and valid.
             */
            BidirectionalIterator operator -(difference_type n) const { auto copy = *this; return copy -= n; }

            /*!
             * \brief Return the distance between two iterators.
             * \pre Both iterators must originate from the same collection, and not mix begin/end with cbegin/cend.
             */
            difference_type operator -(const BidirectionalIterator &other) const { Q_ASSERT(m_pimpl && other.m_pimpl); return *pimpl() - *other.pimpl(); }

            /*!
             * \brief Test for equality.
             * \pre Both iterators must originate from the same collection, and not mix begin/end with cbegin/cend.
             */
            bool operator ==(const BidirectionalIterator &other) const { return (pimpl() && other.pimpl()) ? *pimpl() == *other.pimpl() : pimpl() == other.pimpl(); }

            /*!
             * \brief Test for inequality.
             * \pre Both iterators must originate from the same collection, and not mix begin/end with cbegin/cend.
             */
            bool operator !=(const BidirectionalIterator &other) const { return !(*this == other); }

            /*!
             * \brief For sorting.
             * \pre Both iterators must originate from the same collection, and not mix begin/end with cbegin/cend.
             */
            //! @{
            bool operator <(const BidirectionalIterator &other) const { Q_ASSERT(m_pimpl && other.m_pimpl); return *pimpl() < *other.pimpl(); }
            bool operator >(const BidirectionalIterator &other) const { Q_ASSERT(m_pimpl && other.m_pimpl); return *pimpl() > *other.pimpl(); }
            bool operator <=(const BidirectionalIterator &other) const { Q_ASSERT(m_pimpl && other.m_pimpl); return *pimpl() <= *other.pimpl(); }
            bool operator >=(const BidirectionalIterator &other) const { Q_ASSERT(m_pimpl && other.m_pimpl); return *pimpl() >= *other.pimpl(); }
            //! @}

            /*!
             * \brief Return opaque pointer to underlying implementation iterator object.
             * \pre The iterator must have been initialized.
             * \todo Returning by void* is rotten, but GCC gives a very cryptic error if I make it a function template with a cast inside.
             */
            void *getImpl() { return pimpl()->impl(); }

        private:
            class PimplBase
            {
            public:
                virtual ~PimplBase() {}
                virtual PimplBase *clone() const = 0;
                virtual const_reference operator *() const = 0;
                virtual reference operator *() = 0;
                virtual void operator ++() = 0;
                virtual void operator --() = 0;
                virtual void operator +=(difference_type) = 0;
                virtual void operator -=(difference_type) = 0;
                virtual difference_type operator -(const PimplBase &) const = 0;
                virtual bool operator ==(const PimplBase &) const = 0;
                virtual bool operator <(const PimplBase &) const = 0;
                virtual bool operator >(const PimplBase &) const = 0;
                virtual bool operator <=(const PimplBase &) const = 0;
                virtual bool operator >=(const PimplBase &) const = 0;
                virtual void *impl() = 0;
            };

            template <class I> class Pimpl : public PimplBase
            {
            public:
                static_assert(std::is_same<T, typename std::iterator_traits<I>::value_type>::value,
                    "BidirectionalIterator must be initialized from an iterator with the same value_type.");
                Pimpl(I &&i) : m_impl(std::move(i)) {}
                virtual PimplBase *clone() const override { return new Pimpl(*this); }
                virtual const_reference operator *() const override { return *m_impl; }
                virtual reference operator *() override { return *m_impl; }
                virtual void operator ++() override { ++m_impl; }
                virtual void operator --() override { --m_impl; }
                virtual void operator +=(difference_type n) override { m_impl += n; }
                virtual void operator -=(difference_type n) override { m_impl -= n; }
                virtual difference_type operator -(const PimplBase &other) const override { return m_impl - static_cast<const Pimpl&>(other).m_impl; }
                virtual bool operator ==(const PimplBase &other) const override { return m_impl == static_cast<const Pimpl&>(other).m_impl; }
                virtual bool operator <(const PimplBase &other) const override { return m_impl < static_cast<const Pimpl&>(other).m_impl; }
                virtual bool operator >(const PimplBase &other) const override { return m_impl > static_cast<const Pimpl&>(other).m_impl; }
                virtual bool operator <=(const PimplBase &other) const override { return m_impl <= static_cast<const Pimpl&>(other).m_impl; }
                virtual bool operator >=(const PimplBase &other) const override { return m_impl >= static_cast<const Pimpl&>(other).m_impl; }
                virtual void *impl() override { return &m_impl; }
            private:
                I m_impl;
            };

            typedef QScopedPointer<PimplBase> PimplPtr;
            PimplPtr m_pimpl;

            explicit BidirectionalIterator(PimplBase *pimpl) : m_pimpl(pimpl) {} // private ctor used by fromImpl()

            // using these methods to access m_pimpl.data() eases the cognitive burden of correctly forwarding const
            PimplBase *pimpl() { return m_pimpl.data(); }
            const PimplBase *pimpl() const { return m_pimpl.data(); }
        };

    } //namespace Iterators

} //namespace BlackMisc

#endif //BLACKMISC_ITERATOR_H