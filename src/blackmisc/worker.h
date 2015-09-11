/* Copyright (C) 2014
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_WORKER_H
#define BLACKMISC_WORKER_H

#include "blackmiscexport.h"
#include "variant.h"
#include <QThread>
#include <QMutex>
#include <QTimer>
#include <QSharedPointer>
#include <QWeakPointer>
#include <functional>
#include <atomic>

namespace BlackMisc
{

    //! \private Class for synchronizing singleShot() task with its owner.
    class BLACKMISC_EXPORT CSingleShotController : public QObject
    {
        Q_OBJECT
    public:
        CSingleShotController(QObject *parent) : QObject(parent), m_strongRef(QSharedPointer<int>::create(0)) {}
        ~CSingleShotController() Q_DECL_NOEXCEPT { auto wr = weakRef(); m_strongRef.clear(); waitForNull(wr); }
        QWeakPointer<int> weakRef() const { return m_strongRef.toWeakRef(); }
    private:
        static void waitForNull(QWeakPointer<int> wp) { while (wp) { QThread::msleep(10); } }
        QSharedPointer<int> m_strongRef; // pointee type doesn't matter, we only care about the reference count
    };

    /*!
     * Starts a single-shot timer which will run in an existing thread and call a task when it times out.
     *
     * Useful when a worker thread wants to push small sub-tasks back to the thread which spawned it.
     *
     * If an owner pointer is specified, then the task may be cancelled if the owner is deleted, but the
     * owner will not be deleted while the task is running (its destructor will wait for the task to end).
     */
    //! @{
    template <typename F>
    void singleShot(int msec, QThread *target, F task)
    {
        auto *timer = new QTimer;
        timer->setSingleShot(true);
        timer->moveToThread(target);
        QObject::connect(timer, &QTimer::timeout, [ = ]()
        {
            task();
            timer->deleteLater();
        });
        QMetaObject::invokeMethod(timer, "start", Q_ARG(int, msec));
    }
    template <typename F>
    void singleShot(int msec, QThread *target, QObject *owner, F task)
    {
        Q_ASSERT(QThread::currentThread() == owner->thread());
        auto weakRef = (new CSingleShotController(owner))->weakRef();
        singleShot(msec, target, [ = ]()
        {
            auto strongRef = weakRef.toStrongRef();
            if (strongRef) { task(); }
        });
    }
    //! @}

    /*!
     * Just a subclass of QThread whose destructor waits for the thread to finish.
     */
    class BLACKMISC_EXPORT CRegularThread : public QThread
    {
    public:
        //! Constructor
        CRegularThread(QObject *parent = nullptr) : QThread(parent) {}

        //! Destructor
        ~CRegularThread() Q_DECL_NOEXCEPT
        {
            quit();
            wait();
        }
    };

    /*!
     * Base class for CWorker and CContinuousWorker.
     */
    class BLACKMISC_EXPORT CWorkerBase : public QObject
    {
        Q_OBJECT

    public:
        //! Connects to a functor or method which will be called when the task is finished.
        //! \threadsafe
        template <typename T, typename F>
        void then(T *context, F functor)
        {
            Q_ASSERT(context->thread() == QThread::currentThread());
            QMutexLocker lock(&m_finishedMutex);
            connect(this, &CWorkerBase::finished, context, functor);
            if (m_finished) { invoke(context, functor); }
        }

        //! Connects to a functor which will be called when the task is finished.
        //! \threadsafe
        template <typename F>
        void then(F functor)
        {
            QMutexLocker lock(&m_finishedMutex);
            connect(this, &CWorkerBase::finished, functor);
            if (m_finished) { functor(); }
        }

        //! Returns true if the task has finished.
        //! \threadsafe But don't rely on this condition remaining true for any length of time.
        bool isFinished() const
        {
            QMutexLocker lock(&m_finishedMutex);
            return m_finished;
        }

        //! Executes some code (in the caller's thread) if the task has finished.
        //! \threadsafe
        template <typename F>
        void doIfFinished(F functor) const
        {
            QMutexLocker lock(&m_finishedMutex);
            if (m_finished) { functor(); }
        }

        //! Executes some code (in the caller's thread) if the task has not finished.
        //! \threadsafe
        template <typename F>
        void doIfNotFinished(F functor) const
        {
            QMutexLocker lock(&m_finishedMutex);
            if (! m_finished) { functor(); }
        }

        //! Executes some code (in the caller's thread) if the task has not finished and some different code if it has finished.
        //! \threadsafe
        template <typename F1, typename F2>
        void doIfFinishedElse(F1 ifFunctor, F2 elseFunctor) const
        {
            QMutexLocker lock(&m_finishedMutex);
            if (m_finished) { ifFunctor(); } else { elseFunctor(); }
        }

        //! Blocks until the task is finished.
        //! \threadsafe
        void waitForFinished() Q_DECL_NOEXCEPT;

    signals:
        //! Emitted when the task is finished.
        void finished();

    protected:
        //! Mark the task as finished.
        void setFinished()
        {
            QMutexLocker lock(&m_finishedMutex);
            m_finished = true;
            emit finished();
        }

        //! Uniform way to invoke either a functor or a method.
        template <typename T, typename F, typename... Ts>
        static auto invoke(T *object, F func, Ts &&... args) -> typename std::enable_if<std::is_member_function_pointer<F>::value>::type
        {
            return (object->*func)(std::forward<Ts>(args)...);
        }

        //! Uniform way to invoke either a functor or a method.
        template <typename T, typename F, typename... Ts>
        static auto invoke(T *, F func, Ts &&... args) -> typename std::enable_if<! std::is_member_function_pointer<F>::value>::type
        {
            return func(std::forward<Ts>(args)...);
        }

    private:
        bool m_finished = false;
        mutable QMutex m_finishedMutex { QMutex::Recursive };
    };

    /*!
     * Class for doing some arbitrary parcel of work in its own thread.
     *
     * The task is exposed as a function object, so could be a lambda or a hand-written closure.
     * CWorker can not be subclassed, instead it can be extended with rich callable task objects.
     */
    class BLACKMISC_EXPORT CWorker final : public CWorkerBase
    {
        Q_OBJECT

    public:
        /*!
         * Returns a new worker object which lives in a new thread.
         * \note The worker calls its own deleteLater method when finished.
         *       Typically assign it to a QPointer if you want to store it.
         * \param owner Will be the parent of the new thread (the worker has no parent).
         * \param name A name for the task, which will be used to create a name for the thread.
         * \param task A function object which will be run by the worker in its thread.
         * \todo An MSVC bug prevents perfect-forwarding of task.
         */
        template <typename F>
        static CWorker *fromTask(QObject *owner, const QString &name, F task)
        {
            return fromTaskImpl(owner, name, [task]() { return CVariant::fromResultOf(task); });
        }

        //! Connects to a functor to which will be passed the result when the task is finished.
        //! \tparam R The return type of the task.
        //! \threadsafe
        template <typename R, typename F>
        void thenWithResult(F functor) { then([this, functor]() { functor(this->result<R>()); }); }

        //! Connects to a functor or method to which will be passed the result when the task is finished.
        //! \tparam R The return type of the task.
        //! \threadsafe
        template <typename R, typename T, typename F>
        void thenWithResult(T *context, F functor) { then(context, [this, context, functor]() { invoke(context, functor, this->result<R>()); }); }

        //! Returns the result of the task, waiting for it to finish if necessary.
        //! \tparam R The return type of the task.
        //! \threadsafe
        template <typename R>
        R result() { waitForFinished(); Q_ASSERT(m_result.canConvert<R>()); return m_result.value<R>(); }

    private slots:
        //! Called when the worker has been moved into its new thread.
        void ps_runTask();

    private:
        CWorker(std::function<CVariant()> task) : m_task(task) {}
        static CWorker *fromTaskImpl(QObject *owner, const QString &name, std::function<CVariant()> task);

        std::function<CVariant()> m_task;
        CVariant m_result;
    };

    /*!
     * Base class for a long-lived worker object which lives in its own thread.
     */
    class BLACKMISC_EXPORT CContinuousWorker : public CWorkerBase
    {
        Q_OBJECT

    public:
        /*!
         * Constructor.
         * \param owner Will be the parent of the new thread (the worker has no parent).
         * \param name A name for the worker, which will be used to create a name for the thread.
         */
        CContinuousWorker(QObject *owner, const QString &name = "") : m_owner(owner), m_name(name) {}

        //! Starts a thread and moves the worker into it.
        void start(QThread::Priority priority = QThread::InheritPriority);

        //! Stops the thread the next time around its event loop.
        //! The thread and the worker will then be deleted.
        //! \threadsafe
        void quit() Q_DECL_NOEXCEPT;

        //! Calls quit() and blocks until the thread is finished.
        //! \threadsafe Will deadlock if called by the worker thread.
        void quitAndWait() Q_DECL_NOEXCEPT;

    protected slots:
        //! Called when the thread is started.
        virtual void initialize() {}

        //! Called when the thread is finished.
        virtual void cleanup() {}

    private slots:
        //! Called after cleanup().
        void ps_finish();

    private:
        QObject *m_owner = nullptr;
        QString m_name;
    };

}

#endif
