/* Copyright (C) 2014
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_WORKER_H
#define BLACKMISC_WORKER_H

#include <QThread>
#include <QMutex>
#include <functional>
#include <atomic>

namespace BlackMisc
{

    /*!
     * Just a subclass of QThread whose destructor waits for the thread to finish.
     */
    class CRegularThread : public QThread
    {
    public:
        //! Constructor
        CRegularThread(QObject *parent = nullptr) : QThread(parent) {}

        //! Destructor
        ~CRegularThread()
        {
            quit();
            wait();
        }
    };

    /*!
     * Class for doing some arbitrary task in its own thread.
     *
     * The task is exposed as a function object, so could be a lambda or a hand-written closure.
     * CWorker can not be subclassed, instead it can be extended with rich callable task objects.
     */
    class CWorker final : public QObject
    {
        Q_OBJECT

    public:
        /*!
         * Returns a new worker object which lives in a new thread.
         * \param owner Will be the parent of the new thread (the worker has no parent).
         * \param name A name for the task, which will be used to create a name for the thread.
         * \param task A function object which will be run by the worker in its thread.
         */
        static CWorker *fromTask(QObject *owner, const QString &name, std::function<void()> task);

        //! Connects to a slot which will be called when the task is finished.
        //! \threadsafe
        template <typename T, typename F>
        auto then(T *receiver, F slot) -> typename std::enable_if<std::is_member_function_pointer<F>::value>::type
        {
            Q_ASSERT(receiver->thread() == QThread::currentThread());
            QMutexLocker lock(&m_finishedMutex);
            connect(this, &CWorker::finished, receiver, slot);
            if (m_finished) { (receiver->*slot)(); }
        }

        //! Connects to a functor which will be called when the task is finished.
        //! \threadsafe
        template <typename T, typename F>
        auto then(T *context, F functor) -> typename std::enable_if<! std::is_member_function_pointer<F>::value>::type
        {
            Q_ASSERT(context->thread() == QThread::currentThread());
            QMutexLocker lock(&m_finishedMutex);
            connect(this, &CWorker::finished, context, functor);
            if (m_finished) { functor(); }
        }

        //! Connects to a functor which will be called when the task is finished.
        //! \threadsafe
        template <typename F>
        void then(F functor)
        {
            QMutexLocker lock(&m_finishedMutex);
            connect(this, &CWorker::finished, functor);
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

    signals:
        //! Emitted when the task is finished.
        void finished();

    private slots:
        //! Called when the worker has been moved into its new thread.
        void ps_runTask();

    private:
        CWorker(std::function<void()> task) : m_task(task) {}

        bool m_finished = false;
        mutable QMutex m_finishedMutex { QMutex::Recursive };
        std::function<void()> m_task;
    };

}

#endif
