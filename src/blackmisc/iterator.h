/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_ITERATOR_H
#define BLACKMISC_ITERATOR_H

#include "optional.h"
#include "typetraits.h"
#include <QScopedPointer>
#include <algorithm>
#include <type_traits>
#include <iterator>
#include <utility>
#include <typeindex>

namespace BlackMisc
{
    namespace Iterators
    {
        /*!
         * Configurable output iterator using a provided functor to do the insertion.
         */
        template <class F> class OutputIterator : public std::iterator<std::output_iterator_tag, void, void, void, void>
        {
        public:
            //! Constructor
            //! @{
            explicit OutputIterator(const F &func) : m_func(func) {}
            explicit OutputIterator(F &&func) : m_func(std::move(func)) {}
            //! @}

            //! Advance the iterator (no-op)
            //! @{
            OutputIterator &operator ++() { return *this; }
            OutputIterator operator ++(int) { return *this; }
            //! @}

            //! Dereference (no-op)
            OutputIterator &operator *() { return *this; }

            //! Assignment operator performs the output
            template <typename T, std::enable_if_t<! std::is_convertible<T, OutputIterator>::value, int> = 0>
            OutputIterator &operator =(T &&value) { m_func(std::forward<T>(value)); return *this; }

            //! Copy assignment operator
            OutputIterator &operator =(const OutputIterator &other)
            {
                // Work around lambda's deleted copy assignment operator
                this->~OutputIterator();
                return *new (this) OutputIterator(other);
            }

            //! Destructor.
            ~OutputIterator() = default;

        private:
            F m_func;
        };

        /*!
         * Return an output iterator of type deduced from the argument.
         */
        template <class F> auto makeOutputIterator(F &&func)
        {
            return OutputIterator<std::decay_t<F>>(std::forward<F>(func));
        }

        namespace Private
        {
            //! \private
            template <class T> auto makeInsertIterator(T &container, std::true_type)
            {
                return makeOutputIterator([&container](auto &&v) { container.push_back(std::forward<decltype(v)>(v)); });
            }
            //! \private
            template <class T> auto makeInsertIterator(T &container, std::false_type)
            {
                return makeOutputIterator([&container](auto &&v) { container.insert(std::forward<decltype(v)>(v)); });
            }
        }

        /*!
         * Return an insert iterator appropriate to the container type (uses push_back or insert).
         */
        template <class T> auto makeInsertIterator(T &container)
        {
            return Private::makeInsertIterator(container, THasPushBack<T>());
        }

        /*!
         * Iterator wrapper for Qt's STL-style associative container iterators, when dereferenced return the key instead of the value.
         *
         * By creating a CRange from such iterators, it is possible to create a container of keys without copying them.
         */
        template <class I> class KeyIterator
            : public std::iterator<std::bidirectional_iterator_tag, std::decay_t<decltype(std::declval<I>().key())>>
        {
        public:
            //! Constructor
            KeyIterator(I iterator) : m_iterator(iterator) {}

            //! Advance to the next element.
            //! Undefined if iterator is at the end.
            //! @{
            KeyIterator &operator ++() { ++m_iterator; return *this; }
            KeyIterator operator ++(int) { auto copy = *this; ++m_iterator; return copy; }
            //! @}

            //! Regress to the previous element.
            //! Undefined if iterator is at the beginning.
            //! @{
            KeyIterator &operator --() { --m_iterator; return *this; }
            KeyIterator operator --(int) { auto copy = *this; --m_iterator; return copy; }
            //! @}

            //! Return the value at this iterator position.
            auto value() const { return m_iterator.value(); }

            //! Return the key at this iterator position.
            //! @{
            auto key() const { return m_iterator.key(); }
            auto operator *() const { return key(); }
            //! @}

            //! Indirection operator: pointer to the key at this iterator position.
            auto operator ->() const { return &key(); }

            //! Equality operators.
            //! @{
            bool operator ==(const KeyIterator &other) const { return m_iterator == other.m_iterator; }
            bool operator !=(const KeyIterator &other) const { return m_iterator != other.m_iterator; }
            //! @}

        private:
            I m_iterator;
        };

