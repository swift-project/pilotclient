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

#include "blackgui/blackguiexport.h"
#include "blackgui/dropbase.h"
#include "blackgui/models/columns.h"
#include "blackgui/models/modelfilter.h"
#include "blackgui/models/selectionmodel.h"
#include "blackmisc/digestsignal.h"
#include "blackmisc/variant.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QModelIndex>
#include <QModelIndexList>
#include <QObject>
#include <QStandardItemModel>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QVector>
#include <Qt>
#include <memory>
#include <type_traits>

class QMimeData;
class QModelIndex;

namespace BlackMisc { class CWorker; }
namespace BlackGui
{
    namespace Models
    {
        //! Non templated base class, allows Q_OBJECT and signals to be used
        class BLACKGUI_EXPORT CListModelBaseNonTemplate :
            public QStandardItemModel,
            public BlackGui::CDropBase
        {
            Q_OBJECT

        public:
            //! Number of elements when to use asynchronous updates
            static constexpr int asyncThreshold = 50;

            //! Destructor
            virtual ~CListModelBaseNonTemplate() {}

            //! \name Functions from QStandardItemModel
            //! @{
            virtual int columnCount(const QModelIndex &modelIndex = QModelIndex()) const final override;
            virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const final override;
            virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const final override;
            virtual QModelIndex parent(const QModelIndex &child) const final override;
            virtual Qt::ItemFlags flags(const QModelIndex &index) const final override;
            virtual Qt::DropActions supportedDragActions() const final override;
            virtual Qt::DropActions supportedDropActions() const final override;
            virtual QStringList mimeTypes() const final override;
            //! @}

            //! Column to property index
            virtual BlackMisc::CPropertyIndex columnToPropertyIndex(int column) const;

            //! Property index to column number
            virtual int propertyIndexToColumn(const BlackMisc::CPropertyIndex &propertyIndex) const;

            //! Index to property index
            virtual BlackMisc::CPropertyIndex modelIndexToPropertyIndex(const QModelIndex &index) const;

            //! Set sort column
            virtual void setSortColumn(int column) { this->m_sortColumn = column; }

            //! Sort by index
            void sortByPropertyIndex(const BlackMisc::CPropertyIndex &propertyIndex, Qt::SortOrder order = Qt::AscendingOrder);

            //! Set column for sorting
            //! \param propertyIndex index of column to be sorted
            virtual void setSortColumnByPropertyIndex(const BlackMisc::CPropertyIndex &propertyIndex);

            //! Sorting
            virtual void setSorting(const BlackMisc::CPropertyIndex &propertyIndex, Qt::SortOrder order = Qt::AscendingOrder);

            //! Get sort column property index
            virtual int getSortColumn() const { return this->m_sortColumn; }

            //! Has valid sort column?
            virtual bool hasValidSortColumn() const;

            //! Get sort order
            virtual Qt::SortOrder getSortOrder() const { return this->m_sortOrder; }

            //! Translation context
            virtual const QString &getTranslationContext() const;

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
            void setDropActions(Qt::DropActions dropActions) { this->m_dropActions = dropActions; }

            //! Send signal that data have been changed.
            //! \note Meant for scenarios where the container is directly updated and a subsequent signal is required
            void emitDataChanged(int startRowIndex, int endRowIndex);

            //! Convert to JSON
            virtual QJsonObject toJson() const = 0;

            //! Convert to JSON string
            virtual QString toJsonString(QJsonDocument::JsonFormat format = QJsonDocument::Indented) const = 0;

        signals:
            //! Asynchronous update finished
            void asyncUpdateFinished();

            //! Data changed
            //! \remark passing back selected objects so they can be reselected
            void modelDataChanged(int count, bool withFilter);

            //! Data changed, digest version
            void modelDataChangedDigest(int count, bool withFilter);

            //! Model has been changed
            void changed();

            //! Model has been changed, digest signal
            void changedDigest();

            //! Template free information, that object changed
            void objectChanged(const BlackMisc::CVariant &object, const BlackMisc::CPropertyIndex &changedIndex);

        protected slots:
            //! Feedback when QStandardItemModel::dataChanged was called
            virtual void ps_onDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles) = 0;

            //! Digest signal
            virtual void ps_onChangedDigest() = 0;

        protected:
            //! Constructor
            //! \param translationContext I18N context
            //! \param parent
            CListModelBaseNonTemplate(const QString &translationContext, QObject *parent = nullptr);

            CColumns        m_columns;                         //!< columns metadata
            int             m_sortColumn;                      //!< currently sorted column
            bool            m_modelDestroyed = false;          //!< model is about to be destroyed
            Qt::SortOrder   m_sortOrder;                       //!< sort order (asc/desc)
            Qt::DropActions m_dropActions = Qt::IgnoreAction;  //!< drop actions

        private:
            BlackMisc::CDigestSignal m_dsModelsChanged { this, &CListModelBaseNonTemplate::changed, &CListModelBaseNonTemplate::ps_onChangedDigest, 500, 10 };
        };

