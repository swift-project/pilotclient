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
        int CListModelBaseNonTemplate::columnCount(const QModelIndex & /** modelIndex **/) const
        {
            int c = this->m_columns.size();
            return c;
        }

        /*
         * Header data
         */
        QVariant CListModelBaseNonTemplate::headerData(int section, Qt::Orientation orientation, int role) const
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
         * Column to property index
         */
        BlackMisc::CPropertyIndex CListModelBaseNonTemplate::columnToPropertyIndex(int column) const
        {
            return this->m_columns.columnToPropertyIndex(column);
        }

        /*
         * Sort column?
         */
        bool CListModelBaseNonTemplate::hasValidSortColumn() const
        {
            return this->m_sortedColumn >= 0 && this->m_sortedColumn < this->m_columns.size();
        }

        /*
         * Make editable
         */
        Qt::ItemFlags CListModelBaseNonTemplate::flags(const QModelIndex &index) const
        {
            Qt::ItemFlags f = QAbstractListModel::flags(index);
            if (this->m_columns.isEditable(index))
                return f | Qt::ItemIsEditable;
            else
                return f;
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
         * Valid index?
         */
        template <typename ObjectType, typename ContainerType>
        bool CListModelBase<ObjectType, ContainerType>::isValidIndex(const QModelIndex &index) const
        {
            if (!index.isValid()) return false;
            return (index.row() >= 0 && index.row() < this->m_container.size() &&
                    index.column() >= 0 && index.column() < this->columnCount(index));
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
            return formatter->data(role, obj.propertyByIndex(propertyIndex)).toQVariant();
        }

        /*
         * Update
         */
        template <typename ObjectType, typename ContainerType>
        int CListModelBase<ObjectType, ContainerType>::update(const ContainerType &container, bool sort)
        {
            // Keep sorting out of begin/end reset model
            ContainerType sortedContainer;
            int oldSize = this->m_container.size();
            bool performSort = sort && container.size() > 1 && this->hasValidSortColumn();
            if (performSort)
            {
                sortedContainer = this->sortContainerByColumn(container, this->getSortColumn(), this->m_sortOrder);
            }

            this->beginResetModel();
            this->m_container = performSort ? sortedContainer : container;
            this->endResetModel();

            int newSize = this->m_container.size();
            if (oldSize != newSize) {  rowCountChanged(newSize); }
            return newSize;
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
         * Async update
         */
        template <typename ObjectType, typename ContainerType>
        BlackMisc::CWorker *CListModelBase<ObjectType, ContainerType>::updateAsync(const ContainerType &container, bool sort)
        {
            auto sortColumn = this->getSortColumn();
            auto sortOrder = this->getSortOrder();
            BlackMisc::CWorker *worker = BlackMisc::CWorker::fromTask(this, "ModelSort", [this, container, sort, sortColumn, sortOrder]()
            {
                ContainerType sortedContainer = this->sortContainerByColumn(container, sortColumn, sortOrder);
                QMetaObject::invokeMethod(this, "updateContainer",
                    Q_ARG(BlackMisc::CVariant, sortedContainer.toCVariant()), Q_ARG(bool, false));
            });
            worker->then(this, &CListModelBase::asyncUpdateFinished);
            return worker;
        }

        /*
         * Container size decides async/sync
         */
        template <typename ObjectType, typename ContainerType>
        void CListModelBase<ObjectType, ContainerType>::updateContainerMaybeAsync(const ContainerType &container, bool sort)
        {
            if (container.size() > asyncThreshold && sort)
            {
                // larger container with sorting
                updateAsync(container, sort);
            }
            else
            {
                update(container, sort);
            }
        }

        /*
         * At
         */
        template <typename ObjectType, typename ContainerType>
        const ObjectType &CListModelBase<ObjectType, ContainerType>::at(const QModelIndex &index) const
        {
            if (index.row() < 0 || index.row() >= this->m_container.size())
            {
                static const ObjectType def{}; // default object
                return def;
            }
            else
            {
                return this->m_container[index.row()];
            }
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
            emit rowCountChanged(this->m_container.size());
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
            int newSize = this->m_container.size();
            emit rowCountChanged(newSize);
        }

        /*
         * Remove object
         */
        template <typename ObjectType, typename ContainerType>
        void CListModelBase<ObjectType, ContainerType>::remove(const ObjectType &object)
        {
            int oldSize = this->m_container.size();
            beginRemoveRows(QModelIndex(), 0, 0);
            this->m_container.remove(object);
            endRemoveRows();
            int newSize = this->m_container.size();
            if (oldSize != newSize) { emit rowCountChanged(newSize); }
        }

        /*
         * Clear
         */
        template <typename ObjectType, typename ContainerType> void CListModelBase<ObjectType, ContainerType>::clear()
        {
            int oldSize = this->m_container.size();
            beginResetModel();
            this->m_container.clear();
            endResetModel();
            if (oldSize > 0) { emit rowCountChanged(0);}
        }

        /*
         * Update on container
         */
        template <typename ObjectType, typename ContainerType> int CListModelBase<ObjectType, ContainerType>::performUpdateContainer(const BlackMisc::CVariant &variant, bool sort)
        {
            ContainerType c;
            c.convertFromCVariant(variant);
            return this->update(c, sort);
        }

        /*
         * Sort requested by abstract model
         */
        template <typename ObjectType, typename ContainerType> void CListModelBase<ObjectType, ContainerType>::sort(int column, Qt::SortOrder order)
        {
            if (column == this->m_sortedColumn && order == this->m_sortOrder) { return; }

            // new order
            this->m_sortedColumn = column;
            this->m_sortOrder    = order;
            if (this->m_container.size() < 2) return; // nothing to do

            // sort the values
            this->updateContainerMaybeAsync(this->m_container, true);
        }

        /*
         * Sort list
         */
        template <typename ObjectType, typename ContainerType> ContainerType CListModelBase<ObjectType, ContainerType>::sortContainerByColumn(const ContainerType &container, int column, Qt::SortOrder order) const
        {
            if (container.size() < 2) return container; // nothing to do

            // this is the only part not really thread safe, but columns do not change so far
            BlackMisc::CPropertyIndex propertyIndex = this->m_columns.columnToPropertyIndex(column);
            Q_ASSERT(!propertyIndex.isEmpty());
            if (propertyIndex.isEmpty()) return container; // at release build do nothing

            // sort the values
            const auto p = [ = ](const ObjectType & a, const ObjectType & b) -> bool
            {
                BlackMisc::CVariant aQv = a.propertyByIndex(propertyIndex);
                BlackMisc::CVariant bQv = b.propertyByIndex(propertyIndex);
                return (order == Qt::AscendingOrder) ? (aQv < bQv) : (bQv < aQv);
            };

            // KWB: qDebug() will be removed soon
            QTime t;
            t.start();
            const ContainerType sorted = container.sorted(p);
            qDebug() << "Sort" << this->objectName() << "column" << column << "index:" << propertyIndex.toQString() << "ms:" << t.elapsed() << "thread:" << QThread::currentThreadId();
            return sorted;
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

    } // namespace
} // namespace
