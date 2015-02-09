/* Copyright (C) 2014
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "worker.h"

namespace BlackMisc
{

    CWorker *CWorker::fromTask(QObject *owner, const QString &name, std::function<void()> task)
    {
        auto *thread = new CRegularThread(owner);
        auto *worker = new CWorker(task);

        QString ownerName = owner->objectName().isEmpty() ? owner->metaObject()->className() : owner->objectName();
        thread->setObjectName(ownerName + ":" + name);
        worker->setObjectName(name);

        worker->moveToThread(thread);
        QMetaObject::invokeMethod(worker, "ps_runTask");
        thread->start();

        return worker;
    }

    void CWorker::ps_runTask()
    {
        m_task();

        setFinished();

        auto *ownThread = thread();
        moveToThread(ownThread->thread()); // move worker back to the thread which constructed it, so there is no race on deletion
        QMetaObject::invokeMethod(ownThread, "deleteLater");
        QMetaObject::invokeMethod(this, "deleteLater");
    }

    void CContinuousWorker::start(QThread::Priority priority)
    {
        if (m_name.isEmpty()) { m_name = metaObject()->className(); }

        auto *thread = new CRegularThread(m_owner);

        if (m_owner)
        {
            QString ownerName = m_owner->objectName().isEmpty() ? m_owner->metaObject()->className() : m_owner->objectName();
            thread->setObjectName(ownerName + ":" + m_name);
        }
        setObjectName(m_name);

        moveToThread(thread);
        connect(thread, &QThread::started, this, &CContinuousWorker::initialize);
        connect(thread, &QThread::finished, this, &CContinuousWorker::cleanup);
        connect(thread, &QThread::finished, this, &CContinuousWorker::ps_finish);
        thread->start(priority);
    }

    void CContinuousWorker::quit()
    {
        thread()->quit();
    }

    void CContinuousWorker::quitAndWait()
    {
        auto *ownThread = thread();
        quit();
        ownThread->wait();
    }

    void CContinuousWorker::ps_finish()
    {
        setFinished();

        auto *ownThread = thread();
        moveToThread(ownThread->thread()); // move worker back to the thread which constructed it, so there is no race on deletion
        QMetaObject::invokeMethod(ownThread, "deleteLater");
        QMetaObject::invokeMethod(this, "deleteLater");
    }

}
