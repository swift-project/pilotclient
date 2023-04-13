/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackgui/models/modelfilter.h"
#include "blackmisc/logmessage.h"
#include <QtGlobal>

using namespace BlackMisc;

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
    template class IModelFilter<BlackMisc::CStatusMessageList>;
    template class IModelFilter<BlackMisc::CNameVariantPairList>;
    template class IModelFilter<BlackMisc::CIdentifierList>;

    template class IModelFilter<BlackMisc::Aviation::CAtcStationList>;
    template class IModelFilter<BlackMisc::Aviation::CAirportList>;
    template class IModelFilter<BlackMisc::Aviation::CLiveryList>;
    template class IModelFilter<BlackMisc::Aviation::CAircraftIcaoCodeList>;
    template class IModelFilter<BlackMisc::Aviation::CAirlineIcaoCodeList>;
    template class IModelFilter<BlackMisc::CCountryList>;

    template class IModelFilter<BlackMisc::Network::CServerList>;
    template class IModelFilter<BlackMisc::Network::CUserList>;
    template class IModelFilter<BlackMisc::Network::CClientList>;
    template class IModelFilter<BlackMisc::Network::CTextMessageList>;

    template class IModelFilter<BlackMisc::Simulation::CSimulatedAircraftList>;
    template class IModelFilter<BlackMisc::Simulation::CAircraftModelList>;
    template class IModelFilter<BlackMisc::Simulation::CDistributorList>;

} // namespace
