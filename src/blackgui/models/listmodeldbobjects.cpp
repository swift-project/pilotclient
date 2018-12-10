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
#include "blackmisc/db/datastoreobjectlist.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/distributor.h"
#include "blackmisc/simulation/distributorlist.h"
#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/aviation/aircrafticaocodelist.h"
#include "blackmisc/aviation/airlineicaocode.h"
#include "blackmisc/aviation/airlineicaocodelist.h"
#include "blackmisc/aviation/livery.h"
#include "blackmisc/aviation/liverylist.h"
#include "blackmisc/country.h"
#include "blackmisc/countrylist.h"
#include "blackmisc/orderable.h"

#include <QBrush>
#include <QModelIndex>
#include <type_traits>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Db;

namespace BlackGui
{
    namespace Models
    {
        template <typename ObjectType, typename ContainerType, typename KeyType, bool UseCompare>
        CListModelDbObjects<ObjectType, ContainerType, KeyType, UseCompare>::CListModelDbObjects(const QString &translationContext, QObject *parent) :
            CListModelBase<ObjectType, ContainerType, UseCompare>(translationContext, parent)
        {
            constexpr bool hasIntegerKey = std::is_base_of<IDatastoreObjectWithIntegerKey, ObjectType>::value && std::is_same<int, KeyType>::value;
            constexpr bool hasStringKey = std::is_base_of<IDatastoreObjectWithStringKey, ObjectType>::value && std::is_base_of<QString, KeyType>::value;
            static_assert(hasIntegerKey || hasStringKey, "ObjectType needs to implement IDatastoreObjectWithXXXXKey and have appropriate KeyType");
        }

        template <typename ObjectType, typename ContainerType, typename KeyType, bool UseCompare>
        QVariant CListModelDbObjects<ObjectType, ContainerType, KeyType, UseCompare>::data(const QModelIndex &index, int role) const
        {
            if (role != Qt::BackgroundRole) { return CListModelBase<ObjectType, ContainerType, UseCompare>::data(index, role); }
            if (isHighlightedIndex(index) ) { return QBrush(m_highlightColor); }
            return CListModelBase<ObjectType, ContainerType, UseCompare>::data(index, role);
        }

        template <typename ObjectType, typename ContainerType, typename KeyType, bool UseCompare>
        KeyType CListModelDbObjects<ObjectType, ContainerType, KeyType, UseCompare>::dbKeyForIndex(const QModelIndex &index) const
        {
            if (!index.isValid()) { return ObjectType::invalidDbKey(); }
            return this->at(index).getDbKey();
        }

        template <typename ObjectType, typename ContainerType, typename KeyType, bool UseCompare>
        bool CListModelDbObjects<ObjectType, ContainerType, KeyType, UseCompare>::isHighlightedIndex(const QModelIndex &index) const
        {
            if (!index.isValid()) { return false; }
            if (m_highlightKeys.isEmpty()) { return false; }
            return m_highlightKeys.contains(dbKeyForIndex(index));
        }

        template <typename ObjectType, typename ContainerType, typename KeyType, bool UseCompare>
        COrderableListModelDbObjects<ObjectType, ContainerType, KeyType, UseCompare>::COrderableListModelDbObjects(const QString &translationContext, QObject *parent)
            : CListModelDbObjects<ObjectType, ContainerType, KeyType, UseCompare>(translationContext, parent)
        { }

        template <typename ObjectType, typename ContainerType, typename KeyType, bool UseCompare>
        void COrderableListModelDbObjects<ObjectType, ContainerType, KeyType, UseCompare>::moveItems(const ContainerType &items, int position)
        {
            if (items.isEmpty()) { return; }
            ContainerType container(this->container());
            int order = 0;
            if (position >= 0 && position < container.size())
            {
                order = container[position].getOrder();
            }
            this->setSortColumnToOrder();
            container.moveTo(items, order);

            // update container
            this->updateContainerMaybeAsync(container);
        }

        template <typename ObjectType, typename ContainerType, typename KeyType, bool UseCompare>
        bool COrderableListModelDbObjects<ObjectType, ContainerType, KeyType, UseCompare>::setSortColumnToOrder()
        {
            // force sorted by order, otherwise display looks confusing
            return this->setSorting(IOrderable::IndexOrder);
        }

        template <typename ObjectType, typename ContainerType, typename KeyType, bool UseCompare>
        int COrderableListModelDbObjects<ObjectType, ContainerType, KeyType, UseCompare>::update(const ContainerType &container, bool sort)
        {
            if (container.needsOrder())
            {
                ContainerType orderable(container);
                orderable.resetOrder();
                return CListModelDbObjects<ObjectType, ContainerType, KeyType, UseCompare>::update(orderable, sort);
            }
            return CListModelDbObjects<ObjectType, ContainerType, KeyType, UseCompare>::update(container, sort);
        }

        template class CListModelDbObjects<BlackMisc::Aviation::CLivery, BlackMisc::Aviation::CLiveryList, int, true>;
        template class CListModelDbObjects<BlackMisc::CCountry, BlackMisc::CCountryList, QString, true>;
        template class CListModelDbObjects<BlackMisc::Aviation::CAircraftIcaoCode, BlackMisc::Aviation::CAircraftIcaoCodeList, int, true>;
        template class CListModelDbObjects<BlackMisc::Aviation::CAirlineIcaoCode, BlackMisc::Aviation::CAirlineIcaoCodeList, int, true>;
        template class CListModelDbObjects<BlackMisc::Simulation::CAircraftModel, BlackMisc::Simulation::CAircraftModelList, int, true>;
        template class CListModelDbObjects<BlackMisc::Simulation::CDistributor, BlackMisc::Simulation::CDistributorList, QString, true>;
        template class COrderableListModelDbObjects<BlackMisc::Simulation::CAircraftModel, BlackMisc::Simulation::CAircraftModelList, int, true>;
        template class COrderableListModelDbObjects<BlackMisc::Simulation::CDistributor, BlackMisc::Simulation::CDistributorList, QString, true>;

    } // namespace
} // namespace
