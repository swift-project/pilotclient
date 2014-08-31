/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_LISTMODELBASE_H
#define BLACKGUI_LISTMODELBASE_H

#include "blackgui/models/columns.h"
#include "blackmisc/propertyindex.h"
#include <QAbstractItemModel>

namespace BlackGui
{
    namespace Models
    {

        /*!
         * List model
         */
        template <typename ObjectType, typename ContainerType> class CListModelBase : public QAbstractListModel
        {

        public:

            //! Destructor
            virtual ~CListModelBase() {}

            //! \copydoc QAbstractListModel::columnCount()
            virtual int columnCount(const QModelIndex &modelIndex) const override;

            //! \copydoc QAbstractItemModel::headerData()
            virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

            //! Column to property index
            virtual BlackMisc::CPropertyIndex columnToPropertyIndex(int column) const;

            //! Index to property index
            virtual BlackMisc::CPropertyIndex modelIndexToPropertyIndex(const QModelIndex &index) const
            {
                return this->columnToPropertyIndex(index.column());
            }

            //! Valid index (in range)
            virtual bool isValidIndex(const QModelIndex &index) const
            {
                if (!index.isValid()) return false;
                return (index.row() >= 0 && index.row() < this->m_container.size() &&
                        index.column() >= 0 && index.column() < this->columnCount(index));
            }

            //! Set sort column
            virtual void setSortColumn(int column) { this->m_sortedColumn = column; }

            /*!
             * Set column for sorting
             * \param propertyIndex index of column to be sorted by
             */
            virtual void setSortColumnByPropertyIndex(const BlackMisc::CPropertyIndex &propertyIndex)
            {
                this->m_sortedColumn = this->m_columns.propertyIndexToColumn(propertyIndex);
            }

            //! Get sort column property index
            virtual int getSortColumn() const { return this->m_sortedColumn; }

            //! Has valid sort column?
            virtual bool hasValidSortColumn() const
            {
                return this->m_sortedColumn >= 0 && this->m_sortedColumn < this->m_columns.size();
            }

            //! Get sort order
            virtual Qt::SortOrder getSortOrder() const { return this->m_sortOrder; }

            //! Used container data
            virtual const ContainerType &getContainer() const { return this->m_container; }

            //! \copydoc QAbstractListModel::data()
            virtual QVariant data(const QModelIndex &index, int role) const override;

            //! \copydoc QAbstractListModel::rowCount()
            virtual int rowCount(const QModelIndex &index = QModelIndex()) const override;

            //! \copydoc QAbstractTableModel::flags
            Qt::ItemFlags flags(const QModelIndex &index) const override;

            //! Update by new container
            //! \remarks a sorting is performed if a valid sort column is set
            virtual int update(const ContainerType &container);

            //! Update single element
            virtual void update(const QModelIndex &index, const ObjectType &object);

            //! Update single element
            virtual void update(int rowIndex, const ObjectType &object)
            {
                this->update(this->index(rowIndex), object);
            }

            //! Object at row position
            virtual const ObjectType &at(const QModelIndex &index) const
            {
                if (index.row() < 0 || index.row() >= this->m_container.size())
                {
                    const static ObjectType def; // default object
                    return def;
                }
                else
                {
                    return this->m_container[index.row()];
                }
            }

            //! \copydoc QAbstractListModel::sort()
            virtual void sort(int column, Qt::SortOrder order) override;

            //! Similar to ContainerType::push_back
            virtual void push_back(const ObjectType &object);

            //! Similar to ContainerType::insert here inserts at first position
            virtual void insert(const ObjectType &object);

            //! Remove object
            virtual void remove(const ObjectType &object);

            //! Clear the list
            virtual void clear();

            //! Translation context
            virtual const QString &getTranslationContext() const
            {
                return m_columns.getTranslationContext();
            }

        protected:
            ContainerType m_container; //!< used container
            CColumns m_columns;        //!< columns metadata
            int m_sortedColumn;        //!< current sort column
            Qt::SortOrder m_sortOrder; //!< sort order (asc/desc)

            /*!
             * Constructor
             * \param translationContext    I18N context
             * \param parent
             */
            CListModelBase(const QString &translationContext, QObject *parent = nullptr)
                : QAbstractListModel(parent), m_columns(translationContext), m_sortedColumn(-1), m_sortOrder(Qt::AscendingOrder)
            {
                // non unique default name, set translation context as default
                this->setObjectName(translationContext);
            }

            /*!
             * Sort container by given column / order. This is used by sort().
             * \param list      used list
             * \param column    column inder
             * \param order     sort order (ascending / descending)
             * \return
             */
            ContainerType sortListByColumn(const ContainerType &list, int column, Qt::SortOrder order);

        };
    }
}
#endif // guard
