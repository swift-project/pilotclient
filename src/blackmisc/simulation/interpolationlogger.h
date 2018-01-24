/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_INTERPOLATIONLOGGER_H
#define BLACKMISC_SIMULATION_INTERPOLATIONLOGGER_H

#include "interpolationrenderingsetup.h"
#include "blackmisc/simulation/remoteaircraftprovider.h"
#include "blackmisc/aviation/aircraftpartslist.h"
#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/aviation/aircraftpartslist.h"
#include "blackmisc/logcategorylist.h"
#include <QObject>
#include <QStringList>
#include <QtGlobal>

namespace BlackMisc
{
    class CWorker;
    namespace Simulation
    {
        //! Record internal state of interpolator for debugging
        class BLACKMISC_EXPORT CInterpolationLogger : public QObject
        {
            Q_OBJECT

        public:
            //! Constructor
            CInterpolationLogger(QObject *parent = nullptr);

            //! Log categories
            static const CLogCategoryList &getLogCategories();

            //! Write a log in background
            CWorker *writeLogInBackground();

            //! Clear log file
            void clearLog();

            //! Latest log files: 0: Interpolation / 1: Parts
            static QStringList getLatestLogFiles();

            //! Get the log directory
            static QString getLogDirectory();

            //! Log entry for situation interpolation
            struct SituationLog
            {
                QChar interpolator;          //!< what interpolator is used
                qint64 timestamp = -1;       //!< current timestamp
                double groundFactor = -1;    //!< current ground factor
                double vtolAircraft = false; //!< VTOL aircraft
                double deltaTimeMs = 0;      //!< delta time to last situation
                double simulationTimeFraction = -1; //!< time fraction, normally 0..1
                double deltaTimeFractionMs = -1;    //!< delta time fraction
                bool useParts = false;              //!< supporting aircraft parts
                Aviation::CCallsign callsign;       //!< current callsign
                Aviation::CAircraftParts parts;     //!< corresponding parts used in interpolator
                Aviation::CAircraftSituation oldSituation;     //!< old situation
                Aviation::CAircraftSituation newSituation;     //!< new situation
                Aviation::CAircraftSituation currentSituation; //!< interpolated situation
                PhysicalQuantities::CLength cgAboveGround;     //!< center of gravity
            };

            //! Log entry for parts interpolation
            struct PartsLog
            {
                qint64 timestamp = -1; //!< current timestamp
                bool empty = false;    //!< empty parts?
                Aviation::CCallsign callsign;   //!< current callsign
                Aviation::CAircraftParts parts; //!< parts to be logged
            };

            //! Log current interpolation cycle, only stores in memory, for performance reasons
            //! \remark const to allow const interpolator functions
            //! \threadsafe
            void logInterpolation(const SituationLog &log);

            //! Log current parts cycle, only stores in memory, for performance reasons
            //! \remark const to allow const interpolator functions
            //! \threadsafe
            void logParts(const PartsLog &log);

            //! Max.situations logged
            void setMaxSituations(int max);

            //! All situation logs
            //! \threadsafe
            QList<SituationLog> getSituationsLog() const;

            //! All parts logs
            //! \threadsafe
            QList<PartsLog> getPartsLog() const;

            //! All situation logs for callsign
            //! \threadsafe
            QList<SituationLog> getSituationsLog(const Aviation::CCallsign &cs) const;

            //! All parts logs for callsign
            //! \threadsafe
            QList<PartsLog> getPartsLog(const Aviation::CCallsign &cs) const;

            //! Get last situation
            //! \threadsafe
            Aviation::CAircraftSituation getLastSituation() const;

            //! Get last situation
            //! \threadsafe
            Aviation::CAircraftSituation getLastSituation(const Aviation::CCallsign &cs) const;

            //! Get last parts
            //! \threadsafe
            Aviation::CAircraftParts getLastParts() const;

            //! Get last parts
            //! \threadsafe
            Aviation::CAircraftParts getLastParts(const Aviation::CCallsign &cs) const;

            //! File pattern for interpolation log
            static const QString &filePatternInterpolationLog();

            //! File pattern for parts log
            static const QString &filePatternPartsLog();

            //! All log.file patterns
            static const QStringList &filePatterns();

        private:
            //! Get log as HTML table
            static QString getHtmlInterpolationLog(const QList<SituationLog> &getSituationsLog);

            //! Get log as HTML table
            static QString getHtmlPartsLog(const QList<PartsLog> &getSituationsLog);

            //! Write log to file
            static CStatusMessageList writeLogFile(const QList<SituationLog> &interpolation, const QList<PartsLog> &getPartsLog);

            //! Status of file operation
            static CStatusMessage logStatusFileWriting(bool success, const QString &fileName);

            //! Create readable time
            static QString msSinceEpochToTime(qint64 ms);

            //! Create readable time
            static QString msSinceEpochToTime(qint64 t1, qint64 t2, qint64 t3 = -1);

            mutable QReadWriteLock m_lockSituations; //!< lock logging
            mutable QReadWriteLock m_lockParts;      //!< lock logging
            int m_maxSituations = 2500;
            QList<PartsLog> m_partsLogs;             //!< logs of parts
            QList<SituationLog> m_situationLogs;     //!< logs of interpolation
        };
    } // namespace
} // namespace
#endif // guard
