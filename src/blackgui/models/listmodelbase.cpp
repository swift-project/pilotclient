/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "listmodelbase.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/namevariantpairlist.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/avatcstationlist.h"
#include "blackmisc/avaircraftlist.h"
#include "blackmisc/avairportlist.h"
#include "blackmisc/nwserverlist.h"
#include "blackmisc/nwuserlist.h"
#include "blackmisc/nwclientlist.h"
#include "blackmisc/nwaircraftmappinglist.h"
#include "blackmisc/setkeyboardhotkeylist.h"
#include "blackmisc/simulation/simulatedaircraftlist.h"
#include "blackmisc/variant.h"
#include "blackmisc/blackmiscfreefunctions.h"

using namespace BlackMisc;

namespace BlackGui
{
    namespace Models
    {
        int CListModelBaseNonTemplate::columnCount(const QModelIndex &modelIndex) const
        {
            Q_UNUSED(modelIndex);
            int c = this->m_columns.size();
            return c;
        }

        QVariant CListModelBaseNonTemplate::headerData(int section, Qt::Orientation orientation, int role) const
        {
            if (orientation != Qt::Horizontal) { return QVariant(); }
            bool handled = (role == Qt::DisplayRole || role == Qt::ToolTipRole || role == Qt::InitialSortOrderRole);
            if (!handled) {return QVariant();}
            if (section < 0 || section >= this->m_columns.size()) { return QVariant(); }

            if (role == Qt::DisplayRole)
            {
                QString header = this->m_columns.at(section).getColumnName(false);
                return QVariant(header);
            }
            else if (role == Qt::ToolTipRole)
            {
                QString header = this->m_columns.at(section).getColumnToolTip(false);
                return header.isEmpty() ? QVariant() : QVariant(header);
            }
            return QVariant();
        }

        QModelIndex CListModelBaseNonTemplate::index(int row, int column, const QModelIndex &parent) const
        {
            Q_UNUSED(parent);
            return QAbstractItemModel::createIndex(row, column);
        }

        QModelIndex CListModelBaseNonTemplate::parent(const QModelIndex &child) const
        {
            Q_UNUSED(child);
            return QModelIndex();
        }

        BlackMisc::CPropertyIndex CListModelBaseNonTemplate::columnToPropertyIndex(int column) const
        {
            return this->m_columns.columnToPropertyIndex(column);
        }

        int CListModelBaseNonTemplate::propertyIndexToColumn(const CPropertyIndex &propertyIndex) const
        {
            return m_columns.propertyIndexToColumn(propertyIndex);
        }

        BlackMisc::CPropertyIndex CListModelBaseNonTemplate::modelIndexToPropertyIndex(const QModelIndex &index) const
        {
            return this->columnToPropertyIndex(index.column());
        }

        void CListModelBaseNonTemplate::setSortColumnByPropertyIndex(const BlackMisc::CPropertyIndex &propertyIndex)
        {
            this->m_sortedColumn = this->m_columns.propertyIndexToColumn(propertyIndex);
        }

        bool CListModelBaseNonTemplate::hasValidSortColumn() const
        {

            if (!(this->m_sortedColumn >= 0 && this->m_sortedColumn < this->m_columns.size())) { return false; }
            return this->m_columns.isSortable(this->m_sortedColumn);
        }

        Qt::ItemFlags CListModelBaseNonTemplate::flags(const QModelIndex &index) const
        {
            Qt::ItemFlags f = QAbstractItemModel::flags(index);
            if (!index.isValid()) { return f; }
            bool editable = this->m_columns.isEditable(index);
            f = editable ? (f | Qt::ItemIsEditable) : (f ^ Qt::ItemIsEditable);
            const CDefaultFormatter *formatter = this->m_columns.getFormatter(index);
            if (formatter)
            {
                return formatter->flags(f, editable);
            }

            // fallback behaviour with no formatter
            return f;
        }

        const QString &CListModelBaseNonTemplate::getTranslationContext() const
        {
            return m_columns.getTranslationContext();
        }

