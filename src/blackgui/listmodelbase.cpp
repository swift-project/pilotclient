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
#include "blackmisc/hwkeyboardkeylist.h"
#include "blackmisc/blackmiscfreefunctions.h"

namespace BlackGui
{

    /*
     * Column count
     */
    template <typename ObjectType, typename ContainerType>
    int CListModelBase<ObjectType, ContainerType>::columnCount(const QModelIndex & /** modelIndex **/) const
    {
        int c = this->m_columns.size();
        return c;
    }

    /*
     * Row count
     */
    template <typename ObjectType, typename ContainerType>
    int CListModelBase<ObjectType, ContainerType>::rowCount(const QModelIndex & /** parent */) const
    {
        return this->m_container.size();
    }

    /*
     * Column to property index
     */
    template <typename ObjectType, typename ContainerType>
    int CListModelBase<ObjectType, ContainerType>::columnToPropertyIndex(int column) const
    {
        return this->m_columns.columnToPropertyIndex(column);
    }

    /*
     * Header data
     */
    template <typename ObjectType, typename ContainerType> QVariant
    CListModelBase<ObjectType, ContainerType>::headerData(int section, Qt::Orientation orientation, int role) const
    {
        if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
        {
            if (section < 0 || section >= this->m_columns.size()) return QVariant();
            QString header = this->m_columns.at(section).getColumnName(false);
            return QVariant(header);
        }
        return QVariant();
    }

    /*
     * Data
     */
    template <typename ObjectType, typename ContainerType>
    QVariant CListModelBase<ObjectType,  ContainerType>::data(const QModelIndex &index, int role) const
    {
        // checks
        if (index.row() < 0 || index.row() >= this->m_container.size() ||
                index.column() < 0 || index.column() >= this->columnCount(index))
        {
            return QVariant();
        }

        if (role == Qt::DisplayRole)
        {
            ObjectType obj = this->m_container[index.row()];
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
    template <typename ObjectType, typename ContainerType>
    int CListModelBase<ObjectType, ContainerType>::update(const ContainerType &list)
    {
        ContainerType copyList = (list.size() > 1 && this->hasValidSortColumn() ?
                                  this->sortListByColumn(list, this->m_sortedColumn, this->m_sortOrder) :
                                  list);
        this->beginResetModel();
        this->m_container.clear();
        foreach(ObjectType object, copyList)
        {
            this->m_container.push_back(object);
        }
        this->endResetModel();
        return this->m_container.size();
    }


    /*
     * Update
     */
    template <typename ObjectType, typename ContainerType>
    void CListModelBase<ObjectType, ContainerType>::update(const QModelIndex &index, const ObjectType &object)
    {
        if (index.row() >= this->m_container.size()) return;
        this->m_container[index.row()] = object;

        QModelIndex i1 = index.sibling(index.row(), 0);
        QModelIndex i2 = index.sibling(index.row(), this->columnCount(index) - 1);
        emit this->dataChanged(i1, i2);
    }

    /*
     * Push back
     */
    template <typename ObjectType, typename ContainerType>
    void CListModelBase<ObjectType, ContainerType>::push_back(const ObjectType &object)
    {
        beginInsertRows(QModelIndex(), this->m_container.size(), this->m_container.size());
        this->m_container.push_back(object);
        endInsertRows();
    }

    /*
     * Push back
     */
    template <typename ObjectType, typename ContainerType>
    void CListModelBase<ObjectType, ContainerType>::insert(const ObjectType &object)
    {
        beginInsertRows(QModelIndex(), 0, 0);
        this->m_container.insert(this->m_container.begin(), object);
        endInsertRows();
    }

    /*
     * Clear
     */
    template <typename ObjectType, typename ContainerType>
    void CListModelBase<ObjectType, ContainerType>::clear()
    {
        beginResetModel();
        this->m_container.clear();
        endResetModel();
    }

    /*
     * Sort
     */
    template <typename ObjectType, typename ContainerType> void CListModelBase<ObjectType, ContainerType>::sort(int column, Qt::SortOrder order)
    {
        this->m_sortedColumn = column;
        this->m_sortOrder = order;
        if (this->m_container.size() < 2) return; // nothing to do

        // sort the values
        this->update(
            this->sortListByColumn(this->m_container, column, order)
        );
    }

    /*
     * Sort list
     */
    template <typename ObjectType, typename ContainerType> ContainerType CListModelBase<ObjectType, ContainerType>::sortListByColumn(const ContainerType &list, int column, Qt::SortOrder order)
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
               ); // sorted
    }

    /*
     * Make editable
     */
    template <typename ObjectType, typename ContainerType>  Qt::ItemFlags CListModelBase<ObjectType, ContainerType>::flags(const QModelIndex &index) const
    {
        Qt::ItemFlags f = QAbstractListModel::flags(index);
        if (this->m_columns.isEditable(index))
            return f | Qt::ItemIsEditable;
        else
            return f;
    }

    // see here for the reason of thess forward instantiations
    // http://www.parashift.com/c++-faq/separate-template-class-defn-from-decl.html
    template class CListModelBase<BlackMisc::CStatusMessage, BlackMisc::CStatusMessageList>;
    template class CListModelBase<BlackMisc::Aviation::CAtcStation, BlackMisc::Aviation::CAtcStationList>;
    template class CListModelBase<BlackMisc::Aviation::CAircraft, BlackMisc::Aviation::CAircraftList>;
    template class CListModelBase<BlackMisc::Network::CServer, BlackMisc::Network::CServerList>;
    template class CListModelBase<BlackMisc::Network::CUser, BlackMisc::Network::CUserList>;
    template class CListModelBase<BlackMisc::Hardware::CKeyboardKey, BlackMisc::Hardware::CKeyboardKeyList>;

} // namespace
