/* Copyright (C) 2017
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/db/distributionlist.h"
#include "blackmisc/stringutils.h"

namespace BlackMisc::Db
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

    void CDistributionList::sortByStability(Qt::SortOrder order)
    {
        this->sort([order](const CDistribution & a, const CDistribution & b)
        {
            const int as = a.getStability();
            const int bs = b.getStability();
            return order == Qt::AscendingOrder ? as < bs : bs < as;
        });
    }

    bool CDistributionList::containsEqualOrMoreStable(CDistribution &distribution) const
    {
        return containsBy([&distribution](const CDistribution & dist) { return dist.isStabilitySameOrBetter(distribution); });
    }

    bool CDistributionList::containsUnrestricted() const
    {
        return this->contains(&CDistribution::isRestricted, false);
    }

    bool CDistributionList::containsChannel(const QString &channel) const
    {
        return this->contains(&CDistribution::getChannel, channel);
    }

    CDistribution CDistributionList::findFirstByChannelOrDefault(const QString &channel) const
    {
        return this->findFirstByOrDefault(&CDistribution::getChannel, channel);
    }

    CDistributionList CDistributionList::findByRestriction(bool restricted) const
    {
        return this->findBy(&CDistribution::isRestricted, restricted);
    }

    CDistribution CDistributionList::getMostStableOrDefault() const
    {
        if (this->size() < 2) { return this->frontOrDefault(); }
        CDistributionList copy(*this);
        copy.sortByStability();
        return copy.back();
    }

    CDistribution CDistributionList::getLeastStableOrDefault() const
    {
        if (this->size() < 2) { return this->frontOrDefault(); }
        CDistributionList copy(*this);
        copy.sortByStability();
        return copy.front();
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