        int CListModelBaseNonTemplate::ps_updateContainer(const CVariant &variant, bool sort)
        {
            return this->performUpdateContainer(variant, sort);
        }

        template <typename ObjectType, typename ContainerType>
        int CListModelBase<ObjectType, ContainerType>::rowCount(const QModelIndex &parentIndex) const
        {
            Q_UNUSED(parentIndex);
            return this->getContainerOrFilteredContainer().size();
        }

        template <typename ObjectType, typename ContainerType>
        bool CListModelBase<ObjectType, ContainerType>::isValidIndex(const QModelIndex &index) const
        {
            if (!index.isValid()) { return false; }
            return (index.row() >= 0 && index.row() < this->rowCount(index) &&
                    index.column() >= 0 && index.column() < this->columnCount(index));
        }

        template <typename ObjectType, typename ContainerType>
        QVariant CListModelBase<ObjectType, ContainerType>::data(const QModelIndex &index, int role) const
        {
            // check / init
            if (!this->isValidIndex(index)) { return QVariant(); }
            const CDefaultFormatter *formatter = this->m_columns.getFormatter(index);
            Q_ASSERT(formatter);
            if (!formatter) { return QVariant(); }

            //! Formatted data
            ObjectType obj = this->getContainerOrFilteredContainer()[index.row()];
            BlackMisc::CPropertyIndex propertyIndex = this->columnToPropertyIndex(index.column());
            return formatter->data(role, obj.propertyByIndex(propertyIndex)).toQVariant();
        }

        template <typename ObjectType, typename ContainerType>
        bool CListModelBase<ObjectType, ContainerType>::setData(const QModelIndex &index, const QVariant &value, int role)
        {
            Qt::ItemDataRole dataRole = static_cast<Qt::ItemDataRole>(role);
            if (!(dataRole == Qt::UserRole || dataRole == Qt::EditRole)) { return false; }

            // check / init
            if (!this->isValidIndex(index)) { return false; }
            if (!this->m_columns.isEditable(index)) { return false; }
            const CDefaultFormatter *formatter = this->m_columns.getFormatter(index);
            Q_ASSERT(formatter);
            if (!formatter) { return false; }

            ObjectType obj = this->m_container[index.row()];
            ObjectType currentObject(obj);
            BlackMisc::CPropertyIndex propertyIndex = this->columnToPropertyIndex(index.column());
            obj.setPropertyByIndex(value, propertyIndex);

            if (obj != currentObject)
            {
                QModelIndex topLeft = index.sibling(index.row(), 0);
                QModelIndex bottomRight = index.sibling(index.row(), this->columnCount() - 1);
                this->m_container[index.row()] = obj;
                const CVariant co = CVariant::from(obj);
                emit objectChanged(co, propertyIndex);
                emit dataChanged(topLeft, bottomRight);
                this->updateFilteredContainer();
                return true;
            }
            return false;
        }

        template <typename ObjectType, typename ContainerType>
        int CListModelBase<ObjectType, ContainerType>::update(const ContainerType &container, bool sort)
        {
            // Keep sorting out of begin/end reset model
            ContainerType sortedContainer;
            int oldSize = this->m_container.size();
            bool performSort = sort && container.size() > 1 && this->hasValidSortColumn();
            if (performSort)
            {
                int sortColumn = this->getSortColumn();
                sortedContainer = this->sortContainerByColumn(container, sortColumn, this->m_sortOrder);
            }

            this->beginResetModel();
            this->m_container = performSort ? sortedContainer : container;
            this->updateFilteredContainer();
            this->endResetModel();

            int newSize = this->m_container.size();
            if (oldSize != newSize) {  this->emitRowCountChanged(); }
            return newSize;
        }

        template <typename ObjectType, typename ContainerType>
        void CListModelBase<ObjectType, ContainerType>::update(const QModelIndex &index, const ObjectType &object)
        {
            if (index.row() >= this->m_container.size()) { return; }
            this->m_container[index.row()] = object;

            QModelIndex i1 = index.sibling(index.row(), 0);
            QModelIndex i2 = index.sibling(index.row(), this->columnCount(index) - 1);
            emit this->dataChanged(i1, i2); // which range has been changed
        }

