/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_MEMOTABLE_H
#define BLACKMISC_MEMOTABLE_H

#include "blackmisc/dictionary.h"
#include "blackmisc/sequence.h"

namespace BlackMisc
{
    /*!
     * A data memoization pattern useful for compressing JSON representations of containers.
     */
    template <typename T>
    class CMemoTable
    {
    public:
        //! Return the index of a value, inserting it if it is not already in the table.
        int getIndex(const T &value)
        {
            int &index = m_dict[value];
            if (! index)
            {
                m_list.push_back(value);
                index = m_list.size();
            }
            return index - 1;
        }

        //! Return the values in the table as a flat list.
        const CSequence<T> &getTable() const
        {
            return m_list;
        }

    private:
        CSequence<T> m_list;
        CDictionary<T, int, QMap> m_dict;
    };
}

#endif
