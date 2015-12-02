/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "modelswithdbkey.h"
#include "allmodelcontainers.h"

namespace BlackGui
{
    namespace Models
    {
        template <typename ObjectType, typename ContainerType, typename KeyType, bool UseCompare>
        CModelsWithDbKeysBase<ObjectType, ContainerType, KeyType, UseCompare>::CModelsWithDbKeysBase(const QString &translationContext, QObject *parent) :
            CListModelBase<ObjectType, ContainerType, UseCompare>(translationContext, parent)
        { }

        template <typename ObjectType, typename ContainerType, typename KeyType, bool UseCompare>
        QVariant CModelsWithDbKeysBase<ObjectType, ContainerType, KeyType, UseCompare>::data(const QModelIndex &index, int role) const
        {
            if (role == Qt::BackgroundRole)
            {
                if (isHighlightIndex(index)) { return QBrush(m_highlightColor); }
            }
            return CListModelBase<ObjectType, ContainerType, UseCompare>::data(index, role);
        }

        template <typename ObjectType, typename ContainerType, typename KeyType, bool UseCompare>
        KeyType CModelsWithDbKeysBase<ObjectType, ContainerType, KeyType, UseCompare>::dbKeyForIndex(const QModelIndex &index) const
        {
            if (!index.isValid()) { return ObjectType::invalidDbKey(); }
            return this->at(index).getDbKey();
        }

        template <typename ObjectType, typename ContainerType, typename KeyType, bool UseCompare>
        bool CModelsWithDbKeysBase<ObjectType, ContainerType, KeyType, UseCompare>::isHighlightIndex(const QModelIndex &index) const
        {
            if (!index.isValid()) { return false; }
            if (m_highlightIntKeys.isEmpty()) { return false; }
            return m_highlightIntKeys.contains(dbKeyForIndex(index));
        }

        // see here for the reason of thess forward instantiations
        // http://www.parashift.com/c++-faq/separate-template-class-defn-from-decl.html
        template class CModelsWithDbKeysBase<BlackMisc::Simulation::CAircraftModel, BlackMisc::Simulation::CAircraftModelList, int, true>;

    } // namespace
} // namespace
