/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_INTERPOLATIONLOGGER_H
#define BLACKMISC_SIMULATION_INTERPOLATIONLOGGER_H

#include "interpolationrenderingsetup.h"
#include "blackmisc/simulation/remoteaircraftprovider.h"
#include "blackmisc/aviation/aircraftsituationlist.h"
#include "blackmisc/aviation/aircraftpartslist.h"
#include "blackmisc/aviation/aircraftsituationchange.h"
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
            QChar interpolator;             //!< what interpolator is used
            qint64 tsCurrent          = -1; //!< current timestamp
            qint64 tsInterpolated     = -1; //!< timestamp interpolated
            double groundFactor       = -1; //!< current ground factor
            double simTimeFraction    = -1; //!< time fraction, expected 0..1
            double deltaSampleTimesMs = -1; //!< delta time between samples (i.e. 2 situations)
            bool useParts          = false; //!< supporting aircraft parts
            bool vtolAircraft      = false; //!< VTOL aircraft
            bool interpolantRecalc = false; //!< interpolant recalculated
            int noNetworkSituations = 0;    //!< available network situations
            int noInvalidSituations = 0;    //!< invalid situations, missing situations for timestampd
            QString elevationInfo;          //!< info about elevation retrieval
            QString altCorrection;          //!< info about altitude correction as CAircraftSituation::AltitudeCorrection
            Aviation::CCallsign callsign;   //!< current callsign
            Aviation::CAircraftParts parts; //!< corresponding parts used in interpolator
            Aviation::CAircraftSituationList interpolationSituations; //!< the interpolator uses 2, 3 situations (latest at end)
            Aviation::CAircraftSituation situationCurrent; //!< interpolated situation
            Aviation::CAircraftSituationChange change;     //!< change
            PhysicalQuantities::CLength cgAboveGround = PhysicalQuantities::CLength::null(); //!< center of gravity (CG)
            PhysicalQuantities::CLength sceneryOffset = PhysicalQuantities::CLength::null(); //!< scenery offset
            CInterpolationAndRenderingSetupPerCallsign usedSetup; //!< used setup

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

            //! The second latest situation (spline)
            const Aviation::CAircraftSituation &secondInterpolationSituation() const;

            //! To string
            QString toQString(bool withSetup,
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
            static const QStringList &getLogCategories();

            //! Write a log in background
            CWorker *writeLogInBackground(bool clearLog);

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
            static QString getHtmlInterpolationLog(const QList<SituationLog> &logs);

            //! Get log as KML (changed situations)
            static QString getKmlChangedSituations(const QList<SituationLog> &logs);

            //! Get log as KML (elevations)
            static QString getKmlElevations(const QList<SituationLog> &logs);

            //! Get log as KML (changed situations)
            static QString getKmlInterpolatedSituations(const QList<SituationLog> &logs);

            //! Get log as HTML table
            static QString getHtmlPartsLog(const QList<PartsLog> &logs);

            //! Write log to file
            static CStatusMessageList writeLogFiles(const QList<SituationLog> &interpolation, const QList<PartsLog> &getPartsLog);

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
