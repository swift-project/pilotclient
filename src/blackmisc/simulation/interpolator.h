/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_INTERPOLATOR_H
#define BLACKMISC_SIMULATION_INTERPOLATOR_H

#include "interpolationrenderingsetup.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/aviation/aircraftpartslist.h"
#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/aviation/aircraftpartslist.h"
#include "blackmisc/simulation/remoteaircraftprovider.h"

#include <QObject>
#include <QString>
#include <QtGlobal>

namespace BlackMisc
{
    class CWorker;
    namespace Aviation { class CCallsign; }
    namespace Simulation
    {
        class CInterpolationHints;

        //! Interpolator, calculation inbetween positions
        class BLACKMISC_EXPORT IInterpolator :
            public QObject,
            public BlackMisc::Simulation::CRemoteAircraftAware
        {
            Q_OBJECT

        public:
            //! Virtual destructor
            virtual ~IInterpolator();

            //! Log category
            static QString getLogCategory() { return "swift.interpolator"; }

            //! Status of interpolation
            struct BLACKMISC_EXPORT InterpolationStatus // does not link without export/allTrue, reset
            {
            public:
                //! Did interpolation succeed?
                bool didInterpolationSucceed() const { return m_interpolationSucceeded; }

                //! Set succeeded
                void setInterpolationSucceeded(bool succeeded) { m_interpolationSucceeded = succeeded; }

                //! Changed position?
                bool hasChangedPosition() const { return m_changedPosition; }

                //! Set as changed
                void setChangedPosition(bool changed) { m_changedPosition = changed; }

                //! all OK
                bool allTrue() const;

                //! Reset to default values
                void reset();

            private:
                bool m_changedPosition = false;        //!< position was changed
                bool m_interpolationSucceeded = false; //!< interpolation succeeded (means enough values, etc.)
            };

            //! Status regarding parts
            struct BLACKMISC_EXPORT PartsStatus // does not link without export/allTrue, resetx
            {
            public:
                //! all OK
                bool allTrue() const;

                //! Supporting parts
                bool isSupportingParts() const { return m_supportsParts; }

                //! Set support flag
                void setSupportsParts(bool supports) { m_supportsParts = supports; }

                //! Reset to default values
                void reset();

            private:
                bool m_supportsParts = false;   //!< supports parts for given callsign
            };

            //! Current interpolated situation
            //! \threadsafe
            virtual BlackMisc::Aviation::CAircraftSituation getInterpolatedSituation(
                const BlackMisc::Aviation::CCallsign &callsign, qint64 currentTimeSinceEpoc,
                const CInterpolationHints &hints, InterpolationStatus &status) const;

            //! Current interpolated situation, to be implemented by subclass
            //! \threadsafe
            //! \remark public only for XP driver
            virtual BlackMisc::Aviation::CAircraftSituation getInterpolatedSituation(
                const BlackMisc::Aviation::CCallsign &callsign,
                const BlackMisc::Aviation::CAircraftSituationList &situations, qint64 currentTimeSinceEpoc,
                const CInterpolationHints &hints, InterpolationStatus &status) const = 0;

            //! Parts before given offset time (aka pending parts)
            //! \threadsafe
            virtual BlackMisc::Aviation::CAircraftParts getInterpolatedParts(
                const Aviation::CCallsign &callsign,
                const BlackMisc::Aviation::CAircraftPartsList &parts, qint64 cutoffTime,
                PartsStatus &partsStatus, bool log = false) const;

            //! Parts before given offset time (aka pending parts)
            //! \threadsafe
            virtual BlackMisc::Aviation::CAircraftParts getInterpolatedParts(
                const BlackMisc::Aviation::CCallsign &callsign, qint64 cutoffTime,
                PartsStatus &partsStatus, bool log = false) const;

            //! Enable debug messages etc.
            //! \threadsafe
            void setInterpolatorSetup(const CInterpolationAndRenderingSetup &setup);

            //! Write a log in background
            BlackMisc::CWorker *writeLogInBackground();

            //! Clear log file
            void clearLog();

            //! Enable log messages etc.
            //! \threadsafe
            //! \remark public for FS9 to get setup in Fs9Client
            CInterpolationAndRenderingSetup getInterpolatorSetup() const;

            /*!
             * Takes input between 0 and 1 and returns output between 0 and 1 smoothed with an S-shaped curve.
             *
             * Useful for making interpolation seem smoother, efficiently as it just uses simple arithmetic.
             * \see https://en.wikipedia.org/wiki/Smoothstep
             * \see http://sol.gfxile.net/interpolation/
             */
            static double smootherStep(double x)
            {
                return x * x * x * (x * (x * 6.0 - 15.0) + 10.0);
            }

        protected:
            //! Log for interpolation
            struct InterpolationLog
            {
                qint64 timestamp = -1;       //!< current timestamp
                double groundFactor = -1;    //!< current ground factor
                double vtolAircraft = false; //!< VTOL aircraft
                double deltaTimeMs = 0;      //!< delta time to last situation
                double simulationTimeFraction = -1;        //!< time fraction, normally 0..1
                double deltaTimeFractionMs = -1;           //!< delta time fraction
                bool useParts = false;                     //!< supporting aircraft parts
                BlackMisc::Aviation::CCallsign callsign;   //!< current callsign
                BlackMisc::Aviation::CAircraftParts parts; //!< corresponding parts used in interpolator
                BlackMisc::Aviation::CAircraftSituation oldSituation;     //!< old situation
                BlackMisc::Aviation::CAircraftSituation newSituation;     //!< new situation
                BlackMisc::Aviation::CAircraftSituation currentSituation; //!< interpolated situation
            };

            //! Log for parts
            struct PartsLog
            {
                qint64 timestamp = -1; //!< current timestamp
                BlackMisc::Aviation::CCallsign callsign;   //!< current callsign
                BlackMisc::Aviation::CAircraftParts parts; //!< parts to be logged
            };

            //! Constructor
            IInterpolator(BlackMisc::Simulation::IRemoteAircraftProvider *provider, const QString &objectName, QObject *parent);

            //! Log current interpolation cycle, only stores in memory, for performance reasons
            //! \remark const to allow const interpolator functions
            //! \threadsafe
            void logInterpolation(const InterpolationLog &log) const;

            //! Log current parts cycle, only stores in memory, for performance reasons
            //! \remark const to allow const interpolator functions
            //! \threadsafe
            void logParts(const PartsLog &parts) const;

            //! Get log as HTML table
            //! \threadsafe
            static QString getHtmlInterpolationLog(const QList<InterpolationLog> &logs);

            //! Get log as HTML table
            //! \threadsafe
            static QString getHtmlPartsLog(const QList<PartsLog> &logs);

            //! Set the ground elevation from hints
            static void setGroundElevationFromHint(const CInterpolationHints &hints, BlackMisc::Aviation::CAircraftSituation &situation, bool override = true);

            //! Set on ground flag
            static void setGroundFlagFromInterpolator(const CInterpolationHints &hints, double groundFactor, BlackMisc::Aviation::CAircraftSituation &situation);

            CInterpolationAndRenderingSetup m_setup; //!< allows to enable/disable debug/log messages

        private:
            //! Write log to file
            static CStatusMessageList writeLogFile(const QList<InterpolationLog> &interpolation, const QList<PartsLog> &parts);

            //! Status of file operation
            static CStatusMessage logStatusFileWriting(bool success, const QString &fileName);

            //! Create readable time
            static QString msSinceEpochToTime(qint64 ms);

            //! Create readable time
            static QString msSinceEpochToTime(qint64 t1, qint64 t2, qint64 t3 = -1);

            mutable QReadWriteLock  m_lockSetup; //!< lock setup
            mutable QReadWriteLock  m_lockLogs;  //!< lock logging
            mutable QList<PartsLog> m_partsLogs; //!< logs of parts
            mutable QList<InterpolationLog> m_interpolationLogs; //!< logs of interpolation
        };
    } // namespace
} // namespace
#endif // guard
