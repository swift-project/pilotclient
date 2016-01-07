/* Copyright (C) 2053
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "listmodeldbobjects.h"
#include "allmodelcontainers.h"

using namespace BlackMisc;
using namespace BlackMisc::Aviation;

namespace BlackGui
{
    namespace Models
    {
        template <typename ObjectType, typename ContainerType, typename KeyType, bool UseCompare>
        CListModelDbObjects<ObjectType, ContainerType, KeyType, UseCompare>::CListModelDbObjects(const QString &translationContext, QObject *parent) :
            CListModelBase<ObjectType, ContainerType, UseCompare>(translationContext, parent)
        {
            // void
        }

        template <typename ObjectType, typename ContainerType, typename KeyType, bool UseCompare>
        QVariant CListModelDbObjects<ObjectType, ContainerType, KeyType, UseCompare>::data(const QModelIndex &index, int role) const
        {
            if (role != Qt::BackgroundRole) { return CListModelBase<ObjectType, ContainerType, UseCompare>::data(index, role); }

            if (isHighlightIndex(index)) { return QBrush(m_highlightColor); }
            if (!highlightDbData()) { return CListModelBase<ObjectType, ContainerType, UseCompare>::data(index, role); }

            ObjectType obj(this->at(index));
            // highlight DB models
            if (obj.hasValidDbKey())
            {
                static const QBrush b(Qt::green);
                return b;
            }
            return QVariant();
        }

        template <typename ObjectType, typename ContainerType, typename KeyType, bool UseCompare>
        KeyType CListModelDbObjects<ObjectType, ContainerType, KeyType, UseCompare>::dbKeyForIndex(const QModelIndex &index) const
        {
            if (!index.isValid()) { return ObjectType::invalidDbKey(); }
            return this->at(index).getDbKey();
        }

        template <typename ObjectType, typename ContainerType, typename KeyType, bool UseCompare>
        bool CListModelDbObjects<ObjectType, ContainerType, KeyType, UseCompare>::isHighlightIndex(const QModelIndex &index) const
        {
            if (!index.isValid()) { return false; }
            if (m_highlightKeys.isEmpty()) { return false; }
            return m_highlightKeys.contains(dbKeyForIndex(index));
        }

        // see here for the reason of thess forward instantiations
        // http://www.parashift.com/c++-faq/separate-template-class-defn-from-decl.html
        template class CListModelDbObjects<BlackMisc::Aviation::CLivery, BlackMisc::Aviation::CLiveryList, int, true>;
        template class CListModelDbObjects<BlackMisc::CCountry, BlackMisc::CCountryList, QString, true>;
        template class CListModelDbObjects<BlackMisc::Aviation::CAircraftIcaoCode, BlackMisc::Aviation::CAircraftIcaoCodeList, int, true>;
        template class CListModelDbObjects<BlackMisc::Aviation::CAirlineIcaoCode, BlackMisc::Aviation::CAirlineIcaoCodeList, int, true>;
        template class CListModelDbObjects<BlackMisc::Simulation::CAircraftModel, BlackMisc::Simulation::CAircraftModelList, int, true>;
        template class CListModelDbObjects<BlackMisc::Simulation::CDistributor, BlackMisc::Simulation::CDistributorList, QString, true>;

    } // namespace
} // namespace
