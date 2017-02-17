/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_LISTMODELDBOBJECTS_H
#define BLACKGUI_LISTMODELDBOBJECTS_H

#include "blackgui/models/listmodelbase.h"
#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/aviation/aircrafticaocodelist.h"
#include "blackmisc/aviation/airlineicaocode.h"
#include "blackmisc/aviation/airlineicaocodelist.h"
#include "blackmisc/aviation/livery.h"
#include "blackmisc/aviation/liverylist.h"
#include "blackmisc/country.h"
#include "blackmisc/countrylist.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/distributor.h"
#include "blackmisc/simulation/distributorlist.h"

#include <QColor>
#include <QList>
#include <QString>
#include <QVariant>
#include <Qt>

class QModelIndex;
class QObject;

namespace BlackGui
{
    namespace Models
    {
        //! List model for DB objects
        template <typename ObjectType, typename ContainerType, typename KeyType, bool UseCompare = false> class CListModelDbObjects :
            public CListModelBase<ObjectType, ContainerType, UseCompare>
        {
        public:
            //! Destructor
            virtual ~CListModelDbObjects() {}

            //! Keys to be highlighted
            void setHighlightDbKeys(const QList<KeyType> &keys) { m_highlightKeys = keys; }

            //! Clear the highlighted keys
            void clearHighlightingDbKeys() { m_highlightKeys.clear(); }

            //! \copydoc BlackGui::Models::CListModelBaseNonTemplate::clearHighlighting
            virtual void clearHighlighting() override
            {
                this->clearHighlightingDbKeys();
                CListModelBase<ObjectType, ContainerType, UseCompare>::clearHighlighting();
            }

            //! Set color for highlighting
            void setHighlightColor(QColor color) { m_highlightColor = color; }

            //! Get data for index and role
            virtual QVariant data(const QModelIndex &index, int role) const override;

            //! DB key for given index
            KeyType dbKeyForIndex(const QModelIndex &index) const;

            //! Highlight index
            bool isHighlightIndex(const QModelIndex &index) const;

        protected:
            //! Constructor
            CListModelDbObjects(const QString &translationContext, QObject *parent = nullptr);

        private:
            QList<KeyType> m_highlightKeys; //!< keys to be highlighted
            QColor         m_highlightColor = Qt::green;
        };

        //! List model for DB objects
        template <typename ObjectType, typename ContainerType, typename KeyType, bool UseCompare = false> class COrderableListModelDbObjects :
            public CListModelDbObjects<ObjectType, ContainerType, KeyType, UseCompare>
        {
        public:
            //! Destructor
            virtual ~COrderableListModelDbObjects() {}

            //! \name specialized BlackGui::Models::CListModelDbObjects functions for ordering
            //! @{
            using COrderableListModelDbObjects::CListModelDbObjects::update;
            virtual int update(const ContainerType &container, bool sort) override;
            virtual void moveItems(const ContainerType &items, int position) override;
            //! @}

            //! Sort order to order property BlackMisc::IOrderable::IndexOrder
            void setSortColumnToOrder();

        protected:
            //! Constructor
            COrderableListModelDbObjects(const QString &translationContext, QObject *parent = nullptr);
        };
    } // namespace
} // namespace
#endif // guard
