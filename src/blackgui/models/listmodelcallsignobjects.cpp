/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "listmodelcallsignobjects.h"
#include "blackgui/models/allmodelcontainers.h"
#include "blackmisc/orderable.h"

#include <QBrush>
#include <QModelIndex>
#include <type_traits>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;

namespace BlackGui::Models
{
    template <typename T, bool UseCompare>
    CListModelCallsignObjects<T, UseCompare>::CListModelCallsignObjects(const QString &translationContext, QObject *parent) : CListModelBase<ContainerType, UseCompare>(translationContext, parent)
    {}

    template <typename T, bool UseCompare>
    QVariant CListModelCallsignObjects<T, UseCompare>::data(const QModelIndex &index, int role) const
    {
        if (role != Qt::BackgroundRole) { return CListModelBase<ContainerType, UseCompare>::data(index, role); }
        if (isHighlightedIndex(index)) { return QBrush(m_highlightColor); }
        return CListModelBase<ContainerType, UseCompare>::data(index, role);
    }

    template <typename T, bool UseCompare>
    BlackMisc::Aviation::CCallsign CListModelCallsignObjects<T, UseCompare>::callsignForIndex(const QModelIndex &index) const
    {
        if (!index.isValid()) { return CCallsign(); }
        return this->at(index).getCallsign();
    }

    template <typename T, bool UseCompare>
    bool CListModelCallsignObjects<T, UseCompare>::isHighlightedIndex(const QModelIndex &index) const
    {
        if (!index.isValid()) { return false; }
        if (m_highlightCallsigns.isEmpty()) { return false; }
        return m_highlightCallsigns.contains(callsignForIndex(index));
    }

    // see here for the reason of thess forward instantiations
    // https://isocpp.org/wiki/faq/templates#separate-template-fn-defn-from-decl
    template class CListModelCallsignObjects<BlackMisc::Aviation::CAtcStationList, true>;
    template class CListModelCallsignObjects<BlackMisc::Simulation::CSimulatedAircraftList, true>;
    template class CListModelCallsignObjects<BlackMisc::Simulation::CInterpolationSetupList, false>;
} // namespace
