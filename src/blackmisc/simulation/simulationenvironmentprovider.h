/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_SIMULATIONENVIRONMENTPROVIDER_H
#define BLACKMISC_SIMULATION_SIMULATIONENVIRONMENTPROVIDER_H

#include "simulatorplugininfo.h"
#include "aircraftmodel.h"
#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/aviation/percallsign.h"
#include "blackmisc/geo/coordinategeodeticlist.h"
#include "blackmisc/geo/elevationplane.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/provider.h"

#include <QHash>
#include <QObject>
#include <QPair>

namespace BlackMisc
{
    namespace Simulation
    {
        //! Direct in memory access to elevation data
        //! \remark we are interested in elevations at airports mostly
        class BLACKMISC_EXPORT ISimulationEnvironmentProvider : public IProvider
        {
        public:
            //! All remembered coordiantes
            //! \threadsafe
            Geo::CCoordinateGeodeticList getElevationCoordinates() const;

            //! Find closest elevation
            //! \threadsafe
            Geo::CElevationPlane findClosestElevationWithinRange(const Geo::ICoordinateGeodetic &reference, const PhysicalQuantities::CLength &range) const;

            //! Find closest elevation
            //! \threadsafe
            Geo::CElevationPlane findClosestElevationWithinRangeOrRequest(const Geo::ICoordinateGeodetic &reference, const PhysicalQuantities::CLength &range, const Aviation::CCallsign &callsign);

            //! Request elevation, there is no guarantee the requested elevation will be available in the provider
            //! \threadsafe
            virtual bool requestElevation(const Geo::ICoordinateGeodetic &reference, const Aviation::CCallsign &callsign) = 0;

            //! Request elevation, there is no guarantee the requested elevation will be available in the provider
            //! \threadsafe
            bool requestElevationBySituation(const BlackMisc::Aviation::CAircraftSituation &situation);

            //! Elevations found/missed statistics
            //! \threadsafe
            QPair<int, int> getElevationsFoundMissed() const;

            //! The elevation request times
            //! \threadsafe
            QPair<qint64, qint64> getElevationRequestTimes() const;

            //! Elevations found/missed statistics info as string
            //! \threadsafe
            QString getElevationsFoundMissedInfo() const;

            //! Elevation request times
            //! \threadsafe
            QString getElevationRequestTimesInfo() const;

            //! Get the represented plugin
            //! \threadsafe
            CSimulatorPluginInfo getSimulatorPluginInfo() const;

            //! Get the represented simulator
            //! \threadsafe
            CSimulatorInfo getSimulatorInfo() const;

            //! Version and simulator details info
            //! \threadsafe
            QString getSimulatorNameAndVersion() const;

            //! Default model
            //! \threadsafe
            CAircraftModel getDefaultModel() const;

            //! Get CG per callsign, NULL if not found
            //! \threadsafe
            PhysicalQuantities::CLength getCG(const Aviation::CCallsign &callsign) const;

            //! Get CG per model string, NULL if not found
            //! \threadsafe
            PhysicalQuantities::CLength getCGPerModelString(const QString &modelString) const;

            //! Has a CG?
            //! \threadsafe
            bool hasCG(const Aviation::CCallsign &callsign) const;

            //! Has the same CG?
            //! \threadsafe
            bool hasSameCG(const PhysicalQuantities::CLength &cg, const Aviation::CCallsign &callsign) const;

            //! Set number of elevations kept
            //! \threadsafe
            int setMaxElevationsRemembered(int max);

            //! Get number of max. number of elevations
            //! \threadsafe
            int getMaxElevationsRemembered() const;

            //! Reset statistics
            //! \threadsafe
            void resetSimulationEnvironmentStatistics();

        protected:
            //! Ctor
            ISimulationEnvironmentProvider(const CSimulatorPluginInfo &pluginInfo);

            //! Ctor
            ISimulationEnvironmentProvider(const CSimulatorPluginInfo &pluginInfo, bool supportElevation, bool supportCG);

