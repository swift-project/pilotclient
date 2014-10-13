/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_ALGORITHM_H
#define BLACKMISC_ALGORITHM_H

#include <QtGlobal>
#include <algorithm>
#include <iterator>

namespace BlackMisc
{
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
}

#endif
