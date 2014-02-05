/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "listmodelbase.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/avatcstationlist.h"
#include "blackmisc/avaircraftlist.h"
#include "blackmisc/nwserverlist.h"
#include "blackmisc/nwuserlist.h"
#include "blackmisc/blackmiscfreefunctions.h"

namespace BlackGui
{

    /*
     * Column count
     */
    template <typename ObjectType, typename ListType>
    int CListModelBase<ObjectType, ListType>::columnCount(const QModelIndex & /** modelIndex **/) const
    {
        int c = this->m_columns.size();
        return c;
    }

    /*
     * Row count
     */
    template <typename ObjectType, typename ListType>
    int CListModelBase<ObjectType, ListType>::rowCount(const QModelIndex & /** parent */) const
    {
        return this->m_list.size();
    }

    /*
     * Column to property index
     */
    template <typename ObjectType, typename ListType>
    int CListModelBase<ObjectType, ListType>::columnToPropertyIndex(int column) const
    {
        return this->m_columns.columnToPropertyIndex(column);
    }

    /*
     * Header data
     */
    template <typename ObjectType, typename ListType> QVariant
    CListModelBase<ObjectType, ListType>::headerData(int section, Qt::Orientation orientation, int role) const
    {
        if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
        {
            if (section < 0 || section >= this->m_columns.size()) return QVariant();
            QString col = this->m_columns.columnToName(section);
            col = QCoreApplication::translate(this->m_columns.getTranslationContext(), col.toUtf8().constData());
            return QVariant(col);
        }
        return QVariant();
    }

    /*
     * Data
     */
    template <typename ObjectType, typename ListType>
    QVariant CListModelBase<ObjectType,  ListType>::data(const QModelIndex &index, int role) const
    {
        // checks
        if (index.row() < 0 || index.row() >= this->m_list.size() ||
                index.column() < 0 || index.column() >= this->columnCount(index))
        {
            return QVariant();
        }

        if (role == Qt::DisplayRole)
        {
            ObjectType obj = this->m_list[index.row()];
            int propertyIndex = this->columnToPropertyIndex(index.column());
            QString propertyString = obj.propertyByIndexAsString(propertyIndex, true);
            return QVariant::fromValue(propertyString);
        }
        else if (role == Qt::TextAlignmentRole)
        {
            return this->m_columns.aligmentAsQVariant(index);
        }
        return QVariant();
    }

    /*
     * Update
     */
    template <typename ObjectType, typename ListType>
    int CListModelBase<ObjectType, ListType>::update(const ListType &list)
    {
        ListType copyList = (list.size() > 1 && this->hasValidSortColumn() ?
                             this->sortListByColumn(list, this->m_sortedColumn, this->m_sortOrder) :
                             list);
        this->beginResetModel();
        this->m_list.clear();
        foreach(ObjectType object, copyList)
        {
            this->m_list.push_back(object);
        }
        this->endResetModel();
        return this->m_list.size();
    }

    /*
     * Push back
     */
    template <typename ObjectType, typename ListType>
    void CListModelBase<ObjectType, ListType>::push_back(const ObjectType &object)
    {
        beginInsertRows(QModelIndex(), this->m_list.size(), this->m_list.size());
        this->m_list.push_back(object);
        endInsertRows();
    }

    /*
     * Push back
     */
    template <typename ObjectType, typename ListType>
    void CListModelBase<ObjectType, ListType>::insert(const ObjectType &object)
    {
        beginInsertRows(QModelIndex(), 0, 0);
        this->m_list.insert(this->m_list.begin(), object);
        endInsertRows();
    }

    /*
     * Clear
     */
    template <typename ObjectType, typename ListType>
    void CListModelBase<ObjectType, ListType>::clear()
    {
        beginResetModel();
        this->m_list.clear();
        endResetModel();
    }

    /*
     * Sort
     */
    template <typename ObjectType, typename ListType> void CListModelBase<ObjectType, ListType>::sort(int column, Qt::SortOrder order)
    {
        this->m_sortedColumn = column;
        this->m_sortOrder = order;
        if (this->m_list.size() < 2) return; // nothing to do

        // sort the values
        this->update(
            this->sortListByColumn(this->m_list, column, order)
        );
    }

    /*
     * Sort list
     */
    template <typename ObjectType, typename ListType> ListType CListModelBase<ObjectType, ListType>::sortListByColumn(const ListType &list, int column, Qt::SortOrder order)
    {
        if (list.size() < 2) return list; // nothing to do
        int propertyIndex = this->m_columns.columnToPropertyIndex(column);
        Q_ASSERT(propertyIndex >= 0);
        if (propertyIndex < 0) return list; // at release build do nothing

        // sort the values
        return list.sorted
               ([ = ](const ObjectType & a, const ObjectType & b) -> bool
        {
            QVariant aQv = a.propertyByIndex(propertyIndex);
            QVariant bQv = b.propertyByIndex(propertyIndex);
            int compare = (order == Qt::AscendingOrder) ?
            BlackMisc::compareQVariants(aQv, bQv) :
            BlackMisc::compareQVariants(bQv, aQv);
            return compare < 0;
        }
               );

    }

    // see here for the reason of thess forward instantiations
    // http://www.parashift.com/c++-faq/separate-template-class-defn-from-decl.html
    template class CListModelBase<BlackMisc::CStatusMessage, BlackMisc::CStatusMessageList>;
    template class CListModelBase<BlackMisc::Aviation::CAtcStation, BlackMisc::Aviation::CAtcStationList>;
    template class CListModelBase<BlackMisc::Aviation::CAircraft, BlackMisc::Aviation::CAircraftList>;
    template class CListModelBase<BlackMisc::Network::CServer, BlackMisc::Network::CServerList>;
    template class CListModelBase<BlackMisc::Network::CUser, BlackMisc::Network::CUserList>;


} // namespace