            //! Provider enabled
            //! \threadsafe
            //! @{
            bool isCgProviderEnabled() const;
            bool isElevationProviderEnabled() const;
            void setCgProviderEnabled(bool enabled);
            void setElevationProviderEnabled(bool enabled);
            void setSimulationProviderEnabled(bool elvEnabled, bool cgEnabled);
            //! @}

            //! All remembered coordiantes plus max.remembered situations
            //! \threadsafe
            Geo::CCoordinateGeodeticList getElevationCoordinates(int &maxRemembered) const;

            //! New plugin info and default model
            //! \remark normally only used by emulated driver
            //! \threadsafe
            void setNewPluginInfo(const CSimulatorPluginInfo &info, const CAircraftModel &defaultModel);

            //! Set version and simulator details from running simulator
            //! \threadsafe
            void setSimulatorDetails(const QString &name, const QString &details, const QString &version);

            //! Simulator name as set from the running simulator
            //! \threadsafe
            //! \remark something like "Name: 'Lockheed Martin® Prepar3D® v4"
            QString getSimulatorName() const;

            //! Simulator version as set from the running simulator
            //! \threadsafe
            //! \remark something like "4.1.7.22841"
            QString getSimulatorVersion() const;

            //! Simulator details as set from the running simulator
            //! \threadsafe
            //! \remark something like "Name: 'Lockheed Martin® Prepar3D® v4' Version: 4.1.7.22841 SimConnect: 4.1.0.0"
            QString getSimulatorDetails() const;

            //! Default model
            //! \threadsafe
            void setDefaultModel(const CAircraftModel &defaultModel);

            //! Clear default model
            //! \threadsafe
            void clearDefaultModel();

            //! Clear elevations
            void clearElevations();

            //! Clear CGs
            //! \threadsafe
            void clearCGs();

            //! Clear data
            //! \threadsafe
            void clearSimulationEnvironmentData();

            //! Only keep closest ones
            //! \threadsafe
            int cleanUpElevations(const Geo::ICoordinateGeodetic &referenceCoordinate, int maxNumber = -1);

            //! Remember a given elevation
            //! \threadsafe
            bool rememberGroundElevation(const Aviation::CCallsign &requestedForCallsign, const Geo::ICoordinateGeodetic &elevationCoordinate, const PhysicalQuantities::CLength &epsilon = Geo::CElevationPlane::singlePointRadius());

            //! Remember a given elevation
            //! \threadsafe
            bool rememberGroundElevation(const Aviation::CCallsign &requestedForCallsign, const Geo::CElevationPlane &elevationPlane) ;

            //! Insert or replace a CG
            //! \remark passing a NULL value will remove the CG
            //! \threadsafe
            bool insertCG(const PhysicalQuantities::CLength &cg, const Aviation::CCallsign &cs);

            //! Insert or replace a CG
            //! \remark passing a NULL value will remove the CG
            //! \threadsafe
            bool insertCG(const PhysicalQuantities::CLength &cg, const QString &modelString, const Aviation::CCallsign &cs);

            //! Insert or replace a CG
            //! \remark passing a NULL value will remove the CG
            //! \threadsafe
            bool insertCGForModelString(const PhysicalQuantities::CLength &cg, const QString &modelString);

            //! Remove a CG
            //! \threadsafe
            int removeCG(const Aviation::CCallsign &cs);

            //! Min.range considered as single point
            static PhysicalQuantities::CLength minRange(const PhysicalQuantities::CLength &range);

        private:
            CSimulatorPluginInfo m_simulatorPluginInfo; //!< info object
            QString m_simulatorName;       //!< name of simulator
            QString m_simulatorDetails;    //!< describes version etc.
            QString m_simulatorVersion;    //!< Simulator version
            CAircraftModel m_defaultModel; //!< default model
            int m_maxElevations = 100;     //!< How many elevations we keep
            Geo::CCoordinateGeodeticList    m_elvCoordinates; //!< elevation cache
            Aviation::CTimestampPerCallsign m_pendingElevationRequests; //!< pending elevation requests for aircraft callsign
            Aviation::CLengthPerCallsign    m_cgsPerCallsign;           //!< CGs per callsign
            QHash<QString, PhysicalQuantities::CLength> m_cgsPerModel;  //!< CGs per model string
            qint64 m_statsMaxElevRequestTimeMs     = -1;
            qint64 m_statsCurrentElevRequestTimeMs = -1;