        /*!
         * Iterator wrapper which applies some transformation function to each element.
         *
         * By creating a CRange from such iterators, it is possible to perform a transformation on a container without copying elements.
         */
        template <class I, class F> class TransformIterator
            : public std::iterator<std::forward_iterator_tag,
                                   std::decay_t<decltype(std::declval<F>()(std::declval<typename std::iterator_traits<I>::value_type>()))>>
        {
        public:
            //! The type returned by the transformation function, which may or may not be a reference.
            using undecayed_type = decltype(std::declval<F>()(std::declval<typename std::iterator_traits<I>::value_type>()));

            //! \private A pointer-like wrapper returned by the arrow operator if the transformation function returns by value.
            struct PointerWrapper
            {
                PointerWrapper(std::decay_t<undecayed_type> *obj) : m_obj(std::move(*obj)) {}
                std::decay_t<undecayed_type> const *operator ->() const { return &m_obj; }
                std::decay_t<undecayed_type> operator *() const & { return m_obj; }
                std::decay_t<undecayed_type> operator *() && { return std::move(m_obj); }
            private:
                const std::decay_t<undecayed_type> m_obj;
            };

            //! The type returned by this iterator's arrow operator, which may be a pointer or a pointer-like wrapper object
            using pointer = typename std::conditional<std::is_reference<undecayed_type>::value,
                                                      std::remove_reference_t<undecayed_type> *,
                                                      PointerWrapper>::type;

            //! Constructor.
            TransformIterator(I iterator, F function) : m_iterator(iterator), m_function(function) {}

            //! Implicit conversion from an end iterator.
            TransformIterator(I end) : m_iterator(end) {}

            //! Advance to the next element.
            //! Undefined if iterator is at the end.
            //! @{
            TransformIterator &operator ++() { ++m_iterator; return *this; }
            TransformIterator operator ++(int) { auto copy = *this; ++m_iterator; return copy; }
            //! @}

            //! Dereference operator, returns the transformed object reference by the iterator.
            //! Undefined if iterator is at the end.
            undecayed_type operator *() { Q_ASSERT(m_function); return (*m_function)(*m_iterator); }

            //! Indirection operator, returns a pointer to the transformed object,
            //! or a pointer-like wrapper object if the transformation function returns by value.
            //! Undefined if iterator is at the end.
            pointer operator ->() { Q_ASSERT(m_function); auto &&obj = (*m_function)(*m_iterator); return &obj; }

            //! Comparison operators.
            //! @{
            bool operator ==(const TransformIterator &other) const { return m_iterator == other.m_iterator; }
            bool operator !=(const TransformIterator &other) const { return m_iterator != other.m_iterator; }
            bool operator <(const TransformIterator &other) const { return m_iterator < other.m_iterator; }
            bool operator <=(const TransformIterator &other) const { return m_iterator <= other.m_iterator; }
            bool operator >(const TransformIterator &other) const { return m_iterator > other.m_iterator; }
            bool operator >=(const TransformIterator &other) const { return m_iterator >= other.m_iterator; }
            //! @}

        private:
            I m_iterator;
            Optional<F> m_function;
        };

        /*!
         * Iterator wrapper which skips over any elements which do not satisfy a given condition predicate.
         *
         * By creating a CRange from such iterators, it is possible to return the results of predicate methods without copying elements.
         */
        template <class I, class F> class ConditionalIterator : public std::iterator<std::forward_iterator_tag, typename std::iterator_traits<I>::value_type>
        {
        public:
            //! Constructor.
            ConditionalIterator(I iterator, I end, F predicate) : m_iterator(iterator), m_end(end), m_predicate(predicate)
            {
                while (m_iterator != m_end && !(*m_predicate)(*m_iterator))
                {
                    ++m_iterator;
                }
            }

            //! Implicit conversion from an end iterator.
            ConditionalIterator(I end) : m_iterator(end), m_end(end) {}

            //! Advance the iterator to the next element which matches the predicate, or the end if there are none remaining.
            //! Undefined if the iterator is already at the end.
            //! @{
            ConditionalIterator &operator ++()
            {
                Q_ASSERT(m_predicate);
                do
                {
                    ++m_iterator;
                } while (m_iterator != m_end && !(*m_predicate)(*m_iterator));
                return *this;
            }
            ConditionalIterator operator ++(int) { auto copy = *this; ++(*this); return copy; }
            //! @}

            //! Indirection operator, returns the underlying iterator.
            //! Undefined if iterator is at the end.
            I operator ->() { return m_iterator; }

            //! Dereference operator, returns the object referenced by the iterator.
            //! Undefined if iterator is at the end.
            typename std::iterator_traits<I>::reference operator *() { return *m_iterator; }

            //! Comparison operators.
            //! @{
            bool operator ==(const ConditionalIterator &other) const { return m_iterator == other.m_iterator; }
            bool operator !=(const ConditionalIterator &other) const { return m_iterator != other.m_iterator; }
            bool operator <(const ConditionalIterator &other) const { return m_iterator < other.m_iterator; }
            bool operator <=(const ConditionalIterator &other) const { return m_iterator <= other.m_iterator; }
            bool operator >(const ConditionalIterator &other) const { return m_iterator > other.m_iterator; }
            bool operator >=(const ConditionalIterator &other) const { return m_iterator >= other.m_iterator; }
            //! @}

            //! \private
            void checkEnd(const ConditionalIterator &other) // debugging
            {
                Q_ASSERT(m_end == other.m_end && m_end == other.m_iterator);
                Q_UNUSED(other);
            }

        private:
            I m_iterator;
            I m_end;
            Optional<F> m_predicate;
        };

