/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_THREADED_READER_H
#define BLACKCORE_THREADED_READER_H

#include "blackcore/vatsim/vatsimsettings.h"
#include "blackcore/blackcoreexport.h"
#include "blackmisc/logcategorylist.h"
#include "blackmisc/worker.h"

#include <QDateTime>
#include <QObject>
#include <QReadWriteLock>
#include <QString>
#include <QtGlobal>
#include <atomic>

class QNetworkReply;
class QTimer;

namespace BlackCore
{
    //! Support for threaded based reading and parsing tasks such
    //! as data files via http, or file system and parsing (such as FSX models)
    class BLACKCORE_EXPORT CThreadedReader : public BlackMisc::CContinuousWorker
    {
    public:
        //! Log categories
        static const BlackMisc::CLogCategoryList &getLogCategories();

        //! Destructor
        virtual ~CThreadedReader();

        //! Thread safe, get update timestamp
        //! \threadsafe
        QDateTime getUpdateTimestamp() const;

        //! Thread safe, set update timestamp
        //! \threadsafe
        void setUpdateTimestamp(const QDateTime &updateTimestamp = QDateTime::currentDateTimeUtc());

        //! Was setup read within last xx milliseconds
        //! \threadsafe
        bool updatedWithinLastMs(qint64 timeLastMs);

        //! Network accessible?
        bool isNetworkAccessible() const;

        //! Is marked as read failed
        //! \threadsafe
        bool isMarkedAsFailed() const;

        //! Set marker for read failed
        //! \threadsafe
        void setMarkedAsFailed(bool failed);

        //! Starts the reader
        //! \threadsafe
        void startReader();

        //! Pauses the reader
        //! \threadsafe
        void pauseReader();

        //! Used in unit test
        void markAsUsedInUnitTest() { m_unitTest = true; }

    protected:
        mutable QReadWriteLock m_lock {QReadWriteLock::Recursive}; //!< lock which can be used from the derived classes

        //! Constructor
        CThreadedReader(QObject *owner, const QString &name);

        //! When was reply last modified, -1 if N/A
        qint64 lastModifiedMsSinceEpoch(QNetworkReply *nwReply) const;

        //! Make sure everthing runs correctly in own thread
        void threadAssertCheck() const;

        //! Stores new content hash and returns if content changed (based on hash value
        //! \threadsafe
        bool didContentChange(const QString &content, int startPosition = -1);

        //! \copydoc BlackMisc::CContinuousWorker::cleanup
        virtual void cleanup() override;

        //! Set initial and periodic times
        void setInitialAndPeriodicTime(int initialTime, int periodicTime);

        //! This method does the actual work in the derived class
        virtual void doWorkImpl() {}

        //! Still enabled etc.
        bool doWorkCheck() const;

    private:
        //! Trigger doWorkImpl
        void doWork();

        int               m_initialTime = -1;         //!< Initial start delay
        int               m_periodicTime = -1;        //!< Periodic time after which the task is repeated
        QDateTime         m_updateTimestamp;          //!< when file/resource was read
        uint              m_contentHash = 0;          //!< has of the content given
        std::atomic<bool> m_markedAsFailed { false }; //!< marker if reading failed
        QTimer            m_updateTimer { this };
        bool              m_unitTest { false };       //!< mark as unit test
    };
} // namespace

#endif // guard
