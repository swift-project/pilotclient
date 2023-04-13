/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackcore/threadedreader.h"
#include "blackcore/application.h"
#include "blackmisc/network/networkutils.h"
#include "blackmisc/threadutils.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/verify.h"

#include <QCoreApplication>
#include <QMetaObject>
#include <QReadLocker>
#include <QThread>
#include <QTimer>
#include <QWriteLocker>

using namespace BlackMisc;
using namespace BlackMisc::Network;
using namespace BlackCore::Vatsim;

namespace BlackCore
{
    const QStringList &CThreadedReader::getLogCategories()
    {
        static const QStringList cats { BlackMisc::CLogCategories::worker() };
        return cats;
    }

    CThreadedReader::CThreadedReader(QObject *owner, const QString &name) : CContinuousWorker(owner, name)
    {
        connect(&m_updateTimer, &QTimer::timeout, this, &CThreadedReader::doWork);
        m_updateTimer.setSingleShot(true);
    }

    CThreadedReader::~CThreadedReader()
    {}

    qint64 CThreadedReader::lastModifiedMsSinceEpoch(QNetworkReply *nwReply) const
    {
        return CNetworkUtils::lastModifiedMsSinceEpoch(nwReply);
    }

    QDateTime CThreadedReader::getUpdateTimestamp() const
    {
        QReadLocker lock(&m_lock);
        return m_updateTimestamp;
    }

    void CThreadedReader::setUpdateTimestamp(const QDateTime &updateTimestamp)
    {
        QWriteLocker lock(&m_lock);
        m_updateTimestamp = updateTimestamp;
    }

    bool CThreadedReader::updatedWithinLastMs(qint64 timeLastMs)
    {
        QDateTime dt(getUpdateTimestamp());
        if (dt.isNull() || !dt.isValid()) { return false; }
        qint64 delta = QDateTime::currentMSecsSinceEpoch() - dt.toMSecsSinceEpoch();
        return delta <= timeLastMs;
    }

    bool CThreadedReader::isInternetAccessible(const QString &logWarningMessage) const
    {
        const bool a = sApp->isInternetAccessible();
        if (!a && !logWarningMessage.isEmpty())
        {
            CLogMessage(this).warning(logWarningMessage);
        }
        return a;
    }

    void CThreadedReader::startReader()
    {
        Q_ASSERT(m_initialTime > 0);
        QTimer::singleShot(m_initialTime, this, [=] { this->doWork(); });
    }

    void CThreadedReader::pauseReader()
    {
        QTimer::singleShot(0, &m_updateTimer, &QTimer::stop);
    }

    bool CThreadedReader::hasPendingUrls() const
    {
        QReadLocker l(&m_lock);
        return m_urlReadLog.hasPending();
    }

    CUrlLogList CThreadedReader::getUrlLogList() const
    {
        QReadLocker l(&m_lock);
        return m_urlReadLog;
    }

    bool CThreadedReader::didContentChange(const QString &content, int startPosition)
    {
        uint oldHash = 0;
        {
            QReadLocker rl(&m_lock);
            oldHash = m_contentHash;
        }
        uint newHash = qHash(startPosition < 0 ? content : content.mid(startPosition));
        if (oldHash == newHash) { return false; }
        {
            QWriteLocker wl(&m_lock);
            m_contentHash = newHash;
        }
        return true;
    }

    bool CThreadedReader::isMarkedAsFailed() const
    {
        return m_markedAsFailed;
    }

    void CThreadedReader::setMarkedAsFailed(bool failed)
    {
        m_markedAsFailed = failed;
    }

    CUrlLogList CThreadedReader::getReadLog() const
    {
        QReadLocker rl(&m_lock);
        return m_urlReadLog;
    }

    void CThreadedReader::threadAssertCheck() const
    {
        Q_ASSERT_X(QCoreApplication::instance()->thread() != QThread::currentThread(), Q_FUNC_INFO, "Needs to run in own thread");
        Q_ASSERT_X(this->thread() == QThread::currentThread(), Q_FUNC_INFO, "Wrong object thread");
    }

    void CThreadedReader::setInitialAndPeriodicTime(int initialTime, int periodicTime)
    {
        m_initialTime = initialTime;
        m_periodicTime = periodicTime;

        // if timer is active start with delta time
        // remark: will be reset in doWork
        if (m_updateTimer.isActive())
        {
            const int oldPeriodicTime = m_updateTimer.interval();
            const int delta = m_periodicTime - oldPeriodicTime + m_updateTimer.remainingTime();
            m_updateTimer.start(qMax(delta, 0));
        }
    }

    void CThreadedReader::doWork()
    {
        if (!doWorkCheck()) { return; }
        this->doWorkImpl();
        Q_ASSERT(m_periodicTime > 0);
        m_updateTimer.start(m_periodicTime); // restart
    }

    bool CThreadedReader::doWorkCheck() const
    {
        // sApp->hasWebDataServices() cannot be used, as some readers are already used during init phase
        if (!this->isEnabled()) { return false; }

        // MS 2019-02-23 isAbandoned() check only makes sense when called by worker thread (T541)
        if (CThreadUtils::isInThisThread(this) && this->isAbandoned()) { return false; }

        if (!m_unitTest && (!sApp || sApp->isShuttingDown())) { return false; }
        return true;
    }

    QNetworkReply *CThreadedReader::getFromNetworkAndLog(const CUrl &url, const CSlot<void(QNetworkReply *)> &callback)
    {
        QWriteLocker wl(&m_lock);
        const CUrlLogList outdatedPendingUrls = m_urlReadLog.findOutdatedPending(OutdatedPendingCallMs);
        if (!outdatedPendingUrls.isEmpty())
        {
            CLogMessage(this).warning(u"Detected outdated reader pending calls: '%1'") << outdatedPendingUrls.toQString(true);
            m_urlReadLog.removeOlderThanNowMinusOffset(OutdatedPendingCallMs); // clean up
        }

        const int id = m_urlReadLog.addPendingUrl(url);
        wl.unlock();

        // returned QNetworkReply normally nullptr since QAM is in different thread
        QNetworkReply *nr = sApp->getFromNetwork(url, id, callback, { this, &CThreadedReader::networkReplyProgress });
        return nr;
    }

    QPair<qint64, qint64> CThreadedReader::getNetworkReplyBytes() const
    {
        return QPair<qint64, qint64>(m_networkReplyCurrent, m_networkReplyNax);
    }

    void CThreadedReader::networkReplyProgress(int logId, qint64 current, qint64 max, const QUrl &url)
    {
        // max can be -1 if file size is not available
        m_networkReplyProgress = (current > 0 && max > 0) ? static_cast<int>((current * 100) / max) : -1;
        m_networkReplyCurrent = current;
        m_networkReplyNax = max;

        Q_UNUSED(url);
        Q_UNUSED(logId);
    }

    void CThreadedReader::logNetworkReplyReceived(QNetworkReply *reply)
    {
        if (!reply) { return; }
        QWriteLocker wl(&m_lock);
        m_urlReadLog.markAsReceived(reply, reply->error() == QNetworkReply::NoError);
    }

    void CThreadedReader::logInconsistentData(const CStatusMessage &msg, const char *funcInfo)
    {
        if (msg.isEmpty()) { return; }
        CStatusMessage logMsg(msg);
        logMsg.addCategory(CLogCategories::dataInconsistency());
        if (funcInfo)
        {
            const QByteArray m(logMsg.getMessage().toLatin1());
            BLACK_AUDIT_X(false, funcInfo, m.constData());
        }
        CLogMessage::preformatted(logMsg);
    }
} // namespace