            bool m_enableElevation = true;
            bool m_enableCG = true;

            mutable int m_elvFound  = 0;   //!< statistics only
            mutable int m_elvMissed = 0;   //!< statistics only

            mutable QReadWriteLock m_lockElvCoordinates; //!< lock m_coordinates, m_pendingElevationRequests
            mutable QReadWriteLock m_lockCG;      //!< lock CGs
            mutable QReadWriteLock m_lockModel;   //!< lock models
            mutable QReadWriteLock m_lockSimInfo; //!< lock plugin info
        };

        //! Class which can be directly used to access an \sa ISimulationEnvironmentProvider object
        class BLACKMISC_EXPORT CSimulationEnvironmentAware : public IProviderAware<ISimulationEnvironmentProvider>
        {
            virtual void anchor();

        public:
            //! Set the provider
            void setSimulationEnvironmentProvider(ISimulationEnvironmentProvider *provider) { this->setProvider(provider); }

            //! \copydoc ISimulationEnvironmentProvider::findClosestElevationWithinRange
            Geo::CElevationPlane findClosestElevationWithinRange(const Geo::ICoordinateGeodetic &reference, const PhysicalQuantities::CLength &range) const;

            //! \copydoc ISimulationEnvironmentProvider::findClosestElevationWithinRangeOrRequest
            Geo::CElevationPlane findClosestElevationWithinRangeOrRequest(const Geo::ICoordinateGeodetic &reference, const PhysicalQuantities::CLength &range, const Aviation::CCallsign &callsign);

            //! \copydoc ISimulationEnvironmentProvider::requestElevation
            bool requestElevation(const Geo::ICoordinateGeodetic &reference, const Aviation::CCallsign &callsign);

            //! \copydoc ISimulationEnvironmentProvider::requestElevation
            bool requestElevation(const Aviation::CAircraftSituation &situation);

            //! \copydoc ISimulationEnvironmentProvider::getElevationsFoundMissed
            QPair<int, int> getElevationsFoundMissed() const;

            //! \copydoc ISimulationEnvironmentProvider::getElevationsFoundMissedInfo
            QString getElevationsFoundMissedInfo() const;

            //! \copydoc ISimulationEnvironmentProvider::getElevationRequestTimes
            QPair<qint64, qint64> getElevationRequestTimes() const;

            //! \copydoc ISimulationEnvironmentProvider::getElevationRequestTimesInfo
            QString getElevationRequestTimesInfo() const;

            //! \copydoc ISimulationEnvironmentProvider::getSimulatorPluginInfo
            CSimulatorPluginInfo getSimulatorPluginInfo() const;

            //! \copydoc ISimulationEnvironmentProvider::getSimulatorPluginInfo
            CSimulatorInfo getSimulatorInfo() const;

            //! \copydoc ISimulationEnvironmentProvider::getSimulatorNameAndVersion
            QString getSimulatorNameAndVersion() const;

            //! \copydoc ISimulationEnvironmentProvider::getDefaultModel
            CAircraftModel getDefaultModel() const;

            //! \copydoc ISimulationEnvironmentProvider::getCG
            PhysicalQuantities::CLength getCG(const Aviation::CCallsign &callsign) const;

            //! \copydoc ISimulationEnvironmentProvider::hasCG
            bool hasCG(const Aviation::CCallsign &callsign) const;

        protected:
            //! Default constructor
            CSimulationEnvironmentAware() {}

            //! Constructor
            CSimulationEnvironmentAware(ISimulationEnvironmentProvider *simEnvProvider) : IProviderAware(simEnvProvider) { Q_ASSERT(simEnvProvider); }
        };
    } // namespace
} // namespace

Q_DECLARE_INTERFACE(BlackMisc::Simulation::ISimulationEnvironmentProvider, "org.swift-project.blackmisc::simulation::isimulationenvironmentprovider")

#endif // guard
