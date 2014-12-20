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

#include "blackgui/models/columns.h"
#include "blackmisc/worker.h"
#include "blackmisc/propertyindex.h"
#include <QAbstractItemModel>
#include <QThread>

namespace BlackGui
{
    namespace Models
    {
        //! Non templated base class, allows Q_OBJECT and signals to be used
        class CListModelBaseNonTemplate : public QAbstractListModel
        {
            Q_OBJECT

        public:
            //! Number of elements when to use asynchronous updates
            static const int asyncThreshold = 50;

            //! Destructor
            virtual ~CListModelBaseNonTemplate() {}

            //! \copydoc QAbstractListModel::columnCount()
            virtual int columnCount(const QModelIndex &modelIndex) const override;

            //! \copydoc QAbstractItemModel::headerData()
            virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

            //! Column to property index
            virtual BlackMisc::CPropertyIndex columnToPropertyIndex(int column) const;

            //! Index to property index
            virtual BlackMisc::CPropertyIndex modelIndexToPropertyIndex(const QModelIndex &index) const
            {
                return this->columnToPropertyIndex(index.column());
            }

            //! Set sort column
            virtual void setSortColumn(int column) { this->m_sortedColumn = column; }

            /*!
             * Set column for sorting
             * \param propertyIndex index of column to be sorted
             */
            virtual void setSortColumnByPropertyIndex(const BlackMisc::CPropertyIndex &propertyIndex)
            {
                this->m_sortedColumn = this->m_columns.propertyIndexToColumn(propertyIndex);
            }

            //! Get sort column property index
            virtual int getSortColumn() const { return this->m_sortedColumn; }

            //! Has valid sort column?
            virtual bool hasValidSortColumn() const;

            //! Get sort order
            virtual Qt::SortOrder getSortOrder() const { return this->m_sortOrder; }

            //! \copydoc QAbstractTableModel::flags
            Qt::ItemFlags flags(const QModelIndex &index) const override;

            //! Translation context
            virtual const QString &getTranslationContext() const
            {
                return m_columns.getTranslationContext();
            }

        signals:
            //! Asynchronous update finished
            void asyncUpdateFinished();

            //! Number of elements changed
            void rowCountChanged(int count);

        protected slots:
            //! Helper method with template free signature
            //! \param variant container is transferred in variant
            //! \param sort
            int updateContainer(const BlackMisc::CVariant &variant, bool sort)
            {
                return this->performUpdateContainer(variant, sort);
            }

        protected:
            /*!
             * Constructor
             * \param translationContext    I18N context
             * \param parent
             */
            CListModelBaseNonTemplate(const QString &translationContext, QObject *parent = nullptr)
                : QAbstractListModel(parent), m_columns(translationContext), m_sortedColumn(-1), m_sortOrder(Qt::AscendingOrder)
            {
                // non unique default name, set translation context as default
                this->setObjectName(translationContext);
            }

            //! Helper method with template free signature
            virtual int performUpdateContainer(const BlackMisc::CVariant &variant, bool sort) = 0;

            CColumns m_columns;        //!< columns metadata
            int m_sortedColumn;        //!< current sort column
            Qt::SortOrder m_sortOrder; //!< sort order (asc/desc)
        };

        //! List model
        template <typename ObjectType, typename ContainerType> class CListModelBase :
            public CListModelBaseNonTemplate
        {

        public:
            //! Destructor
            virtual ~CListModelBase() {}

            //! Valid index (in range)
            virtual bool isValidIndex(const QModelIndex &index) const;

            //! Used container data
            virtual const ContainerType &getContainer() const { return this->m_container; }

            //! \copydoc QAbstractListModel::data()
            virtual QVariant data(const QModelIndex &index, int role) const override;

            //! \copydoc QAbstractListModel::rowCount()
            virtual int rowCount(const QModelIndex &index = QModelIndex()) const override;

            //! Update by new container
            //! \remarks a sorting is performed only if a valid sort column is set
            virtual int update(const ContainerType &container, bool sort = true);

            //! Asynchronous update
            virtual BlackMisc::CWorker *updateAsync(const ContainerType &container, bool sort = true);

            //! Update by new container
            virtual void updateContainerMaybeAsync(const ContainerType &container, bool sort = true);

            //! Update single element
            virtual void update(const QModelIndex &index, const ObjectType &object);

            //! Update single element
            virtual void update(int rowIndex, const ObjectType &object)
            {
                this->update(this->index(rowIndex), object);
            }

            //! Object at row position
            virtual const ObjectType &at(const QModelIndex &index) const;

            //! \copydoc QAbstractListModel::sort()
            virtual void sort(int column, Qt::SortOrder order) override;

            /*!
             * Sort container by given column / order. This is used by sort() but als
             * for asynchronous updates in the views
             * \param container used list
             * \param column    column inder
             * \param order     sort order (ascending / descending)
             * \threadsafe under normal conditions thread safe as long as the column metadata are not changed
             */
            ContainerType sortContainerByColumn(const ContainerType &container, int column, Qt::SortOrder order) const;

            //! Similar to ContainerType::push_back
            virtual void push_back(const ObjectType &object);

            //! Similar to ContainerType::insert here inserts at first position
            virtual void insert(const ObjectType &object);

            //! Remove object
            virtual void remove(const ObjectType &object);

            //! \copydoc ContainerBase::removeIf
            template <class K0, class V0, class... KeysValues>
            int removeIf(K0 k0, V0 v0, KeysValues... keysValues)
            {
                int c = m_container.removeIf(BlackMisc::Predicates::MemberEqual(k0, v0, keysValues...));
                if (c > 0) { emit rowCountChanged(this->rowCount());}
                return c;
            }

            //! Clear the list
            virtual void clear();

        protected:
            ContainerType m_container; //!< used container

            /*!
             * Constructor
             * \param translationContext    I18N context
             * \param parent
             */
            CListModelBase(const QString &translationContext, QObject *parent = nullptr)
                : CListModelBaseNonTemplate(translationContext, parent)
            { }

            //! \copydoc CModelBaseNonTemplate::performUpdateContainer
            virtual int performUpdateContainer(const BlackMisc::CVariant &variant, bool sort) override;
        };

    } // namespace
} // namespace
#endif // guard
