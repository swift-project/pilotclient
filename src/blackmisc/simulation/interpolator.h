/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_INTERPOLATOR_H
#define BLACKMISC_SIMULATION_INTERPOLATOR_H

#include "blackmisc/simulation/interpolationrenderingsetup.h"
#include "blackmisc/simulation/remoteaircraftprovider.h"
#include "blackmisc/simulation/interpolationsetupprovider.h"
#include "blackmisc/simulation/simulationenvironmentprovider.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/aviation/aircraftsituationchange.h"
#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/aviation/aircraftpartslist.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/logcategories.h"
#include "blackmisc/statusmessagelist.h"

#include <QObject>
#include <QString>
#include <QtGlobal>
#include <QTimer>

namespace BlackMisc
{
    namespace Aviation
    {
        class CAircraftSituation;
        class CAircraftSituationChange;
    }
    namespace Simulation
    {
        class CInterpolationLogger;
        class CInterpolatorLinear;
        class CInterpolatorSpline;

        //! Status of interpolation
        struct BLACKMISC_EXPORT CInterpolationStatus
        {
        public:
            //! Did interpolation succeed?
            bool isInterpolated() const { return m_isInterpolated; }

            //! Set succeeded
            void setInterpolated(bool interpolated) { m_isInterpolated = interpolated; }

            //! Interpolating between 2 same situations?
            bool isSameSituation() const { return m_isSameSituation; }

            //! Interpolating between 2 same situations?
            void setSameSituation(bool same) { m_isSameSituation = same; }

            //! Set situations count
            void setSituationsCount(int count) { m_situations = count; }

            //! Extra info
            void setExtraInfo(const QString &info);

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
            bool m_isSameSituation = false;  //!< interpolation between 2 same situations
            int  m_situations = -1;          //!< number of situations used for interpolation
            QString m_extraInfo;             //!< optional details
        };

        //! Status regarding parts
        struct BLACKMISC_EXPORT CPartsStatus
        {
        public:
            //! Ctor
            CPartsStatus() {}

            //! Ctor
            CPartsStatus(bool supportsParts) : m_supportsParts(supportsParts) {}

            //! Supporting parts
            bool isSupportingParts() const { return m_supportsParts; }

            //! Set support flag
            void setSupportsParts(bool supports) { m_supportsParts = supports; }

            //! Is a reused parts object?
            //! \remark means using last value again
            bool isReusedParts() const { return m_resusedParts; }

            //! Mark as reused
            void setReusedParts(bool reused) { m_resusedParts = reused; }

            //! Same parts as last parts?
            bool isSameParts() const { return m_isSameParts; }

            //! Same parts as last parts?
            void setSameParts(bool same) { m_isSameParts = same; }

            //! Reset to default values
            void reset();

            //! Info string
            QString toQString() const;

        private:
            bool m_supportsParts = false; //!< supports parts for given callsign
            bool m_resusedParts  = false; //!< reusing from last step
            bool m_isSameParts   = false; //!< same as last parts?
        };

        //! Combined results
        class BLACKMISC_EXPORT CInterpolationResult
        {
        public:
            //! Ctor
            CInterpolationResult();

            //! Get situation
            const Aviation::CAircraftSituation &getInterpolatedSituation() const { return m_interpolatedSituation; }

            //! Get parts (interpolated or guessed)
            const Aviation::CAircraftParts &getInterpolatedParts() const { return m_interpolatedParts; }

            //! Get status
            const CInterpolationStatus &getInterpolationStatus() const { return m_interpolationStatus; }

            //! Get status
            const CPartsStatus &getPartsStatus() const { return m_partsStatus; }

            //! Set situation
            void setInterpolatedSituation(const Aviation::CAircraftSituation &situation) { m_interpolatedSituation = situation; }

            //! Set parts (interpolated or guessed)
            void setInterpolatedParts(const Aviation::CAircraftParts &parts) { m_interpolatedParts = parts; }

            //! Set values
            void setValues(const Aviation::CAircraftSituation &situation, const Aviation::CAircraftParts &parts);

            //! Set status
            void setInterpolationStatus(const CInterpolationStatus &status) { m_interpolationStatus = status; }

            //! Set status
            void setPartsStatus(const CPartsStatus &status) { m_partsStatus = status; }

            //! Set status values
            void setStatus(const CInterpolationStatus &interpolation, const CPartsStatus &parts);

            //! Reset values
            void reset();

            //! @{
            //! Implicit conversion
            operator const Aviation::CAircraftSituation &() const { return m_interpolatedSituation; }
            operator const Aviation::CAircraftParts &()     const { return m_interpolatedParts; }
            //! @}

        private:
            Aviation::CAircraftSituation m_interpolatedSituation; //!< interpolated situation
            Aviation::CAircraftParts     m_interpolatedParts;     //!< guessed or interpolated parts
            CInterpolationStatus         m_interpolationStatus;   //!< interpolation status
            CPartsStatus                 m_partsStatus;           //!< parts status
        };

