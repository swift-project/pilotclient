/* Copyright (C) 2017
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "distributionlist.h"
#include "blackmisc/stringutils.h"

namespace BlackMisc
{
    namespace Db
    {
        CDistributionList::CDistributionList() { }

        CDistributionList::CDistributionList(const CSequence<CDistribution> &other) :
            CSequence<CDistribution>(other)
        { }

        QStringList CDistributionList::getChannels() const
        {
            QStringList channels;
            for (const CDistribution &distribution : *this)
            {
                if (distribution.getChannel().isEmpty()) { continue; }
                channels << distribution.getChannel();
            }
            return channels;
        }

        CDistribution CDistributionList::findByChannelOrDefault(const QString &channel) const
        {
            return this->findFirstByOrDefault(&CDistribution::getChannel, channel);
        }

        QString CDistributionList::getVersionForChannelAndPlatform(const QString &channel, const QString &platform) const
        {
            const CDistribution dist = this->findByChannelOrDefault(channel);
            return dist.getVersionString(platform);
        }

        QVersionNumber CDistributionList::getQVersionForChannelAndPlatform(const QString &channel, const QString &platform) const
        {
            const CDistribution dist = this->findByChannelOrDefault(channel);
            return dist.getQVersion(platform);
        }

        QString CDistributionList::getVersionForChannelAndPlatform(const QStringList &channelPlatform) const
        {
            Q_ASSERT_X(channelPlatform.length() != 2, Q_FUNC_INFO, "Wrong size");
            return this->getVersionForChannelAndPlatform(channelPlatform.first(), channelPlatform.last());
        }

        QVersionNumber CDistributionList::getQVersionForChannelAndPlatform(const QStringList &channelPlatform) const
        {
            Q_ASSERT_X(channelPlatform.length() == 2, Q_FUNC_INFO, "Wrong size");
            return this->getQVersionForChannelAndPlatform(channelPlatform.first(), channelPlatform.last());
        }

        CDistributionList CDistributionList::fromDatabaseJson(const QJsonArray &array)
        {
            CDistributionList distributions;
            for (const QJsonValue &value : array)
            {
                const CDistribution distribution(CDistribution::fromDatabaseJson(value.toObject()));
                distributions.push_back(distribution);
            }
            return distributions;
        }

        CDistributionList CDistributionList::fromDatabaseJson(const QString &json)
        {
            if (json.isEmpty()) { return CDistributionList(); }
            return CDistributionList::fromDatabaseJson(Json::jsonArrayFromString(json));
        }
    } // namespace
} // namespace
