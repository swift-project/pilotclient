/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_ITERATOR_H
#define BLACKMISC_ITERATOR_H

#include "blackmisc/optional.h"
#include "blackmisc/typetraits.h"
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
        template <class F> class OutputIterator
        {
        public:
            //! Types
            //! @{
            using iterator_category = std::output_iterator_tag;
            using value_type = void;
            using difference_type = void;
            using pointer = void;
            using reference = void;
            //! @}

            //! Constructor
            //! @{
            explicit OutputIterator(const F &func) : m_func(func) {}
            explicit OutputIterator(F &&func) : m_func(std::move(func)) {}
            OutputIterator(const OutputIterator &other) : m_func(other.m_func) {}
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
                new (this) OutputIterator(other);
                return *this;
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
         * Iterator wrapper which applies some transformation function to each element.
         *
         * By creating a CRange from such iterators, it is possible to perform a transformation on a container without copying elements.
         */
        template <class I, class F> class TransformIterator
        {
        public:
            //! Types
            //! @{
            using iterator_category = std::forward_iterator_tag;
            using value_type = std::decay_t<decltype(std::declval<F>()(std::declval<typename std::iterator_traits<I>::value_type>()))>;
            using difference_type = typename std::iterator_traits<I>::difference_type;
            using reference = typename std::iterator_traits<I>::reference;
            //! @}

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
        template <class I, class F> class ConditionalIterator
        {
        public:
            //! Types
            //! @{
            using iterator_category = std::forward_iterator_tag;
            using value_type = typename std::iterator_traits<I>::value_type;
            using difference_type = typename std::iterator_traits<I>::difference_type;
            using pointer = typename std::iterator_traits<I>::pointer;
            using reference = typename std::iterator_traits<I>::reference;
            //! @}

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
        template <class I> class ConcatIterator
        {
        public:
            //! Types
            //! @{
            using iterator_category = std::forward_iterator_tag;
            using value_type = typename std::iterator_traits<I>::value_type;
            using difference_type = typename std::iterator_traits<I>::difference_type;
            using pointer = typename std::iterator_traits<I>::pointer;
            using reference = typename std::iterator_traits<I>::reference;
            //! @}

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

    } //namespace Iterators

} //namespace BlackMisc

#endif //BLACKMISC_ITERATOR_H
