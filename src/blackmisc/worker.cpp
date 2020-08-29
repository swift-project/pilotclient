/* Copyright (C) 2014
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/worker.h"
#include "blackmisc/threadutils.h"
#include "blackmisc/verify.h"
#include "blackmisc/logmessage.h"

#include <future>
#include <QTimer>
#include <QPointer>
#include <QCoreApplication>

#ifdef Q_OS_WIN32
#include <Windows.h>
#endif

namespace BlackMisc
{
    QSet<CWorkerBase *> CWorkerBase::s_allWorkers;

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
        const QString name = this->objectName();

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
                case WAIT_FAILED:   qWarning() << "Thread" << name << "unspecified error"; break;
                case WAIT_OBJECT_0: qWarning() << "Thread" << name << "unsafely terminated by program shutdown"; break;
                case WAIT_TIMEOUT: break;
                }
            }
        }
#endif
        quit();

        // the wait avoids: QThread: Destroyed while thread is still running
        const unsigned long timeoutMs = 5 * 1000;
        const bool ok = wait(timeoutMs); //! \todo KB 2017-10 temp workaround: in T145 this will be fixed, sometimes (very rarely) hanging here during shutdown
        const QString as = QStringLiteral("Wait timeout after %1ms for '%2'").arg(timeoutMs).arg(name);
        const QByteArray asBA = as.toLatin1();
        BLACK_AUDIT_X(ok, Q_FUNC_INFO, asBA); // MS 2018-09 assert because we want a stack trace of all threads, via breakpad
        Q_UNUSED(ok)
    }

    CWorker *CWorker::fromTaskImpl(QObject *owner, const QString &name, int typeId, const std::function<QVariant()> &task)
    {
        auto *worker = new CWorker(task);
        emit worker->aboutToStart();
        worker->setStarted();
        auto *thread = new CRegularThread(owner);

        if (typeId != QMetaType::Void) { worker->m_result = QVariant(typeId, nullptr); }

        const QString ownerName = owner->objectName().isEmpty() ? owner->metaObject()->className() : owner->objectName();
        thread->setObjectName(ownerName + ":" + name);
        worker->setObjectName(name);

        worker->moveToThread(thread);
        const bool s = QMetaObject::invokeMethod(worker, &CWorker::ps_runTask);
        Q_ASSERT_X(s, Q_FUNC_INFO, "cannot invoke");
        Q_UNUSED(s)
        thread->start();
        return worker;
    }

    void CWorker::ps_runTask()
    {
        m_result = m_task();

        this->setFinished();

        QThread *workerThread = this->thread();
        Q_ASSERT_X(workerThread->thread()->isRunning(), Q_FUNC_INFO, "Owner thread's event loop already ended");

        // MS 2018-09 Now we post the DeferredDelete event from within the worker thread, but rely on it being dispatched
        //            by the owner thread. Posted events are moved along with the object when moveToThread is called.
        //            We also connect its destroyed signal to delete the worker thread at the same time.
        this->deleteLater();
        connect(this, &QObject::destroyed, workerThread, &QObject::deleteLater);

        this->moveToThread(workerThread->thread()); // move worker back to the thread which constructed it, so there is no race on deletion
        // must not access the worker beyond this point, as it now lives in the owner's thread and could be deleted at any moment
    }

    CWorkerBase::CWorkerBase()
    {
        s_allWorkers.insert(this);
    }

    CWorkerBase::~CWorkerBase()
    {
        s_allWorkers.remove(this);
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
        if (thread() != thread()->thread()) { thread()->requestInterruption(); }
        quit();
    }

    void CWorkerBase::abandonAndWait() noexcept
    {
        if (thread() != thread()->thread()) { thread()->requestInterruption(); }
        quitAndWait();
    }

    bool CWorkerBase::isAbandoned() const
    {
        Q_ASSERT(thread() == QThread::currentThread());
        return thread()->isInterruptionRequested();
    }

    CContinuousWorker::CContinuousWorker(QObject *owner, const QString &name) :
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
        Q_ASSERT_X(CThreadUtils::isInThisThread(m_owner), Q_FUNC_INFO, "Needs to be started in owner thread");
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
        connect(thread, &QThread::started,  this, &CContinuousWorker::initialize);
        connect(thread, &QThread::finished, &m_updateTimer, &QTimer::stop);
        connect(thread, &QThread::finished, this, &CContinuousWorker::cleanup);
        connect(thread, &QThread::finished, this, &CContinuousWorker::finish);
        thread->start(priority);
    }

    void CContinuousWorker::quit() noexcept
    {
        this->setEnabled(false);

        // already in owner's thread? then return
        if (this->thread() == m_owner->thread()) { return; }

        // remark: cannot stop timer here, as I am normally not in the correct thread
        this->beforeQuit();
        thread()->quit();
    }

    void CContinuousWorker::quitAndWait() noexcept
    {
        this->setEnabled(false);

        // already in owner's thread? then return
        if (this->thread() == m_owner->thread()) { return; }

        // called by own thread, will deadlock, return
        if (CThreadUtils::isInThisThread(this)) { return; }

        QThread *workerThread = thread(); // must be before quit()
        this->quit();

        // T647, discussed here: https://discordapp.com/channels/539048679160676382/539925070550794240/573260844004016148
        const unsigned long waitTimeoutMs = this->waitTimeoutMs();
        const QString name(this->getName());
        qint64 waitTime = QDateTime::currentMSecsSinceEpoch();
        const bool ok = workerThread->wait(waitTimeoutMs); //! \todo KB 2017-10 temp workaround: in T145 this will be fixed, sometimes (very rarely) hanging here during shutdown
        waitTime = QDateTime::currentMSecsSinceEpoch() - waitTime;
        const QString msg = QStringLiteral("Waiting for quitAndWait of '%1' for %2ms").arg(name).arg(waitTime);
        const QByteArray msgBA = msg.toLatin1();
        BLACK_AUDIT_X(ok, Q_FUNC_INFO, msgBA); // MS 2019-05 AUDIT because we want a stack trace of all threads, via breakpad
        CLogMessage(this).info(msg);
        Q_UNUSED(ok)
    }

    void CContinuousWorker::startUpdating(int updateTimeSecs)
    {
        Q_ASSERT_X(this->hasStarted(), Q_FUNC_INFO, "Worker not yet started");
        if (!CThreadUtils::isInThisThread(this))
        {
            // shift in correct thread
            QPointer<CContinuousWorker> myself(this);
            QTimer::singleShot(0, this, [ = ]
            {
                if (!myself) { return; }
                this->doIfNotFinished([ = ] { startUpdating(updateTimeSecs); });
            });
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

    void CContinuousWorker::stopUpdateTimer()
    {
        if (!m_updateTimer.isActive()) { return; }

        // avoid "Timers cannot be stopped from another thread"
        if (CThreadUtils::isInThisThread(&m_updateTimer))
        {
            m_updateTimer.stop();
        }
        else
        {
            QPointer<CContinuousWorker> myself(this);
            QTimer::singleShot(0, &m_updateTimer, [ = ]
            {
                // stop timer in timer thread
                if (!myself) { return; }
                m_updateTimer.stop();
            });
        }
    }

    void CContinuousWorker::finish()
    {
        this->setFinished();

        Q_ASSERT_X(m_owner->thread()->isRunning(), Q_FUNC_INFO, "Owner thread's event loop already ended");

        // MS 2018-09 Now we post the DeferredDelete event from within the worker thread, but rely on it being dispatched
        //            by the owner thread. Posted events are moved along with the object when moveToThread is called.
        //            We also connect its destroyed signal to delete the worker thread at the same time.
        this->deleteLater();
        connect(this, &QObject::destroyed, this->thread(), &QObject::deleteLater);

        this->moveToThread(m_owner->thread()); // move worker back to the thread which constructed it, so there is no race on deletion
        // must not access the worker beyond this point, as it now lives in the owner's thread and could be deleted at any moment
    }
} // ns
