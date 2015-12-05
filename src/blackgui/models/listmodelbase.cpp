/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

// Drag and drop docu:
// http://doc.qt.io/qt-5/model-view-programming.html#using-drag-and-drop-with-item-views

#include "listmodelbase.h"
#include "allmodelcontainers.h"
#include "blackgui/guiutility.h"
#include "blackmisc/variant.h"
#include "blackmisc/json.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/comparefunctions.h"
#include <QMimeData>
#include <QJsonDocument>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;

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
            if (orientation != Qt::Horizontal)
            {
                return QVariant();
            }
            bool handled = (role == Qt::DisplayRole || role == Qt::ToolTipRole || role == Qt::InitialSortOrderRole);
            if (!handled)
            {
                return QVariant();
            }
            if (section < 0 || section >= this->m_columns.size())
            {
                return QVariant();
            }

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

            // flags from formatter
            const CDefaultFormatter *formatter = this->m_columns.getFormatter(index);
            if (formatter) { f = formatter->flags(f, editable); }

            // drag and rop
            f = f | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;

            return f;
        }

        const QString &CListModelBaseNonTemplate::getTranslationContext() const
        {
            return m_columns.getTranslationContext();
        }

        Qt::DropActions CListModelBaseNonTemplate::supportedDragActions() const
        {
            return Qt::CopyAction;
        }

        Qt::DropActions CListModelBaseNonTemplate::supportedDropActions() const
        {
            return QAbstractItemModel::supportedDropActions();
        }

        QStringList CListModelBaseNonTemplate::mimeTypes() const
        {
            static const QStringList mimes( { "application/swift.container.json" });
            return mimes;
        }

        void CListModelBaseNonTemplate::markDestroyed()
        {
            this->m_modelDestroyed = true;
        }

        bool CListModelBaseNonTemplate::isModelDestroyed()
        {
            return m_modelDestroyed;
        }

        int CListModelBaseNonTemplate::ps_updateContainer(const CVariant &variant, bool sort)
        {
            return this->performUpdateContainer(variant, sort);
        }

        CListModelBaseNonTemplate::CListModelBaseNonTemplate(const QString &translationContext, QObject *parent)
            : QAbstractItemModel(parent), m_columns(translationContext), m_sortedColumn(-1), m_sortOrder(Qt::AscendingOrder)
        {
            // non unique default name, set translation context as default
            this->setObjectName(translationContext);
        }

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        int CListModelBase<ObjectType, ContainerType, UseCompare>::rowCount(const QModelIndex &parentIndex) const
        {
            Q_UNUSED(parentIndex);
            return this->getContainerOrFilteredContainer().size();
        }

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        bool CListModelBase<ObjectType, ContainerType, UseCompare>::isValidIndex(const QModelIndex &index) const
        {
            if (!index.isValid()) { return false; }
            return (index.row() >= 0 && index.row() < this->rowCount(index) &&
                    index.column() >= 0 && index.column() < this->columnCount(index));
        }

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        QVariant CListModelBase<ObjectType, ContainerType, UseCompare>::data(const QModelIndex &index, int role) const
        {
            // check / init
            if (!this->isValidIndex(index)) { return QVariant(); }
            const CDefaultFormatter *formatter = this->m_columns.getFormatter(index);
            Q_ASSERT_X(formatter, Q_FUNC_INFO, "Missing formatter");
            if (!formatter) { return QVariant(); }

            //! Formatted data
            ObjectType obj = this->getContainerOrFilteredContainer()[index.row()];
            BlackMisc::CPropertyIndex propertyIndex = this->columnToPropertyIndex(index.column());
            return formatter->data(role, obj.propertyByIndex(propertyIndex)).getQVariant();
        }

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        bool CListModelBase<ObjectType, ContainerType, UseCompare>::setData(const QModelIndex &index, const QVariant &value, int role)
        {
            Qt::ItemDataRole dataRole = static_cast<Qt::ItemDataRole>(role);
            if (!(dataRole == Qt::UserRole || dataRole == Qt::EditRole))
            {
                return false;
            }

            // check / init
            if (!this->isValidIndex(index))
            {
                return false;
            }
            if (!this->m_columns.isEditable(index))
            {
                return false;
            }
            const CDefaultFormatter *formatter = this->m_columns.getFormatter(index);
            Q_ASSERT(formatter);
            if (!formatter)
            {
                return false;
            }

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

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        int CListModelBase<ObjectType, ContainerType, UseCompare>::update(const ContainerType &container, bool sort)
        {
            if (m_modelDestroyed) { return 0; }

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
            if (oldSize != newSize)
            {
                this->emitRowCountChanged();
            }
            return newSize;
        }

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        void CListModelBase<ObjectType, ContainerType, UseCompare>::update(const QModelIndex &index, const ObjectType &object)
        {
            if (m_modelDestroyed) { return; }
            if (index.row() >= this->m_container.size()) { return; }
            this->m_container[index.row()] = object;

            QModelIndex i1 = index.sibling(index.row(), 0);
            QModelIndex i2 = index.sibling(index.row(), this->columnCount(index) - 1);
            emit this->dataChanged(i1, i2); // which range has been changed
        }

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        void CListModelBase<ObjectType, ContainerType, UseCompare>::update(int rowIndex, const ObjectType &object)
        {
            this->update(this->index(rowIndex, 0), object);
        }

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        CWorker *CListModelBase<ObjectType, ContainerType, UseCompare>::updateAsync(const ContainerType &container, bool sort)
        {
            Q_UNUSED(sort);
            if (m_modelDestroyed) { return nullptr; }
            auto sortColumn = this->getSortColumn();
            auto sortOrder = this->getSortOrder();
            CWorker *worker = BlackMisc::CWorker::fromTask(this, "ModelSort", [this, container, sortColumn, sortOrder]()
            {
                return this->sortContainerByColumn(container, sortColumn, sortOrder);
            });
            worker->thenWithResult<ContainerType>(this, [this](const ContainerType & sortedContainer)
            {
                if (this->m_modelDestroyed) { return;  }
                this->ps_updateContainer(CVariant::from(sortedContainer), false);
            });
            worker->then(this, &CListModelBase::asyncUpdateFinished);
            return worker;
        }

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        void CListModelBase<ObjectType, ContainerType, UseCompare>::updateContainerMaybeAsync(const ContainerType &container, bool sort)
        {
            if (m_modelDestroyed) { return; }
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

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        bool CListModelBase<ObjectType, ContainerType, UseCompare>::hasFilter() const
        {
            return m_filter && m_filter->isValid() ? true : false;
        }

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        void CListModelBase<ObjectType, ContainerType, UseCompare>::removeFilter()
        {
            if (!this->hasFilter()) { return; }
            this->m_filter.reset(nullptr);
            this->beginResetModel();
            this->updateFilteredContainer();
            this->endResetModel();
            this->emitRowCountChanged();
        }

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        void CListModelBase<ObjectType, ContainerType, UseCompare>::takeFilterOwnership(std::unique_ptr<IModelFilter<ContainerType> > &filter)
        {
            if (!filter)
            {
                this->removeFilter();    // empty filter
                return;
            }
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

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        const ObjectType &CListModelBase<ObjectType, ContainerType, UseCompare>::at(const QModelIndex &index) const
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

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        const ContainerType &CListModelBase<ObjectType, ContainerType, UseCompare>::container() const
        {
            return this->m_container;
        }

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        void CListModelBase<ObjectType, ContainerType, UseCompare>::push_back(const ObjectType &object)
        {
            beginInsertRows(QModelIndex(), this->m_container.size(), this->m_container.size());
            this->m_container.push_back(object);
            endInsertRows();
            this->updateFilteredContainer();
            this->emitRowCountChanged();
        }

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        void CListModelBase<ObjectType, ContainerType, UseCompare>::insert(const ObjectType &object)
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

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        void CListModelBase<ObjectType, ContainerType, UseCompare>::remove(const ObjectType &object)
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

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        void CListModelBase<ObjectType, ContainerType, UseCompare>::clear()
        {
            beginResetModel();
            this->m_container.clear();
            this->m_containerFiltered.clear();
            endResetModel();
            this->emitRowCountChanged();
        }

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        bool CListModelBase<ObjectType, ContainerType, UseCompare>::isEmpty() const
        {
            return this->m_container.isEmpty();
        }

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        int CListModelBase<ObjectType, ContainerType, UseCompare>::performUpdateContainer(const BlackMisc::CVariant &variant, bool sort)
        {
            ContainerType c(variant.to<ContainerType>());
            return this->update(c, sort);
        }

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        const ContainerType &CListModelBase<ObjectType, ContainerType, UseCompare>::getContainerOrFilteredContainer() const
        {
            if (!this->hasFilter())
            {
                return this->m_container;
            }
            return m_containerFiltered;
        }

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        void CListModelBase<ObjectType, ContainerType, UseCompare>::updateFilteredContainer()
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

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        void CListModelBase<ObjectType, ContainerType, UseCompare>::emitRowCountChanged()
        {
            int n = this->getContainerOrFilteredContainer().size();
            emit this->rowCountChanged(n, this->hasFilter());
        }

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        void CListModelBase<ObjectType, ContainerType, UseCompare>::sort()
        {
            this->sort(this->getSortColumn(), this->getSortOrder());
        }

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        void CListModelBase<ObjectType, ContainerType, UseCompare>::sort(int column, Qt::SortOrder order)
        {
            if (column == this->m_sortedColumn && order == this->m_sortOrder) { return; }

            // new order
            this->m_sortedColumn = column;
            this->m_sortOrder    = order;
            if (this->m_container.size() < 2)
            {
                return; // nothing to do
            }

            // sort the values
            this->updateContainerMaybeAsync(this->m_container, true);
        }

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        void CListModelBase<ObjectType, ContainerType, UseCompare>::truncate(int maxNumber, bool forceSort)
        {
            if (this->rowCount() <= maxNumber) { return; }
            if (forceSort)
            {
                this->sort();    // make sure container is sorted
            }
            ContainerType container(this->container());
            container.truncate(maxNumber);
            this->updateContainerMaybeAsync(container, false);
        }

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        ContainerType CListModelBase<ObjectType, ContainerType, UseCompare>::sortContainerByColumn(const ContainerType &container, int column, Qt::SortOrder order) const
        {
            if (m_modelDestroyed) { return container; }
            if (container.size() < 2 || !this->m_columns.isSortable(column))
            {
                return container;    // nothing to do
            }

            // this is the only part not really thread safe, but columns do not change so far
            BlackMisc::CPropertyIndex propertyIndex = this->m_columns.columnToSortPropertyIndex(column);
            Q_ASSERT(!propertyIndex.isEmpty());
            if (propertyIndex.isEmpty())
            {
                return container;    // at release build do nothing
            }

            // sort the values
            std::integral_constant<bool, UseCompare> marker {};
            const auto p = [ = ](const ObjectType & a, const ObjectType & b) -> bool
            {
                return Private::compareForModelSort<ObjectType>(a, b, order, propertyIndex, marker);
            };

            //! \todo Time measurement will be removed
            QTime t;
            t.start();
            const ContainerType sorted = container.sorted(p);
            int te = t.elapsed();
            CLogMessage(this).info("Sorted %1 in %2") << typeid(ObjectType).name() <<  te;
            return sorted;
        }

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        QMimeData *CListModelBase<ObjectType, ContainerType, UseCompare>::mimeData(const QModelIndexList &indexes) const
        {
            QMimeData *mimeData = new QMimeData();
            if (indexes.isEmpty()) { return mimeData; }

            ContainerType container;
            QList<int> rows; // avoid redundant objects

            // Indexes are per row and column
            for (const QModelIndex &index : indexes)
            {
                if (!index.isValid()) { continue; }
                int r = index.row();
                if (rows.contains(r)) { continue; }
                container.push_back(this->at(index));
                rows.append(r);
            }

            // to JSON via CVariant
            const QJsonDocument containerJson(CVariant::fromValue(container).toJson());
            const QString jsonString(containerJson.toJson(QJsonDocument::Compact));

            mimeData->setData(CGuiUtility::swiftJsonDragAndDropMimeType(), jsonString.toUtf8());
            return mimeData;
        }

        // see here for the reason of thess forward instantiations
        // http://www.parashift.com/c++-faq/separate-template-class-defn-from-decl.html
        template class CListModelBase<BlackMisc::Aviation::CLivery, BlackMisc::Aviation::CLiveryList, true>;
        template class CListModelBase<BlackMisc::CIdentifier, BlackMisc::CIdentifierList, false>;
        template class CListModelBase<BlackMisc::CStatusMessage, BlackMisc::CStatusMessageList, false>;
        template class CListModelBase<BlackMisc::CNameVariantPair, BlackMisc::CNameVariantPairList, false>;
        template class CListModelBase<BlackMisc::CCountry, BlackMisc::CCountryList, true>;
        template class CListModelBase<BlackMisc::Aviation::CAtcStation, BlackMisc::Aviation::CAtcStationList, false>;
        template class CListModelBase<BlackMisc::Aviation::CAirport, BlackMisc::Aviation::CAirportList, true>;
        template class CListModelBase<BlackMisc::Aviation::CAircraftIcaoCode, BlackMisc::Aviation::CAircraftIcaoCodeList, true>;
        template class CListModelBase<BlackMisc::Aviation::CAirlineIcaoCode, BlackMisc::Aviation::CAirlineIcaoCodeList, true>;
        template class CListModelBase<BlackMisc::Network::CServer, BlackMisc::Network::CServerList, false>;
        template class CListModelBase<BlackMisc::Network::CUser, BlackMisc::Network::CUserList, true>;
        template class CListModelBase<BlackMisc::Network::CTextMessage, BlackMisc::Network::CTextMessageList, false>;
        template class CListModelBase<BlackMisc::Network::CClient, BlackMisc::Network::CClientList, false>;
        template class CListModelBase<BlackMisc::Simulation::CAircraftModel, BlackMisc::Simulation::CAircraftModelList, true>;
        template class CListModelBase<BlackMisc::Simulation::CSimulatedAircraft, BlackMisc::Simulation::CSimulatedAircraftList, true>;
        template class CListModelBase<BlackMisc::Simulation::CDistributor, BlackMisc::Simulation::CDistributorList, true>;

    } // namespace
} // namespace
