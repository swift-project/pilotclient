/* Copyright (C) 2014
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/threadutils.h"
#include "blackmisc/worker.h"
#include "blackmisc/verify.h"

#include <future>
#include <QTimer>
#include <QPointer>
#include <QCoreApplication>

#ifdef Q_OS_WIN32
#include <Windows.h>
#endif

namespace BlackMisc
{
    void CRegularThread::run()
    {
#ifdef Q_OS_WIN32
        m_handle = GetCurrentThread();
        QThread::run();
        m_handle = nullptr;
#else
        QThread::run();
#endif
    }

    CRegularThread::~CRegularThread()
    {
#ifdef Q_OS_WIN32
        auto handle = m_handle.load();
        if (handle)
        {
            const auto status = WaitForSingleObject(handle, 0);
            if (isRunning())
            {
                switch (status)
                {
                default:
                case WAIT_FAILED: qWarning() << "Thread" << objectName() << "unspecified error"; break;
                case WAIT_OBJECT_0: qWarning() << "Thread" << objectName() << "unsafely terminated by program shutdown"; break;
                case WAIT_TIMEOUT: break;
                }
            }
        }
#endif
        quit();

        bool ok = wait(30 * 1000); //! \todo KB 2017-10 temp workaround: in T145 this will be fixed, sometimes (very rarely) hanging here during shutdown
        Q_ASSERT_X(ok, Q_FUNC_INFO, "Wait timeout"); // MS 2018-09 assert because we want a stack trace of all threads, via breakpad
        Q_UNUSED(ok);
    }

    CWorker *CWorker::fromTaskImpl(QObject *owner, const QString &name, int typeId, std::function<CVariant()> task)
    {
        auto *worker = new CWorker(task);
        emit worker->aboutToStart();
        worker->setStarted();
        auto *thread = new CRegularThread(owner);

        if (typeId != QMetaType::Void) { worker->m_result = CVariant(typeId, nullptr); }

        const QString ownerName = owner->objectName().isEmpty() ? owner->metaObject()->className() : owner->objectName();
        thread->setObjectName(ownerName + ":" + name);
        worker->setObjectName(name);

        worker->moveToThread(thread);
        const bool s = QMetaObject::invokeMethod(worker, "ps_runTask");
        Q_ASSERT_X(s, Q_FUNC_INFO, "cannot invoke");
        Q_UNUSED(s);
        thread->start();
        return worker;
    }

    void CWorker::ps_runTask()
    {
        m_result = m_task();

        this->setFinished();

        QThread *ownThread = this->thread();
        Q_ASSERT_X(ownThread->thread()->isRunning(), Q_FUNC_INFO, "Owner thread's event loop already ended");

        this->moveToThread(ownThread->thread()); // move worker back to the thread which constructed it, so there is no race on deletion
        Q_ASSERT_X(this->thread() == ownThread->thread(), Q_FUNC_INFO, "moveToThread failed");

        //! \todo KB 2018-97 new syntax not yet supported on Jenkins QMetaObject::invokeMethod(ownThread, &CWorker::deleteLater)
        QMetaObject::invokeMethod(ownThread, "deleteLater");
        QMetaObject::invokeMethod(this, "deleteLater");
    }

    const CLogCategoryList &CWorkerBase::getLogCategories()
    {
        static const BlackMisc::CLogCategoryList cats { CLogCategory::worker() };
        return cats;
    }

    void CWorkerBase::waitForFinished() noexcept
    {
        std::promise<void> promise;
        then([ & ] { promise.set_value(); });
        promise.get_future().wait();
    }

    void CWorkerBase::abandon() noexcept
    {
        thread()->requestInterruption();
        quit();
    }

    void CWorkerBase::abandonAndWait() noexcept
    {
        thread()->requestInterruption();
        quitAndWait();
    }

    bool CWorkerBase::isAbandoned() const
    {
        return thread()->isInterruptionRequested();
    }

    CContinuousWorker::CContinuousWorker(QObject *owner, const QString &name) :
        CIdentifiable(name),
        m_owner(owner), m_name(name)
    {
        Q_ASSERT_X(!name.isEmpty(), Q_FUNC_INFO, "Empty name");
        this->setObjectName(m_name);
        m_updateTimer.setObjectName(m_name + ":timer");
    }

    void CContinuousWorker::start(QThread::Priority priority)
    {
        BLACK_VERIFY_X(!hasStarted(), Q_FUNC_INFO, "Tried to start a worker that was already started");
        if (hasStarted()) { return; }

        // avoid message "QObject: Cannot create children for a parent that is in a different thread"
        Q_ASSERT_X(CThreadUtils::isCurrentThreadObjectThread(m_owner), Q_FUNC_INFO, "Needs to be started in owner thread");
        emit this->aboutToStart();
        setStarted();
        auto *thread = new CRegularThread(m_owner);

        Q_ASSERT(m_owner); // must not be null, see (9) https://dev.vatsim-germany.org/issues/402
        if (m_owner)
        {
            const QString ownerName = m_owner->objectName().isEmpty() ? m_owner->metaObject()->className() : m_owner->objectName();
            thread->setObjectName(ownerName + ": " + m_name);
        }

        moveToThread(thread);
        connect(thread, &QThread::started, this, &CContinuousWorker::initialize);
        connect(thread, &QThread::finished, &m_updateTimer, &QTimer::stop);
        connect(thread, &QThread::finished, this, &CContinuousWorker::cleanup);
        connect(thread, &QThread::finished, this, &CContinuousWorker::finish);
        thread->start(priority);
    }

    void CContinuousWorker::quit() noexcept
    {
        this->setEnabled(false);

        // already in different thread? otherwise return
        if (CThreadUtils::isApplicationThreadObjectThread(this)) { return; }

        // remark: cannot stop timer here, as I am normally not in the correct thread
        thread()->quit();
    }

    void CContinuousWorker::quitAndWait() noexcept
    {
        this->setEnabled(false);

        // already in application (main) thread? => return
        if (CThreadUtils::isApplicationThreadObjectThread(this)) { return; }

        // called by own thread, will deadlock, return
        if (CThreadUtils::isCurrentThreadObjectThread(this)) { return; }

        QThread *ownThread = thread(); // must be before quit()
        this->quit();

        bool ok = ownThread->wait(30 * 1000); //! \todo KB 2017-10 temp workaround: in T145 this will be fixed, sometimes (very rarely) hanging here during shutdown
        Q_ASSERT_X(ok, Q_FUNC_INFO, "Wait timeout"); // MS 2018-09 assert because we want a stack trace of all threads, via breakpad
        Q_UNUSED(ok);
    }

    void CContinuousWorker::startUpdating(int updateTimeSecs)
    {
        Q_ASSERT_X(hasStarted(), Q_FUNC_INFO, "Worker not yet started");
        if (!CThreadUtils::isCurrentThreadObjectThread(this))
        {
            // shift in correct thread
            if (!this->isFinished())
            {
                QPointer<CContinuousWorker> myself(this);
                QTimer::singleShot(0, this, [ = ]
                {
                    if (!myself) { return; }
                    if (this->isFinished()) { return; }
                    this->startUpdating(updateTimeSecs);
                });
            }
            return;
        }

        // here in correct timer thread
        if (updateTimeSecs < 0)
        {
            this->setEnabled(false);
            m_updateTimer.stop();
        }
        else
        {
            this->setEnabled(true);
            m_updateTimer.start(1000 * updateTimeSecs);
        }
    }

    void CContinuousWorker::finish()
    {
        this->setFinished();

        Q_ASSERT_X(m_owner->thread()->isRunning(), Q_FUNC_INFO, "Owner thread's event loop already ended");

        QThread *ownThread = this->thread();
        this->moveToThread(m_owner->thread()); // move worker back to the thread which constructed it, so there is no race on deletion
        Q_ASSERT_X(this->thread() == m_owner->thread(), Q_FUNC_INFO, "moveToThread failed");

        //! \todo new syntax not yet supported on Jenkins QMetaObject::invokeMethod(ownThread, &CWorker::deleteLater)
        QMetaObject::invokeMethod(ownThread, "deleteLater");
        QMetaObject::invokeMethod(this, "deleteLater");
    }
} // ns
