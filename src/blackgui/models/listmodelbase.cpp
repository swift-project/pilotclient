/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

// Drag and drop docu:
// http://doc.qt.io/qt-5/model-view-programming.html#using-drag-and-drop-with-item-views

#include "blackgui/models/listmodelbase.h"
#include "blackgui/models/allmodelcontainers.h"
#include "blackgui/guiutility.h"
#include "blackmisc/variant.h"
#include "blackmisc/worker.h"

#include <QFlags>
#include <QJsonDocument>
#include <QList>
#include <QMimeData>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;

namespace BlackGui
{
    namespace Models
    {
        template <typename T, bool UseCompare>
        CListModelBase<T, UseCompare>::CListModelBase(const QString &translationContext, QObject *parent)
            : CListModelBaseNonTemplate(translationContext, parent)
        { }

        template <typename T, bool UseCompare>
        int CListModelBase<T, UseCompare>::rowCount(const QModelIndex &parentIndex) const
        {
            Q_UNUSED(parentIndex)
            return this->containerOrFilteredContainer().size();
        }

        template <typename T, bool UseCompare>
        bool CListModelBase<T, UseCompare>::canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const
        {
            Q_UNUSED(action)
            Q_UNUSED(row)
            Q_UNUSED(column)
            Q_UNUSED(parent)
            if (!this->isDropAllowed()) { return false; }
            if (!this->acceptDrop(data)) { return false; }
            return true;
        }

        template <typename T, bool UseCompare>
        bool CListModelBase<T, UseCompare>::dropMimeData(const QMimeData *mimeData, Qt::DropAction action, int row, int column, const QModelIndex &parent)
        {
            Q_UNUSED(row)
            Q_UNUSED(column)

            if (!this->isOrderable() || !this->acceptDrop(mimeData)) { return false; }
            const CVariant valueVariant(this->toCVariant(mimeData));
            if (valueVariant.isValid())
            {
                if (action == Qt::MoveAction)
                {
                    const ContainerType container(valueVariant.value<ContainerType>());
                    if (container.isEmpty()) { return false; }
                    const int position = parent.row();
                    this->moveItems(container, position);
                }
            }
            return true;
        }

        template <typename T, bool UseCompare>
        bool CListModelBase<T, UseCompare>::isValidIndex(const QModelIndex &index) const
        {
            if (!index.isValid()) { return false; }
            return (index.row() >= 0 && index.row() < this->rowCount(index) &&
                    index.column() >= 0 && index.column() < this->columnCount(index));
        }

        template <typename T, bool UseCompare>
        QVariant CListModelBase<T, UseCompare>::data(const QModelIndex &index, int role) const
        {
            // check / init
            if (!this->isValidIndex(index)) { return QVariant(); }

            if (role == Qt::BackgroundRole)
            {
                return CListModelBaseNonTemplate::data(index, role);
            }

            // Formatter
            const CDefaultFormatter *formatter = m_columns.getFormatter(index);
            Q_ASSERT_X(formatter, Q_FUNC_INFO, "Missing formatter");

            // Upfront checking avoids unnecessary data fetching
            if (!formatter || !formatter->supportsRole(role)) { return CListModelBaseNonTemplate::data(index, role); }

            // index, updront checking
            const int row = index.row();
            const int col = index.column();
            const CPropertyIndex propertyIndex = this->columnToPropertyIndex(col);
            const int propertyIndexFront = propertyIndex.frontCasted<int>();

            // special cases
            switch (propertyIndexFront)
            {
            case CPropertyIndex::GlobalIndexLineNumber: return QVariant::fromValue(row + 1);
            case CPropertyIndex::GlobalIndexEmpty: return {};
            default: break; // continue here
            }

            // Formatted data
            const ObjectType obj = this->containerOrFilteredContainer()[row];
            return formatter->data(role, obj.propertyByIndex(propertyIndex)).getQVariant();
        }

        template <typename T, bool UseCompare>
        bool CListModelBase<T, UseCompare>::setData(const QModelIndex &index, const QVariant &value, int role)
        {
            Qt::ItemDataRole dataRole = static_cast<Qt::ItemDataRole>(role);
            if (!(dataRole == Qt::UserRole || dataRole == Qt::EditRole)) { return false; }

            // check / init
            if (!this->isValidIndex(index)) { return false; }
            if (!m_columns.isEditable(index)) { return false; }
            const CDefaultFormatter *formatter = m_columns.getFormatter(index);
            Q_ASSERT(formatter);
            if (!formatter) { return false; }

            ObjectType obj = m_container[index.row()];
            const ObjectType currentObject(obj);
            const CPropertyIndex propertyIndex = this->columnToPropertyIndex(index.column());
            obj.setPropertyByIndex(propertyIndex, value);

            if (obj != currentObject)
            {
                const QModelIndex topLeft = index.sibling(index.row(), 0);
                const QModelIndex bottomRight = index.sibling(index.row(), this->columnCount() - 1);
                m_container[index.row()] = obj;
                const CVariant co = CVariant::fromValue(obj);
                emit objectChanged(co, propertyIndex);
                emit this->dataChanged(topLeft, bottomRight);
                this->updateFilteredContainer();
                return true;
            }
            return false;
        }

