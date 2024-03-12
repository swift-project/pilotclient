// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKCORE_THREADED_READER_H
#define BLACKCORE_THREADED_READER_H

#include "blackcore/vatsim/vatsimsettings.h"
#include "blackcore/blackcoreexport.h"
#include "blackmisc/network/urlloglist.h"
#include "blackmisc/logcategories.h"
#include "blackmisc/worker.h"

#include <QDateTime>
#include <QObject>
#include <QReadWriteLock>
#include <QString>
#include <QtGlobal>
#include <QPair>
#include <atomic>

class QNetworkReply;
class QTimer;

namespace BlackCore
{
    //! Support for threaded based reading and parsing tasks such
    //! as data files via http, or file system and parsing (such as FSX models)
    class BLACKCORE_EXPORT CThreadedReader : public BlackMisc::CContinuousWorker
    {
        Q_OBJECT

    public:
        //! Log categories
        static const QStringList &getLogCategories();

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

        //! Is marked as read failed
        //! \threadsafe
        //! \deprecated likely to be removed
        bool isMarkedAsFailed() const;

        //! Set marker for read failed
        //! \threadsafe
        //! \deprecated 2017-09 likely to be removed, using m_urlReadLog in future
        void setMarkedAsFailed(bool failed);

        //! Get the read log
        //! \threadsafe
        BlackMisc::Network::CUrlLogList getReadLog() const;

        //! Starts the reader
        //! \threadsafe
        void startReader();

        //! Pauses the reader
        //! \threadsafe
        void pauseReader();

        //! Used in unit test
        //! \remark needs to be done before started in different thread
        void markAsUsedInUnitTest() { m_unitTest = true; }

        //! Has pending URLs?
        //! \threadsafe
        bool hasPendingUrls() const;

        //! Get the URL log list
        //! \threadsafe
        BlackMisc::Network::CUrlLogList getUrlLogList() const;

        //! Progress 0..100
        //! \threadsafe
        int getNetworkReplyProgress() const { return m_networkReplyProgress; }

        //! Max./current bytes
        QPair<qint64, qint64> getNetworkReplyBytes() const;

    protected:
        mutable QReadWriteLock m_lock { QReadWriteLock::Recursive }; //!< lock which can be used from the derived classes
        std::atomic_int m_networkReplyProgress; //!< Progress percentage 0...100
        std::atomic_llong m_networkReplyCurrent; //!< current bytes
        std::atomic_llong m_networkReplyNax; //!< max bytes

        //! Constructor
        CThreadedReader(QObject *owner, const QString &name);

        //! When was reply last modified, -1 if N/A
        qint64 lastModifiedMsSinceEpoch(QNetworkReply *nwReply) const;

        //! Make sure everthing runs correctly in own thread
        void threadAssertCheck() const;

        //! Stores new content hash and returns if content changed (based on hash value
        //! \threadsafe
        bool didContentChange(const QString &content, int startPosition = -1);

        //! Set initial and periodic times
        void setInitialAndPeriodicTime(int initialTime, int periodicTime);

        //! This method does the actual work in the derived class
        virtual void doWorkImpl() {}

        //! Still enabled etc.?
        //! \threadsafe under normal conditions
        bool doWorkCheck() const;

        //! Get request from network, and log with m_urlReadLog
        //! \threadsafe read log access is thread safe
        QNetworkReply *getFromNetworkAndLog(const BlackMisc::Network::CUrl &url, const BlackMisc::CSlot<void(QNetworkReply *)> &callback);

        //! Network request progress
        virtual void networkReplyProgress(int logId, qint64 current, qint64 max, const QUrl &url);

        //! Network reply received, mark in m_urlReadLog
        //! \threadsafe
        void logNetworkReplyReceived(QNetworkReply *reply);

        //! Use this to log inconsistent data
        //! \remark here in a single function severity / format can be adjusted
        static void logInconsistentData(const BlackMisc::CStatusMessage &msg, const char *funcInfo = nullptr);

    private:
        //! Trigger doWorkImpl
        void doWork();

        static constexpr int OutdatedPendingCallMs = 30 * 1000; //!< when is a call considered "outdated"

        int m_initialTime = -1; //!< Initial start delay
        int m_periodicTime = -1; //!< Periodic time after which the task is repeated
        QDateTime m_updateTimestamp; //!< when file/resource was read
        uint m_contentHash = 0; //!< has of the content given
        std::atomic_bool m_markedAsFailed { false }; //!< marker if reading failed
        bool m_unitTest { false }; //!< mark as unit test
        BlackMisc::Network::CUrlLogList m_urlReadLog; //!< URL based reading can be logged
    };
} // namespace

#endif // guard