        /*!
         * Iterator wrapper which concatenates zero or more pairs of begin and end iterators.
         */
        template <class I> class ConcatIterator : public std::iterator<std::forward_iterator_tag, typename std::iterator_traits<I>::value_type>
        {
        public:
            //! Constructor.
            ConcatIterator(QVector<I> iterators) : m_iterators(std::move(iterators))
            {
                Q_ASSERT(m_iterators.size() % 2 == 0);
                while (!m_iterators.empty() && m_iterators[0] == m_iterators[1]) { m_iterators.remove(0, 2); }
            }

            //! Implicit conversion from an end iterator.
            ConcatIterator(I end) { Q_UNUSED(end); }

            //! Advance to the next element.
            //! Undefined if iterator is at the end.
            //! @{
            ConcatIterator &operator ++()
            {
                ++(m_iterators[0]);
                while (!m_iterators.empty() && m_iterators[0] == m_iterators[1]) { m_iterators.remove(0, 2); }
                return *this;
            }
            ConcatIterator operator ++(int) { auto copy = *this; ++(*this); return copy; }
            //! @}

            //! Indirection operator, returns the underlying iterator.
            //! Undefined if iterator is at the end.
            I operator ->() { return m_iterators[0]; }

            //! Dereference operator, returns the object referenced by the iterator.
            //! Undefined if iterator is at the end.
            typename std::iterator_traits<I>::reference operator *() { return *(m_iterators[0]); }

            //! Comparison operators.
            //! @{
            bool operator ==(const ConcatIterator &other) const { return m_iterators == other.m_iterators; }
            bool operator !=(const ConcatIterator &other) const { return m_iterators != other.m_iterators; }
            bool operator <(const ConcatIterator &other) const { return m_iterators < other.m_iterators; }
            bool operator <=(const ConcatIterator &other) const { return m_iterators <= other.m_iterators; }
            bool operator >(const ConcatIterator &other) const { return m_iterators > other.m_iterators; }
            bool operator >=(const ConcatIterator &other) const { return m_iterators >= other.m_iterators; }
            //! @}

        private:
            QVector<I> m_iterators;
        };

        /*!
         * Construct a KeyIterator of the appropriate type from deduced template function argument.
         */
        template <class I> auto makeKeyIterator(I iterator) -> KeyIterator<I>
        {
            return { iterator };
        }

        /*!
         * Construct a TransformIterator of the appropriate type from deduced template function arguments.
         */
        template <class I, class F> auto makeTransformIterator(I iterator, F function) -> TransformIterator<I, F>
        {
            return { iterator, function };
        }

        /*!
         * Construct a ConditionalIterator of the appropriate type from deduced template function arguments.
         */
        template <class I, class F> auto makeConditionalIterator(I iterator, I end, F predicate) -> ConditionalIterator<I, F>
        {
            return { iterator, end, predicate };
        }

        /*!
         * Construct a ConcatIterator of the appropriate type from deduced template function arguments.
         */
        template <class I> auto makeConcatIterator(QVector<I> iterators) -> ConcatIterator<I>
        {
            return { std::move(iterators) };
        }

