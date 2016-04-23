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

#include "listmodelbase.h"
#include "blackgui/blackguiexport.h"

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

            //! Highlight the DB models
            bool highlightDbData() const { return m_highlightDbData; }

            //! Highlight the DB models
            void setHighlightDbData(bool highlightDbData) { m_highlightDbData = highlightDbData; }

            //! Keys to be highlighted
            void setHighlightDbKeys(const QList<KeyType> &keys) { m_highlightKeys = keys; }

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
            bool m_highlightDbData = false; //!< highlight if DB data entry (valid key)
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
            virtual int update(const ContainerType &container, bool sort) override;
            virtual void moveItems(const ContainerType &items, int position) override;
            //! @}

        protected:
            //! Constructor
            COrderableListModelDbObjects(const QString &translationContext, QObject *parent = nullptr);
        };
    } // namespace
} // namespace
#endif // guard
