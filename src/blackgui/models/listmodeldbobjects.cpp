/* Copyright (C) 2053
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "listmodeldbobjects.h"
#include "allmodelcontainers.h"
#include "blackmisc/db/datastoreobjectlist.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/simulation/distributorlist.h"
#include "blackmisc/simulation/distributor.h"
#include "blackmisc/aviation/aircrafticaocodelist.h"
#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/aviation/aircraftcategorylist.h"
#include "blackmisc/aviation/aircraftcategory.h"
#include "blackmisc/aviation/airlineicaocodelist.h"
#include "blackmisc/aviation/airlineicaocode.h"
#include "blackmisc/aviation/liverylist.h"
#include "blackmisc/aviation/livery.h"
#include "blackmisc/countrylist.h"
#include "blackmisc/country.h"
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
        template <typename T, typename K, bool UseCompare>
        CListModelDbObjects<T, K, UseCompare>::CListModelDbObjects(const QString &translationContext, QObject *parent) :
            CListModelBase<ContainerType, UseCompare>(translationContext, parent)
        {
            CListModelBaseNonTemplate::m_sortTieBreakers.push_front(ObjectType::keyIndex());

            constexpr bool hasIntegerKey = std::is_base_of_v<IDatastoreObjectWithIntegerKey, ObjectType> && std::is_same_v<int, KeyType>;
            constexpr bool hasStringKey  = std::is_base_of_v<IDatastoreObjectWithStringKey, ObjectType> && std::is_base_of_v<QString, KeyType>;
            static_assert(hasIntegerKey || hasStringKey, "ObjectType needs to implement IDatastoreObjectWithXXXXKey and have appropriate KeyType");
        }

        template <typename T, typename K, bool UseCompare>
        QVariant CListModelDbObjects<T, K, UseCompare>::data(const QModelIndex &index, int role) const
        {
            if (role != Qt::BackgroundRole) { return CListModelBase<ContainerType, UseCompare>::data(index, role); }
            if (isHighlightedIndex(index)) { return QBrush(m_highlightColor); }
            return CListModelBase<ContainerType, UseCompare>::data(index, role);
        }

        template <typename T, typename K, bool UseCompare>
        K CListModelDbObjects<T, K, UseCompare>::dbKeyForIndex(const QModelIndex &index) const
        {
            if (!index.isValid()) { return ObjectType::invalidDbKey(); }
            return this->at(index).getDbKey();
        }

        template <typename T, typename K, bool UseCompare>
        bool CListModelDbObjects<T, K, UseCompare>::isHighlightedIndex(const QModelIndex &index) const
        {
            if (!index.isValid()) { return false; }
            if (m_highlightKeys.isEmpty()) { return false; }
            return m_highlightKeys.contains(dbKeyForIndex(index));
        }

        template <typename T, typename K, bool UseCompare>
        COrderableListModelDbObjects<T, K, UseCompare>::COrderableListModelDbObjects(const QString &translationContext, QObject *parent)
            : CListModelDbObjects<ContainerType, KeyType, UseCompare>(translationContext, parent)
        { }

        template <typename T, typename K, bool UseCompare>
        void COrderableListModelDbObjects<T, K, UseCompare>::moveItems(const ContainerType &items, int position)
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

        template <typename T, typename K, bool UseCompare>
        bool COrderableListModelDbObjects<T, K, UseCompare>::setSortColumnToOrder()
        {
            // force sorted by order, otherwise display looks confusing
            return this->setSorting(IOrderable::IndexOrder);
        }

        template <typename T, typename K, bool UseCompare>
        int COrderableListModelDbObjects<T, K, UseCompare>::update(const ContainerType &container, bool sort)
        {
            if (container.needsOrder())
            {
                ContainerType orderable(container);
                orderable.resetOrder();
                return CListModelDbObjects<ContainerType, KeyType, UseCompare>::update(orderable, sort);
            }
            return CListModelDbObjects<ContainerType, KeyType, UseCompare>::update(container, sort);
        }

        template class CListModelDbObjects<BlackMisc::Aviation::CLiveryList, int, true>;
        template class CListModelDbObjects<BlackMisc::CCountryList, QString, true>;
        template class CListModelDbObjects<BlackMisc::Aviation::CAircraftIcaoCodeList, int, true>;
        template class CListModelDbObjects<BlackMisc::Aviation::CAircraftCategoryList, int, true>;
        template class CListModelDbObjects<BlackMisc::Aviation::CAirlineIcaoCodeList, int, true>;
        template class CListModelDbObjects<BlackMisc::Simulation::CAircraftModelList, int, true>;
        template class CListModelDbObjects<BlackMisc::Simulation::CDistributorList, QString, true>;
        template class COrderableListModelDbObjects<BlackMisc::Simulation::CAircraftModelList, int, true>;
        template class COrderableListModelDbObjects<BlackMisc::Simulation::CDistributorList, QString, true>;

    } // namespace
} // namespace
