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
#include <QCoreApplication>

namespace BlackMisc
{
    /*!
     * Support for threaded based reading and parsing tasks such
     * as data files via http, or file system and parsing (such as FSX models)
     */
    class CThreadedReader : public CContinuousWorker
    {
    public:
        //! Thread safe, set update timestamp
        //! \threadsafe
        QDateTime getUpdateTimestamp() const;

        //! Thread safe, set update timestamp
        //! \threadsafe
        void setUpdateTimestamp(const QDateTime &updateTimestamp);

        //! Request to stop
        //! \threadsafe
        void requestStop();

        //! Destructor
        virtual ~CThreadedReader();

        //! \copydoc CContinuousWorker::cleanup
        virtual void cleanup() override;

        //! Thread ended of stop requested
        virtual bool isFinished() const override;

        /*!
         * Set the update time
         * \param updatePeriodMs <=0 stops the timer
         * \threadsafe
         */
        void setInterval(int updatePeriodMs);

        //! Get the timer interval (ms)
        //! \threadsafe
        int interval() const;

    protected:
        //! Constructor
        CThreadedReader(QObject *owner, const QString &name);

        QTimer *m_updateTimer = nullptr;         //!< update timer
        mutable QReadWriteLock m_lock {QReadWriteLock::Recursive}; //!< lock

        //! Make sure everthing runs correctly in own thread
        void threadAssertCheck() const;

    private:
        QDateTime m_updateTimestamp; //!< when was file / resource read
        bool m_stopped = false;      //!< optional network reply to be stopped
    };
} // namespace

#endif // guard
