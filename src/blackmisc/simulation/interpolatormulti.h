/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_INTERPOLATORMULTI_H
#define BLACKMISC_SIMULATION_INTERPOLATORMULTI_H

#include "blackmisc/simulation/interpolatorlinear.h"
#include "blackmisc/simulation/interpolatorspline.h"
#include "blackmisc/simulation/interpolatordummy.h"

namespace BlackMisc
{
    namespace Simulation
    {
        /*!
         * Multiplexed interpolator which allows switching between modes at runtime.
         * \remark currently switching mode is only a developer feature, see https://swift-project.slack.com/archives/C04J6J76N/p1504536854000049
         */
        class BLACKMISC_EXPORT CInterpolatorMulti : public QObject
        {
        public:
            //! Constructor
            CInterpolatorMulti(const BlackMisc::Aviation::CCallsign &callsign, QObject *parent = nullptr);

            //! \copydoc CInterpolator::getInterpolatedSituation
            BlackMisc::Aviation::CAircraftSituation getInterpolatedSituation(
                qint64 currentTimeSinceEpoc, const CInterpolationAndRenderingSetup &setup,
                const CInterpolationHints &hints, CInterpolationStatus &status);

            //! \copydoc CInterpolator::getInterpolatedParts
            BlackMisc::Aviation::CAircraftParts getInterpolatedParts(
                qint64 currentTimeSinceEpoc, const CInterpolationAndRenderingSetup &setup,
                CPartsStatus &partsStatus, bool log = false);

            //! \copydoc CInterpolator::addAircraftSituation
            void addAircraftSituation(const BlackMisc::Aviation::CAircraftSituation &situation);

            //! \copydoc CInterpolator::hasAircraftSituations
            bool hasAircraftSituations() const;

            //! \copydoc CInterpolator::addAircraftParts
            void addAircraftParts(const BlackMisc::Aviation::CAircraftParts &parts);

            //! \copydoc CInterpolator::hasAircraftParts
            bool hasAircraftParts() const;

            //! \copydoc CInterpolator::attachLogger
            void attachLogger(CInterpolationLogger *logger);

            //! Supported interpolation modes.
            enum Mode
            {
                ModeSpline, //!< spline interpolation mode
                ModeLinear, //!< linear interpolation mode
                ModeUnknown
            };

            //! Set interpolation mode. Return true if mode was changed. Mode will not be changed in release build.
            bool setMode(Mode mode);

            //! Set interpolation mode. Return true if mode was changed. Mode will not be changed in release build.
            bool setMode(const QString &mode);

            //! Get active interpolation mode.
            Mode getMode() const { return m_mode; }

            //! Toogle interpolator Mode
            void toggleMode();

            //! Mode from string
            static Mode modeFromString(const QString &mode);

            //! Mode to string
            static const QString &modeToString(Mode mode);

        private:
            Mode m_mode = ModeSpline;
            CInterpolatorSpline m_spline;
#ifdef QT_DEBUG
            //! only in a dev.environment, otherwise replaced by low footprint dummy driver
            CInterpolatorLinear m_linear;
#else
            CInterpolatorDummy m_linear;
#endif
        };
    }
}

#endif
