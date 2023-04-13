/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_MODELS_LISTMODELBASENONTEMPLATE_H
#define BLACKGUI_MODELS_LISTMODELBASENONTEMPLATE_H

#include "blackgui/models/columns.h"
#include "blackgui/dropbase.h"
#include "blackgui/blackguiexport.h"
#include "blackmisc/propertyindexlist.h"
#include "blackmisc/digestsignal.h"
#include "blackmisc/variant.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QModelIndex>
#include <QModelIndexList>
#include <QStandardItemModel>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QVector>

class QMimeData;
class QModelIndex;

namespace BlackMisc
{
    class CWorker;
}
namespace BlackGui::Models
{
    //! Non templated base class, allows Q_OBJECT and signals to be used
    class BLACKGUI_EXPORT CListModelBaseNonTemplate :
        public QStandardItemModel,
        public CDropBase
    {
        Q_OBJECT

    public:
        //! Number of elements when to use asynchronous updates
        static constexpr int asyncThreshold = 50;

        //! Destructor
        virtual ~CListModelBaseNonTemplate() override {}

        //! \name Functions from QStandardItemModel
        //! @{
        //! \copydoc QStandardItemModel::columnCount
        virtual int columnCount(const QModelIndex &modelIndex = QModelIndex()) const final override;

        //! \copydoc QStandardItemModel::headerData
        virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const final override;

        //! \copydoc QStandardItemModel::index
        virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const final override;

        //! \copydoc QStandardItemModel::parent
        virtual QModelIndex parent(const QModelIndex &child) const final override;

        //! \copydoc QStandardItemModel::flags
        virtual Qt::ItemFlags flags(const QModelIndex &index) const final override;

        //! \copydoc QStandardItemModel::supportedDragActions
        virtual Qt::DropActions supportedDragActions() const final override;

        //! \copydoc QStandardItemModel::supportedDropActions
        virtual Qt::DropActions supportedDropActions() const final override;

        //! \copydoc QStandardItemModel::mimeTypes
        virtual QStringList mimeTypes() const final override;
        //! @}

        //! Column to property index
        virtual BlackMisc::CPropertyIndex columnToPropertyIndex(int column) const;

        //! Property index to column number
        virtual int propertyIndexToColumn(const BlackMisc::CPropertyIndex &propertyIndex) const;

        //! Index to property index
        virtual BlackMisc::CPropertyIndex modelIndexToPropertyIndex(const QModelIndex &index) const;

        //! Set sort column
        virtual void setSortColumn(int column) { m_sortColumn = column; }

        //! Disable sorting
        void setNoSorting() { this->setSortColumn(-1); }

        //! Sort by index
        void sortByPropertyIndex(const BlackMisc::CPropertyIndex &propertyIndex, Qt::SortOrder order = Qt::AscendingOrder);

        //! Set column for sorting
        //! \param propertyIndex index of column to be sorted
        virtual bool setSortColumnByPropertyIndex(const BlackMisc::CPropertyIndex &propertyIndex);

        //! Sorting
        virtual bool setSorting(const BlackMisc::CPropertyIndex &propertyIndex, Qt::SortOrder order = Qt::AscendingOrder);

        //! Get sort column property index
        virtual int getSortColumn() const { return m_sortColumn; }

        //! Sorted by property
        BlackMisc::CPropertyIndex getSortProperty() const;

        //! Has valid sort column?
        virtual bool hasValidSortColumn() const;

        //! Get sort order
        virtual Qt::SortOrder getSortOrder() const { return m_sortOrder; }

        //! Orderable, normally use a container BlackMisc::IOrderableList
        virtual bool isOrderable() const = 0;

        //! Mark as about to be destroyed, normally marked from view
        void markDestroyed();

        //! Model about to be destroyed?
        bool isModelDestroyed();

        //! Remove all highlighting
        virtual void clearHighlighting();

        //! Has highlighted rows?
        virtual bool hasHighlightedRows() const;

        //! Drop actions
        void setDropActions(Qt::DropActions dropActions) { m_dropActions = dropActions; }

        //! Send signal that data have been changed.
        //! \note Meant for scenarios where the container is directly updated and a subsequent signal is required
        void emitDataChanged(int startRowIndex, int endRowIndex);

        //! Convert to JSON
        virtual QJsonObject toJson(bool selectedOnly = false) const = 0;

        //! Convert to JSON string
        virtual QString toJsonString(QJsonDocument::JsonFormat format = QJsonDocument::Indented, bool selectedOnly = false) const = 0;

        //! The columns
        const CColumns &getColumns() const { return m_columns; }

        //! Using void column at the end?
        bool endsWithEmptyColumn() const { return m_columns.endsWithEmptyColumn(); }

    signals:
        //! Asynchronous update finished
        void asyncUpdateFinished();

        //! Data changed
        //! \remark passing back selected objects so they can be reselected
        //! \remark condsider modelDataChangedDigest for performance reasons
        void modelDataChanged(int count, bool withFilter);

        //! Data changed, digest version
        void modelDataChangedDigest(int count, bool withFilter);

        //! Model has been changed
        //! Triggered with each change, for performance consider using changedDigest
        void changed();

        //! Model has been changed, digest signal
        void changedDigest();

        //! Template free information, that object changed
        void objectChanged(const BlackMisc::CVariant &object, const BlackMisc::CPropertyIndex &changedIndex);

    protected:
        //! Feedback when QStandardItemModel::dataChanged was called
        virtual void onDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles) = 0;

        //! Digest signal
        virtual void onChangedDigest() = 0;

        //! Constructor
        //! \param translationContext I18N context
        //! \param parent
        CListModelBaseNonTemplate(const QString &translationContext, QObject *parent = nullptr);

        CColumns m_columns; //!< columns metadata
        int m_sortColumn; //!< currently sorted column
        bool m_modelDestroyed = false; //!< \todo rudimentary workaround for T579, can be removed
        Qt::SortOrder m_sortOrder; //!< sort order (asc/desc)
        Qt::DropActions m_dropActions = Qt::IgnoreAction; //!< drop actions
        BlackMisc::CPropertyIndexList m_sortTieBreakers; //!< how column values are sorted if equal, if no value is given this is random

    private:
        BlackMisc::CDigestSignal m_dsModelsChanged { this, &CListModelBaseNonTemplate::changed, &CListModelBaseNonTemplate::onChangedDigest, 500, 10 };
    };

} // namespace
#endif // guard
