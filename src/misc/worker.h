// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_WORKER_H
#define SWIFT_MISC_WORKER_H

#include <algorithm>
#include <atomic>
#include <functional>
#include <future>
#include <memory>
#include <type_traits>

#include <QMetaObject>
#include <QMetaType>
#include <QMutex>
#include <QMutexLocker>
#include <QObject>
#include <QPointer>
#include <QString>
#include <QThread>
#include <QTimer>
#include <QVariant>
#include <QtGlobal>

#include "misc/invoke.h"
#include "misc/promise.h"
#include "misc/swiftmiscexport.h"

namespace swift::misc
{
    /*!
     * Just a subclass of QThread whose destructor waits for the thread to finish.
     */
    class SWIFT_MISC_EXPORT CRegularThread final : public QThread
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CRegularThread(QObject *parent = nullptr) : QThread(parent) {}

        //! Destructor
        ~CRegularThread() override;

        //! Copy constructor
        CRegularThread(const CRegularThread &) = delete;

        //! Copy assignment
        CRegularThread &operator=(const CRegularThread &) = delete;

        //! Move constructor
        CRegularThread(CRegularThread &&) = delete;

        //! Move assignment
        CRegularThread &operator=(CRegularThread &&) = delete;

    protected:
        //! \copydoc QThread::run
        void run() override;

