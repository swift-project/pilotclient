// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_MODELS_LISTMODELBASE_H
#define SWIFT_GUI_MODELS_LISTMODELBASE_H

#include <memory>

#include <QJsonDocument>
#include <QJsonObject>
#include <QModelIndex>
#include <QModelIndexList>
#include <QString>
#include <QVariant>
#include <QVector>

#include "gui/models/listmodelbasenontemplate.h"
#include "gui/models/modelfilter.h"
#include "gui/models/selectionmodel.h"

class QMimeData;
class QModelIndex;

namespace swift::misc
{
    class CWorker;
}
namespace swift::gui::models
{
    //! List model
    template <typename T, bool UseCompare = false>
    class CListModelBase : public CListModelBaseNonTemplate
    {
    public:
        //! Container type
        using ContainerType = T;

        //! Container element type
        using ObjectType = typename T::value_type;

        //! Destructor
        ~CListModelBase() override = default;

        //! \name Functions from QStandardItemModel
        //! @{

        //! \copydoc QStandardItemModel::data
        QVariant data(const QModelIndex &index, int role) const override;

        //! \copydoc QStandardItemModel::setData
        bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) final;

        //! \copydoc QStandardItemModel::mimeData
        QMimeData *mimeData(const QModelIndexList &indexes) const final;

        //! \copydoc QStandardItemModel::sort
        void sort(int column, Qt::SortOrder order) final;

        //! \copydoc QStandardItemModel::rowCount
        int rowCount(const QModelIndex &parentIndex = QModelIndex()) const final;

        //! \copydoc QStandardItemModel::canDropMimeData
        bool canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column,
                             const QModelIndex &parent) const final;

        //! \copydoc QStandardItemModel::dropMimeData
        bool dropMimeData(const QMimeData *mimeData, Qt::DropAction action, int row, int column,
                          const QModelIndex &parent) final;
        //! @}

        //! \name Functions from CListModelBaseNonTemplate
        //! @{

        //! \copydoc swift::gui::models::CListModelBaseNonTemplate::toJson
        QJsonObject toJson(bool selectedOnly = false) const override;

        //! \copydoc swift::gui::models::CListModelBaseNonTemplate::toJsonString
        QString toJsonString(QJsonDocument::JsonFormat format = QJsonDocument::Indented,
                             bool selectedOnly = false) const override;

        //! \copydoc swift::gui::models::CListModelBaseNonTemplate::isOrderable
        bool isOrderable() const override;
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
        virtual swift::misc::CWorker *updateAsync(const ContainerType &container, bool sort = true);

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

        //! Sort by given sort order \sa getSortColumn() \sa getSortOrder()
        //! \remark always sorts, even if columns did no change
        void resort();

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

        //! \copydoc swift::misc::CContainerBase::removeIf
        template <class K0, class V0, class... KeysValues>
        int removeIf(K0 k0, V0 v0, KeysValues... keysValues)
        {
            int c = m_container.removeIf(swift::misc::predicates::MemberEqual(k0, v0, keysValues...));
            this->updateFilteredContainer();
            if (c > 0) { this->emitModelDataChanged(); }
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
        void takeFilterOwnership(std::unique_ptr<IModelFilter<ContainerType>> &filter);

        //! Set the selection model
        void setSelectionModel(swift::gui::models::ISelectionModel<ContainerType> *selectionModel)
        {
            m_selectionModel = selectionModel;
        }

    protected:
        //! Constructor
        CListModelBase(const QString &translationContext, QObject *parent = nullptr);

        //! \name Base class overrides
        //! @{

        //! \copydoc swift::gui::models::CListModelBaseNonTemplate::onDataChanged
        void onDataChanged(const QModelIndex &topLeft, const QModelIndex &bottomLeft,
                           const QVector<int> &roles) override;

        //! \copydoc swift::gui::models::CListModelBaseNonTemplate::onDataChanged
        void onChangedDigest() override;
        //! @}

        //! Update filtered container
        void updateFilteredContainer();

        //! Model changed
        void emitModelDataChanged();

        ContainerType m_container; //!< used container
        ContainerType m_containerFiltered; //!< cache for filtered container data
        std::unique_ptr<IModelFilter<ContainerType>> m_filter; //!< used filter
        ISelectionModel<ContainerType> *m_selectionModel = nullptr; //!< selection model
    };

    namespace Private
    {
        //! Sort with compare function
        template <class ObjectType>
        bool compareForModelSort(const ObjectType &a, const ObjectType &b, Qt::SortOrder order,
                                 const swift::misc::CPropertyIndex &index,
                                 const swift::misc::CPropertyIndexList &tieBreakers, std::true_type)
        {
            const int c = a.comparePropertyByIndex(index, b);
            if (c == 0)
            {
                if (!tieBreakers.isEmpty())
                {
                    return compareForModelSort<ObjectType>(a, b, order, tieBreakers.front(),
                                                           tieBreakers.copyFrontRemoved(), std::true_type());
                }
                return false;
            }
            return (order == Qt::AscendingOrder) ? (c < 0) : (c > 0);
        }

        //! Sort without compare function
        template <typename ObjectType>
        bool compareForModelSort(const ObjectType &a, const ObjectType &b, Qt::SortOrder order,
                                 const swift::misc::CPropertyIndex &index,
                                 const swift::misc::CPropertyIndexList &tieBreakers, std::false_type)
        {
            const swift::misc::CVariant aQv = a.propertyByIndex(index);
            const swift::misc::CVariant bQv = b.propertyByIndex(index);
            if (!tieBreakers.isEmpty() && aQv == bQv)
            {
                return compareForModelSort<ObjectType>(a, b, order, tieBreakers.front(), tieBreakers.copyFrontRemoved(),
                                                       std::false_type());
            }
            return (order == Qt::AscendingOrder) ? (aQv < bQv) : (bQv < aQv);
        }
    } // namespace Private
} // namespace swift::gui::models

#endif // SWIFT_GUI_MODELS_LISTMODELBASE_H