        template <typename T, bool UseCompare>
        bool CListModelBase<T, UseCompare>::setInContainer(const QModelIndex &index, const ObjectType &obj)
        {
            if (!index.isValid()) { return false; }
            const int row = index.row();
            if (row < 0 || row >= this->container().size()) { return false; }
            m_container[row] = obj;
            return true;
        }

        template <typename T, bool UseCompare>
        int CListModelBase<T, UseCompare>::update(const ContainerType &container, bool sort)
        {
            if (m_modelDestroyed) { return 0; }

            // Keep sorting out of begin/end reset model
            ContainerType sortedContainer;
            ContainerType selection;
            if (m_selectionModel)
            {
                selection = m_selectionModel->selectedObjects();
            }
            const int oldSize = m_container.size();
            const bool performSort = sort && container.size() > 1 && this->hasValidSortColumn();
            if (performSort)
            {
                const int sortColumn = this->getSortColumn();
                sortedContainer = this->sortContainerByColumn(container, sortColumn, m_sortOrder);
            }

            this->beginResetModel();
            m_container = performSort ? sortedContainer : container;
            this->updateFilteredContainer(); // use sorted container for filtered if applicable
            this->endResetModel();

            // reselect if implemented in specialized view
            if (!selection.isEmpty())
            {
                m_selectionModel->selectObjects(selection);
            }

            const int newSize = m_container.size();
            Q_UNUSED(oldSize)

            // I have to update even with same size because I cannot tell what/if data are changed
            this->emitModelDataChanged();
            return newSize;
        }

        template <typename T, bool UseCompare>
        void CListModelBase<T, UseCompare>::update(const QModelIndex &index, const ObjectType &object)
        {
            if (m_modelDestroyed) { return; }
            if (index.row() >= m_container.size()) { return; }
            m_container[index.row()] = object;

            const QModelIndex i1 = index.sibling(index.row(), 0);
            const QModelIndex i2 = index.sibling(index.row(), this->columnCount(index) - 1);
            emit this->dataChanged(i1, i2); // which range has been changed
        }

        template <typename T, bool UseCompare>
        void CListModelBase<T, UseCompare>::update(int rowIndex, const ObjectType &object)
        {
            this->update(this->index(rowIndex, 0), object);
        }

        template <typename T, bool UseCompare>
        CWorker *CListModelBase<T, UseCompare>::updateAsync(const ContainerType &container, bool sort)
        {
            Q_UNUSED(sort)
            if (m_modelDestroyed) { return nullptr; }
            const auto sortColumn = this->getSortColumn();
            const auto sortOrder  = this->getSortOrder();
            CWorker *worker = CWorker::fromTask(this, "ModelSort", [this, container, sortColumn, sortOrder]()
            {
                return this->sortContainerByColumn(container, sortColumn, sortOrder);
            });
            worker->thenWithResult<ContainerType>(this, [this](const ContainerType & sortedContainer)
            {
                if (m_modelDestroyed) { return;  }
                this->update(sortedContainer, false);
            });
            worker->then(this, &CListModelBase::asyncUpdateFinished);
            return worker;
        }

        template <typename T, bool UseCompare>
        void CListModelBase<T, UseCompare>::updateContainerMaybeAsync(const ContainerType &container, bool sort)
        {
            if (m_modelDestroyed) { return; }
            if (container.size() > asyncThreshold && sort)
            {
                // larger container with sorting
                this->updateAsync(container, sort);
            }
            else
            {
                this->update(container, sort);
            }
        }

        template <typename T, bool UseCompare>
        bool CListModelBase<T, UseCompare>::hasFilter() const
        {
            const bool f =  m_filter && m_filter->isValid();
            return f;
        }

        template <typename T, bool UseCompare>
        void CListModelBase<T, UseCompare>::removeFilter()
        {
            if (!this->hasFilter()) { return; }
            m_filter.reset(nullptr);
            this->beginResetModel();
            this->updateFilteredContainer();
            this->endResetModel();
            this->emitModelDataChanged();
        }

