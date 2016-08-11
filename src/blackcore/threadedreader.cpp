/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/network/networkutils.h"
#include "blackcore/threadedreader.h"
#include "blackmisc/threadutils.h"

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
    const CLogCategoryList &CThreadedReader::getLogCategories()
    {
        static const BlackMisc::CLogCategoryList cats { BlackMisc::CLogCategory::worker() };
        return cats;
    }

    CThreadedReader::CThreadedReader(QObject *owner, const QString &name) :
        CContinuousWorker(owner, name),
        m_updateTimer(new QTimer(this))
    {
        connect(m_updateTimer, &QTimer::timeout, this, &CThreadedReader::doWork);
        m_updateTimer->setSingleShot(true);
    }

    CThreadedReader::~CThreadedReader()
    {
        gracefulShutdown();
    }

    qint64 CThreadedReader::lastModifiedMsSinceEpoch(QNetworkReply *nwReply) const
    {
        return CNetworkUtils::lastModifiedMsSinceEpoch(nwReply);
    }

    QDateTime CThreadedReader::getUpdateTimestamp() const
    {
        QReadLocker lock(&this->m_lock);
        return this->m_updateTimestamp;
    }

    void CThreadedReader::setUpdateTimestamp(const QDateTime &updateTimestamp)
    {
        QWriteLocker lock(&this->m_lock);
        this->m_updateTimestamp = updateTimestamp;
    }

    bool CThreadedReader::updatedWithinLastMs(qint64 timeLastMs)
    {
        QDateTime dt(getUpdateTimestamp());
        if (dt.isNull() || !dt.isValid()) { return false; }
        qint64 delta = QDateTime::currentMSecsSinceEpoch() - dt.toMSecsSinceEpoch();
        return delta <= timeLastMs;
    }

    bool CThreadedReader::isNetworkAvailable() const
    {
        static const bool nw = CNetworkUtils::hasConnectedInterface();
        return nw;
    }

    void CThreadedReader::gracefulShutdown()
    {
        // if not in main thread stop, otherwise it makes no sense to abandon
        if (!CThreadUtils::isCurrentThreadObjectThread(this))
        {
            this->abandonAndWait();
        }
    }

    void CThreadedReader::startReader()
    {
        Q_ASSERT(m_initialTime > 0);
        QTimer::singleShot(m_initialTime, this, [=] { this->doWork(); });
    }

    void CThreadedReader::pauseReader()
    {
        QTimer::singleShot(0, m_updateTimer, &QTimer::stop);
    }

    bool CThreadedReader::didContentChange(const QString &content, int startPosition)
    {
        uint oldHash = 0;
        {
            QReadLocker rl(&this->m_lock);
            oldHash = this->m_contentHash;
        }
        uint newHash = qHash(startPosition < 0 ? content : content.mid(startPosition));
        if (oldHash == newHash) { return false; }
        {
            QWriteLocker wl(&this->m_lock);
            this->m_contentHash = newHash;
        }
        return true;
    }

    bool CThreadedReader::isMarkedAsFailed() const
    {
        return this->m_markedAsFailed;
    }

    void CThreadedReader::setMarkedAsFailed(bool failed)
    {
        this->m_markedAsFailed = failed;
    }

    void CThreadedReader::threadAssertCheck() const
    {
        Q_ASSERT_X(QCoreApplication::instance()->thread() != QThread::currentThread(), Q_FUNC_INFO, "Needs to run in own thread");
        Q_ASSERT_X(QObject::thread() == QThread::currentThread(), Q_FUNC_INFO, "Wrong object thread");
    }

    void CThreadedReader::setInitialAndPeriodicTime(int initialTime, int periodicTime)
    {
        m_initialTime = initialTime;
        m_periodicTime = periodicTime;
        if (m_updateTimer->isActive())
        {
            int oldPeriodicTime = m_updateTimer->interval();
            int delta = m_periodicTime - oldPeriodicTime + m_updateTimer->remainingTime();
            m_updateTimer->start(qMax(delta, 0));
        }
    }

    void CThreadedReader::doWork()
    {
        if (isFinished()) { return; }
        doWorkImpl();
        Q_ASSERT(m_periodicTime > 0);
        m_updateTimer->start(m_periodicTime);
    }

} // namespace
