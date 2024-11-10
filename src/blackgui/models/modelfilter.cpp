// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackgui/models/modelfilter.h"
#include "misc/logmessage.h"
#include <QtGlobal>

using namespace swift::misc;

namespace BlackGui::Models
{
    template <class ContainerType>
    bool IModelFilter<ContainerType>::stringMatchesFilterExpression(const QString &value, const QString &filter, Qt::CaseSensitivity cs) const
    {
        QString v = value.trimmed();
        QString f = filter.trimmed();

        if (v.isEmpty() && f.isEmpty()) { return true; }
        if (v.isEmpty()) { return false; }

        // no wildcard, just string matching
        if (!filter.contains('*'))
        {
            return (v.indexOf(f, 0, cs) == 0) && (v.length() == f.length());
        }

        const QString filterNoWildcard = stripWildcard(f);

        // included?
        if (f.startsWith('*') && f.endsWith('*'))
        {
            return v.contains(filterNoWildcard, cs);
        }

        // ends with
        if (f.endsWith('*'))
        {
            return v.startsWith(filterNoWildcard, cs);
        }

        // starting with
        if (f.startsWith('*'))
        {
            return v.endsWith(filterNoWildcard, cs);
        }

        // wildcard in middle
        if (f.contains('*'))
        {
            const QStringList parts = v.split('*');
            if (parts.size() < 2) { return false; }
            const bool s = v.startsWith(parts.front(), cs) && v.endsWith(parts.back());
            return s;
        }

        // should never happen
        CLogMessage(this).error(u"Illegal search pattern : '%1'") << f;
        return false;
    }

    template <class ContainerType>
    QString IModelFilter<ContainerType>::stripWildcard(const QString &value) const
    {
        QString sw(value);
        return sw.remove('*');
    }

    // Forward instantiations
    template class IModelFilter<swift::misc::CStatusMessageList>;
    template class IModelFilter<swift::misc::CNameVariantPairList>;
    template class IModelFilter<swift::misc::CIdentifierList>;

    template class IModelFilter<swift::misc::aviation::CAtcStationList>;
    template class IModelFilter<swift::misc::aviation::CAirportList>;
    template class IModelFilter<swift::misc::aviation::CLiveryList>;
    template class IModelFilter<swift::misc::aviation::CAircraftIcaoCodeList>;
    template class IModelFilter<swift::misc::aviation::CAirlineIcaoCodeList>;
    template class IModelFilter<swift::misc::CCountryList>;

    template class IModelFilter<swift::misc::network::CServerList>;
    template class IModelFilter<swift::misc::network::CUserList>;
    template class IModelFilter<swift::misc::network::CClientList>;
    template class IModelFilter<swift::misc::network::CTextMessageList>;

    template class IModelFilter<swift::misc::simulation::CSimulatedAircraftList>;
    template class IModelFilter<swift::misc::simulation::CAircraftModelList>;
    template class IModelFilter<swift::misc::simulation::CDistributorList>;

} // namespace