        template <typename T, bool UseCompare>
        void CListModelBase<T, UseCompare>::takeFilterOwnership(std::unique_ptr<IModelFilter<ContainerType> > &filter)
        {
            ContainerType selection;
            if (m_selectionModel)
            {
                selection = m_selectionModel->selectedObjects();
            }

            if (!filter)
            {
                this->removeFilter(); // clear filter
                return;
            }
            if (filter->isValid())
            {
                m_filter = std::move(filter);
                this->beginResetModel();
                this->updateFilteredContainer();
                this->endResetModel();
                this->emitModelDataChanged();
            }
            else
            {
                // invalid filter, so clear filter
                this->removeFilter();
            }

            // reselect if implemented in specialized views
            if (!selection.isEmpty())
            {
                m_selectionModel->selectObjects(selection);
            }
        }

        template <typename T, bool UseCompare>
        const typename CListModelBase<T, UseCompare>::ObjectType &CListModelBase<T, UseCompare>::at(const QModelIndex &index) const
        {
            if (index.row() < 0 || index.row() >= this->rowCount())
            {
                static const ObjectType def {}; // default object
                return def;
            }
            else
            {
                return this->containerOrFilteredContainer()[index.row()];
            }
        }

        template <typename T, bool UseCompare>
        const typename CListModelBase<T, UseCompare>::ContainerType &CListModelBase<T, UseCompare>::container() const
        {
            return m_container;
        }

        template <typename T, bool UseCompare>
        const typename CListModelBase<T, UseCompare>::ContainerType &CListModelBase<T, UseCompare>::containerFiltered() const
        {
            return m_containerFiltered;
        }

        template <typename T, bool UseCompare>
        const typename CListModelBase<T, UseCompare>::ContainerType &CListModelBase<T, UseCompare>::containerOrFilteredContainer(bool *filtered) const
        {
            if (this->hasFilter())
            {
                if (filtered) { *filtered = true; }
                return m_containerFiltered;
            }
            else
            {
                if (filtered) { *filtered = false; }
                return m_container;
            }
        }

        template <typename T, bool UseCompare>
        void CListModelBase<T, UseCompare>::push_back(const ObjectType &object)
        {
            beginInsertRows(QModelIndex(), m_container.size(), m_container.size());
            m_container.push_back(object);
            endInsertRows();

            if (this->hasFilter())
            {
                // this will change the whole used model as we cannot predict the filter
                this->beginResetModel();
                this->updateFilteredContainer();
                this->endResetModel();
            }
            this->emitModelDataChanged();
        }

        template<typename T, bool UseCompare>
        void CListModelBase<T, UseCompare>::push_back(const ContainerType &container)
        {
            beginInsertRows(QModelIndex(), m_container.size(), m_container.size());
            m_container.push_back(container);
            endInsertRows();

            if (this->hasFilter())
            {
                // this will change the whole used model as we cannot predict the filter
                this->beginResetModel();
                this->updateFilteredContainer();
                this->endResetModel();
            }
            this->emitModelDataChanged();
        }

        template <typename T, bool UseCompare>
        void CListModelBase<T, UseCompare>::insert(const ObjectType &object)
        {
            beginInsertRows(QModelIndex(), 0, 0);
            m_container.insert(m_container.begin(), object);
            endInsertRows();

            if (this->hasFilter())
            {
                // this will change the whole used model as we cannot predict the filter
                this->beginResetModel();
                this->updateFilteredContainer();
                this->endResetModel();
            }
            this->emitModelDataChanged();
        }

        template <typename T, bool UseCompare>
        void CListModelBase<T, UseCompare>::insert(const ContainerType &container)
        {
            if (container.isEmpty()) { return; }
            beginInsertRows(QModelIndex(), 0, 0);
            m_container.push_back(container);
            endInsertRows();

            if (this->hasFilter())
            {
                // this will change the whole used model as we cannot predict the filter
                this->beginResetModel();
                this->updateFilteredContainer();
                this->endResetModel();
            }
            this->emitModelDataChanged();
        }

        template <typename T, bool UseCompare>
        void CListModelBase<T, UseCompare>::remove(const ObjectType &object)
        {
            const int oldSize = m_container.size();
            beginRemoveRows(QModelIndex(), 0, 0);
            m_container.remove(object);
            endRemoveRows();
            const int newSize = m_container.size();
            if (oldSize != newSize)
            {
                this->emitModelDataChanged();
                if (this->hasFilter())
                {
                    this->beginResetModel();
                    this->updateFilteredContainer();
                    this->endResetModel();
                }
            }
        }

        template <typename T, bool UseCompare>
        void CListModelBase<T, UseCompare>::clear()
        {
            beginResetModel();
            m_container.clear();
            m_containerFiltered.clear();
            endResetModel();
            this->emitModelDataChanged();
        }

        template <typename T, bool UseCompare>
        bool CListModelBase<T, UseCompare>::isEmpty() const
        {
            return m_container.isEmpty();
        }

