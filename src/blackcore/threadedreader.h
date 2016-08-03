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

        //! Request new reading
        //! \note override as required, default is to call initialize()
        virtual void requestReload();

        //! Network available
        bool isNetworkAvailable() const;

        //! Get the timer interval (ms)
        //! \threadsafe
        int interval() const;

        //! Is timer running
        //! \threadsafe
        bool isTimerActive() const;

        //! Set inverval from settings and start
        void setIntervalFromSettingsAndStart();

    public slots:
        //! Graceful shutdown
        //! \threadsafe
        void gracefulShutdown();

    protected:
        QTimer *m_updateTimer = nullptr;  //!< update timer
        mutable QReadWriteLock m_lock {QReadWriteLock::Recursive}; //!< lock which can be used from the derived classes

        //! Constructor
        CThreadedReader(QObject *owner, const QString &name);

        //! When was reply last modified, -1 if N/A
        qint64 lastModifiedMsSinceEpoch(QNetworkReply *nwReply) const;

        //! Make sure everthing runs correctly in own thread
        void threadAssertCheck() const;

        //! Get settings, default implementation returns BlackCore::Settings::CReaderSettings::neverUpdateSettings
        virtual BlackCore::Vatsim::CReaderSettings getSettings() const;

        //! Set initial time
        void setInitialTime();

        //! Set periodic time
        void setPeriodicTime();

        //! Set the update time
        //! \param updatePeriodMs <=0 stops the timer
        //! \threadsafe
        void setInterval(int updatePeriodMs);

        //! Restart timer
        //! \threadsafe
        void restartTimer(bool onlyWhenActive = false);

        //! Stores new content hash and returns if content changed (based on hash value
        //! \threadsafe
        bool didContentChange(const QString &content, int startPosition = -1);

    private:
        QDateTime               m_updateTimestamp;  //!< when file/resource was read
        uint                    m_contentHash = 0;  //!< has of the content given
        QMetaObject::Connection m_toggleConnection; //!< connection to switch interval from initial to periodic

    private slots:
        //! switch from initial to periodic
        void ps_toggleInterval();
    };
} // namespace

#endif // guard
