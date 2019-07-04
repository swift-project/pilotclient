/* Copyright (C) 2019
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "network.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/range.h"
#include "blackconfig/buildconfig.h"

#include <QStringBuilder>

using namespace BlackConfig;
using namespace BlackMisc;
using namespace BlackMisc::Network;
using namespace BlackMisc::Simulation;

namespace BlackCore
{
    INetwork::INetwork(IClientProvider *clientProvider, IOwnAircraftProvider *ownAircraftProvider, IRemoteAircraftProvider *remoteAircraftProvider, QObject *parent) :
        QObject(parent),
        CClientAware(clientProvider),
        COwnAircraftAware(ownAircraftProvider),
        CRemoteAircraftAware(remoteAircraftProvider)
    {
        if (CBuildConfig::isLocalDeveloperDebugBuild())
        {
            CLogMessage("Enabled network statistics");
            m_statistics = true;
        }
    }
    int INetwork::increaseStatisticsValue(const QString &identifier, const QString &appendix)
    {
        if (identifier.isEmpty() || !m_statistics) { return -1; }
        int &v = appendix.isEmpty() ? m_callStatistics[identifier] : m_callStatistics[identifier % u"." % appendix];
        v++;
        return v;
    }

    int INetwork::increaseStatisticsValue(const QString &identifier, int value)
    {
        return this->increaseStatisticsValue(identifier, QString::number(value));
    }

    QString INetwork::getNetworkStatisticsAsText(bool reset, const QString &separator)
    {
        QMap<int, QString> transformed;
        for (const auto pair : makePairsRange(as_const(m_callStatistics)))
        {
            // key is pair.first, value is pair.second
            transformed.insertMulti(pair.second, pair.first);
        }

        if (reset) { this->clearStatistics(); }

        // sorted by value
        QString stats;
        for (const auto pair : makePairsRange(as_const(transformed)))
        {
            stats +=
                (stats.isEmpty() ? QString() : separator) %
                pair.second % u": " % QString::number(pair.first);
        }
        return stats;
    }
} // ns
