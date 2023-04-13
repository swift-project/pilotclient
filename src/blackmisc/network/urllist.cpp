/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/network/urllist.h"
#include "blackmisc/network/networkutils.h"
#include "blackmisc/math/mathutils.h"
#include "blackmisc/stringutils.h"

#include <QtGlobal>

using namespace BlackMisc::Math;

BLACK_DEFINE_SEQUENCE_MIXINS(BlackMisc::Network, CUrl, CUrlList)

namespace BlackMisc::Network
{
    CUrlList::CUrlList() {}

    CUrlList::CUrlList(const CSequence<CUrl> &other) : CSequence<CUrl>(other)
    {}

    CUrlList::CUrlList(const QStringList &listOfUrls, bool removeDuplicates)
    {
        QStringList urlList(listOfUrls);
        if (removeDuplicates) { urlList.removeDuplicates(); }
        for (const QString &url : urlList)
        {
            this->push_back(CUrl(url));
        }
    }

    CUrl CUrlList::getRandomUrl() const
    {
        if (this->isEmpty()) { return CUrl(); }
        if (this->size() == 1) { return this->front(); }
        const int i = CMathUtils::randomInteger(0, this->size() - 1);
        return (*this)[i];
    }

    CUrl CUrlList::getRandomWorkingUrl(int maxTrials, int timeoutMs) const
    {
        if (this->isEmpty()) { return CUrl(); }
        if (maxTrials < 1) { return CUrl(); }
        CUrlList trials;

        if (timeoutMs < 0) { timeoutMs = CNetworkUtils::getTimeoutMs(); }
        for (int t = 0; t < maxTrials && t < this->size(); t++)
        {
            const CUrl url(this->getRandomWithout(trials));
            trials.push_back(url);
            QString message;
            if (CNetworkUtils::canConnect(url, message, timeoutMs)) { return url; }
        }
        return CUrl();
    }

    CUrl CUrlList::getRandomWithout(const CUrl &exclude) const
    {
        const CUrlList excludes({ exclude });
        return this->getRandomWithout(excludes);
    }

    CUrl CUrlList::getRandomWithout(const CUrlList &exclude) const
    {
        CUrlList copy(*this);
        copy.removeIfIn(exclude);
        if (copy.isEmpty()) { return CUrl(); }
        return copy.getRandomUrl();
    }

    CUrlList CUrlList::withAppendedPath(const QString &path) const
    {
        if (path.isEmpty() || this->isEmpty()) { return (*this); }
        CUrlList urls;
        for (const CUrl &url : (*this))
        {
            urls.push_back(url.withAppendedPath(path));
        }
        return urls;
    }

    CUrlList CUrlList::findByHost(const QString &host, Qt::CaseSensitivity cs) const
    {
        CUrlList result;
        if (host.isEmpty() || this->isEmpty()) { return result; }
        for (const CUrl &url : *this)
        {
            if (stringCompare(url.getHost(), host, cs))
            {
                result.push_back(url);
            }
        }
        return result;
    }

    QString CUrlList::convertToQString(const QString &separator, bool i18n) const
    {
        const QStringList sl(toStringList(i18n));
        return sl.join(separator);
    }

    CUrlList CUrlList::getWithoutDuplicates() const
    {
        if (this->size() < 2) { return (*this); }
        CUrlList withoutDuplicates;
        for (const CUrl &url : (*this))
        {
            withoutDuplicates.replaceOrAdd(url);
        }
        return withoutDuplicates;
    }

    int CUrlList::removeDuplicates()
    {
        if (this->size() < 2) { return 0; }
        const CUrlList withoutDuplicates(getWithoutDuplicates());
        if (this->size() == withoutDuplicates.size()) { return 0; }
        int r = this->size() - withoutDuplicates.size();
        (*this) = withoutDuplicates;
        return r;
    }

    CFailoverUrlList::CFailoverUrlList(int maxTrials) : m_maxTrials(maxTrials)
    {}

    CFailoverUrlList::CFailoverUrlList(const QStringList &listOfUrls, int maxTrials) : CUrlList(listOfUrls), m_maxTrials(maxTrials)
    {}

