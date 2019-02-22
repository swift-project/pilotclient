/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
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

    /*!
     * Helper class for memoizing members of a value object.
     */
    template <typename... Ts>
    struct CMemoHelper
    {
        //! Memoizer for Ts. Other types are passed through.
        class CMemoizer : private CMemoTable<Ts>...
        {
        public:
            //! If T is in Ts, return the index of member in the memo table.
            template <typename T, std::enable_if_t<TIsOneOf<T, Ts...>::value, int> = 0>
            int maybeMemoize(const T &member) { return this->CMemoTable<T>::getIndex(member); }

            //! If T is not in Ts, return member.
            template <typename T, std::enable_if_t<! TIsOneOf<T, Ts...>::value, int> = 0>
            const T &maybeMemoize(const T &member) { return member; }

            //! Return the values in the T table as a flat list.
            template <typename T>
            const CSequence<T> &getTable() const { return this->CMemoTable<T>::getTable(); }
        };

        //! Unmemoizer for Ts. Other types are passed through.
        class CUnmemoizer : private CSequence<Ts>...
        {
        public:
            //! Return reference to the flat list T table.
            template <typename T>
            CSequence<T> &getTable() { return *this; }

            //! If T is in Ts, return proxy that will assign to member through the value at the given index in the flat list.
            template <typename T, std::enable_if_t<TIsOneOf<T, Ts...>::value, int> = 0>
            auto maybeUnmemoize(T &member) const
            {
                struct Memo // clazy:exclude=rule-of-three
                {
                    int index;
                    T &member;
                    const CSequence<T> &list;
                    int &get() { return index; }
                    ~Memo() { if (index >= 0) { member = list[index]; } }
                };
                return Memo { -1, member, static_cast<const CSequence<T> &>(*this) };
            }

            //! If T is not in Ts, return member as std::reference_wrapper.
            template <typename T, std::enable_if_t<! TIsOneOf<T, Ts...>::value, int> = 0>
            auto maybeUnmemoize(T &member) const { return std::ref(member); }
        };
    };
}

#endif
