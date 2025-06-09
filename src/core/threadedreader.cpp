// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "core/threadedreader.h"

#include <QCoreApplication>
#include <QMetaObject>
#include <QReadLocker>
#include <QThread>
#include <QTimer>
#include <QWriteLocker>

#include "core/application.h"
#include "misc/logmessage.h"
#include "misc/network/networkutils.h"
#include "misc/threadutils.h"
#include "misc/verify.h"

using namespace swift::misc;
using namespace swift::misc::network;
using namespace swift::core::vatsim;

namespace swift::core
{
    const QStringList &CThreadedReader::getLogCategories()
    {
        static const QStringList cats { swift::misc::CLogCategories::worker() };
        return cats;
    }

    CThreadedReader::CThreadedReader(QObject *owner, const QString &name) : CContinuousWorker(owner, name) {}

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

    CUrlLogList CThreadedReader::getUrlLogList() const
    {
        QReadLocker l(&m_lock);
        return m_urlReadLog;
    }

    bool CThreadedReader::didContentChange(const QString &content, int startPosition)
    {
        size_t oldHash = 0;
        {
            QReadLocker rl(&m_lock);
            oldHash = m_contentHash;
        }
        size_t newHash = qHash(startPosition < 0 ? content : content.mid(startPosition));
        if (oldHash == newHash) { return false; }
        {
            QWriteLocker wl(&m_lock);
            m_contentHash = newHash;
        }
        return true;
    }

    bool CThreadedReader::isMarkedAsFailed() const { return m_markedAsFailed; }

    void CThreadedReader::setMarkedAsFailed(bool failed) { m_markedAsFailed = failed; }

    CUrlLogList CThreadedReader::getReadLog() const
    {
        QReadLocker rl(&m_lock);
        return m_urlReadLog;
    }

    void CThreadedReader::threadAssertCheck() const
    {
        Q_ASSERT_X(QCoreApplication::instance()->thread() != QThread::currentThread(), Q_FUNC_INFO,
                   "Needs to run in own thread");
        Q_ASSERT_X(this->thread() == QThread::currentThread(), Q_FUNC_INFO, "Wrong object thread");
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
            CLogMessage(this).warning(u"Detected outdated reader pending calls: '%1'")
                << outdatedPendingUrls.toQString(true);
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
        return QPair<qint64, qint64>(m_networkReplyCurrent, m_networkReplyMax);
    }

    void CThreadedReader::networkReplyProgress(int logId, qint64 current, qint64 max, const QUrl &url)
    {
        // max can be -1 if file size is not available
        m_networkReplyProgress = (current > 0 && max > 0) ? static_cast<int>((current * 100) / max) : -1;
        m_networkReplyCurrent = current;
        m_networkReplyMax = max;

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
            SWIFT_AUDIT_X(false, funcInfo, m.constData());
        }
        CLogMessage::preformatted(logMsg);
    }
} // namespace swift::core
