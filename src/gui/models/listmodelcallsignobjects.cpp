// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "listmodelcallsignobjects.h"
#include "gui/models/allmodelcontainers.h"
#include "misc/orderable.h"

#include <QBrush>
#include <QModelIndex>
#include <type_traits>

using namespace swift::misc;
using namespace swift::misc::aviation;

namespace swift::gui::models
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
    swift::misc::aviation::CCallsign CListModelCallsignObjects<T, UseCompare>::callsignForIndex(const QModelIndex &index) const
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
    template class CListModelCallsignObjects<swift::misc::aviation::CAtcStationList, true>;
    template class CListModelCallsignObjects<swift::misc::simulation::CSimulatedAircraftList, true>;
    template class CListModelCallsignObjects<swift::misc::simulation::CInterpolationSetupList, false>;
} // namespace
