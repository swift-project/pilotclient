/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
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
    public:
        //! Sort ascending
        void sortAscendingByOrder();

        //! Sort descending
        void sortDescendingByOrder();

        //! Set order member to current order
        void resetOrder(int offset = 0);

        //! All order values set or missing some?
        bool needsOrder() const;

        //! All order values IOrderable::order
        QList<int> orderValues() const;

        //! Items with order will not be included
        CONTAINER withoutItemsOfSameOrder(const CONTAINER &items) const;

        //! Remove the items based on their order IOrderable::order
        void removeItemsWithSameOrder(const CONTAINER &items);

        //! Move items to given order
        void moveTo(const CONTAINER &items, int targetOrder);

        //! Current order of list will be new order values
        void freezeOrder();

        //! Current reverse order of list will be new order values
        void freezeOrderReverse();

        //! Object with min.order or default
        OBJ minOrderOrDefault() const;

        //! Object with max.order or default
        OBJ maxOrderOrDefault() const;

    protected:
        //! Constructor
        IOrderableList();

        //! Container
        const CONTAINER &container() const;

        //! Container
        CONTAINER &container();
    };

    //! \cond PRIVATE
    class CStatusMessage;
    class CStatusMessageList;

    namespace Simulation
    {
        class CDistributor;
        class CDistributorList;
        class CAircraftModel;
        class CAircraftModelList;
    }

    extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE IOrderableList<BlackMisc::CStatusMessage, BlackMisc::CStatusMessageList>;
    extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE IOrderableList<BlackMisc::Simulation::CAircraftModel, BlackMisc::Simulation::CAircraftModelList>;
    extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE IOrderableList<BlackMisc::Simulation::CDistributor, BlackMisc::Simulation::CDistributorList>;
    //! \endcond

} //namespace

#endif //guard
