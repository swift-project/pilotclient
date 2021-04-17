/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_ORDERABLELIST_H
#define BLACKMISC_ORDERABLELIST_H

#include "blackmisc/blackmiscexport.h"
#include <QList>

namespace BlackMisc
{
    //! List of orderable IOrderable objects
    //! \remark Orerable objects shall implement BlackMisc::IOrderable
    template<class OBJ, class CONTAINER>
    class IOrderableList
    {
        static_assert(std::is_base_of_v<IOrderable, OBJ>, "OBJ needs to implement IOrderable");

    public:
        //! Sort ascending
        void sortAscendingByOrder()
        {
            this->container().sort(Predicates::MemberLess(&OBJ::getOrder));
        }

        //! Sort descending
        void sortDescendingByOrder()
        {
            this->container().sortAscendingByOrder();
            std::reverse(this->container().begin(), this->container().end());
        }

        //! Set order member to current order
        void resetOrder(int offset = 0)
        {
            int c = offset;
            for (OBJ &obj : container())
            {
                obj.setOrder(c++);
            }
        }

        //! All order values set or missing some?
        bool needsOrder() const
        {
            for (const OBJ &obj : container())
            {
                if (!obj.hasValidOrder()) { return true; }
            }
            return false;
        }

        //! All order values IOrderable::order
        QList<int> orderValues() const
        {
            QList<int> orders;
            for (const OBJ &obj : container())
            {
                if (!obj.hasValidOrder()) { continue; }
                orders.append(obj.getOrder());
            }
            return orders;
        }

        //! Items with order will not be included
        CONTAINER withoutItemsOfSameOrder(const CONTAINER &items) const
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

        //! Remove the items based on their order IOrderable::order
        void removeItemsWithSameOrder(const CONTAINER &items)
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

        //! Move items to given order
        void moveTo(const CONTAINER &items, int targetOrder)
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

        //! Current order of list will be new order values
        void freezeOrder()
        {
            int c = 0;
            for (OBJ &obj : container())
            {
                obj.setOrder(c++);
            }
        }

        //! Current reverse order of list will be new order values
        void freezeOrderReverse()
        {
            int c = this->container().size() - 1;
            for (OBJ &obj : container())
            {
                obj.setOrder(c--);
            }
        }

        //! Object with min.order or default
        OBJ minOrderOrDefault() const
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

        //! Object with max.order or default
        OBJ maxOrderOrDefault() const
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

    protected:
        //! Constructor
        IOrderableList() = default;

        //! Container
        const CONTAINER &container() const
        {
            return static_cast<const CONTAINER &>(*this);
        }

        //! Container
        CONTAINER &container()
        {
            return static_cast<CONTAINER &>(*this);
        }
    };
} //namespace

#endif //guard
