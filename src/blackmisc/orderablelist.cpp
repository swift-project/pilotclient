/* Copyright (C) 2016
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/orderablelist.h"
#include "blackmisc/predicates.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/distributorlist.h"

#include <algorithm>
#include <type_traits>

namespace BlackMisc
{
    template <class OBJ, class CONTAINER>
    IOrderableList<OBJ, CONTAINER>::IOrderableList()
    {
        static_assert(std::is_base_of<IOrderable, OBJ>::value, "OBJ needs to implement IOrderable");
    }

    template <class OBJ, class CONTAINER>
    const CONTAINER &IOrderableList<OBJ, CONTAINER>::container() const
    {
        return static_cast<const CONTAINER &>(*this);
    }

    template <class OBJ, class CONTAINER>
    CONTAINER &IOrderableList<OBJ, CONTAINER>::container()
    {
        return static_cast<CONTAINER &>(*this);
    }

    template <class OBJ, class CONTAINER>
    void IOrderableList<OBJ, CONTAINER>::sortDescendingByOrder()
    {
        IOrderableList::container().sortAscendingByOrder();
        std::reverse(this->container().begin(), this->container().end());
    }

    template <class OBJ, class CONTAINER>
    void IOrderableList<OBJ, CONTAINER>::sortAscendingByOrder()
    {
        IOrderableList::container().sort(Predicates::MemberLess(&OBJ::getOrder));
    }

    template <class OBJ, class CONTAINER>
    void IOrderableList<OBJ, CONTAINER>::resetOrder(int offset)
    {
        int c = offset;
        for (OBJ &obj : container())
        {
            obj.setOrder(c++);
        }
    }

    template <class OBJ, class CONTAINER>
    bool IOrderableList<OBJ, CONTAINER>::needsOrder() const
    {
        for (const OBJ &obj : container())
        {
            if (!obj.hasValidOrder()) { return true; }
        }
        return false;
    }

    template <class OBJ, class CONTAINER>
    QList<int> IOrderableList<OBJ, CONTAINER>::orderValues() const
    {
        QList<int> orders;
        for (const OBJ &obj : container())
        {
            if (!obj.hasValidOrder()) { continue; }
            orders.append(obj.getOrder());
        }
        return orders;
    }

    template <class OBJ, class CONTAINER>
    CONTAINER IOrderableList<OBJ, CONTAINER>::withoutItemsOfSameOrder(const CONTAINER &items) const
    {
        const QList<int> orders = items.orderValues();
        if (orders.isEmpty()) { return this->container(); }

        CONTAINER newContainer;
        for (const OBJ &obj : container())
        {
            if (orders.contains(obj.getOrder())) { continue; }
            newContainer.push_back(obj);
        }
        return newContainer;
    }

    template <class OBJ, class CONTAINER>
    void IOrderableList<OBJ, CONTAINER>::removeItemsWithSameOrder(const CONTAINER &items)
    {
        const QList<int> orders = items.orderValues();
        if (orders.isEmpty()) { return; }

        CONTAINER newContainer;
        for (const OBJ &obj : container())
        {
            if (orders.contains(obj.getOrder())) { continue; }
            newContainer.push_back(obj);
        }
        this->container() = newContainer;
    }

    template <class OBJ, class CONTAINER>
    void IOrderableList<OBJ, CONTAINER>::moveTo(const CONTAINER &items, int targetOrder)
    {
        if (items.isEmpty()) { return; }
        CONTAINER newContainer(this->withoutItemsOfSameOrder(items)); // this container without items
        CONTAINER newItems(items);
        const int shift = items.size();
        newContainer.sortAscendingByOrder(); // sorted by old order
        newContainer.resetOrder();
        for (OBJ &obj : newContainer)
        {
            if (obj.getOrder() < targetOrder) { continue; }
            obj.setOrder(obj.getOrder() + shift);
        }
        newItems.sortAscendingByOrder(); // sort by old order
        newItems.resetOrder(targetOrder);
        newContainer.push_back(newItems);
        this->container() = newContainer;
    }

    template <class OBJ, class CONTAINER>
    void IOrderableList<OBJ, CONTAINER>::freezeOrder()
    {
        int c = 0;
        for (OBJ &obj : container())
        {
            obj.setOrder(c++);
        }
    }

    template<class OBJ, class CONTAINER>
    OBJ IOrderableList<OBJ, CONTAINER>::minOrderOrDefault() const
    {
        if (container().isEmpty()) { return OBJ(); }
        OBJ min = container().front();
        for (const OBJ &obj : container())
        {
            if (!obj.hasValidOrder()) { continue; }
            if (obj.getOrder() < min.getOrder())
            {
                min = obj;
            }
        }
        return min;
    }

    template<class OBJ, class CONTAINER>
    OBJ IOrderableList<OBJ, CONTAINER>::maxOrderOrDefault() const
    {
        if (container().isEmpty()) { return OBJ(); }
        OBJ max = container().front();
        for (const OBJ &obj : container())
        {
            if (!obj.hasValidOrder()) { continue; }
            if (obj.getOrder() > max.getOrder())
            {
                max = obj;
            }
        }
        return max;
    }

    //! \cond PRIVATE
    template class BLACKMISC_EXPORT_DEFINE_TEMPLATE IOrderableList<BlackMisc::Simulation::CAircraftModel, BlackMisc::Simulation::CAircraftModelList>;
    template class BLACKMISC_EXPORT_DEFINE_TEMPLATE IOrderableList<BlackMisc::Simulation::CDistributor, BlackMisc::Simulation::CDistributorList>;
    //! \endcond

} // namespace