        /*!
         * Generic type-erased const forward iterator with value semantics.
         * \tparam T the value_type of the container being iterated over.
         *
         * Can take any suitable iterator type as its implementation at runtime.
         */
        template <class T> class ConstForwardIterator
        {
        public:
            //! STL compatibility
            //! @{
            typedef ptrdiff_t difference_type;
            typedef T value_type;
            typedef const T *pointer;
            typedef const T &reference;
            typedef const T *const_pointer;
            typedef const T &const_reference;
            typedef std::forward_iterator_tag iterator_category;
            //! @}

            //! Default constructor.
            ConstForwardIterator() {}

            //! Copy constructor.
            ConstForwardIterator(const ConstForwardIterator &other) : m_pimpl(other.pimpl() ? other.pimpl()->clone() : nullptr) {}

            //! Move constructor.
            ConstForwardIterator(ConstForwardIterator &&other) noexcept : m_pimpl(other.m_pimpl.take()) {}

            //! Copy assignment.
            ConstForwardIterator &operator =(const ConstForwardIterator &other) { m_pimpl.reset(other.pimpl() ? other.pimpl()->clone() : nullptr); return *this; }

            //! Move assignment.
            ConstForwardIterator &operator =(ConstForwardIterator &&other) noexcept { m_pimpl.reset(other.m_pimpl.take()); return *this; }

            //! Destructor.
            ~ConstForwardIterator() = default;

            //! Create a new iterator with a specific implementation type.
            //! \tparam I Becomes the iterator's implementation type.
            //! \param i Initial value for the iterator. The value is copied.
            template <class I> static ConstForwardIterator fromImpl(I i) { return ConstForwardIterator(new Pimpl<I>(std::move(i))); }

            //! Returns a reference to the object pointed to.
            //! \pre The iterator must be initialized and valid.
            const_reference operator *() const { Q_ASSERT(m_pimpl); return **pimpl(); }

            //! Arrow operator provides access to members of the object pointed to.
            //! \pre The iterator must be initialized and valid.
            const_pointer operator ->() const { Q_ASSERT(m_pimpl); return &**pimpl(); }

            //! Prefix increment operator advances the iterator.
            //! \return Reference to the iterator at the new position.
            //! \pre The iterator must be initialized and valid.
            ConstForwardIterator &operator ++() { Q_ASSERT(m_pimpl); ++*pimpl(); return *this; }

            //! Postfix increment operator advances the iterator.
            //! \return Copy of the iterator in the old position.
            //! \pre The iterator must be initialized and valid.
            ConstForwardIterator operator ++(int) { Q_ASSERT(m_pimpl); auto copy = *this; ++*pimpl(); return copy; }

            //! Advance the iterator by a certain amount.
            //! \return Reference to the iterator at the new position.
            //! \pre The iterator must be initialized and valid.
            ConstForwardIterator operator +=(difference_type n) { Q_ASSERT(m_pimpl); *pimpl() += n; return *this; }

            //! Advance the iterator by a certain amount.
            //! \return Copy of the iterator in its new position.
            //! \pre The iterator must be initialized and valid.
            ConstForwardIterator operator +(difference_type n) const { auto copy = *this; return copy += n; }

            //! Test for equality.
            //! \pre Both iterators must originate from the same collection, and not mix begin/end with cbegin/cend.
            bool operator ==(const ConstForwardIterator &other) const { return (pimpl() && other.pimpl()) ? *pimpl() == *other.pimpl() : pimpl() == other.pimpl(); }

            //! Test for inequality.
            //! \pre Both iterators must originate from the same collection, and not mix begin/end with cbegin/cend.
            bool operator !=(const ConstForwardIterator &other) const { return !(*this == other); }

            //! Return opaque pointer to underlying implementation iterator object.
            //! \pre The iterator must have been initialized.
            template <typename U> U &getImpl() { pimpl()->assertType(typeid(std::decay_t<U>)); return *static_cast<U*>(pimpl()->impl()); }

        private:
            class PimplBase
            {
            public:
                PimplBase() {}
                PimplBase(const PimplBase &) = default;
                PimplBase &operator =(const PimplBase &) = delete;
                virtual ~PimplBase() {}
                virtual PimplBase *clone() const = 0;
                virtual const_reference operator *() const = 0;
                virtual void operator ++() = 0;
                virtual void operator +=(difference_type) = 0;
                virtual bool operator ==(const PimplBase &) const = 0;
                virtual void *impl() = 0;
                virtual void assertType(std::type_index) const = 0;
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
                virtual void assertType(std::type_index ti) const override { Q_ASSERT(ti == typeid(I)); Q_UNUSED(ti); }
            private:
                I m_impl;
            };

            using PimplPtr = QScopedPointer<PimplBase>;
            PimplPtr m_pimpl;

            explicit ConstForwardIterator(PimplBase *pimpl) : m_pimpl(pimpl) {} // private ctor used by fromImpl()

            // using these methods to access m_pimpl.data() eases the cognitive burden of correctly forwarding const
            PimplBase *pimpl() { return m_pimpl.data(); }
            const PimplBase *pimpl() const { return m_pimpl.data(); }
        };