        //! List model
        template <typename ObjectType, typename ContainerType, bool UseCompare = false> class CListModelBase : public CListModelBaseNonTemplate
        {
        public:
            //! Destructor
            virtual ~CListModelBase() {}

            //! \name Functions from QStandardItemModel
            //! @{
            virtual QVariant data(const QModelIndex &index, int role) const override;
            virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) final override;
            virtual QMimeData *mimeData(const QModelIndexList &indexes) const final override;
            virtual void sort(int column, Qt::SortOrder order) final override;
            virtual int rowCount(const QModelIndex &parentIndex = QModelIndex()) const final override;
            virtual bool canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const final override;
            virtual bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) final override;
            //! @}

            //! \name Functions from CListModelBaseNonTemplate
            //! @{
            virtual QJsonObject toJson() const override;
            virtual QString toJsonString(QJsonDocument::JsonFormat format = QJsonDocument::Indented) const override;
            virtual bool isOrderable() const override;
            //! @}

            //! Valid index (in range)
            virtual bool isValidIndex(const QModelIndex &index) const;

            //! Used container data
            const ContainerType &container() const;

            //! Used container data
            const ContainerType &containerFiltered() const;

            //! Full container or cached filtered container as approproiate
            const ContainerType &containerOrFilteredContainer(bool *filtered = nullptr) const;

            //! Simple set of data in container, using class is responsible for firing signals etc.
            //! \sa sendDataChanged
            bool setInContainer(const QModelIndex &index, const ObjectType &obj);

            //! Update by new container
            //! \return int size after update
            //! \remarks a sorting is performed only if a valid sort column is set
            virtual int update(const ContainerType &container, bool sort = true);

            //! Asynchronous update
            virtual BlackMisc::CWorker *updateAsync(const ContainerType &container, bool sort = true);

            //! Update by new container
            virtual void updateContainerMaybeAsync(const ContainerType &container, bool sort = true);

            //! Update single element
            virtual void update(const QModelIndex &index, const ObjectType &object);

            //! Update single element
            virtual void update(int rowIndex, const ObjectType &object);

            //! Move items to position, normally called from dropMimeData
            //! \sa dropMimeData
            virtual void moveItems(const ContainerType &items, int position);

            //! Object at row position
            virtual const ObjectType &at(const QModelIndex &index) const;

            //! Sort by given sort order \sa getSortColumn() \sa getSortOrder()
            void sort();

            //! Truncate to given number
            void truncate(int maxNumber, bool forceSort = false);

            //! Sort container by given column / order. This is used by sort() but als
            //! for asynchronous updates in the views
            //! \param container used list
            //! \param column    column inder
            //! \param order     sort order (ascending / descending)
            //! \threadsafe under normal conditions thread safe as long as the column metadata are not changed
            ContainerType sortContainerByColumn(const ContainerType &container, int column, Qt::SortOrder order) const;

            //! Similar to ContainerType::push_back
            virtual void push_back(const ObjectType &object);

            //! Similar to ContainerType::push_back
            virtual void push_back(const ContainerType &container);

            //! Similar to ContainerType::insert here inserts at first position
            virtual void insert(const ObjectType &object);

            //! Similar to ContainerType::insert here inserts at first position
            virtual void insert(const ContainerType &container);

            //! Remove object
            virtual void remove(const ObjectType &object);

            //! \copydoc BlackMisc::CContainerBase::removeIf
            template <class K0, class V0, class... KeysValues>
            int removeIf(K0 k0, V0 v0, KeysValues... keysValues)
            {
                int c = m_container.removeIf(BlackMisc::Predicates::MemberEqual(k0, v0, keysValues...));
                if (c > 0) { this->emitModelDataChanged();}
                this->updateFilteredContainer();
                return c;
            }

            //! Clear the list
            virtual void clear();

            //! Empty?
            virtual bool isEmpty() const;

            //! Filter available
            bool hasFilter() const;

            //! Remove filter
            void removeFilter();

            //! Set the filter
            void takeFilterOwnership(std::unique_ptr<IModelFilter<ContainerType> > &filter);

            //! Set the selection model
            void setSelectionModel(BlackGui::Models::ISelectionModel<ContainerType> *selectionModel)
            {
                m_selectionModel = selectionModel;
            }

        protected:
            //! Constructor
            CListModelBase(const QString &translationContext, QObject *parent = nullptr);

            //! \name Base class overrides
            //! @{
            virtual void ps_onDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomLeft, const QVector<int> &roles) override;
            virtual void ps_onChangedDigest() override;
            //! @}

            //! Update filtered container
            void updateFilteredContainer();

            //! Model changed
            void emitModelDataChanged();

            ContainerType m_container;         //!< used container
            ContainerType m_containerFiltered; //!< cache for filtered container data
            std::unique_ptr<IModelFilter<ContainerType> > m_filter;     //!< used filter
            ISelectionModel<ContainerType> *m_selectionModel = nullptr; //!< selection model
        };

        namespace Private
        {
            //! Sort with compare function
            template<class ObjectType>
            bool compareForModelSort(const ObjectType &a, const ObjectType &b, Qt::SortOrder order, const BlackMisc::CPropertyIndex &index, std::true_type)
            {
                const int c = a.comparePropertyByIndex(index, b);
                if (c == 0) { return false; }
                return (order == Qt::AscendingOrder) ? (c < 0) : (c > 0);
            }

            //! Sort without compare function
            template <typename ObjectType>
            bool compareForModelSort(const ObjectType &a, const ObjectType &b, Qt::SortOrder order, const BlackMisc::CPropertyIndex &index, std::false_type)
            {
                const BlackMisc::CVariant aQv = a.propertyByIndex(index);
                const BlackMisc::CVariant bQv = b.propertyByIndex(index);
                return (order == Qt::AscendingOrder) ? (aQv < bQv) : (bQv < aQv);
            }
        } // namespace
    } // namespace
} // namespace
#endif // guard