    private:
        std::atomic<void *> m_handle { nullptr };
    };

    /*!
     * Base class for CWorker and CContinuousWorker.
     */
    class SWIFT_MISC_EXPORT CWorkerBase : public QObject
    {
        Q_OBJECT

    public:
        //! Destructor.
        ~CWorkerBase() override;

        //! Copy constructor
        CWorkerBase(const CWorkerBase &) = delete;

        //! Copy assignment
        CWorkerBase &operator=(const CWorkerBase &) = delete;

        //! Move constructor
        CWorkerBase(CWorkerBase &&) = delete;

        //! Move assignment
        CWorkerBase &operator=(CWorkerBase &&) = delete;

        //! Log categories
        static const QStringList &getLogCategories();

        //! Connects to a functor or method which will be called when the task is finished.
        //! \threadsafe The functor may not call any method that observes the worker's finished flag.
        template <typename T, typename F>
        void then(T *context, F functor)
        {
            Q_ASSERT(context->thread() == QThread::currentThread());
            QMutexLocker lock(&m_finishedMutex);
            connect(this, &CWorkerBase::finished, context, functor);
            if (m_finished) { private_ns::invokeSlot(functor, context); }
        }

        //! Connects to a functor which will be called when the task is finished.
        //! \threadsafe The functor may not call any method that observes the worker's finished flag.
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
        //! \threadsafe The functor may not call any method that observes the worker's finished flag.
        template <typename F>
        void doIfFinished(F functor) const
        {
            QMutexLocker lock(&m_finishedMutex);
            if (m_finished) { functor(); }
        }

        //! Executes some code (in the caller's thread) if the task has not finished.
        //! \threadsafe The functors may not call any methods that observe the worker's finished flag.
        template <typename F>
        void doIfNotFinished(F functor) const
        {
            QMutexLocker lock(&m_finishedMutex);
            if (!m_finished) { functor(); }
        }

        //! Executes some code (in the caller's thread) if the task has finished and some different code if it has not
        //! finished. \threadsafe The functor may not call any method that observes the worker's finished flag.
        template <typename F1, typename F2>
        void doIfFinishedElse(F1 ifFunctor, F2 elseFunctor) const
        {
            QMutexLocker lock(&m_finishedMutex);
            if (m_finished) { ifFunctor(); }
            else { elseFunctor(); }
        }

        //! Blocks until the task is finished.
        //! \threadsafe Will deadlock if called by the worker thread.
        void waitForFinished() noexcept;

        //! Notify the task that its result is no longer needed, so it can finish early.
        //! \threadsafe
        void abandon() noexcept;

        //! Convenience to call abandon() followed by waitForFinished().
        void abandonAndWait() noexcept;

        //! All workers currently existing.
        static const QSet<CWorkerBase *> &allWorkers() { return s_allWorkers; }

    signals:
        //! Emitted when the task is about to start.
        void aboutToStart();

        //! Emitted when the task is finished.
        //! \note Slots connected to this signal may not call any methods that observe the worker's finished flag.
        void finished();

    protected:
        //! Constructor.
        CWorkerBase();

        //! For the task to check whether it can finish early.
        //! \threadsafe
        bool isAbandoned() const;

        //! True if the worker has started.
        bool hasStarted() const { return m_started; }

        //! Mark the task as started.
        void setStarted() { m_started = true; }

        //! Mark the task as finished.
        void setFinished()
        {
            QMutexLocker lock(&m_finishedMutex);
            m_finished = true;
            emit finished();
        }

    private:
        virtual void quit() noexcept {}
        virtual void quitAndWait() noexcept { waitForFinished(); }

        bool m_started = false;
        bool m_finished = false;
        mutable QRecursiveMutex m_finishedMutex;
        static QSet<CWorkerBase *> s_allWorkers;
    };

    /*!
     * Class for doing some arbitrary parcel of work in its own thread.
     *
     * The task is exposed as a function object, so could be a lambda or a hand-written closure.
     * CWorker can not be subclassed, instead it can be extended with rich callable task objects.
     */
    class SWIFT_MISC_EXPORT CWorker final : public CWorkerBase
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
         */
        template <typename F>
        static CWorker *fromTask(QObject *owner, const QString &name, F &&task)
        {
            int typeId = qMetaTypeId<std::decay_t<decltype(std::forward<F>(task)())>>();
            return fromTaskImpl(owner, name, typeId, [task = std::forward<F>(task)]() mutable {
                if constexpr (std::is_void_v<decltype(task())>)
                {
                    std::move(task)();
                    return QVariant();
                }
                else { return QVariant::fromValue(std::move(task)()); }
            });
        }

        //! Connects to a functor to which will be passed the result when the task is finished.
        //! \tparam R The return type of the task.
        //! \threadsafe The functor may not call any method that observes the worker's finished flag.
        template <typename R, typename F>
        void thenWithResult(F functor)
        {
            Q_ASSERT_X(m_result.canConvert<R>(), Q_FUNC_INFO, "Type in thenWithResult must match return type of task");
            then([this, functor]() { functor(this->resultNoWait<R>()); });
        }

        //! Connects to a functor or method to which will be passed the result when the task is finished.
        //! \tparam R The return type of the task.
        //! \threadsafe The functor may not call any method that observes the worker's finished flag.
        template <typename R, typename T, typename F>
        void thenWithResult(T *context, F functor)
        {
            Q_ASSERT_X(m_result.canConvert<R>(), Q_FUNC_INFO, "Type in thenWithResult must match return type of task");

            // MS 2018-10 It is possible that a queued finished() signal will be delivered after the worker was
            //            destroyed, so we can't refer to the this pointer inside the contextual then() lambda.
            //            Therefore we use a promise to extract the result from a non-contextual then(). See T414.
            auto promise = std::make_shared<std::promise<R>>();
            auto future = promise->get_future().share();
            then([this, promise]() { promise->set_value(this->resultNoWait<R>()); });
            then(context, [context, functor, future]() { private_ns::invokeSlot(functor, context, future.get()); });
        }

        //! Returns the result of the task, waiting for it to finish if necessary.
        //! \tparam R The return type of the task.
        //! \threadsafe
        template <typename R>
        R result()
        {
            waitForFinished();
            return this->resultNoWait<R>();
        }

    private slots:
        //! Called when the worker has been moved into its new thread.
        void ps_runTask();

    private:
        CWorker(const std::function<QVariant()> &task) : m_task(task) {}
        static CWorker *fromTaskImpl(QObject *owner, const QString &name, int typeId,
                                     const std::function<QVariant()> &task);

        template <typename R>
        R resultNoWait()
        {
            Q_ASSERT(m_result.canConvert<R>());
            return m_result.value<R>();
        }

        std::function<QVariant()> m_task;
        QVariant m_result;
    };

    /*!
     * Base class for a long-lived worker object which lives in its own thread.
     */
    class SWIFT_MISC_EXPORT CContinuousWorker : public CWorkerBase
    {
        Q_OBJECT

    public:
        /*!
         * Constructor.
         * \param owner Will be the parent of the new thread (the worker has no parent).
         * \param name A name for the worker, which will be used to create a name for the thread.
         */
        CContinuousWorker(QObject *owner, const QString &name);

        //! Starts a thread and moves the worker into it.
        void start(QThread::Priority priority = QThread::InheritPriority);

        //! Stops the thread the next time around its event loop.
        //! The thread and the worker will then be deleted.
        //! \threadsafe
        void quit() noexcept final;

        //! Calls quit() and blocks until the thread is finished.
        //! \threadsafe Will deadlock if called by the worker thread.
        void quitAndWait() noexcept final;

        //! Enabled (running)?
        //! \threadsafe
        bool isEnabled() const { return m_enabled; }

        //! Name of the worker
        const QString &getName() { return m_name; }

        //! Owner of the worker
        const QObject *owner() const { return m_owner; }

    protected:
        //! Called when the thread is started.
        virtual void initialize() {}

        //! Called when the thread is finished.
        virtual void cleanup() {}

        //! Called before quit is called
        //! \remark can be used to "clean things up" or request work functions to stop
        virtual void beforeQuit() noexcept {}

        //! Wait time for quitAndWait, 0 means not waiting
        virtual unsigned long waitTimeoutMs() const { return 15 * 1000; }

    private:
        //! Called after cleanup().
        void finish();

        //! Enabled (running)?
        //! \threadsafe
        void setEnabled(bool enabled) { m_enabled = enabled; }

        using CWorkerBase::setStarted;
        using CWorkerBase::setFinished;

        QObject *m_owner = nullptr; //!< owner, parent of the QThread
        QString m_name; //!< worker's name
        std::atomic<bool> m_enabled { true }; //!< marker it is enabled
    };
} // namespace swift::misc

#endif // SWIFT_MISC_WORKER_H
