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
#include "blackgui/models/columns.h"
#include "blackgui/models/modelfilter.h"
#include "blackmisc/worker.h"
#include <QStandardItemModel>
#include <QThread>
#include <memory>
#include <iostream>
#include <type_traits>

namespace BlackGui
{
    namespace Models
    {
        //! Non templated base class, allows Q_OBJECT and signals to be used
        class BLACKGUI_EXPORT CListModelBaseNonTemplate : public QStandardItemModel
        {
            Q_OBJECT

        public:
            //! Number of elements when to use asynchronous updates
            static const int asyncThreshold = 50;

            //! Destructor
            virtual ~CListModelBaseNonTemplate() {}

            //! \copydoc QStandardItemModel::columnCount()
            virtual int columnCount(const QModelIndex &modelIndex = QModelIndex()) const override;

            //! \copydoc QStandardItemModel::headerData()
            virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

            //! \copydoc QStandardItemModel::headerData()
            virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;

            //! \copydoc QStandardItemModel::parent()
            virtual QModelIndex parent(const QModelIndex &child) const override;

            //! Column to property index
            virtual BlackMisc::CPropertyIndex columnToPropertyIndex(int column) const;

            //! Property index to column number
            virtual int propertyIndexToColumn(const BlackMisc::CPropertyIndex &propertyIndex) const;

            //! Index to property index
            virtual BlackMisc::CPropertyIndex modelIndexToPropertyIndex(const QModelIndex &index) const;

            //! Set sort column
            virtual void setSortColumn(int column) { this->m_sortedColumn = column; }

            //! Set column for sorting
            //! \param propertyIndex index of column to be sorted
            virtual void setSortColumnByPropertyIndex(const BlackMisc::CPropertyIndex &propertyIndex);

            //! Get sort column property index
            virtual int getSortColumn() const { return this->m_sortedColumn; }

            //! Has valid sort column?
            virtual bool hasValidSortColumn() const;

            //! Get sort order
            virtual Qt::SortOrder getSortOrder() const { return this->m_sortOrder; }

            //! Translation context
            virtual const QString &getTranslationContext() const;

            //! \copydoc QStandardItemModel::flags
            virtual Qt::ItemFlags flags(const QModelIndex &index) const override;

            //! \copydoc QStandardItemModel::supportedDragActions
            virtual Qt::DropActions supportedDragActions() const override;

            //! \copydoc QStandardItemModel::supportedDropActions
            virtual Qt::DropActions supportedDropActions() const override;

            //! \copydoc QStandardItemModel::mimeTypes
            virtual QStringList mimeTypes() const override;

            //! Mark as about to be destroyed, normally marked from view
            void markDestroyed();

            //! Model about to be destroyed?
            bool isModelDestroyed();

            //! Send signal that data have been changed.
            //! \note Meant for scenarios where the container is directly updated and a subsequent signal is required
            void sendDataChanged(int startRowIndex, int endRowIndex);

            //! Convert to JSON
            virtual QJsonObject toJson() const = 0;

            //! Convert to JSON string
            virtual QString toJsonString(QJsonDocument::JsonFormat format = QJsonDocument::Indented) const = 0;

        signals:
            //! Asynchronous update finished
            void asyncUpdateFinished();

            //! Number of elements changed
            void rowCountChanged(int count, bool withFilter);

            //! Model has been changed
            void changed();

            //! Template free information, that object changed
            void objectChanged(const BlackMisc::CVariant &object, const BlackMisc::CPropertyIndex &changedIndex);

        protected slots:
            //! Helper method with template free signature
            //! \param variant container is transferred in variant
            //! \param sort
            int ps_updateContainer(const BlackMisc::CVariant &variant, bool sort);

        protected:
            //! Constructor
            //! \param translationContext    I18N context
            //! \param parent
            CListModelBaseNonTemplate(const QString &translationContext, QObject *parent = nullptr);

            //! Helper method with template free signature
            virtual int performUpdateContainer(const BlackMisc::CVariant &variant, bool sort) = 0;

            CColumns m_columns;            //!< columns metadata
            int m_sortedColumn;            //!< current sort column
            Qt::SortOrder m_sortOrder;     //!< sort order (asc/desc)
            bool m_modelDestroyed = false; //!< model is about to be destroyed
        };

