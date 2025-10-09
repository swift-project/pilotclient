// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_MODELS_LISTMODELDBOBJECTS_H
#define SWIFT_GUI_MODELS_LISTMODELDBOBJECTS_H

#include <QColor>
#include <QList>
#include <QString>
#include <QVariant>
#include <Qt>

#include "gui/models/listmodelbase.h"

class QModelIndex;
class QObject;

namespace swift::gui::models
{
    //! List model for DB objects
    template <typename T, typename K, bool UseCompare = false>
    class CListModelDbObjects : public CListModelBase<T, UseCompare>
    {
    public:
        //! Container type
        using ContainerType = T;

        //! Container element type
        using ObjectType = typename T::value_type;

        //! DB key type
        using KeyType = K;

        //! Destructor
        ~CListModelDbObjects() override = default;

        //! Keys to be highlighted
        void setHighlightedDbKeys(const QList<KeyType> &keys) { m_highlightKeys = keys; }

        //! Clear the highlighted keys
        void clearHighlightedDbKeys() { m_highlightKeys.clear(); }

        //! \copydoc swift::gui::models::CListModelBaseNonTemplate::clearHighlighting
        void clearHighlighting() override
        {
            this->clearHighlightedDbKeys();
            CListModelBase<ContainerType, UseCompare>::clearHighlighting();
        }

        //! \copydoc swift::gui::models::CListModelBaseNonTemplate::hasHighlightedRows
        bool hasHighlightedRows() const override { return !m_highlightKeys.isEmpty(); }

        //! Set color for highlighting
        void setHighlightColor(QColor color) { m_highlightColor = color; }

        //! Get data for index and role
        QVariant data(const QModelIndex &index, int role) const override;

        //! DB key for given index
        KeyType dbKeyForIndex(const QModelIndex &index) const;

        //! Highlight index
        bool isHighlightedIndex(const QModelIndex &index) const;

    protected:
        //! Constructor
        CListModelDbObjects(const QString &translationContext, QObject *parent = nullptr);

    private:
        QList<KeyType> m_highlightKeys; //!< keys to be highlighted
        QColor m_highlightColor = Qt::green;
    };

    //! List model for DB objects
    template <typename T, typename K, bool UseCompare = false>
    class COrderableListModelDbObjects : public CListModelDbObjects<T, K, UseCompare>
    {
    public:
        //! Container type
        using ContainerType = T;

        //! Container element type
        using ObjectType = typename T::value_type;

        //! DB key type
        using KeyType = K;

        //! Destructor
        ~COrderableListModelDbObjects() override = default;

        //! \name specialized swift::gui::models::CListModelDbObjects functions for ordering
        //! @{
        using COrderableListModelDbObjects::CListModelDbObjects::update;

        //! \copydoc swift::gui::models::CListModelDbObjects::update
        int update(const ContainerType &container, bool sort) override;

        //! \copydoc swift::gui::models::CListModelDbObjects::moveItems
        void moveItems(const ContainerType &items, int position) override;
        //! @}

        //! Sort order to order property swift::misc::IOrderable::IndexOrder
        bool setSortColumnToOrder();

    protected:
        //! Constructor
        COrderableListModelDbObjects(const QString &translationContext, QObject *parent = nullptr);
    };
} // namespace swift::gui::models
#endif // SWIFT_GUI_MODELS_LISTMODELDBOBJECTS_H