        //! Interpolator, calculation inbetween positions
        template <typename Derived>
        class CInterpolator :
            protected CSimulationEnvironmentAware,
            protected CInterpolationSetupAware,
            protected CRemoteAircraftAware
        {
        public:
            //! Log categories
            static const QStringList &getLogCategories();

            //! Latest interpolation result
            const Aviation::CAircraftSituation &getLastInterpolatedSituation() const { return m_lastSituation; }

            //! Parts and situation interpolated
            CInterpolationResult getInterpolation(qint64 currentTimeSinceEpoc, const CInterpolationAndRenderingSetupPerCallsign &setup, int aircraftNumber = -1);

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
            //! \remark parts logging has a \c bool \c log flag
            void attachLogger(CInterpolationLogger *logger) { m_logger = logger; }

            //! Is logger attached?
            bool hasAttachedLogger() const { return m_logger; }

            //! Get an interpolator info string (for debug info)
            QString getInterpolatorInfo() const;

            //! Reset last interpolation to null
            //! \remark mainly needed in UNIT tests
            void resetLastInterpolation();

            //! Clear all data
            //! \remark mainly needed in UNIT tests
            //! \private
            void clear();

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

            //! Inits all data for this current interpolation step
            //! \param currentTimeSinceEpoc
            //! \param setup
            //! \param aircraftNumber passing the aircraft number allows to equally distribute among the steps and not to do it always together for all aircraft
            bool initIniterpolationStepData(qint64 currentTimeSinceEpoc, const CInterpolationAndRenderingSetupPerCallsign &setup, int aircraftNumber);

            //! Init the interpolated situation
            Aviation::CAircraftSituation initInterpolatedSituation(const Aviation::CAircraftSituation &oldSituation, const Aviation::CAircraftSituation &newSituation) const;

            //! Current interpolated situation
            Aviation::CAircraftSituation getInterpolatedSituation();

            //! Parts before given offset time
            Aviation::CAircraftParts getInterpolatedParts();

            //! Interpolated parts, if not available guessed parts
            Aviation::CAircraftParts getInterpolatedOrGuessedParts(int aircraftNumber);

            //! Center of gravity
            const PhysicalQuantities::CLength &getModelCG() const { return m_model.getCG(); }

            //! Do logging
            bool doLogging() const;

            //! Decides threshold when situation is considered on ground
            //! \sa BlackMisc::Aviation::CAircraftSituation::setOnGroundFromGroundFactorFromInterpolation
            static double groundInterpolationFactor();

            const Aviation::CCallsign  m_callsign; //!< corresponding callsign
            CAircraftModel m_model; //!< corresponding model

            // values for current interpolation step
            qint64 m_currentTimeMsSinceEpoch = -1;                      //!< current time
            qint64 m_lastInvalidLogTs = -1;                             //!< last invalid situation timestamp
            Aviation::CAircraftSituationList m_currentSituations;       //!< current situations obtained by remoteAircraftSituationsAndChange
            Aviation::CAircraftSituationChange m_pastSituationsChange;  //!< situations change of provider (i.e. network) situations
            CInterpolationAndRenderingSetupPerCallsign m_currentSetup;  //!< used setup
            CInterpolationStatus m_currentInterpolationStatus;          //!< this step's situation status
            CPartsStatus m_currentPartsStatus;                          //!< this step's parts status
            CPartsStatus m_lastPartsStatus;                             //!< status for last parts, used when last parts are re-used because of m_partsToSituationInterpolationRatio
            int m_partsToSituationInterpolationRatio = 2;               //!< ratio between parts and situation interpolation, 1..always, 2..every 2nd situation
            int m_partsToSituationGuessingRatio = 5;                    //!< ratio between parts guessing and situation interpolation
            int m_invalidSituations = 0;                                //!< mainly when there are no new situations
            CStatusMessageList m_interpolationMessages;                 //!< interpolation messages

            Aviation::CAircraftSituation m_lastSituation { Aviation::CAircraftSituation::null() };      //!< latest interpolation
            Aviation::CAircraftParts m_lastParts { Aviation::CAircraftParts::null() };                  //!< latest parts
            PhysicalQuantities::CLength m_currentSceneryOffset { PhysicalQuantities::CLength::null() }; //!< calculated scenery offset if any

            qint64 m_situationsLastModified     { -1 }; //!< when situations were last modified
            qint64 m_situationsLastModifiedUsed { -1 }; //!< interpolant based on situations last updated
            int m_interpolatedSituationsCounter {  0 }; //!< counter for each interpolated situations: used for statistics, every n-th interpolation ....

            bool m_unitTest = false; //!< mark as unit test

            //! Verify gnd flag, times, ... true means "OK"
            bool verifyInterpolationSituations(const Aviation::CAircraftSituation &oldest, const Aviation::CAircraftSituation &newer, const Aviation::CAircraftSituation &latest,
                                               const CInterpolationAndRenderingSetupPerCallsign &setup = CInterpolationAndRenderingSetupPerCallsign::null());

        private:
            CInterpolationLogger *m_logger = nullptr; //!< optional interpolation logger
            QTimer m_initTimer; //!< timer to init model, will be deleted when interpolator is deleted and cancel the call

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

            //! @{
            //! Derived class
            Derived *derived() { return static_cast<Derived *>(this); }
            const Derived *derived() const { return static_cast<const Derived *>(this); }
            //! @}
        };

        //! \cond PRIVATE
        extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE CInterpolator<CInterpolatorLinear>;
        extern template class BLACKMISC_EXPORT_DECLARE_TEMPLATE CInterpolator<CInterpolatorSpline>;
        //! \endcond
    } // namespace
} // namespace
#endif // guard
