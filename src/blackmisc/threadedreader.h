/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#ifndef BLACKMISC_THREADED_READER_H
#define BLACKMISC_THREADED_READER_H

//! \file

#include "worker.h"
#include <QReadWriteLock>
#include <QDateTime>
#include <QTimer>
#include <QNetworkReply>
#include <QFuture>
#include <QCoreApplication>

namespace BlackMisc
{
    /*!
     * Support for threaded based reading and parsing tasks such
     * as data files via http, or file system and parsing (such as FSX models)
     *
     * \remarks Header only class to avoid forward instantiations across subprojects
     */
    template <class FutureRet = void> class CThreadedReader : public CContinuousWorker
    {
    public:
        //! Thread safe, set update timestamp
        //! \threadsafe
        QDateTime getUpdateTimestamp() const
        {
            QReadLocker(&this->m_lock);
            return this->m_updateTimestamp;
        }

        //! Thread safe, set update timestamp
        //! \threadsafe
        void setUpdateTimestamp(const QDateTime &updateTimestamp)
        {
            QWriteLocker(&this->m_lock);
            this->m_updateTimestamp = updateTimestamp;
        }

        //! \copydoc CContinuousWorker::cleanup
        virtual void cleanup() override
        {
            // shutdown pending
            QWriteLocker(&this->m_lock);
            if (this->m_pendingFuture.isRunning())
            {
                // cancel does not work with all futures, especially not with QConcurrent::run
                // the stop flag should the job
                // but I will cancel anyway
                this->m_pendingFuture.cancel();
            }
            if (this->m_pendingNetworkReply && this->m_pendingNetworkReply->isRunning())
            {
                this->m_pendingNetworkReply->abort();
            }

            // cancel or stop flag above should terminate QFuture
            this->m_pendingFuture.waitForFinished();
        }

        /*!
         * Set the update time
         * \param updatePeriodMs <=0 stops the timer
         * \threadsafe
         */
        void setInterval(int updatePeriodMs)
        {
            Q_ASSERT(this->m_updateTimer);
            QWriteLocker(&this->m_lock);
            if (updatePeriodMs < 1)
                this->m_updateTimer->stop();
            else
                this->m_updateTimer->start(updatePeriodMs);
        }

        //! Get the timer interval (ms)
        //! \threadsafe
        int interval() const
        {
            QReadLocker rl(&this->m_lock);
            return this->m_updateTimer->interval();
        }

    protected:
        //! Constructor
        CThreadedReader(QObject *owner) : CContinuousWorker(owner),
            m_updateTimer(new QTimer(this)), m_lock(QReadWriteLock::Recursive)
        {
        }

        //! Has pending network replay
        //! \threadsafe
        void setPendingNetworkReply(QNetworkReply *reply)
        {
            QWriteLocker(&this->m_lock);
            this->m_pendingNetworkReply = reply;
        }

        //! Has pending operation
        //! \threadsafe
        void setPendingFuture(QFuture<FutureRet> future)
        {
            QWriteLocker(&this->m_lock);
            this->m_pendingFuture = future;
        }

        QTimer *m_updateTimer = nullptr;                //!< update timer
        mutable QReadWriteLock m_lock;                  //!< lock

    private:
        QDateTime m_updateTimestamp;                    //!< when was file / resource read

        QFuture<FutureRet> m_pendingFuture;             //!< optional future to be stopped
        QNetworkReply *m_pendingNetworkReply = nullptr; //!< optional network reply to be stopped
    };
} // namespace

#endif // guard
