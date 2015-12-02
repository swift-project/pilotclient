/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_MODELSWITHDBKEY_H
#define BLACKGUI_MODELSWITHDBKEY_H

#include "listmodelbase.h"

namespace BlackGui
{
    namespace Models
    {
        //! Base class for models with DB keys
        template <typename ObjectType, typename ContainerType, typename KeyType = int, bool UseCompare = false>
        class CModelsWithDbKeysBase : public CListModelBase<ObjectType, ContainerType, UseCompare>
        {
        public:
            //! Destructor
            virtual ~CModelsWithDbKeysBase() {}

            //! Keys to be highlighted
            void setHighlightDbKeys(const QList<KeyType> &keys) { m_highlightIntKeys = keys; }

            //! Set color for highlighting
            void setHighlightColor(QColor color) { m_highlightColor = color; }

            //! \copydoc CListModelBase::data
            virtual QVariant data(const QModelIndex &index, int role) const override;

            //! DB key for given index
            KeyType dbKeyForIndex(const QModelIndex &index) const;

            //! Highlight index
            bool isHighlightIndex(const QModelIndex &index) const;

        protected:
            //! \copydoc CListModelBase::CListModelBase
            CModelsWithDbKeysBase(const QString &translationContext, QObject *parent = nullptr);

        private:
            QList<KeyType> m_highlightIntKeys; //!< keys to be highlighted
            QColor         m_highlightColor = Qt::green;
        };
    } // namespace
} // namespace
#endif // guard