        /*!
         * Generic type-erased const random access iterator with value semantics.
         * \tparam T the value_type of the container being iterated over.
         *
         * Can take any suitable iterator type as its implementation at runtime.
         */
        template <class T> class ConstRandomAccessIterator
        {
        public:
            //! STL compatibility
            //! @{
            typedef ptrdiff_t difference_type;
            typedef T value_type;
            typedef const T *pointer;
            typedef const T &reference;
            typedef const T *const_pointer;
            typedef const T &const_reference;
            typedef std::random_access_iterator_tag iterator_category;
            //! @}

            //! Default constructor.
            ConstRandomAccessIterator() {}

            //! Copy constructor.
            ConstRandomAccessIterator(const ConstRandomAccessIterator &other) : m_pimpl(other.pimpl() ? other.pimpl()->clone() : nullptr) {}

            //! Move constructor.
            ConstRandomAccessIterator(ConstRandomAccessIterator &&other) noexcept : m_pimpl(other.m_pimpl.take()) {}

            //! Copy assignment.
            ConstRandomAccessIterator &operator =(const ConstRandomAccessIterator &other) { m_pimpl.reset(other.pimpl() ? other.pimpl()->clone() : nullptr); return *this; }

            //! Move assignment.
            ConstRandomAccessIterator &operator =(ConstRandomAccessIterator &&other) noexcept { m_pimpl.reset(other.m_pimpl.take()); return *this; }

            //! Destructor.
            ~ConstRandomAccessIterator() = default;

            //! Create a new iterator with a specific implementation type.
            //! \tparam I Becomes the iterator's implementation type.
            //! \param i Initial value for the iterator. The value is copied.
            template <class I> static ConstRandomAccessIterator fromImpl(I i) { return ConstRandomAccessIterator(new Pimpl<I>(std::move(i))); }

            //! Returns a reference to the object pointed to.
            //! \pre The iterator must be initialized and valid.
            const_reference operator *() const { Q_ASSERT(m_pimpl); return **pimpl(); }

            //! Arrow operator provides access to members of the object pointed to.
            //! \pre The iterator must be initialized and valid.
            const_pointer operator ->() const { Q_ASSERT(m_pimpl); return &**pimpl(); }

            //! Prefix increment operator advances the iterator.
            //! \return Reference to the iterator at the new position.
            //! \pre The iterator must be initialized and valid.
            ConstRandomAccessIterator &operator ++() { Q_ASSERT(m_pimpl); ++*pimpl(); return *this; }

            //! Postfix increment operator advances the iterator.
            //! \return Copy of the iterator in the old position.
            //! \pre The iterator must be initialized and valid.
            ConstRandomAccessIterator operator ++(int) { Q_ASSERT(m_pimpl); auto copy = *this; ++*pimpl(); return copy; }

            //! Prefix decrement operator backtracks the iterator.
            //! \return Reference to the iterator at the new position.
            //! \pre The iterator must be initialized and valid.
            ConstRandomAccessIterator &operator --() { Q_ASSERT(m_pimpl); --*pimpl(); return *this; }

            //! Postfix decrement operator backtracks the iterator.
            //! \return Copy of the iterator at the old position.
            //! \pre The iterator must be initialized and valid.
            ConstRandomAccessIterator operator --(int) { Q_ASSERT(m_pimpl); auto copy = *this; --*pimpl(); return copy; }

            //! Advance the iterator by a certain amount.
            //! \return Reference to the iterator at the new position.
            //! \pre The iterator must be initialized and valid.
            ConstRandomAccessIterator operator +=(difference_type n) { Q_ASSERT(m_pimpl); *pimpl() += n; return *this; }

            //! Advance the iterator by a certain amount.
            //! \return Copy of the iterator in its new position.
            //! \pre The iterator must be initialized and valid.
            //! @{
            friend ConstRandomAccessIterator operator +(const ConstRandomAccessIterator &i, difference_type n) { auto copy = i; return copy += n; }
            friend ConstRandomAccessIterator operator +(difference_type n, const ConstRandomAccessIterator &i) { auto copy = i; return copy += n; }
            //! @}

            //! Backtrack the iterator by a certain amount.
            //! \return Reference to the iterator at the new position.
            //! \pre The iterator must be initialized and valid.
            ConstRandomAccessIterator operator -=(difference_type n) { Q_ASSERT(m_pimpl); *pimpl() -= n; return *this; }

            //! Backtrack the iterator by a certain amount.
            //! \return Copy of the iterator in its new position.
            //! \pre The iterator must be initialized and valid.
            ConstRandomAccessIterator operator -(difference_type n) const { auto copy = *this; return copy -= n; }

            //! Return the distance between two iterators.
            //! \pre Both iterators must originate from the same collection, and not mix begin/end with cbegin/cend.
            difference_type operator -(const ConstRandomAccessIterator &other) const { Q_ASSERT(m_pimpl && other.m_pimpl); return *pimpl() - *other.pimpl(); }

            //! Test for equality.
            //! \pre Both iterators must originate from the same collection, and not mix begin/end with cbegin/cend.
            bool operator ==(const ConstRandomAccessIterator &other) const { return (pimpl() && other.pimpl()) ? *pimpl() == *other.pimpl() : pimpl() == other.pimpl(); }

            //! Test for inequality.
            //! \pre Both iterators must originate from the same collection, and not mix begin/end with cbegin/cend.
            bool operator !=(const ConstRandomAccessIterator &other) const { return !(*this == other); }

            //! For sorting.
            //! \pre Both iterators must originate from the same collection, and not mix begin/end with cbegin/cend.
            //! @{
            bool operator <(const ConstRandomAccessIterator &other) const { Q_ASSERT(m_pimpl && other.m_pimpl); return *pimpl() < *other.pimpl(); }
            bool operator >(const ConstRandomAccessIterator &other) const { Q_ASSERT(m_pimpl && other.m_pimpl); return *pimpl() > *other.pimpl(); }
            bool operator <=(const ConstRandomAccessIterator &other) const { Q_ASSERT(m_pimpl && other.m_pimpl); return *pimpl() <= *other.pimpl(); }
            bool operator >=(const ConstRandomAccessIterator &other) const { Q_ASSERT(m_pimpl && other.m_pimpl); return *pimpl() >= *other.pimpl(); }
            //! @}

            //! Subscript operator.
            //! \pre `(*this + n)` must be dereferenceable.
            reference operator [](difference_type n) const { return *(*this + n); }

            //! Return opaque pointer to underlying implementation iterator object.
            //! \pre The iterator must have been initialized.
            template <typename U> U &getImpl() { pimpl()->assertType(typeid(std::decay_t<U>)); return *static_cast<U*>(pimpl()->impl()); }

        private:
            class PimplBase
            {
            public:
                PimplBase() {}
                PimplBase(const PimplBase &) = default;
                PimplBase &operator =(const PimplBase &) = delete;
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
                virtual void assertType(std::type_index) const = 0;
            };

            template <class I> class Pimpl : public PimplBase
            {
            public:
                static_assert(std::is_same<T, typename std::iterator_traits<I>::value_type>::value,
                    "ConstRandomAccessIterator must be initialized from an iterator with the same value_type.");
                static_assert(std::is_same<typename std::iterator_traits<I>::iterator_category, std::random_access_iterator_tag>::value,
                    "ConstRandomAccessIterator must be initialized from a random access iterator.");
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
                virtual void assertType(std::type_index ti) const override { Q_ASSERT(ti == typeid(I)); Q_UNUSED(ti); }
            private:
                I m_impl;
            };

            using PimplPtr = QScopedPointer<PimplBase>;
            PimplPtr m_pimpl;

            explicit ConstRandomAccessIterator(PimplBase *pimpl) : m_pimpl(pimpl) {} // private ctor used by fromImpl()

            // using these methods to access m_pimpl.data() eases the cognitive burden of correctly forwarding const
            PimplBase *pimpl() { return m_pimpl.data(); }
            const PimplBase *pimpl() const { return m_pimpl.data(); }
        };

