/* Copyright (C) 2017
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_PROMISE_H
#define BLACKMISC_PROMISE_H

#include <QObject>
#include <QFuture>
#include <QFutureWatcher>
#include <QCoreApplication>
#include <QSharedPointer>
#include <utility>

namespace BlackMisc
{

    namespace Private
    {
        //! \private Shared data for CPromise.
        //! \details QFutureInterface is undocumented but public, see also:
        //!          http://lists.qt-project.org/pipermail/development/2015-July/022572.html
        template <typename T>
        class CPromiseData final : public QFutureInterface<T>
        {
        public:
            CPromiseData() { this->reportStarted(); }
            ~CPromiseData() { if (this->isRunning()) { this->cancel(); } }

            CPromiseData(const CPromiseData &) = delete;
            CPromiseData &operator =(const CPromiseData &) = delete;
        };

        //! \private
        template <typename T, typename F>
        void doAfter(QFuture<T> future, QObject *context, F &&func)
        {
            QSharedPointer<QFutureWatcher<T>> watcher(new QFutureWatcher<T>, &QObject::deleteLater);
            if (!context) { context = watcher.data(); }
            QObject::connect(watcher.data(), &QFutureWatcher<T>::finished, context, [watcher, func = std::forward<F>(func)]() mutable
            {
                if (!watcher->isCanceled()) { func(watcher->future()); }
                watcher.reset();
            });
            watcher->setFuture(future);
            QCoreApplication::sendPostedEvents(watcher.data());
        }
    }

    /*!
     * Connect a slot or function to be invoked in the given context when a QFuture is finished.
     */
    template <typename T, typename F>
    void doAfter(QFuture<T> future, QObject *context, F &&func)
    {
        Private::doAfter(future, context, std::forward<F>(func));
    }

    /*!
     * Connect a slot or function to be invoked in the given context when a void QFuture is finished.
     */
    template <typename F>
    void doAfter(QFuture<void> future, QObject *context, F &&func)
    {
        Private::doAfter(future, context, [func = std::forward<F>(func)](auto&&) { func(); });
    }

    /*!
     * A promise-based interface to QFuture, similar to std::promise for std::future.
     */
    template <typename T>
    class CPromise
    {
    public:
        //! Return a future that can be used to access the result.
        QFuture<T> future() { return m_data->future(); }

        //! Mark the result as cancelled.
        void abandon() { m_data->cancel(); m_data->reportFinished(); }

        //! Set the result value that will be made available through the future.
        void setResult(const T &value) { m_data->reportFinished(&value); }

        //! Set the result value from the given future. Will block if future is not ready.
        template <typename U>
        void setResult(QFuture<U> future) { future.waitForFinished(); future.isCanceled() ? abandon() : setResult(future.result()); }

        //! When the given future is ready, use its result to set the result of this promise.
        template <typename U>
        void chainResult(QFuture<U> future) { doAfter(future, nullptr, [*this](auto &&f) mutable { setResult(f); }); }

        //! Invoke a functor and use its return value to set the result.
        //! \details Useful for uniform syntax in generic code where T could be void.
        template <typename F>
        void setResultFrom(F &&func) { setResult(std::forward<F>(func)()); }

    private:
        QSharedPointer<Private::CPromiseData<T>> m_data { new Private::CPromiseData<T> };
    };

    /*!
     * Specialization of CPromise for void futures.
     */
    template <>
    class CPromise<void>
    {
    public:
        //! Return a future that can be used to detect when the task is complete.
        QFuture<void> future() { return m_data->future(); }

        //! Mark the task as cancelled.
        void abandon() { m_data->cancel(); m_data->reportFinished(); }

        //! Mark the task as complete.
        void setResult() { m_data->reportFinished(); }

        //! Wait for the given future, then mark the task as complete.
        template <typename U>
        void setResult(QFuture<U> future) { future.waitForFinished(); future.isCanceled() ? abandon() : setResult(); }

        //! When the given future is ready, mark this promise as complete.
        template <typename U>
        void chainResult(QFuture<U> future) { doAfter(future, nullptr, [this](auto &&f) { setResult(f); }); }

        //! Invoke a functor and mark the task as complete.
        //! \details Useful for uniform syntax in generic code where T could be void.
        template <typename F>
        void setResultFrom(F &&func) { std::forward<F>(func)(); setResult(); }

    private:
        QSharedPointer<Private::CPromiseData<void>> m_data { new Private::CPromiseData<void> };
    };

}

#endif
