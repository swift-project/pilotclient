// SPDX-FileCopyrightText: Copyright (C) 2021 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_SETBUILDER_H
#define BLACKMISC_SETBUILDER_H

#include "blackmisc/blackmiscexport.h"
#include <QSet>
#include <QList>
#include <QVector>
#include <QStringList>
#include <algorithm>
#include <iterator>
#include <vector>
#include <set>

namespace BlackMisc
{
    /*!
     * Build a QSet more efficiently when calling insert() in a for loop.
     */
    template <typename T>
    class CSetBuilder
    {
    public:
        //! @{
        //! Add an element to the set. Runs in amortized constant time.
        void insert(const T &value) { m_list.push_back(value); }
        void insert(T &&value) { m_list.push_back(std::move(value)); }
        //! @}

        //! True if no elements have been inserted.
        bool isEmpty() const { return m_list.isEmpty(); }

        //! @{
        //! Return a container of unique elements. Runs in log-linear time.
        operator QList<T>() const & { return sortAndDeduplicate(m_list); }
        operator QList<T>() && { return sortAndDeduplicate(std::move(m_list)); }
        operator QSet<T>() const & { return convertTo<QSet>(sortAndDeduplicate(m_list)); }
        operator QSet<T>() && { return convertTo<QSet>(sortAndDeduplicate(std::move(m_list))); }
        operator std::set<T>() const & { return convertTo<std::set>(sortAndDeduplicate(m_list)); }
        operator std::set<T>() && { return convertTo<std::set>(sortAndDeduplicate(std::move(m_list))); }
        operator QVector<T>() const & { return convertTo<QVector>(sortAndDeduplicate(m_list)); }
        operator QVector<T>() && { return convertTo<QVector>(sortAndDeduplicate(std::move(m_list))); }
        operator std::vector<T>() const & { return convertTo<std::vector>(sortAndDeduplicate(m_list)); }
        operator std::vector<T>() && { return convertTo<std::vector>(sortAndDeduplicate(std::move(m_list))); }
        template <typename U = T, typename = std::enable_if_t<std::is_same_v<U, QString>>>
        operator QStringList() const &
        {
            return sortAndDeduplicate(m_list);
        }
        template <typename U = T, typename = std::enable_if_t<std::is_same_v<U, QString>>>
        operator QStringList() &&
        {
            return sortAndDeduplicate(std::move(m_list));
        }
        //! @}

    private:
        QList<T> m_list;

        static QList<T> sortAndDeduplicate(QList<T> list)
        {
            std::sort(list.begin(), list.end());
            list.erase(std::unique(list.begin(), list.end()), list.end());
            return list;
        }

        template <template <typename...> class C>
        static C<T> convertTo(QList<T> &&list)
        {
            return C<T>(std::make_move_iterator(list.begin()), std::make_move_iterator(list.end()));
        }
    };
}

#endif