        template <typename ObjectType, typename ContainerType>
        void CListModelBase<ObjectType, ContainerType>::update(int rowIndex, const ObjectType &object)
        {
            this->update(this->index(rowIndex, 0), object);
        }

        template <typename ObjectType, typename ContainerType>
        BlackMisc::CWorker *CListModelBase<ObjectType, ContainerType>::updateAsync(const ContainerType &container, bool sort)
        {
            auto sortColumn = this->getSortColumn();
            auto sortOrder = this->getSortOrder();
            BlackMisc::CWorker *worker = BlackMisc::CWorker::fromTask(this, "ModelSort", [this, container, sort, sortColumn, sortOrder]()
            {
                ContainerType sortedContainer = this->sortContainerByColumn(container, sortColumn, sortOrder);
                QMetaObject::invokeMethod(this, "ps_updateContainer",
                                          Q_ARG(BlackMisc::CVariant, sortedContainer.toCVariant()), Q_ARG(bool, false));
            });
            worker->then(this, &CListModelBase::asyncUpdateFinished);
            return worker;
        }

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

        template <typename ObjectType, typename ContainerType>
        bool CListModelBase<ObjectType, ContainerType>::hasFilter() const
        {
            return m_filter ? true : false;
        }

        template <typename ObjectType, typename ContainerType>
        void CListModelBase<ObjectType, ContainerType>::removeFilter()
        {
            if (!this->hasFilter()) { return; }
            this->m_filter.reset(nullptr);
            this->beginResetModel();
            this->updateFilteredContainer();
            this->endResetModel();
            this->emitRowCountChanged();
        }

        template <typename ObjectType, typename ContainerType>
        void CListModelBase<ObjectType, ContainerType>::setFilter(std::unique_ptr<IModelFilter<ContainerType> > &filter)
        {
            if (!filter) { this->removeFilter(); return; } // empty filter
            if (filter->isValid())
            {
                this->m_filter = std::move(filter);
                this->beginResetModel();
                this->updateFilteredContainer();
                this->endResetModel();
                this->emitRowCountChanged();
            }
            else
            {
                this->removeFilter();
            }
        }

        template <typename ObjectType, typename ContainerType>
        const ObjectType &CListModelBase<ObjectType, ContainerType>::at(const QModelIndex &index) const
        {
            if (index.row() < 0 || index.row() >= this->rowCount())
            {
                static const ObjectType def {}; // default object
                return def;
            }
            else
            {
                return this->getContainerOrFilteredContainer()[index.row()];
            }
        }

        template <typename ObjectType, typename ContainerType>
        const ContainerType &CListModelBase<ObjectType, ContainerType>::getContainer() const
        {
            return this->m_container;
        }

        template <typename ObjectType, typename ContainerType>
        void CListModelBase<ObjectType, ContainerType>::push_back(const ObjectType &object)
        {
            beginInsertRows(QModelIndex(), this->m_container.size(), this->m_container.size());
            this->m_container.push_back(object);
            endInsertRows();
            this->updateFilteredContainer();
            this->emitRowCountChanged();
        }

        template <typename ObjectType, typename ContainerType>
        void CListModelBase<ObjectType, ContainerType>::insert(const ObjectType &object)
        {
            beginInsertRows(QModelIndex(), 0, 0);
            this->m_container.insert(this->m_container.begin(), object);
            endInsertRows();

            if (this->hasFilter())
            {
                this->beginResetModel();
                this->updateFilteredContainer();
                this->endResetModel();
            }
            this->emitRowCountChanged();
        }

        template <typename ObjectType, typename ContainerType>
        void CListModelBase<ObjectType, ContainerType>::remove(const ObjectType &object)
        {
            int oldSize = this->m_container.size();
            beginRemoveRows(QModelIndex(), 0, 0);
            this->m_container.remove(object);
            endRemoveRows();
            int newSize = this->m_container.size();
            if (oldSize != newSize)
            {
                this->emitRowCountChanged();
                if (this->hasFilter())
                {
                    this->beginResetModel();
                    this->updateFilteredContainer();
                    this->endResetModel();
                }
            }
        }