    CFailoverUrlList::CFailoverUrlList(const CUrlList &urlIst, int maxTrials) : CUrlList(urlIst), m_maxTrials(maxTrials)
    {}

    CUrlList CFailoverUrlList::getWithoutFailed() const
    {
        CUrlList urls(*this);
        urls.removeIfIn(m_failedUrls);
        return urls;
    }

    bool CFailoverUrlList::addFailedUrl(const CUrl &failedUrl)
    {
        Q_ASSERT_X(!failedUrl.isEmpty(), Q_FUNC_INFO, "empty URL as failed");
        m_failedUrls.push_back(failedUrl);
        return hasMoreUrlsToTry();
    }

    bool CFailoverUrlList::addFailedUrls(const CUrlList &failedUrls)
    {
        m_failedUrls.push_back(failedUrls);
        return hasMoreUrlsToTry();
    }

    bool CFailoverUrlList::addFailedHost(const CUrl &failedUrl)
    {
        Q_ASSERT_X(!failedUrl.isEmpty(), Q_FUNC_INFO, "empty URL as failed");
        const QString host = failedUrl.getHost();
        return CFailoverUrlList::addFailedHost(host);
    }

    bool CFailoverUrlList::addFailedHost(const QString &host, Qt::CaseSensitivity cs)
    {
        if (host.isEmpty()) { return this->hasMoreUrlsToTry(); }
        const CUrlList failedUrls = this->findByHost(host, cs);
        return addFailedUrls(failedUrls);
    }

    bool CFailoverUrlList::hasMoreUrlsToTry() const
    {
        return this->numberOfStillValidUrls() > 0;
    }

    int CFailoverUrlList::numberOfStillValidUrls() const
    {
        if (this->isEmpty()) { return 0; }
        if (m_failedUrls.size() >= m_maxTrials) { return 0; }
        const int trailsLeft1 = qMax(m_maxTrials - m_failedUrls.size(), 0);
        const int trailsLeft2 = qMax(this->size() - m_failedUrls.size(), 0);
        return qMin(trailsLeft1, trailsLeft2);
    }

    CUrl CFailoverUrlList::obtainNextWorkingUrl(bool random, int connectTimeoutMs)
    {
        if (!hasMoreUrlsToTry()) { return CUrl(); }
        const CUrl url(this->obtainNextUrlWithout(random, m_failedUrls));
        QString msg;
        if (CNetworkUtils::canConnect(url, msg, connectTimeoutMs)) { return url; }
        if (addFailedUrl(url))
        {
            if (!msg.isEmpty())
            {
                m_errorMsgs.append(QStringLiteral("URL: %1 error: %2").arg(url.toQString(), msg));
            }
            return obtainNextWorkingUrl(random, connectTimeoutMs);
        }
        return CUrl();
    }

    CUrl CFailoverUrlList::obtainNextUrl(bool randomStart)
    {
        if (this->isEmpty()) { return CUrl(); }
        if (this->size() == 1) { return this->front(); }
        if (m_currentIndexDistributedLoad < 0)
        {
            // random start point
            m_currentIndexDistributedLoad = randomStart ?
                                                CMathUtils::randomInteger(0, this->size() - 1) :
                                                0;
        }
        else
        {
            m_currentIndexDistributedLoad++;
            if (m_currentIndexDistributedLoad >= this->size())
            {
                m_currentIndexDistributedLoad = 0;
            }
        }
        return (*this)[m_currentIndexDistributedLoad];
    }

    CUrl CFailoverUrlList::obtainNextUrlWithout(bool randomStart, const CUrlList &exclude) const
    {
        CFailoverUrlList copy(*this);
        copy.removeIfIn(exclude);
        if (copy.isEmpty()) { return CUrl(); }
        return copy.obtainNextUrl(randomStart);
    }

    void CFailoverUrlList::reset(int maxTrials)
    {
        m_failedUrls.clear();
        if (maxTrials >= 0) { m_maxTrials = maxTrials; }
    }
} // namespace
