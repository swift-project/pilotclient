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
#include "blackmisc/simulation/remoteaircraftprovider.h"
#include "blackmisc/aviation/aircraftpartslist.h"
#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/aviation/aircraftpartslist.h"
#include "blackmisc/logcategorylist.h"

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
        class CInterpolationLogger;
        class CInterpolatorLinear;
        class CInterpolatorSpline;
        struct CInterpolationStatus;
        struct CPartsStatus;

        //! Interpolator, calculation inbetween positions
        template <typename Derived>
        class CInterpolator : public QObject
        {
        public:
            //! Log categories
            const CLogCategoryList &getLogCategories();

            //! Current interpolated situation
            Aviation::CAircraftSituation getInterpolatedSituation(
                qint64 currentTimeSinceEpoc, const CInterpolationAndRenderingSetup &setup, const CInterpolationHints &hints, CInterpolationStatus &status);

            //! Parts before given offset time (aka pending parts)
            Aviation::CAircraftParts getInterpolatedParts(
                qint64 currentTimeSinceEpoc, const CInterpolationAndRenderingSetup &setup, CPartsStatus &partsStatus, bool log = false);

            //! Add a new aircraft situation
            void addAircraftSituation(const Aviation::CAircraftSituation &situation);

            //! Any aircraft situations?
            bool hasAircraftSituations() const { return !m_aircraftSituations.isEmpty(); }

            //! Add a new aircraft parts
            void addAircraftParts(const Aviation::CAircraftParts &parts);

            //! Any aircraft parts?
            bool hasAircraftParts() const { return !m_aircraftParts.isEmpty(); }

            //! Takes input between 0 and 1 and returns output between 0 and 1 smoothed with an S-shaped curve.
            //!
            //! Useful for making interpolation seem smoother, efficiently as it just uses simple arithmetic.
            //! \see https://en.wikipedia.org/wiki/Smoothstep
            //! \see http://sol.gfxile.net/interpolation/
            static double smootherStep(double x)
            {
                return x * x * x * (x * (x * 6.0 - 15.0) + 10.0);
            }

            //! Attach an observer to read the interpolator's state for debugging
            //! \remark situation logging requires CInterpolationHints::isLoggingInterpolation to be \c true
            //! \remark parts logging has a \c bool \c log flag
            void attachLogger(CInterpolationLogger *logger) { m_logger = logger; }

            //! Get an interpolator info string (for debug info)
            QString getInterpolatorInfo() const;

        protected:
            Aviation::CAircraftSituationList m_aircraftSituations; //!< recent situations for one aircraft
            Aviation::CAircraftPartsList m_aircraftParts;          //!< recent parts for one aircraft
            Aviation::CCallsign m_callsign;                        //!< callsign
            bool m_isFirstInterpolation = true;                               //!< set to false after the first successful interpolation

            //! Constructor
            CInterpolator(const QString &objectName, const Aviation::CCallsign &callsign, QObject *parent);

            //! Set the ground elevation from hints
            static void setGroundElevationFromHint(const CInterpolationHints &hints, Aviation::CAircraftSituation &situation, bool override = true);

            //! Set on ground flag
            static void setGroundFlagFromInterpolator(const CInterpolationHints &hints, double groundFactor, Aviation::CAircraftSituation &situation);

        private:
            CInterpolationLogger *m_logger = nullptr;

            //! Log parts
            void logParts(qint64 timestamp, const Aviation::CAircraftParts &parts, bool empty, bool log);

            Derived *derived() { return static_cast<Derived *>(this); }
            const Derived *derived() const { return static_cast<const Derived *>(this); }
        };

        //! Simple interpolator for pitch, bank, heading, groundspeed
        class BLACKMISC_EXPORT CInterpolatorPbh
        {
        public:
            //! Constructor
            //! @{
            CInterpolatorPbh()
            {}
            CInterpolatorPbh(const Aviation::CAircraftSituation &older, const Aviation::CAircraftSituation &newer) :
                m_oldSituation(older), m_newSituation(newer)
            {}
            CInterpolatorPbh(double time, const Aviation::CAircraftSituation &older, const Aviation::CAircraftSituation &newer) :
                m_simulationTimeFraction(time), m_oldSituation(older), m_newSituation(newer)
            {}
            //! @}

            //! Getter
            //! @{
            Aviation::CHeading getHeading() const;
            PhysicalQuantities::CAngle getPitch() const;
            PhysicalQuantities::CAngle getBank() const;
            PhysicalQuantities::CSpeed getGroundSpeed() const;
            Aviation::CAircraftSituation getOldSituation() const { return m_oldSituation; }
            Aviation::CAircraftSituation getNewSituation() const { return m_newSituation; }
            //! @}

            //! Change time fraction
            void setTimeFraction(double tf) { m_simulationTimeFraction = tf; }

        private:
            double m_simulationTimeFraction = 0.0;
            Aviation::CAircraftSituation m_oldSituation;
            Aviation::CAircraftSituation m_newSituation;
        };

        //! Status of interpolation
        struct BLACKMISC_EXPORT CInterpolationStatus
        {
        public:
            //! Did interpolation succeed?
            bool isInterpolated() const { return m_isInterpolated; }

            //! Set succeeded
            void setInterpolated(bool interpolated) { m_isInterpolated = interpolated; }

            //! Set succeeded
            void setInterpolatedAndCheckSituation(bool succeeded, const Aviation::CAircraftSituation &situation);

            //! Is the corresponding position valid?
            bool hasValidSituation() const { return m_isValidSituation; }

            //! Valid interpolated situation
            bool hasValidInterpolatedSituation() const;

            //! Is that a valid position?
            void checkIfValidSituation(const Aviation::CAircraftSituation &situation);

            //! Reset to default values
            void reset();

            //! Info string
            QString toQString() const;

        private:
            bool m_isInterpolated = false;   //!< position is interpolated (means enough values, etc.)
            bool m_isValidSituation = false; //!< is valid situation
        };

        //! Status regarding parts
        struct BLACKMISC_EXPORT CPartsStatus
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

            //! Ctor
            CPartsStatus() {}

            //! Ctor
            CPartsStatus(bool supportsParts) : m_supportsParts(supportsParts) {}

        private:
            bool m_supportsParts = false; //!< supports parts for given callsign
        };

        //! \cond PRIVATE
        extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE CInterpolator<CInterpolatorLinear>;
        extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE CInterpolator<CInterpolatorSpline>;
        //! \endcond
    } // namespace
} // namespace
#endif // guard