        template <typename T, bool UseCompare>
        void CListModelBase<T, UseCompare>::updateFilteredContainer()
        {
            if (this->hasFilter())
            {
                m_containerFiltered = m_filter->filter(m_container);
            }
            else
            {
                m_containerFiltered.clear();
            }
        }

        template <typename T, bool UseCompare>
        void CListModelBase<T, UseCompare>::emitModelDataChanged()
        {
            const int n = this->containerOrFilteredContainer().size();
            emit this->modelDataChanged(n, this->hasFilter());
            emit this->changed();
        }

        template <typename T, bool UseCompare>
        void CListModelBase<T, UseCompare>::onDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
        {
            // underlying base class changed
            Q_UNUSED(topLeft)
            Q_UNUSED(bottomRight)
            Q_UNUSED(roles)
            this->emitModelDataChanged();
        }

        template <typename T, bool UseCompare>
        void CListModelBase<T, UseCompare>::onChangedDigest()
        {
            const int n = this->containerOrFilteredContainer().size();
            emit this->changedDigest();
            emit this->modelDataChangedDigest(n, this->hasFilter());
        }

        template <typename T, bool UseCompare>
        void CListModelBase<T, UseCompare>::moveItems(const ContainerType &items, int position)
        {
            // overridden in specialized class
            Q_UNUSED(items)
            Q_UNUSED(position)
        }

        template <typename T, bool UseCompare>
        void CListModelBase<T, UseCompare>::sort()
        {
            this->sort(this->getSortColumn(), this->getSortOrder());
        }

        template<typename T, bool UseCompare>
        void CListModelBase<T, UseCompare>::resort()
        {
            // sort the values
            this->updateContainerMaybeAsync(m_container, true);
        }

        template <typename T, bool UseCompare>
        void CListModelBase<T, UseCompare>::sort(int column, Qt::SortOrder order)
        {
            if (column == m_sortColumn && order == m_sortOrder) { return; }

            // new order
            m_sortColumn = column;
            m_sortOrder  = order;
            if (m_container.size() < 2)
            {
                return; // nothing to do
            }

            // sort the values
            this->updateContainerMaybeAsync(m_container, true);
        }

        template <typename T, bool UseCompare>
        void CListModelBase<T, UseCompare>::truncate(int maxNumber, bool forceSort)
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

        template <typename T, bool UseCompare>
        typename CListModelBase<T, UseCompare>::ContainerType CListModelBase<T, UseCompare>::sortContainerByColumn(const ContainerType &container, int column, Qt::SortOrder order) const
        {
            if (m_modelDestroyed) { return container; }
            if (container.size() < 2 || !m_columns.isSortable(column))
            {
                return container;    // nothing to do
            }

            // this is the only part not really thread safe, but columns do not change so far
            const CPropertyIndex propertyIndex = m_columns.columnToSortPropertyIndex(column);
            Q_ASSERT(!propertyIndex.isEmpty());
            if (propertyIndex.isEmpty())
            {
                return container;    // at release build do nothing
            }

            // sort the values
            const auto tieBreakersCopy = m_sortTieBreakers; //! \todo workaround T579 still not thread-safe, but less likely to crash
            const std::integral_constant<bool, UseCompare> marker {};
            const auto p = [ = ](const ObjectType & a, const ObjectType & b) -> bool
            {
                return Private::compareForModelSort<ObjectType>(a, b, order, propertyIndex, tieBreakersCopy, marker);
            };

            return container.sorted(p);
        }

        template <typename T, bool UseCompare>
        QMimeData *CListModelBase<T, UseCompare>::mimeData(const QModelIndexList &indexes) const
        {
            QMimeData *mimeData = new QMimeData();
            if (indexes.isEmpty()) { return mimeData; }

            ContainerType container;
            QList<int> rows; // avoid redundant objects

            // Indexes are per row and column
            for (const QModelIndex &index : indexes)
            {
                if (!index.isValid()) { continue; }
                const int r = index.row();
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

        template <typename T, bool UseCompare>
        QJsonObject CListModelBase<T, UseCompare>::toJson(bool selectedOnly) const
        {
            const CVariant variant = CVariant::fromValue(
                                         selectedOnly && m_selectionModel ?
                                         m_selectionModel->selectedObjects() :
                                         container());
            return variant.toJson();
        }

        template <typename T, bool UseCompare>
        QString CListModelBase<T, UseCompare>::toJsonString(QJsonDocument::JsonFormat format, bool selectedOnly) const
        {
            const CVariant variant = CVariant::fromValue(
                                         selectedOnly && m_selectionModel ?
                                         m_selectionModel->selectedObjects() :
                                         container());
            return variant.toJsonString(format);
        }

        template <typename T, bool UseCompare>
        bool CListModelBase<T, UseCompare>::isOrderable() const
        {
            return false;
        }

    } // namespace
} // namespace
