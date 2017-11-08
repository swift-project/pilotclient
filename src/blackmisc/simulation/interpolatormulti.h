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
            CInterpolatorMulti(const Aviation::CCallsign &callsign, QObject *parent = nullptr);

            //! \copydoc CInterpolator::getInterpolatedSituation
            Aviation::CAircraftSituation getInterpolatedSituation(
                qint64 currentTimeSinceEpoc,
                const CInterpolationAndRenderingSetup &setup,
                const CInterpolationHints &hints, CInterpolationStatus &status);

            //! \copydoc CInterpolator::getInterpolatedParts
            Aviation::CAircraftParts getInterpolatedParts(
                qint64 currentTimeSinceEpoc,
                const CInterpolationAndRenderingSetup &setup,
                CPartsStatus &partsStatus, bool log = false);

            //! \copydoc CInterpolator::addAircraftSituation
            void addAircraftSituation(const Aviation::CAircraftSituation &situation);

            //! \copydoc CInterpolator::hasAircraftSituations
            bool hasAircraftSituations() const;

            //! \copydoc CInterpolator::addAircraftParts
            void addAircraftParts(const Aviation::CAircraftParts &parts);

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

            //! Info string
            QString getInterpolatorInfo() const;

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

        /**
         * CInterpolatorMulti which can be used with QMap/QHash
         * \remark Use case is emulated driver
         */
        class BLACKMISC_EXPORT CInterpolatorMultiWrapper
        {
        public:
            //! Default ctor, needed for QMap/QHash
            CInterpolatorMultiWrapper();

            //! Constructor
            CInterpolatorMultiWrapper(const Aviation::CCallsign &callsign, QObject *parent = nullptr);

            //! Constructor
            CInterpolatorMultiWrapper(const Aviation::CCallsign &callsign, CInterpolationLogger *logger, QObject *parent = nullptr);

            //! Has interpolator initialized?
            bool hasInterpolator() const { return m_interpolator; }

            //! Interpolator
            CInterpolatorMulti *interpolator() const { return m_interpolator.data(); }

            //! Allows implicit conversion
            operator CInterpolatorMulti *() const { return this->interpolator(); }

            //! Bool conversion
            operator bool() const { return this->hasInterpolator(); }

            //! Bool conversion
            bool operator !() const { return !this->hasInterpolator(); }

            //! * operator
            CInterpolatorMulti &operator *() const { return *this->interpolator(); }

            //! -> operator
            CInterpolatorMulti *operator ->() const { return this->interpolator(); }

        private:
            QSharedPointer<CInterpolatorMulti> m_interpolator; //!< shared pointer because CInterpolatorMultiWrapper can be copied
        };
    } // ns
} // ns

#endif
