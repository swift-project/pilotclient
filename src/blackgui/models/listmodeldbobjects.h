// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_MODELS_LISTMODELDBOBJECTS_H
#define BLACKGUI_MODELS_LISTMODELDBOBJECTS_H

#include "blackgui/models/listmodelbase.h"

#include <QColor>
#include <QList>
#include <QString>
#include <QVariant>
#include <Qt>

class QModelIndex;
class QObject;

namespace BlackGui::Models
{
    //! List model for DB objects
    template <typename T, typename K, bool UseCompare = false>
    class CListModelDbObjects :
        public CListModelBase<T, UseCompare>
    {
    public:
        //! Container type
        using ContainerType = T;

        //! Container element type
        using ObjectType = typename T::value_type;

        //! DB key type
        using KeyType = K;

        //! Destructor
        virtual ~CListModelDbObjects() {}

        //! Keys to be highlighted
        void setHighlightedDbKeys(const QList<KeyType> &keys) { m_highlightKeys = keys; }

        //! Clear the highlighted keys
        void clearHighlightedDbKeys() { m_highlightKeys.clear(); }

        //! \copydoc BlackGui::Models::CListModelBaseNonTemplate::clearHighlighting
        virtual void clearHighlighting() override
        {
            this->clearHighlightedDbKeys();
            CListModelBase<ContainerType, UseCompare>::clearHighlighting();
        }

        //! \copydoc BlackGui::Models::CListModelBaseNonTemplate::hasHighlightedRows
        virtual bool hasHighlightedRows() const override
        {
            return !m_highlightKeys.isEmpty();
        }

        //! Set color for highlighting
        void setHighlightColor(QColor color) { m_highlightColor = color; }

        //! Get data for index and role
        virtual QVariant data(const QModelIndex &index, int role) const override;

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
    class COrderableListModelDbObjects :
        public CListModelDbObjects<T, K, UseCompare>
    {
    public:
        //! Container type
        using ContainerType = T;

        //! Container element type
        using ObjectType = typename T::value_type;

        //! DB key type
        using KeyType = K;

        //! Destructor
        virtual ~COrderableListModelDbObjects() {}

        //! \name specialized BlackGui::Models::CListModelDbObjects functions for ordering
        //! @{
        using COrderableListModelDbObjects::CListModelDbObjects::update;

        //! \copydoc BlackGui::Models::CListModelDbObjects::update
        virtual int update(const ContainerType &container, bool sort) override;

        //! \copydoc BlackGui::Models::CListModelDbObjects::moveItems
        virtual void moveItems(const ContainerType &items, int position) override;
        //! @}

        //! Sort order to order property BlackMisc::IOrderable::IndexOrder
        bool setSortColumnToOrder();

    protected:
        //! Constructor
        COrderableListModelDbObjects(const QString &translationContext, QObject *parent = nullptr);
    };
} // namespace
#endif // guard
