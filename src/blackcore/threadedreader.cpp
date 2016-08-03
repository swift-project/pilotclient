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
        m_toggleConnection = connect(this->m_updateTimer, &QTimer::timeout, this, &CThreadedReader::ps_toggleInterval);
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

    void CThreadedReader::requestReload()
    {
        // default implementation, subclasses shall override as required
        this->initialize();
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

    void CThreadedReader::setInterval(int updatePeriodMs)
    {
        Q_ASSERT(this->m_updateTimer);
        QTimer::singleShot(0, this, [this, updatePeriodMs]
        {
            QWriteLocker wl(&this->m_lock);
            if (updatePeriodMs < 1)
            {
                this->m_updateTimer->stop();
            }
            else {
                this->m_updateTimer->start(updatePeriodMs);
            }
        });
    }

    void CThreadedReader::restartTimer(bool onlyWhenActive)
    {
        const int intervalMs(this->interval());
        if (!onlyWhenActive || this->isTimerActive())
        {
            this->setInterval(intervalMs);
        }
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

    void CThreadedReader::ps_toggleInterval()
    {
        disconnect(this->m_toggleConnection);
        this->setPeriodicTime();
    }

    int CThreadedReader::interval() const
    {
        QReadLocker rl(&this->m_lock);
        return this->m_updateTimer->interval();
    }

    bool CThreadedReader::isTimerActive() const
    {
        QReadLocker rl(&this->m_lock);
        return this->m_updateTimer->isActive();
    }

    bool CThreadedReader::isMarkedAsFailed() const
    {
        return this->m_markedAsFailed;
    }

    void CThreadedReader::setMarkedAsFailed(bool failed)
    {
        this->m_markedAsFailed = failed;
    }

    void CThreadedReader::setIntervalFromSettingsAndStart()
    {
        this->setInitialTime();
    }

    void CThreadedReader::threadAssertCheck() const
    {
        Q_ASSERT_X(QCoreApplication::instance()->thread() != QThread::currentThread(), Q_FUNC_INFO, "Needs to run in own thread");
        Q_ASSERT_X(QObject::thread() == QThread::currentThread(), Q_FUNC_INFO, "Wrong object thread");
    }

    CReaderSettings CThreadedReader::getSettings() const
    {
        return CReaderSettings::neverUpdateSettings();
    }

    void CThreadedReader::setInitialTime()
    {
        const CReaderSettings s = this->getSettings();
        const int ms = s.getInitialTime().toMs();
        this->setInterval(s.isNeverUpdate() ? -1 : ms);
    }

    void CThreadedReader::setPeriodicTime()
    {
        const CReaderSettings s = this->getSettings();
        const int ms = s.getPeriodicTime().toMs();
        this->setInterval(s.isNeverUpdate() ? -1 : ms);
    }
} // namespace