        //! List model
        template <typename ObjectType, typename ContainerType, bool UseCompare = false> class CListModelBase : public CListModelBaseNonTemplate
        {
        public:
            //! Destructor
            virtual ~CListModelBase() {}

            //! Valid index (in range)
            virtual bool isValidIndex(const QModelIndex &index) const;

            //! Used container data
            const ContainerType &container() const;

            //! Full container or cached filtered container as approproiate
            const ContainerType &containerOrFilteredContainer() const;

            //! \copydoc QStandardItemModel::data()
            virtual QVariant data(const QModelIndex &index, int role) const override;

            //! \copydoc QStandardItemModel::setData()
            //! \sa CListModelBaseNonTemplate::flags
            virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

            //! Simple set of data in container, using class is responsible for firing signals etc.
            //! \sa sendDataChanged
            bool setInContainer(const QModelIndex &index, const ObjectType &obj);

            //! \copydoc QStandardItemModel::rowCount()
            virtual int rowCount(const QModelIndex &parentIndex = QModelIndex()) const override;

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

            //! Object at row position
            virtual const ObjectType &at(const QModelIndex &index) const;

            //! Sort by given sort order \sa getSortColumn() \sa getSortOrder()
            void sort();

            //! \copydoc QStandardItemModel::sort()
            virtual void sort(int column, Qt::SortOrder order) override;

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

            //! Similar to ContainerType::insert here inserts at first position
            virtual void insert(const ObjectType &object);

            //! Similar to ContainerType::insert here inserts at first position
            virtual void insert(const ContainerType &container);

            //! Remove object
            virtual void remove(const ObjectType &object);

            //! \copydoc BlackMisc::ContainerBase::removeIf
            template <class K0, class V0, class... KeysValues>
            int removeIf(K0 k0, V0 v0, KeysValues... keysValues)
            {
                int c = m_container.removeIf(BlackMisc::Predicates::MemberEqual(k0, v0, keysValues...));
                if (c > 0) { this->emitRowCountChanged();}
                this->updateFilteredContainer();
                return c;
            }

            //! Clear the list
            virtual void clear();

            //! Empty?
            virtual bool isEmpty() const;

            //! \copydoc QStandardItemModel::mimeData
            virtual QMimeData *mimeData(const QModelIndexList &indexes) const override;

            //! \copydoc CListModelBaseNonTemplate::toJosn
            virtual QJsonObject toJson() const override;

            //! \copydoc CListModelBaseNonTemplate::toJsonString
            virtual QString toJsonString(QJsonDocument::JsonFormat format = QJsonDocument::Indented) const override;

            //! Filter available
            bool hasFilter() const;

            //! Remove filter
            void removeFilter();

            //! Set the filter
            void takeFilterOwnership(std::unique_ptr<IModelFilter<ContainerType> > &filter);

        protected:
            //! \copydoc CListModelBaseNonTemplate::CListModelBaseNonTemplate
            CListModelBase(const QString &translationContext, QObject *parent = nullptr);

            //! \copydoc CModelBaseNonTemplate::performUpdateContainer
            virtual int performUpdateContainer(const BlackMisc::CVariant &variant, bool sort) override;

            //! Update filtered container
            void updateFilteredContainer();

            //! Row count changed
            void emitRowCountChanged();

            std::unique_ptr<IModelFilter<ContainerType> > m_filter; //!< Used filter
            ContainerType m_container;                              //!< used container
            ContainerType m_containerFiltered;                      //!< cache for filtered container data
        };

        namespace Private
        {
            //! Sort with compare function
            template<class ObjectType>
            bool compareForModelSort(const ObjectType &a, const ObjectType &b, Qt::SortOrder order, const BlackMisc::CPropertyIndex &index, std::true_type)
            {
                int c = a.comparePropertyByIndex(b, index);
                if (c == 0) { return false; }
                return (order == Qt::AscendingOrder) ? (c < 0) : (c > 0);
            }

            //! Sort without compare function
            template <typename ObjectType>
            bool compareForModelSort(const ObjectType &a, const ObjectType &b, Qt::SortOrder order, const BlackMisc::CPropertyIndex &index, std::false_type)
            {
                Q_UNUSED(index);
                BlackMisc::CVariant aQv = a.propertyByIndex(index);
                BlackMisc::CVariant bQv = b.propertyByIndex(index);
                return (order == Qt::AscendingOrder) ? (aQv < bQv) : (bQv < aQv);
            }
        } // namespace
    } // namespace
} // namespace
#endif // guard
