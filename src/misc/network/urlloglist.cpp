// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/network/urlloglist.h"

SWIFT_DEFINE_SEQUENCE_MIXINS(swift::misc::network, CUrlLog, CUrlLogList)

namespace swift::misc::network
{
    CUrlLogList::CUrlLogList() {}

    CUrlLogList::CUrlLogList(const CSequence &other) : CSequence<CUrlLog>(other) {}

    int CUrlLogList::addPendingUrl(const CUrl &url, int maxNumber)
    {
        if (maxNumber > 0) this->truncate(maxNumber - 1);
        const CUrlLog rl(url);
        this->push_front(rl);
        return rl.getId();
    }

    int CUrlLogList::addPendingUrl(const CUrl &url, QNetworkReply *nwReply, int maxNumber)
    {
        const int id = this->addPendingUrl(url, maxNumber);
        if (nwReply) { nwReply->setProperty(CUrlLog::propertyNameId(), QVariant::fromValue(id)); }
        return id;
    }

    CUrlLogList CUrlLogList::findPending() const { return this->findBy(&CUrlLog::isPending, true); }

    CUrlLogList CUrlLogList::findOutdatedPending(int outdatedOffsetMs) const
    {
        if (this->isEmpty()) { return CUrlLogList(); }
        return this->findPending().findBeforeNowMinusOffset(outdatedOffsetMs);
    }

    CUrlLogList CUrlLogList::findErrors() const
    {
        return this->findBy(&CUrlLog::isPending, false, &CUrlLog::isSuccess, false);
    }

    int CUrlLogList::sizePending() const
    {
        if (this->isEmpty()) return 0;
        return this->findPending().size();
    }

    bool CUrlLogList::hasPending() const
    {
        // faster as using sizePending()
        return this->contains(&CUrlLog::isPending, true);
    }

    bool CUrlLogList::hasCompleted() const
    {
        // faster as using sizePending()
        return this->contains(&CUrlLog::isPending, false);
    }

    int CUrlLogList::sizeErrors() const
    {
        if (this->isEmpty()) return 0;
        return this->findErrors().size();
    }

    CUrlLog CUrlLogList::findByIdOrDefault(int id) const { return this->findFirstByOrDefault(&CUrlLog::getId, id); }

    bool CUrlLogList::markAsReceived(int id, bool success)
    {
        for (CUrlLog &rl : *this)
        {
            if (rl.getId() == id)
            {
                rl.setResponseTimestampToNow();
                rl.setSuccess(success);
                return true;
            }
        }
        return false;
    }

    bool CUrlLogList::markAsReceived(const QNetworkReply *nwReply, bool success)
    {
        Q_ASSERT_X(nwReply, Q_FUNC_INFO, "missing reply");
        bool ok;
        const int id = nwReply->property(CUrlLog::propertyNameId()).toInt(&ok);
        return (ok && id >= 0) ? this->markAsReceived(id, success) : false;
    }

    bool CUrlLogList::containsId(int id) const { return this->contains(&CUrlLog::getId, id); }

    qint64 CUrlLogList::getMaxResponseTime() const
    {
        qint64 max = 0;
        for (const CUrlLog &rl : *this)
        {
            if (rl.isPending()) { continue; }
            if (rl.getResponseTimeMs() > max) { max = rl.getResponseTimeMs(); }
        }
        return max;
    }

    qint64 CUrlLogList::getMinResponseTime() const
    {
        if (!this->hasCompleted()) { return 0; }
        qint64 min = std::numeric_limits<qint64>::max();
        for (const CUrlLog &rl : *this)
        {
            if (rl.isPending()) { continue; }
            if (rl.getResponseTimeMs() < min) { min = rl.getResponseTimeMs(); }
        }
        return min;
    }

    qint64 CUrlLogList::getAverageResponseTime() const
    {
        qint64 sum = 0;
        int c = 0;
        for (const CUrlLog &rl : *this)
        {
            if (rl.isPending()) { continue; }
            sum += rl.getResponseTimeMs();
            c++;
        }
        if (c == 0) return 0;
        return sum / c;
    }

    QString CUrlLogList::getSummary() const
    {
        static const QString s("Entries: %1, pending: %2, errors: %3, min: %4ms avg: %5ms max: %6ms");
        if (this->isEmpty()) return QStringLiteral("No data");
        return s.arg(this->size())
            .arg(this->sizePending())
            .arg(this->sizeErrors())
            .arg(this->getMinResponseTime())
            .arg(this->getAverageResponseTime())
            .arg(this->getMaxResponseTime());
    }
} // namespace swift::misc::network
