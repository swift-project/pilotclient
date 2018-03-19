/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "listmodelcallsignobjects.h"
#include "allmodelcontainers.h"
#include "blackmisc/db/datastoreobjectlist.h"
#include "blackmisc/orderable.h"

#include <QBrush>
#include <QModelIndex>
#include <type_traits>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;

namespace BlackGui
{
    namespace Models
    {
        template <typename ObjectType, typename ContainerType, bool UseCompare>
        CListModelCallsignObjects<ObjectType, ContainerType, UseCompare>::CListModelCallsignObjects(const QString &translationContext, QObject *parent) :
            CListModelBase<ObjectType, ContainerType, UseCompare>(translationContext, parent)
        { }

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        QVariant CListModelCallsignObjects<ObjectType, ContainerType, UseCompare>::data(const QModelIndex &index, int role) const
        {
            if (role != Qt::BackgroundRole) { return CListModelBase<ObjectType, ContainerType, UseCompare>::data(index, role); }
            if (isHighlightedIndex(index)) { return QBrush(m_highlightColor); }
            return CListModelBase<ObjectType, ContainerType, UseCompare>::data(index, role);
        }

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        BlackMisc::Aviation::CCallsign CListModelCallsignObjects<ObjectType, ContainerType, UseCompare>::callsignForIndex(const QModelIndex &index) const
        {
            if (!index.isValid()) { return CCallsign(); }
            return this->at(index).getCallsign();
        }

        template <typename ObjectType, typename ContainerType, bool UseCompare>
        bool CListModelCallsignObjects<ObjectType, ContainerType, UseCompare>::isHighlightedIndex(const QModelIndex &index) const
        {
            if (!index.isValid()) { return false; }
            if (m_highlightCallsigns.isEmpty()) { return false; }
            return m_highlightCallsigns.contains(callsignForIndex(index));
        }

        // see here for the reason of thess forward instantiations
        // https://isocpp.org/wiki/faq/templates#separate-template-fn-defn-from-decl
        template class CListModelCallsignObjects<BlackMisc::Aviation::CAtcStation, BlackMisc::Aviation::CAtcStationList, true>;
        template class CListModelCallsignObjects<BlackMisc::Simulation::CSimulatedAircraft, BlackMisc::Simulation::CSimulatedAircraftList, true>;
        template class CListModelCallsignObjects<BlackMisc::Simulation::CInterpolationAndRenderingSetupPerCallsign, BlackMisc::Simulation::CInterpolationSetupList, false>;
    } // namespace
} // namespace
