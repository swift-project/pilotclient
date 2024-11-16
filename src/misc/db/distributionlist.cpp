// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/db/distributionlist.h"

#include "misc/stringutils.h"

SWIFT_DEFINE_SEQUENCE_MIXINS(swift::misc::db, CDistribution, CDistributionList)

namespace swift::misc::db
{
    CDistributionList::CDistributionList() {}

    CDistributionList::CDistributionList(const CSequence<CDistribution> &other) : CSequence<CDistribution>(other) {}

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
        this->sort([order](const CDistribution &a, const CDistribution &b) {
            const int as = a.getStability();
            const int bs = b.getStability();
            return order == Qt::AscendingOrder ? as < bs : bs < as;
        });
    }

    bool CDistributionList::containsEqualOrMoreStable(CDistribution &distribution) const
    {
        return containsBy(
            [&distribution](const CDistribution &dist) { return dist.isStabilitySameOrBetter(distribution); });
    }

    bool CDistributionList::containsUnrestricted() const { return this->contains(&CDistribution::isRestricted, false); }

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
        return CDistributionList::fromDatabaseJson(json::jsonArrayFromString(json));
    }
} // namespace swift::misc::db
