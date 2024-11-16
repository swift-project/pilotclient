// SPDX-FileCopyrightText: Copyright (C) 2053 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "listmodeldbobjects.h"

#include <type_traits>

#include <QBrush>
#include <QModelIndex>

#include "gui/models/allmodelcontainers.h"
#include "misc/aviation/aircraftcategory.h"
#include "misc/aviation/aircraftcategorylist.h"
#include "misc/aviation/aircrafticaocode.h"
#include "misc/aviation/aircrafticaocodelist.h"
#include "misc/aviation/airlineicaocode.h"
#include "misc/aviation/airlineicaocodelist.h"
#include "misc/aviation/livery.h"
#include "misc/aviation/liverylist.h"
#include "misc/country.h"
#include "misc/countrylist.h"
#include "misc/db/datastoreobjectlist.h"
#include "misc/orderable.h"
#include "misc/simulation/aircraftmodel.h"
#include "misc/simulation/aircraftmodellist.h"
#include "misc/simulation/distributor.h"
#include "misc/simulation/distributorlist.h"

using namespace swift::misc;
using namespace swift::misc::aviation;
using namespace swift::misc::db;

namespace swift::gui::models
{
    template <typename T, typename K, bool UseCompare>
    CListModelDbObjects<T, K, UseCompare>::CListModelDbObjects(const QString &translationContext, QObject *parent)
        : CListModelBase<ContainerType, UseCompare>(translationContext, parent)
    {
        CListModelBaseNonTemplate::m_sortTieBreakers.push_front(ObjectType::keyIndex());

        constexpr bool hasIntegerKey =
            std::is_base_of_v<IDatastoreObjectWithIntegerKey, ObjectType> && std::is_same_v<int, KeyType>;
        constexpr bool hasStringKey =
            std::is_base_of_v<IDatastoreObjectWithStringKey, ObjectType> && std::is_base_of_v<QString, KeyType>;
        static_assert(hasIntegerKey || hasStringKey,
                      "ObjectType needs to implement IDatastoreObjectWithXXXXKey and have appropriate KeyType");
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
    COrderableListModelDbObjects<T, K, UseCompare>::COrderableListModelDbObjects(const QString &translationContext,
                                                                                 QObject *parent)
        : CListModelDbObjects<ContainerType, KeyType, UseCompare>(translationContext, parent)
    {}

    template <typename T, typename K, bool UseCompare>
    void COrderableListModelDbObjects<T, K, UseCompare>::moveItems(const ContainerType &items, int position)
    {
        if (items.isEmpty()) { return; }
        ContainerType container(this->container());
        int order = 0;
        if (position >= 0 && position < container.size()) { order = container[position].getOrder(); }
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

    template class CListModelDbObjects<swift::misc::aviation::CLiveryList, int, true>;
    template class CListModelDbObjects<swift::misc::CCountryList, QString, true>;
    template class CListModelDbObjects<swift::misc::aviation::CAircraftIcaoCodeList, int, true>;
    template class CListModelDbObjects<swift::misc::aviation::CAircraftCategoryList, int, true>;
    template class CListModelDbObjects<swift::misc::aviation::CAirlineIcaoCodeList, int, true>;
    template class CListModelDbObjects<swift::misc::simulation::CAircraftModelList, int, true>;
    template class CListModelDbObjects<swift::misc::simulation::CDistributorList, QString, true>;
    template class COrderableListModelDbObjects<swift::misc::simulation::CAircraftModelList, int, true>;
    template class COrderableListModelDbObjects<swift::misc::simulation::CDistributorList, QString, true>;

} // namespace swift::gui::models
