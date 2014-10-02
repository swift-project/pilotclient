/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "listmodelbase.h"
#include "blackmisc/namevariantpairlist.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/avatcstationlist.h"
#include "blackmisc/avaircraftlist.h"
#include "blackmisc/avairportlist.h"
#include "blackmisc/nwserverlist.h"
#include "blackmisc/nwuserlist.h"
#include "blackmisc/nwclientlist.h"
#include "blackmisc/setkeyboardhotkeylist.h"
#include "blackmisc/blackmiscfreefunctions.h"

namespace BlackGui
{
    namespace Models
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
        BlackMisc::CPropertyIndex CListModelBase<ObjectType, ContainerType>::columnToPropertyIndex(int column) const
        {
            return this->m_columns.columnToPropertyIndex(column);
        }

        /*
         * Header data
         */
        template <typename ObjectType, typename ContainerType> QVariant
        CListModelBase<ObjectType, ContainerType>::headerData(int section, Qt::Orientation orientation, int role) const
        {
            if (orientation == Qt::Horizontal)
            {
                if (role == Qt::DisplayRole)
                {
                    if (section < 0 || section >= this->m_columns.size()) { return QVariant(); }
                    QString header = this->m_columns.at(section).getColumnName(false);
                    return QVariant(header);
                }
                else if (role == Qt::ToolTipRole)
                {
                    if (section < 0 || section >= this->m_columns.size()) { return QVariant(); }
                    QString header = this->m_columns.at(section).getColumnToolTip(false);
                    return header.isEmpty() ? QVariant() : QVariant(header);
                }
            }
            return QVariant();
        }

        /*
         * Data
         */
        template <typename ObjectType, typename ContainerType>
        QVariant CListModelBase<ObjectType,  ContainerType>::data(const QModelIndex &index, int role) const
        {
            // check / init
            if (!this->isValidIndex(index)) { return QVariant(); }
            const CDefaultFormatter *formatter = this->m_columns.getFormatter(index);
            Q_ASSERT(formatter);
            if (!formatter) { return QVariant(); }

            //! Formatted data
            ObjectType obj = this->m_container[index.row()];
            BlackMisc::CPropertyIndex propertyIndex = this->columnToPropertyIndex(index.column());
            return formatter->data(role, obj.propertyByIndex(propertyIndex));
        }

        /*
         * Update
         */
        template <typename ObjectType, typename ContainerType>
        int CListModelBase<ObjectType, ContainerType>::update(const ContainerType &container, bool sort)
        {
            // KWB remove: qDebug() will be removed soon
            qDebug() << "update" << this->objectName() << "size" << container.size();
            this->beginResetModel();
            this->m_container = (sort && container.size() > 1 && this->hasValidSortColumn() ?
                                 this->sortListByColumn(container, this->getSortColumn(), this->m_sortOrder) :
                                 container);
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
            emit this->dataChanged(i1, i2); // which range has been changed
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
         * Remove object
         */
        template <typename ObjectType, typename ContainerType>
        void CListModelBase<ObjectType, ContainerType>::remove(const ObjectType &object)
        {
            beginRemoveRows(QModelIndex(), 0, 0);
            this->m_container.remove(object);
            endRemoveRows();
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
         * Sort requested by abstract model
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
            BlackMisc::CPropertyIndex propertyIndex = this->m_columns.columnToPropertyIndex(column);
            Q_ASSERT(!propertyIndex.isEmpty());
            if (propertyIndex.isEmpty()) return list; // at release build do nothing

            // sort the values
            auto p = [ = ](const ObjectType & a, const ObjectType & b) -> bool
            {
                QVariant aQv = a.propertyByIndex(propertyIndex);
                QVariant bQv = b.propertyByIndex(propertyIndex);
                int compare = (order == Qt::AscendingOrder) ?
                BlackMisc::compareQVariants(aQv, bQv) :
                BlackMisc::compareQVariants(bQv, aQv);
                return compare < 0;
            };

            // KWB: qDebug() will be removed soon
            qDebug() << "sort" << this->objectName() << "column" << column << propertyIndex.toQString();
            return list.sorted(p); // synchronous sorted
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
        template class CListModelBase<BlackMisc::CNameVariantPair, BlackMisc::CNameVariantPairList>;
        template class CListModelBase<BlackMisc::Aviation::CAtcStation, BlackMisc::Aviation::CAtcStationList>;
        template class CListModelBase<BlackMisc::Aviation::CAircraft, BlackMisc::Aviation::CAircraftList>;
        template class CListModelBase<BlackMisc::Aviation::CAirport, BlackMisc::Aviation::CAirportList>;
        template class CListModelBase<BlackMisc::Network::CServer, BlackMisc::Network::CServerList>;
        template class CListModelBase<BlackMisc::Network::CUser, BlackMisc::Network::CUserList>;
        template class CListModelBase<BlackMisc::Network::CClient, BlackMisc::Network::CClientList>;
        template class CListModelBase<BlackMisc::Settings::CSettingKeyboardHotkey, BlackMisc::Settings::CSettingKeyboardHotkeyList>;
    }
} // namespace
