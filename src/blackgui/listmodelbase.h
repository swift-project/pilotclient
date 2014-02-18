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
    template <typename ObjectType, typename ContainerType> class CListModelBase : public QAbstractListModel
    {

    protected:
        ContainerType m_container;    //!< used container
        CColumns m_columns; //!< columns metadata
        int m_sortedColumn; //!< current sort column
        Qt::SortOrder m_sortOrder; //!< sort order (asc/desc)

        /*!
         * \brief Constructor
         * \param translationContext    I18N context
         * \param parent
         */
        CListModelBase(const QString &translationContext, QObject *parent = nullptr)
            : QAbstractListModel(parent), m_columns(translationContext), m_sortedColumn(-1), m_sortOrder(Qt::AscendingOrder)
        {
            // void
        }

        /*!
         * \brief Sort list by given order
         * \param list      used list
         * \param column    column inder
         * \param order     sort order (asccending / descending)
         * \return
         */
        ContainerType sortListByColumn(const ContainerType &list, int column, Qt::SortOrder order);

    public:

        /*!
         * \brief Destructor
         */
        virtual ~CListModelBase() {}

        /*!
         * \copydoc QAbstractListModel::columnCount()
         */
        virtual int columnCount(const QModelIndex &modelIndex) const;

        /*!
         * \copydoc QAbstractItemModel::headerData()
         */
        virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;

        /*!
         * \brief Column to property index
         * \param column
         */
        virtual int columnToPropertyIndex(int column) const;

        /*!
         * \brief Index to property index
         * \param index
         */
        virtual int indexToPropertyIndex(const QModelIndex &index) const {
            return this->columnToPropertyIndex(index.column());
        }

        /*!
         * \brief Set sort column
         * \param column column index
         */
        virtual void setSortColumn(int column)
        {
            this->m_sortedColumn = column;
        }

        /*!
         * \brief Set column for sort
         * \param propertyIndex index of column to be sorted by
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
         * \brief Has valid sort column?
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

        //! \brief Used container data
        virtual const ContainerType &getContainer() const
        {
            return this->m_container;
        }

        //! \copydoc QAbstractListModel::data()
        virtual QVariant data(const QModelIndex &index, int role) const;

        //! \copydoc QAbstractListModel::rowCount()
        virtual int rowCount(const QModelIndex &index = QModelIndex()) const;

        //! \copydoc QAbstractTableModel::flags
        Qt::ItemFlags flags(const QModelIndex &index) const override;

        /*!
         * \brief Update by new list
         * \param list
         * \return new list size
         */
        virtual int update(const ContainerType &list);

        /*!
         * \brief Update single element
         * \param index
         * \param object
         */
        virtual void update(const QModelIndex &index, const ObjectType &object);

        /*!
         * \brief Object at row position
         * \param index
         * \return
         */
        virtual const ObjectType &at(const QModelIndex &index) const
        {
            if (index.row() < 0 || index.row() >= this->m_container.size())
            {
                const static ObjectType def;
                return def;
            }
            else
            {
                return this->m_container[index.row()];
            }
        }

        //! \copydoc QAbstractListModel::sort()
        virtual void sort(int column, Qt::SortOrder order);

        //! \brief Similar to ContainerType::push_back
        virtual void push_back(const ObjectType &object);

        //! \brief Similar to ContainerType::insert here inserts at first position
        virtual void insert(const ObjectType &object);

        //! \brief clear the list
        virtual void clear();
    };
}
#endif // guard
