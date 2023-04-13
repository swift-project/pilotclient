/* Copyright (C) 2021
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_MAPBUILDER_H
#define BLACKMISC_MAPBUILDER_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/range.h"
#include <QMap>
#include <QHash>
#include <QList>
#include <algorithm>
#include <iterator>
#include <map>

namespace BlackMisc
{
    /*!
     * Build a QMap more efficiently when calling insert() in a for loop.
     */
    template <typename K, typename V>
    class CMapBuilder
    {
    public:
        //! Add an key/value pair to the map. Runs in amortized constant time.
        template <typename... Ts>
        void insert(Ts &&...keyAndValue)
        {
            m_list.push_back(std::make_pair(std::forward<Ts>(keyAndValue)...));
        }

        //! True if no pairs have been inserted.
        bool isEmpty() const { return m_list.isEmpty(); }

        //! @{
        //! Return a container with unique keys. Runs in log-linear time.
        operator QMap<K, V>() const & { return convertToMap(sortAndDeduplicate(m_list)); }
        operator QMap<K, V>() && { return convertToMap(sortAndDeduplicate(std::move(m_list))); }
        operator QHash<K, V>() const & { return convertTo<QHash>(sortAndDeduplicate(m_list)); }
        operator QHash<K, V>() && { return convertTo<QHash>(sortAndDeduplicate(std::move(m_list))); }
        operator std::map<K, V>() const & { return convertTo<std::map>(sortAndDeduplicate(m_list)); }
        operator std::map<K, V>() && { return convertTo<std::map>(sortAndDeduplicate(std::move(m_list))); }
        //! @}

    private:
        QList<std::pair<K, V>> m_list;

        static QList<std::pair<K, V>> sortAndDeduplicate(QList<std::pair<K, V>> list)
        {
            std::sort(list.begin(), list.end(), [](auto &&a, auto &&b) { return std::less<>()(a.first, b.first); });
            list.erase(std::unique(list.begin(), list.end(), [](auto &&a, auto &&b) { return a.first == b.first; }), list.end());
            return list;
        }

        template <template <typename...> class C>
        static C<K, V> convertTo(QList<std::pair<K, V>> &&list)
        {
            return C<K, V>(std::make_move_iterator(list.begin()), std::make_move_iterator(list.end()));
        }

        static QMap<K, V> convertToMap(QList<std::pair<K, V>> &&list)
        {
            QMap<K, V> map;
            for (auto &pair : makeRange(list).reverse()) { map.insert(map.cbegin(), std::move(pair.first), std::move(pair.second)); }
            return map;
        }
    };
}

#endif
