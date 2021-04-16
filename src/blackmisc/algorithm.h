/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_ALGORITHM_H
#define BLACKMISC_ALGORITHM_H

#include <QtGlobal>
#include <algorithm>
#include <iterator>
#include <utility>
#include <random>
#include <tuple>

namespace BlackMisc
{
    /*!
     * Removes those elements in range 1 that appear also in range 2 leaving only those that
     * do not appear in range 2. Returns an iterator one past the new end of range 1.
     * \pre All the elements of range 2 must be present in the same order in range 1.
     */
    template <typename I, typename J>
    auto removeIfIn(I begin1, I end1, J begin2, J end2)
    {
        auto newEnd = end1;
        std::for_each(begin2, end2, [&](const auto &rm)
        {
            const auto found = std::find(begin1, end1, rm);
            Q_ASSERT(found != end1);
            if (newEnd == end1) { newEnd = found; }
            else { newEnd = std::move(begin1, found, newEnd); }
            begin1 = std::next(found);
        });
        if (newEnd != end1) { newEnd = std::move(begin1, end1, newEnd); }
        return newEnd;
    }

    namespace Private
    {
        //! \private A high quality pseudo-random number generator.
        //! \threadsafe
        inline std::mt19937 &defaultRandomGenerator()
        {
            thread_local std::mt19937 rng(std::random_device{}());
            return rng;
        }
    }

    /*!
     * Use the random number generator rng to choose n elements from the range [in,end) and copy them to out.
     */
    template <typename ForwardIt, typename OutputIt, typename Generator>
    void copyRandomElements(ForwardIt in, ForwardIt end, OutputIt out, int n, Generator &&rng)
    {
        for (auto size = static_cast<int>(std::distance(in, end)); in != end && n > 0; ++in, --size)
        {
            if (std::uniform_int_distribution<>(0, size - 1)(rng) < n)
            {
                *out++ = *in;
                --n;
            }
        }
    }

    /*!
     * Randomly choose n elements from the range [in,end) and copy them to out.
     */
    template <typename ForwardIt, typename OutputIt>
    void copyRandomElements(ForwardIt in, ForwardIt end, OutputIt out, int n)
    {
        copyRandomElements(in, end, out, n, Private::defaultRandomGenerator());
    }

    /*!
     * Split the range [in,end) into n equal chunks and use the random number generator rng to choose one element from each.
     */
    template <typename ForwardIt, typename OutputIt, typename Generator>
    void copySampleElements(ForwardIt in, ForwardIt end, OutputIt out, int n, Generator &&rng)
    {
        const auto size = static_cast<int>(std::distance(in, end));
        for (int i = 0; i < std::min(n, size); ++i)
        {
            const auto index = std::uniform_int_distribution<>(0, std::max(size / n, 1) - 1)(rng);
            std::advance(in, index);
            *out++ = *in;
            std::advance(in, std::max(size / n, 1) - index);
        }
    }

    /*!
     * Split the range [in,end) into n equal chunks and randomly choose one element from each.
     */
    template <typename ForwardIt, typename OutputIt>
    void copySampleElements(ForwardIt in, ForwardIt end, OutputIt out, int n)
    {
        copySampleElements(in, end, out, n, Private::defaultRandomGenerator());
    }

    /*!
     * Topological sorting algorithm.
     *
     * \param begin      Begin iterator of the range to sort.
     * \param end        End iterator of the range to sort.
     * \param comparator A binary function which defines a less-than relation between elements of the range.
     *                   The ordering it induces must be a partial ordering, which is a more relaxed requirement
     *                   than the strict weak ordering required by most standard sorting algorithms.
     *
     * \see https://en.wikipedia.org/wiki/Topological_sorting
     * \see https://en.wikipedia.org/wiki/Partial_ordering
     */
    template <typename I, typename F>
    void topologicalSort(I begin, I end, F comparator)
    {
        using value_type = typename std::iterator_traits<I>::value_type;
        auto part = begin;
        while (part != end)
        {
            auto newPart = std::partition(part, end, [ = ](const value_type &a)
            {
                return std::none_of(part, end, [ =, &a ](const value_type &b)
                {
                    return comparator(b, a);
                });
            });
            Q_ASSERT_X(newPart != part, "BlackMisc::topologicalSort", "Cyclic less-than relation detected (not a partial ordering)");
            part = newPart;
        }
    }

    /*!
     * Insert an element into a sequential container while preserving the topological ordering of the container.
     *
     * \param container  A sequential container.
     * \param value      The value to insert.
     * \param comparator A binary function which defines a less-than relation between elements of the container.
     *                   The ordering it induces must be a partial ordering, which is a more relaxed requirement
     *                   than the strict weak ordering required by most standard sorting algorithms.
     *
     * \see https://en.wikipedia.org/wiki/Topological_sorting
     * \see https://en.wikipedia.org/wiki/Partial_ordering
     */
    template <typename C, typename T, typename F>
    void topologicallySortedInsert(C &container, T &&value, F comparator)
    {
        using value_type = typename C::value_type;
        using reverse = std::reverse_iterator<typename C::iterator>;
        auto rit = std::find_if(reverse(container.end()), reverse(container.begin()), [ =, &value ](const value_type &lhs)
        {
            return comparator(lhs, value);
        });
        Q_ASSERT_X(std::none_of(rit, reverse(container.begin()), [ =, &value ](const value_type &rhs) { return comparator(value, rhs); }),
            "BlackMisc::topologicallySortedInsert", "Cyclic less-than relation detected (not a partial ordering)");
        container.insert(rit.base(), std::forward<T>(value));
    }

    namespace Private
    {
        //! \private
        template <typename T, typename F, size_t... Is>
        void tupleForEachPairImpl(T &&tuple, F &&visitor, std::index_sequence<Is...>)
        {
            // cppcheck-suppress accessForwarded
            (static_cast<void>(std::forward<F>(visitor)(std::get<Is * 2>(std::forward<T>(tuple)), std::get<Is * 2 + 1>(std::forward<T>(tuple)))), ...);
        }
    }

    /*!
     * Invoke a visitor function on each pair of elements of a tuple in order.
     */
    template <typename T, typename F>
    void tupleForEachPair(T &&tuple, F &&visitor)
    {
        using seq = std::make_index_sequence<std::tuple_size<std::decay_t<T>>::value / 2>;
        return Private::tupleForEachPairImpl(std::forward<T>(tuple), std::forward<F>(visitor), seq());
    }
}

#endif
