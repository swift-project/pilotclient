/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/network/networkutils.h"
#include "blackmisc/threadedreader.h"
#include "blackmisc/threadutils.h"

#include <QCoreApplication>
#include <QMetaObject>
#include <QReadLocker>
#include <QThread>
#include <QTimer>
#include <QWriteLocker>

using namespace BlackMisc;
using namespace BlackMisc::Network;

namespace BlackMisc
{
    CThreadedReader::CThreadedReader(QObject *owner, const QString &name) :
        CContinuousWorker(owner, name),
        m_updateTimer(new QTimer(this))
    {
        // void
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

    void CThreadedReader::gracefulShutdown()
    {
        // if not in main thread stop, otherwise it makes no sense to abandon
        if (!CThreadUtils::isCurrentThreadObjectThread(this))
        {
            this->abandonAndWait();
        }
    }

    CThreadedReader::~CThreadedReader()
    {
        gracefulShutdown();
    }

    void CThreadedReader::setInterval(int updatePeriodMs)
    {
        Q_ASSERT(this->m_updateTimer);
        bool s;
        if (updatePeriodMs < 1)
        {
            s = QMetaObject::invokeMethod(m_updateTimer, "stop");
        }
        else
        {
            s = QMetaObject::invokeMethod(m_updateTimer, "start", Q_ARG(int, updatePeriodMs));
        }
        Q_ASSERT_X(s, Q_FUNC_INFO, "Failed invoke");
        Q_UNUSED(s);
    }

    int CThreadedReader::interval() const
    {
        QReadLocker rl(&this->m_lock);
        return this->m_updateTimer->interval();
    }

    void CThreadedReader::threadAssertCheck() const
    {
        Q_ASSERT_X(QCoreApplication::instance()->thread() != QThread::currentThread(), Q_FUNC_INFO, "Needs to run in own thread");
        Q_ASSERT_X(QObject::thread() == QThread::currentThread(), Q_FUNC_INFO, "Wrong object thread");
    }
} // namespace