        template <typename ObjectType, typename ContainerType>
        void CListModelBase<ObjectType, ContainerType>::clear()
        {
            beginResetModel();
            this->m_container.clear();
            this->m_containerFiltered.clear();
            endResetModel();
            this->emitRowCountChanged();
        }

        template <typename ObjectType, typename ContainerType>
        bool CListModelBase<ObjectType, ContainerType>::isEmpty() const
        {
            return this->m_container.isEmpty();
        }

        template <typename ObjectType, typename ContainerType>
        int CListModelBase<ObjectType, ContainerType>::performUpdateContainer(const BlackMisc::CVariant &variant, bool sort)
        {
            ContainerType c;
            c.convertFromCVariant(variant);
            return this->update(c, sort);
        }

        template <typename ObjectType, typename ContainerType>
        const ContainerType &CListModelBase<ObjectType, ContainerType>::getContainerOrFilteredContainer() const
        {
            if (!this->hasFilter()) { return this->m_container; }
            return m_containerFiltered;
        }

        template <typename ObjectType, typename ContainerType>
        void CListModelBase<ObjectType, ContainerType>::updateFilteredContainer()
        {
            if (this->hasFilter())
            {
                this->m_containerFiltered = this->m_filter->filter(this->m_container);
            }
            else
            {
                this->m_containerFiltered.clear();
            }
        }

        template <typename ObjectType, typename ContainerType>
        void CListModelBase<ObjectType, ContainerType>::emitRowCountChanged()
        {
            int n = this->getContainerOrFilteredContainer().size();
            emit this->rowCountChanged(n, this->hasFilter());
        }

        /*
         * Sort requested by abstract model
         */
        template <typename ObjectType, typename ContainerType>
        void CListModelBase<ObjectType, ContainerType>::sort(int column, Qt::SortOrder order)
        {
            if (column == this->m_sortedColumn && order == this->m_sortOrder) { return; }

            // new order
            this->m_sortedColumn = column;
            this->m_sortOrder    = order;
            if (this->m_container.size() < 2) { return; } // nothing to do

            // sort the values
            this->updateContainerMaybeAsync(this->m_container, true);
        }

        template <typename ObjectType, typename ContainerType>
        ContainerType CListModelBase<ObjectType, ContainerType>::sortContainerByColumn(const ContainerType &container, int column, Qt::SortOrder order) const
        {
            if (container.size() < 2 || !this->m_columns.isSortable(column)) { return container; } // nothing to do

            // this is the only part not really thread safe, but columns do not change so far
            BlackMisc::CPropertyIndex propertyIndex = this->m_columns.columnToSortPropertyIndex(column);
            Q_ASSERT(!propertyIndex.isEmpty());
            if (propertyIndex.isEmpty()) { return container; } // at release build do nothing

            // sort the values
            const auto p = [ = ](const ObjectType & a, const ObjectType & b) -> bool
            {
                BlackMisc::CVariant aQv = a.propertyByIndex(propertyIndex);
                BlackMisc::CVariant bQv = b.propertyByIndex(propertyIndex);
                return (order == Qt::AscendingOrder) ? (aQv < bQv) : (bQv < aQv);
            };

            const ContainerType sorted = container.sorted(p);
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
        template class CListModelBase<BlackMisc::Simulation::CAircraftModel, BlackMisc::Simulation::CAircraftModelList>;
        template class CListModelBase<BlackMisc::Network::CAircraftMapping, BlackMisc::Network::CAircraftMappingList>;
        template class CListModelBase<BlackMisc::Simulation::CSimulatedAircraft, BlackMisc::Simulation::CSimulatedAircraftList>;
        template class CListModelBase<BlackMisc::Settings::CSettingKeyboardHotkey, BlackMisc::Settings::CSettingKeyboardHotkeyList>;

    } // namespace
} // namespace
