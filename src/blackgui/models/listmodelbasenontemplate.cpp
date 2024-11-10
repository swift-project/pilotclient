// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

// Drag and drop docu:
// http://doc.qt.io/qt-5/model-view-programming.html#using-drag-and-drop-with-item-views

#include "blackgui/models/listmodelbasenontemplate.h"
#include "blackmisc/verify.h"

using namespace BlackMisc;

namespace BlackGui::Models
{
    int CListModelBaseNonTemplate::columnCount(const QModelIndex &modelIndex) const
    {
        Q_UNUSED(modelIndex)
        int c = m_columns.size();
        return c;
    }

    QVariant CListModelBaseNonTemplate::headerData(int section, Qt::Orientation orientation, int role) const
    {
        if (orientation != Qt::Horizontal) { return QVariant(); }
        const bool handled = (role == Qt::DisplayRole || role == Qt::ToolTipRole || role == Qt::InitialSortOrderRole);
        if (!handled) { return QVariant(); }
        if (section < 0 || section >= m_columns.size()) { return QVariant(); }

        if (role == Qt::DisplayRole)
        {
            return QVariant(m_columns.at(section).getColumnName());
        }
        if (role == Qt::ToolTipRole)
        {
            return QVariant(m_columns.at(section).getColumnToolTip());
        }
        return QVariant();
    }

    QModelIndex CListModelBaseNonTemplate::index(int row, int column, const QModelIndex &parent) const
    {
        Q_UNUSED(parent)
        return QStandardItemModel::createIndex(row, column);
    }

    QModelIndex CListModelBaseNonTemplate::parent(const QModelIndex &child) const
    {
        Q_UNUSED(child)
        return QModelIndex();
    }

    CPropertyIndex CListModelBaseNonTemplate::columnToPropertyIndex(int column) const
    {
        return m_columns.columnToPropertyIndex(column);
    }

    int CListModelBaseNonTemplate::propertyIndexToColumn(const CPropertyIndex &propertyIndex) const
    {
        return m_columns.propertyIndexToColumn(propertyIndex);
    }

    CPropertyIndex CListModelBaseNonTemplate::modelIndexToPropertyIndex(const QModelIndex &index) const
    {
        return this->columnToPropertyIndex(index.column());
    }

    void CListModelBaseNonTemplate::sortByPropertyIndex(const CPropertyIndex &propertyIndex, Qt::SortOrder order)
    {
        const int column = this->propertyIndexToColumn(propertyIndex);
        this->sort(column, order);
    }

    bool CListModelBaseNonTemplate::setSortColumnByPropertyIndex(const CPropertyIndex &propertyIndex)
    {
        const int column = m_columns.propertyIndexToColumn(propertyIndex);
        if (m_sortColumn == column) { return false; } // not changed
        m_sortColumn = column;
        return true; // changed
    }

    bool CListModelBaseNonTemplate::setSorting(const CPropertyIndex &propertyIndex, Qt::SortOrder order)
    {
        if (propertyIndex.isEmpty())
        {
            this->setNoSorting();
            return false;
        }

        const bool changedColumn = this->setSortColumnByPropertyIndex(propertyIndex);
        const bool changedOrder = (m_sortOrder == order);
        m_sortOrder = order;
        return changedColumn || changedOrder;
    }

    CPropertyIndex CListModelBaseNonTemplate::getSortProperty() const
    {
        if (!this->hasValidSortColumn()) { return CPropertyIndex::empty(); }
        return m_columns.at(m_sortColumn).getPropertyIndex();
    }

    bool CListModelBaseNonTemplate::hasValidSortColumn() const
    {
        if (!(m_sortColumn >= 0 && m_sortColumn < m_columns.size())) { return false; }
        return m_columns.isSortable(m_sortColumn);
    }

    Qt::ItemFlags CListModelBaseNonTemplate::flags(const QModelIndex &index) const
    {
        Qt::ItemFlags f = QStandardItemModel::flags(index);
        if (!index.isValid()) { return f; }
        const bool editable = m_columns.isEditable(index);
        f = editable ? (f | Qt::ItemIsEditable) : (f & ~Qt::ItemIsEditable);

        // flags from formatter
        const CDefaultFormatter *formatter = m_columns.getFormatter(index);
        if (formatter) { f = formatter->flags(f, editable); }

        // drag and drop
        f = f | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
        return f;
    }

    Qt::DropActions CListModelBaseNonTemplate::supportedDragActions() const
    {
        return isOrderable() ? Qt::CopyAction | Qt::MoveAction : Qt::CopyAction;
    }

    Qt::DropActions CListModelBaseNonTemplate::supportedDropActions() const
    {
        return m_dropActions;
    }

    QStringList CListModelBaseNonTemplate::mimeTypes() const
    {
        static const QStringList mimes({ "application/swift.container.json" });
        return mimes;
    }

    void CListModelBaseNonTemplate::markDestroyed()
    {
        m_modelDestroyed = true;
    }

    bool CListModelBaseNonTemplate::isModelDestroyed()
    {
        return m_modelDestroyed;
    }

    void CListModelBaseNonTemplate::clearHighlighting()
    {
        // can be overridden to delete highlighting
    }

    bool CListModelBaseNonTemplate::hasHighlightedRows() const
    {
        return false;
        // can be overridden to enable highlighting based operations
    }

    void CListModelBaseNonTemplate::emitDataChanged(int startRowIndex, int endRowIndex)
    {
        SWIFT_VERIFY_X(startRowIndex <= endRowIndex, Q_FUNC_INFO, "check rows");
        SWIFT_VERIFY_X(startRowIndex >= 0 && endRowIndex >= 0, Q_FUNC_INFO, "check rows");

        const int columns = columnCount();
        const int rows = rowCount();
        if (columns < 1) { return; }
        if (startRowIndex < 0) { startRowIndex = 0; }
        if (endRowIndex >= rows) { endRowIndex = rows - 1; }
        const QModelIndex topLeft(createIndex(startRowIndex, 0));
        const QModelIndex bottomRight(createIndex(endRowIndex, columns - 1));
        emit this->dataChanged(topLeft, bottomRight);
    }

    CListModelBaseNonTemplate::CListModelBaseNonTemplate(const QString &translationContext, QObject *parent)
        : QStandardItemModel(parent), m_columns(translationContext), m_sortColumn(-1), m_sortOrder(Qt::AscendingOrder)
    {
        // non unique default name, set translation context as default
        this->setObjectName(translationContext);

        // connect
        connect(this, &CListModelBaseNonTemplate::dataChanged, this, &CListModelBaseNonTemplate::onDataChanged);
    }

} // namespace
