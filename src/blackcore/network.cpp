/* Copyright (C) 2019
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "network.h"
#include "blackmisc/fileutils.h"
#include "blackmisc/directoryutils.h"
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

    INetwork::~INetwork()
    { }

    int INetwork::increaseStatisticsValue(const QString &identifier, const QString &appendix)
    {
        if (identifier.isEmpty() || !m_statistics) { return -1; }
        const QString i = appendix.isEmpty() ? identifier : identifier % u"." % appendix;
        int &v =  m_callStatistics[i];
        v++;

        constexpr int MaxTimeValues = 50;
        m_callByTime.push_front(QPair<qint64, QString>(QDateTime::currentMSecsSinceEpoch(), i));
        if (m_callByTime.size() > MaxTimeValues) { m_callByTime.removeLast(); }
        return v;
    }

    int INetwork::increaseStatisticsValue(const QString &identifier, int value)
    {
        return this->increaseStatisticsValue(identifier, QString::number(value));
    }

    void INetwork::clearStatistics()
    {
        m_callStatistics.clear();
        m_callByTime.clear();
    }

    QString INetwork::getNetworkStatisticsAsText(bool reset, const QString &separator)
    {
        QVector<std::pair<int, QString>> transformed;
        if (m_callStatistics.isEmpty()) { return QString(); }

        for (const auto pair : makePairsRange(as_const(m_callStatistics)))
        {
            // key is pair.first, value is pair.second
            transformed.push_back({ pair.second, pair.first });
        }

        // sorted by value
        std::sort(transformed.begin(), transformed.end(), std::greater<>());
        QString stats;
        for (const auto &pair : transformed)
        {
            stats +=
                (stats.isEmpty() ? QString() : separator) %
                pair.second % u": " % QString::number(pair.first);
        }

        for (const auto &pair : transformed)
        {
            stats +=
                (stats.isEmpty() ? QString() : separator) %
                pair.second % u": " % QString::number(pair.first);
        }

        if (!m_callByTime.isEmpty())
        {
            const qint64 lastTs = m_callByTime.front().first;
            for (const auto &pair : m_callByTime)
            {
                const qint64 deltaTs = lastTs - pair.first;
                stats += separator % QStringLiteral("%1").arg(deltaTs, 5, 10, QChar('0')) % u": " % pair.second;
            }
        }

        if (reset) { this->clearStatistics(); }
        return stats;
    }

    bool INetwork::saveNetworkStatistics(const QString &server)
    {
        if (m_callStatistics.isEmpty()) { return false; }

        const QString s = this->getNetworkStatisticsAsText(false, "\n");
        if (s.isEmpty()) { return false; }
        const QString fn = QStringLiteral("networkstatistics_%1_%2.log").arg(QDateTime::currentDateTimeUtc().toString("yyMMddhhmmss"), server);
        const QString fp = CFileUtils::appendFilePaths(CDirectoryUtils::logDirectory(), fn);
        return CFileUtils::writeStringToFile(s, fp);
    }

} // ns
