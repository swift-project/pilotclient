/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "threadedreader.h"

namespace BlackMisc
{
    CThreadedReader::CThreadedReader(QObject *owner, const QString &name) :
        CContinuousWorker(owner, name),
        m_updateTimer(new QTimer(this))
    {  }

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

    void CThreadedReader::requestStop()
    {
        setFinished();
        QMetaObject::invokeMethod(m_updateTimer, "stop");
    }

    void CThreadedReader::gracefulShutdown()
    {
        this->m_shutdown = true;
        this->requestStop();
    }

    CThreadedReader::~CThreadedReader()
    {
        cleanup();
    }

    void CThreadedReader::cleanup()
    {
        // cleanup code would go here
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