        /*!
         * Generic type-erased non-const bidirectional iterator with value semantics.
         * \tparam T the value_type of the container being iterated over.
         *
         * Can take any suitable iterator type as its implementation at runtime.
         */
        template <class T> class RandomAccessIterator
        {
        public:
            //! STL compatibility
            //! @{
            typedef ptrdiff_t difference_type;
            typedef T value_type;
            typedef T *pointer;
            typedef T &reference;
            typedef const T *const_pointer;
            typedef const T &const_reference;
            typedef std::random_access_iterator_tag iterator_category;
            //! @}

            //! Default constructor.
            RandomAccessIterator() {}

            //! Copy constructor.
            RandomAccessIterator(const RandomAccessIterator &other) : m_pimpl(other.pimpl() ? other.pimpl()->clone() : nullptr) {}

            //! Move constructor.
            RandomAccessIterator(RandomAccessIterator &&other) noexcept : m_pimpl(other.m_pimpl.take()) {}

            //! Copy assignment.
            RandomAccessIterator &operator =(const RandomAccessIterator &other) { m_pimpl.reset(other.pimpl() ? other.pimpl()->clone() : nullptr); return *this; }

            //! Move assignment.
            RandomAccessIterator &operator =(RandomAccessIterator &&other) noexcept { m_pimpl.reset(other.m_pimpl.take()); return *this; }

            //! Destructor.
            ~RandomAccessIterator() = default;

            //! Create a new iterator with a specific implementation type.
            //! \tparam I Becomes the iterator's implementation type.
            //! \param i Initial value for the iterator. The value is copied.
            template <class I> static RandomAccessIterator fromImpl(I i) { return RandomAccessIterator(new Pimpl<I>(std::move(i))); }

            //! Returns a reference to the object pointed to.
            //! \pre The iterator must be initialized and valid.
            const_reference operator *() const { Q_ASSERT(m_pimpl); return **pimpl(); }

            //! Returns a reference to the object pointed to.
            //! \pre The iterator must be initialized and valid.
            reference operator *() { Q_ASSERT(m_pimpl); return **pimpl(); }

            //! Arrow operator provides access to members of the object pointed to.
            //! \pre The iterator must be initialized and valid.
            const_pointer operator ->() const { Q_ASSERT(m_pimpl); return &**pimpl(); }

            //! Arrow operator provides access to members of the object pointed to.
            //! \pre The iterator must be initialized and valid.
            pointer operator ->() { Q_ASSERT(m_pimpl); return &**pimpl(); }

            //! Prefix increment operator advances the iterator.
            //! \return Reference to the iterator at the new position.
            //! \pre The iterator must be initialized and valid.
            RandomAccessIterator &operator ++() { Q_ASSERT(m_pimpl); ++*pimpl(); return *this; }

            //! Postfix increment operator advances the iterator.
            //! \return Copy of the iterator in the old position.
            //! \pre The iterator must be initialized and valid.
            RandomAccessIterator operator ++(int) { Q_ASSERT(m_pimpl); auto copy = *this; ++*pimpl(); return copy; }

            //! Prefix decrement operator backtracks the iterator.
            //! \return Reference to the iterator at the new position.
            //! \pre The iterator must be initialized and valid.
            RandomAccessIterator &operator --() { Q_ASSERT(m_pimpl); --*pimpl(); return *this; }

            //! Postfix decrement operator backtracks the iterator.
            //! \return Copy of the iterator at the old position.
            //! \pre The iterator must be initialized and valid.
            RandomAccessIterator operator --(int) { Q_ASSERT(m_pimpl); auto copy = *this; --*pimpl(); return copy; }

            //! Advance the iterator by a certain amount.
            //! \return Reference to the iterator at the new position.
            //! \pre The iterator must be initialized and valid.
            RandomAccessIterator operator +=(difference_type n) { Q_ASSERT(m_pimpl); *pimpl() += n; return *this; }

            //! Advance the iterator by a certain amount.
            //! \return Copy of the iterator in its new position.
            //! \pre The iterator must be initialized and valid.
            //! @{
            friend RandomAccessIterator operator +(const RandomAccessIterator &i, difference_type n) { auto copy = i; return copy += n; }
            friend RandomAccessIterator operator +(difference_type n, const RandomAccessIterator &i) { auto copy = i; return copy += n; }
            //! @}

            //! Backtrack the iterator by a certain amount.
            //! \return Reference to the iterator at the new position.
            //! \pre The iterator must be initialized and valid.
            RandomAccessIterator operator -=(difference_type n) { Q_ASSERT(m_pimpl); *pimpl() -= n; return *this; }

            //! Backtrack the iterator by a certain amount.
            //! \return Copy of the iterator in its new position.
            //! \pre The iterator must be initialized and valid.
            RandomAccessIterator operator -(difference_type n) const { auto copy = *this; return copy -= n; }

            //! Return the distance between two iterators.
            //! \pre Both iterators must originate from the same collection, and not mix begin/end with cbegin/cend.
            difference_type operator -(const RandomAccessIterator &other) const { Q_ASSERT(m_pimpl && other.m_pimpl); return *pimpl() - *other.pimpl(); }

            //! Test for equality.
            //! \pre Both iterators must originate from the same collection, and not mix begin/end with cbegin/cend.
            bool operator ==(const RandomAccessIterator &other) const { return (pimpl() && other.pimpl()) ? *pimpl() == *other.pimpl() : pimpl() == other.pimpl(); }

            //! Test for inequality.
            //! \pre Both iterators must originate from the same collection, and not mix begin/end with cbegin/cend.
            bool operator !=(const RandomAccessIterator &other) const { return !(*this == other); }

            //! For sorting.
            //! \pre Both iterators must originate from the same collection, and not mix begin/end with cbegin/cend.
            //! @{
            bool operator <(const RandomAccessIterator &other) const { Q_ASSERT(m_pimpl && other.m_pimpl); return *pimpl() < *other.pimpl(); }
            bool operator >(const RandomAccessIterator &other) const { Q_ASSERT(m_pimpl && other.m_pimpl); return *pimpl() > *other.pimpl(); }
            bool operator <=(const RandomAccessIterator &other) const { Q_ASSERT(m_pimpl && other.m_pimpl); return *pimpl() <= *other.pimpl(); }
            bool operator >=(const RandomAccessIterator &other) const { Q_ASSERT(m_pimpl && other.m_pimpl); return *pimpl() >= *other.pimpl(); }
            //! @}

            //! Subscript operator.
            //! \pre `(*this + n)` must be dereferenceable.
            reference operator [](difference_type n) const { return *(*this + n); }

            //! Return opaque pointer to underlying implementation iterator object.
            //! \pre The iterator must have been initialized.
            template <typename U> U &getImpl() { pimpl()->assertType(typeid(std::decay_t<U>)); return *static_cast<U*>(pimpl()->impl()); }

        private:
            class PimplBase
            {
            public:
                PimplBase() {}
                PimplBase(const PimplBase &) = default;
                PimplBase &operator =(const PimplBase &) = delete;
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
                virtual void assertType(std::type_index) const = 0;
            };

            template <class I> class Pimpl : public PimplBase
            {
            public:
                static_assert(std::is_same<T, typename std::iterator_traits<I>::value_type>::value,
                    "RandomAccessIterator must be initialized from an iterator with the same value_type.");
                static_assert(std::is_same<typename std::iterator_traits<I>::iterator_category, std::random_access_iterator_tag>::value,
                    "RandomAccessIterator must be initialized from a random access iterator.");
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
                virtual void assertType(std::type_index ti) const override { Q_ASSERT(ti == typeid(I)); Q_UNUSED(ti); }
            private:
                I m_impl;
            };

            using PimplPtr = QScopedPointer<PimplBase>;
            PimplPtr m_pimpl;

            explicit RandomAccessIterator(PimplBase *pimpl) : m_pimpl(pimpl) {} // private ctor used by fromImpl()

            // using these methods to access m_pimpl.data() eases the cognitive burden of correctly forwarding const
            PimplBase *pimpl() { return m_pimpl.data(); }
            const PimplBase *pimpl() const { return m_pimpl.data(); }
        };

    } //namespace Iterators

} //namespace BlackMisc

#endif //BLACKMISC_ITERATOR_H
