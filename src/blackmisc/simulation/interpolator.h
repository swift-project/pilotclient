// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_SIMULATION_INTERPOLATOR_H
#define BLACKMISC_SIMULATION_INTERPOLATOR_H

#include "blackmisc/simulation/interpolationresult.h"
#include "blackmisc/simulation/interpolationstatus.h"
#include "blackmisc/simulation/partsstatus.h"
#include "blackmisc/simulation/interpolationrenderingsetup.h"
#include "blackmisc/simulation/remoteaircraftprovider.h"
#include "blackmisc/simulation/interpolationsetupprovider.h"
#include "blackmisc/simulation/simulationenvironmentprovider.h"
#include "blackmisc/simulation/interpolationlogger.h"
#include "blackmisc/simulation/interpolant.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/aviation/aircraftsituationchange.h"
#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/aviation/aircraftpartslist.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/logcategories.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/blackmiscexport.h"

#include <QString>
#include <QStringList>
#include <QtGlobal>
#include <QTimer>

namespace BlackMisc::Simulation
{
    class CInterpolationLogger;
    class CInterpolatorLinear;
    class CInterpolatorSpline;

    //! Base class for interpolating (calculate positions inbetween updates).
    //! One instance is responsible for one aircraft
    //! This class provides the high level functions for interpolation (called from the simulator plugin), logging functionality, as well as the logic to interpolate aircraft parts.
    //! Information for the position interpolation (basically aircraft updates from FSD) are provided from this class.
    class BLACKMISC_EXPORT CInterpolator :
        protected CSimulationEnvironmentAware,
        protected CInterpolationSetupAware,
        protected CRemoteAircraftAware
    {
    public:
        //! Log categories
        static const QStringList &getLogCategories();

        //! Latest interpolation result
        const Aviation::CAircraftSituation &getLastInterpolatedSituation() const { return m_lastSituation; }

        //! Get interpolated situation
        //! \param currentTimeSinceEpoch milliseconds since epoch for which the situation should be interpolated
        //! \param setup interpolation setup
        //! \param aircraftNumber number used to spread the computational load of part interpolation
        //! \return interpolation result
        CInterpolationResult getInterpolation(qint64 currentTimeSinceEpoch, const CInterpolationAndRenderingSetupPerCallsign &setup, uint32_t aircraftNumber);

        //! Attach an observer to read the interpolator's state for debugging
        //! \remark parts logging has a \c bool \c log flag
        void attachLogger(CInterpolationLogger *logger) { m_logger = logger; }

        //! Is logger attached?
        bool hasAttachedLogger() const { return m_logger; }

        //! Get an interpolator info string (for debug info)
        QString getInterpolatorInfo() const;

        //! Reset last interpolation to null
        //! \remark mainly needed in UNIT tests
        void resetLastInterpolation();

        //! Init, or re-init the corressponding model
        //! \remark either by passing a model or using the provider
        void initCorrespondingModel(const CAircraftModel &model = {});

        //! Mark as unit test
        void markAsUnitTest();

        //! Get count of invalid situations
        int getInvalidSituationsCount() const { return m_invalidSituations; }

        //! Interpolation messages
        const CStatusMessageList &getInterpolationMessages() const { return m_interpolationMessages; }

        //! Do we have interpolation messages
        bool hasInterpolationMessages() const { return !m_interpolationMessages.isEmpty(); }

    protected:
        //! Constructor
        CInterpolator(const Aviation::CCallsign &callsign,
                      ISimulationEnvironmentProvider *simEnvProvider, IInterpolationSetupProvider *setupProvider, IRemoteAircraftProvider *remoteProvider,
                      CInterpolationLogger *logger);

        //! Center of gravity
        const PhysicalQuantities::CLength &getModelCG() const { return m_model.getCG(); }

        //! Do logging
        bool doLogging() const;

        const Aviation::CCallsign m_callsign; //!< corresponding callsign
        CAircraftModel m_model; //!< corresponding model (required for CG)

        // values for current interpolation step
        qint64 m_currentTimeMsSinceEpoch = -1; //!< current time
        qint64 m_lastInvalidLogTs = -1; //!< last invalid situation timestamp
        Aviation::CAircraftSituationList m_currentSituations; //!< current situations obtained by remoteAircraftSituationsAndChange
        Aviation::CAircraftSituationChange m_pastSituationsChange; //!< situations change of provider (i.e. network) situations
        CInterpolationAndRenderingSetupPerCallsign m_currentSetup; //!< used setup
        CInterpolationStatus m_currentInterpolationStatus; //!< this step's situation status
        CPartsStatus m_currentPartsStatus; //!< this step's parts status
        CPartsStatus m_lastPartsStatus; //!< status for last parts, used when last parts are re-used because of m_partsToSituationInterpolationRatio
        int m_partsToSituationInterpolationRatio = 2; //!< ratio between parts and situation interpolation, 1..always, 2..every 2nd situation
        int m_partsToSituationGuessingRatio = 5; //!< ratio between parts guessing and situation interpolation
        int m_invalidSituations = 0; //!< mainly when there are no new situations
        CStatusMessageList m_interpolationMessages; //!< interpolation messages

        Aviation::CAircraftSituation m_lastSituation { Aviation::CAircraftSituation::null() }; //!< latest interpolation
        Aviation::CAircraftParts m_lastParts { Aviation::CAircraftParts::null() }; //!< latest parts
        PhysicalQuantities::CLength m_currentSceneryOffset { PhysicalQuantities::CLength::null() }; //!< calculated scenery offset if any

        qint64 m_situationsLastModified { -1 }; //!< when situations were last modified
        qint64 m_situationsLastModifiedUsed { -1 }; //!< interpolant based on situations last updated
        int m_interpolatedSituationsCounter { 0 }; //!< counter for each interpolated situations: used for statistics, every n-th interpolation ....

    private:
        CInterpolationLogger *m_logger = nullptr; //!< optional interpolation logger
        QTimer m_initTimer; //!< timer to init model, will be deleted when interpolator is deleted and cancel the call
        bool m_unitTest = false; //!< mark as unit test

        //! Inits all data members for this current interpolation step
        //! \param currentTimeSinceEpoch milliseconds since epoch for which the situation should be interpolated
        //! \param setup interpolation setup
        //! \param aircraftNumber passing the aircraft number allows to equally distribute among the steps and not to do it always together for all aircraft
        bool initIniterpolationStepData(qint64 currentTimeSinceEpoch, const CInterpolationAndRenderingSetupPerCallsign &setup, int aircraftNumber);

        //! Init the interpolated situation
        Aviation::CAircraftSituation initInterpolatedSituation(const Aviation::CAircraftSituation &oldSituation, const Aviation::CAircraftSituation &newSituation) const;

        //! Current interpolated situation
        Aviation::CAircraftSituation getInterpolatedSituation();

        //! Parts before given offset time
        Aviation::CAircraftParts getInterpolatedParts();

        //! Interpolated parts, if not available guessed parts
        Aviation::CAircraftParts getInterpolatedOrGuessedParts(int aircraftNumber);

        //! Guessed parts
        static Aviation::CAircraftParts guessParts(const Aviation::CAircraftSituation &situation, const Aviation::CAircraftSituationChange &change, const Simulation::CAircraftModel &model);

        //! Log parts
        void logParts(const Aviation::CAircraftParts &parts, int partsNo, bool empty) const;

        //! Get situations and calculate change, also correct altitudes if applicable
        //! \remark calculates offset (scenery) and situations change
        Aviation::CAircraftSituationList remoteAircraftSituationsAndChange(const CInterpolationAndRenderingSetupPerCallsign &setup);

        //! Center of gravity, fetched from provider in case needed
        PhysicalQuantities::CLength getAndFetchModelCG(const PhysicalQuantities::CLength &dbCG);

        //! Preset the ground elevation based on info we already have, either by transfer or elevation
        //! \remark either sets a gnd. elevation or sets it to null
        //! \remark situationToPreset position is unknown
        //! \remark situationToPreset needs to be between oldSituation and newSituation
        //! \sa CAircraftSituation::transferGroundElevation
        static bool presetGroundElevation(Aviation::CAircraftSituation &situationToPreset, const Aviation::CAircraftSituation &oldSituation, const Aviation::CAircraftSituation &newSituation, const Aviation::CAircraftSituationChange &change);

        //! Deferred init
        void deferredInit();

        //! Return NULL parts and log
        const BlackMisc::Aviation::CAircraftParts &logAndReturnNullParts(const QString &info, bool log);

        //! Get the interpolant for the given time point
        virtual const IInterpolant &getInterpolant(SituationLog &log) = 0;
    };
} // namespace
// namespace
#endif // guard
