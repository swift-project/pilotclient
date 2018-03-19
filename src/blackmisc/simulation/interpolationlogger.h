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
#include "interpolationhints.h"
#include "blackmisc/simulation/remoteaircraftprovider.h"
#include "blackmisc/aviation/aircraftpartslist.h"
#include "blackmisc/aviation/aircraftsituationlist.h"
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
        //! Log entry for situation interpolation
        struct BLACKMISC_EXPORT SituationLog
        {
            QChar interpolator;          //!< what interpolator is used
            qint64 tsCurrent = -1;       //!< current timestamp
            qint64 tsInterpolated = -1;  //!< timestamp interpolated
            double groundFactor = -1;    //!< current ground factor
            double vtolAircraft = false; //!< VTOL aircraft
            double simulationTimeFraction = -1; //!< time fraction, expected 0..1
            double deltaSampleTimesMs = -1;     //!< delta time between samples (i.e. 2 situations)
            bool useParts = false;              //!< supporting aircraft parts
            int noNetworkSituations = 0;        //!< available network situations
            int noTransferredElevations = 0;    //!< transferred elevation to n situations
            QString elevationInfo;              //!< info about elevation retrieval
            Aviation::CCallsign callsign;       //!< current callsign
            Aviation::CAircraftParts parts;     //!< corresponding parts used in interpolator
            Aviation::CAircraftSituationList interpolationSituations; //!< the interpolator uses 2, 3 situations (oldest at end)
            Aviation::CAircraftSituation situationCurrent; //!< interpolated situation
            PhysicalQuantities::CLength  cgAboveGround;    //!< center of gravity
            CInterpolationAndRenderingSetupPerCallsign usedSetup; //!< used setup
            CInterpolationHints usedHints; //!< hints

            //! Delta time between interpolation and current time
            double deltaCurrentToInterpolatedTime() const
            {
                return static_cast<double>(tsCurrent - tsInterpolated);
            }

            //! Full name of interpolator
            const QString &interpolationType() const;

            //! The oldest situation
            const Aviation::CAircraftSituation &oldestInterpolationSituation() const
            {
                return interpolationSituations.frontOrDefault();
            }

            //! The newest situation
            const Aviation::CAircraftSituation &newestInterpolationSituation() const
            {
                return interpolationSituations.backOrDefault();
            }

            //! To string
            QString toQString(
                bool withHints, bool withSetup,
                bool withCurrentSituation, bool withElevation,
                bool withOtherPositions, bool withDeltaTimes, const QString &separator = {" "}) const;
        };

        //! Log entry for parts interpolation
        struct BLACKMISC_EXPORT PartsLog
        {
            qint64 tsCurrent = -1;  //!< current timestamp
            bool empty = false;     //!< empty parts?
            int noNetworkParts = 0; //!< available network situations
            Aviation::CCallsign callsign;   //!< current callsign
            Aviation::CAircraftParts parts; //!< parts to be logged

            //! To string
            QString toQString(const QString &separator = {" "}) const;
        };

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

            //! Get last log
            //! \threadsafe
            SituationLog getLastSituationLog() const;

            //! Get last log
            //! \threadsafe
            SituationLog getLastSituationLog(const Aviation::CCallsign &cs) const;

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

            //! Get last parts
            //! \threadsafe
            PartsLog getLastPartsLog() const;

            //! Get last parts
            //! \threadsafe
            PartsLog getLastPartsLog(const Aviation::CCallsign &cs) const;

            //! File pattern for interpolation log
            static const QString &filePatternInterpolationLog();

            //! File pattern for parts log
            static const QString &filePatternPartsLog();

            //! All log.file patterns
            static const QStringList &filePatterns();

            //! Create readable time
            static QString msSinceEpochToTime(qint64 ms);

            //! Create readable time plus timestamp
            static QString msSinceEpochToTimeAndTimestamp(qint64 ms);

            //! Create readable time
            static QString msSinceEpochToTime(qint64 t1, qint64 t2, qint64 t3 = -1);

        private:
            //! Get log as HTML table
            static QString getHtmlInterpolationLog(const QList<SituationLog> &getSituationsLog);

            //! Get log as HTML table
            static QString getHtmlPartsLog(const QList<PartsLog> &getSituationsLog);

            //! Write log to file
            static CStatusMessageList writeLogFile(const QList<SituationLog> &interpolation, const QList<PartsLog> &getPartsLog);

            //! Status of file operation
            static CStatusMessage logStatusFileWriting(bool success, const QString &fileName);

            mutable QReadWriteLock m_lockSituations; //!< lock logging situations
            mutable QReadWriteLock m_lockParts;      //!< lock logging parts
            int m_maxSituations = 2500;              //!< max.number of situations
            QList<PartsLog> m_partsLogs;             //!< logs of parts
            QList<SituationLog> m_situationLogs;     //!< logs of interpolation
        };
    } // namespace
} // namespace
#endif // guard
