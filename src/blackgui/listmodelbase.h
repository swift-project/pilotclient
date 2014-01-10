/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*!
    \file
*/

#ifndef BLACKGUI_LISTMODELBASE_H
#define BLACKGUI_LISTMODELBASE_H

#include "blackgui/columns.h"
#include <QAbstractItemModel>

namespace BlackGui
{
    /*!
     * \brief List model
     */
    template <typename ObjectType, typename ListType> class CListModelBase : public QAbstractListModel
    {

    protected:
        ListType m_list;
        CColumns m_columns;
        int m_sortedColumn;
        Qt::SortOrder m_sortOrder;

        /*!
         * \brief Constructor
         * \param translationContext
         * \param parent
         */
        CListModelBase(const QString &translationContext, QObject *parent = nullptr)
            : QAbstractListModel(parent), m_columns(translationContext), m_sortedColumn(-1), m_sortOrder(Qt::AscendingOrder)
        {
            // void
        }

        /*!
         * \brief Sort list by given order
         * \param list
         * \param column
         * \param order
         * \return
         */
        ListType sortListByColumn(const ListType &list, int column, Qt::SortOrder order);

    public:

        /*!
         * \brief Destructor
         */
        virtual ~CListModelBase() {}

        /*!
         * \brief Column count
         * \return
         */
        virtual int columnCount(const QModelIndex &modelIndex) const;

        /*!
         * \brief Header data
         * \param section
         * \param orientation
         * \param role
         * \return
         */
        virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;

        /*!
         * \brief Column to property index
         * \param column
         */
        virtual int columnToPropertyIndex(int column) const;

        /*!
         * \brief Set column for sort
         * \param column
         */
        virtual void setSortColumn(int column)
        {
            this->m_sortedColumn = column;
        }

        /*!
         * \brief Set column for sort
         * \param
         */
        virtual void setSortColumnByPropertyIndex(int propertyIndex)
        {
            this->m_sortedColumn = this->m_columns.propertyIndexToColumn(propertyIndex);
        }

        /*!
         * \brief Get sort column
         * \return
         */
        virtual int getSortColumn() const
        {
            return this->m_sortedColumn;
        }

        /*!
         * \brief Has valid sort column
         * \return
         */
        virtual bool hasValidSortColumn() const
        {
            return this->m_sortedColumn >= 0 && this->m_sortedColumn < this->m_columns.size();
        }

        /*!
         * \brief Get sort column
         * \return
         */
        virtual Qt::SortOrder getSortOrder() const
        {
            return this->m_sortOrder;
        }

        /*!
         * \brief data
         * \param index
         * \param role
         * \return
         */
        virtual QVariant data(const QModelIndex &index, int role) const;

        /*!
         * \brief Row count
         * \return
         */
        virtual int rowCount(const QModelIndex &index = QModelIndex()) const;

        /*!
         * \brief Update
         * \param list
         * \return
         */
        virtual int update(const ListType &list);

        /*!
         * \brief Object at row position
         * \param index
         * \return
         */
        virtual const ObjectType &at(const QModelIndex &index) const
        {
            if (index.row() < 0 || index.row() >= this->m_list.size())
            {
                const static ObjectType def;
                return def;
            }
            else
            {
                return this->m_list[index.row()];
            }
        }

        /*!
         * \brief sort
         * \param column
         * \param order
         */
        virtual void sort(int column, Qt::SortOrder order);

    };
}
#endif // guard
