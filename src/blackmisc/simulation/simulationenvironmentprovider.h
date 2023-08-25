// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_SIMULATION_SIMULATIONENVIRONMENTPROVIDER_H
#define BLACKMISC_SIMULATION_SIMULATIONENVIRONMENTPROVIDER_H

#include "blackmisc/simulation/simulatorplugininfo.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/simulation/settings/simulatorsettings.h"
#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/aviation/percallsign.h"
#include "blackmisc/geo/coordinategeodeticlist.h"
#include "blackmisc/geo/elevationplane.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/provider.h"

#include <QHash>
#include <QObject>
#include <QPair>

namespace BlackMisc::Simulation
{
    //! Direct in memory access to elevation data
    //! \remark we are interested in elevations at airports mostly
    class BLACKMISC_EXPORT ISimulationEnvironmentProvider : public IProvider
    {
    public:
        //! All remembered coordinates
        //! \threadsafe
        Geo::CCoordinateGeodeticList getAllElevationCoordinates() const;

        //! All remembered coordinates
        //! \threadsafe
        Geo::CCoordinateGeodeticList getElevationCoordinatesOnGround() const;

        //! Average elevation of "on ground" cached values
        //! \threadsafe
        Geo::CElevationPlane averageElevationOfOnGroundAircraft(const Aviation::CAircraftSituation &reference, const PhysicalQuantities::CLength &range, int minValues, int sufficientValues) const;

        //! Highest elevation
        //! \threadsafe
        Aviation::CAltitude highestElevation() const;

        //! Find closest elevation (or return NULL)
        //! \threadsafe
        Geo::CElevationPlane findClosestElevationWithinRange(const Geo::ICoordinateGeodetic &reference, const PhysicalQuantities::CLength &range) const;

        //! Find closest elevation or request elevation
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

        //! All CGs per callsign
        //! \threadsafe
        Aviation::CLengthPerCallsign getSimulatorCGsPerCallsign() const;

        //! All CGs per modelstring
        //! \threadsafe
        QHash<QString, PhysicalQuantities::CLength> getSimulatorCGsPerModelString() const;

        //! Get CG per callsign, NULL if not found
        //! \threadsafe
        PhysicalQuantities::CLength getSimulatorCG(const Aviation::CCallsign &callsign) const;

        //! Get CG per callsign, NULL if not found
        //! \threadsafe
        PhysicalQuantities::CLength getSimulatorOrDbCG(const Aviation::CCallsign &callsign, const PhysicalQuantities::CLength &dbCG) const;

        //! Get CG per model string, NULL if not found
        //! \threadsafe
        PhysicalQuantities::CLength getSimulatorCGPerModelString(const QString &modelString) const;

        //! Get CG per model string, NULL if not found
        //! \threadsafe
        PhysicalQuantities::CLength getSimulatorOrDbCGPerModelString(const QString &modelString, const PhysicalQuantities::CLength &dbCG) const;

        //! Return the overridden CG value or the given default CG value
        //! \threadsafe
        PhysicalQuantities::CLength overriddenCGorDefault(const PhysicalQuantities::CLength &defaultCG, const QString &modelString) const;

        //! Has a CG?
        //! \threadsafe
        bool hasSimulatorCG(const Aviation::CCallsign &callsign) const;

        //! Has the same CG?
        //! \threadsafe
        bool hasSameSimulatorCG(const PhysicalQuantities::CLength &cg, const Aviation::CCallsign &callsign) const;

        //! Set number of elevations kept
        //! \threadsafe
        int setMaxElevationsRemembered(int max);

        //! Get number of max. number of elevations
        //! \threadsafe
        int getMaxElevationsRemembered() const;

        //! Reset statistics
        //! \threadsafe
        void resetSimulationEnvironmentStatistics();

        //! Remove cached elevations outside range,
        //! "forced" cleans always, otherwise only if max.values are reached
        //! \threadsafe
        bool cleanElevationValues(const Aviation::CAircraftSituation &reference, const PhysicalQuantities::CLength &keptRange, bool forced = false);

        //! Remove cached elevations inside range,
        //! \remark this removes inside range, cleanElevationValues outside the range
        //! \threadsafe
        int removeElevationValues(const Aviation::CAircraftSituation &reference, const PhysicalQuantities::CLength &removeRange);

    protected:
        //! Ctor
        ISimulationEnvironmentProvider(const CSimulatorPluginInfo &pluginInfo);

        //! Ctor
        ISimulationEnvironmentProvider(const CSimulatorPluginInfo &pluginInfo, const Settings::CSimulatorSettings &settings, bool supportElevation, bool supportCG);

        //! @{
        //! Provider enabled
        //! \threadsafe
        bool isCgProviderEnabled() const;
        bool isElevationProviderEnabled() const;
        void setCgProviderEnabled(bool enabled);
        void setElevationProviderEnabled(bool enabled);
        void setSimulationProviderEnabled(bool elvEnabled, bool cgEnabled);
        //! @}

        //! All remembered coordiantes plus max.remembered ground situations
        //! \threadsafe
        Geo::CCoordinateGeodeticList getAllElevationCoordinates(int &maxRemembered) const;

        //! New plugin info and default model
        //! \remark normally only used by emulated driver
        //! \threadsafe
        void setNewPluginInfo(const CSimulatorPluginInfo &info, const Settings::CSimulatorSettings &settings, const CAircraftModel &defaultModel);

        //! New plugin info and default model
        //! \threadsafe
        void setNewPluginInfo(const CSimulatorPluginInfo &info, const Settings::CSimulatorSettings &settings);

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
        bool rememberGroundElevation(const Aviation::CCallsign &requestedForCallsign, bool likelyOnGroundElevation, const Geo::ICoordinateGeodetic &elevationCoordinate, const PhysicalQuantities::CLength &epsilon = Geo::CElevationPlane::singlePointRadius());

        //! Remember a given elevation
        //! \threadsafe
        bool rememberGroundElevation(const Aviation::CCallsign &requestedForCallsign, bool likelyOnGroundElevation, const Geo::CElevationPlane &elevationPlane);

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
        bool insertCGOverridden(const PhysicalQuantities::CLength &cg, const Aviation::CCallsign &cs);

        //! Insert or replace an overridden CG
        //! \remark passing a NULL value will remove the CG
        //! \threadsafe
        bool insertCGOverridden(const PhysicalQuantities::CLength &cg, const Aviation::CCallsignSet &callsigns);

        //! Insert or replace a CG
        //! \remark passing a NULL value will remove the CG
        //! \threadsafe
        bool insertCGForModelString(const PhysicalQuantities::CLength &cg, const QString &modelString);

        //! Insert or replace a CG (overridden mode)
        //! \remark passing a NULL value will remove the CG
        //! \threadsafe
        bool insertCGForModelStringOverridden(const PhysicalQuantities::CLength &cg, const QString &modelString);

        //! Clear all overridden values
        //! \threadsafe
        Aviation::CLengthPerCallsign clearCGOverrides();

        //! Remove a CG for a given callsign
        //! \threadsafe
        int removeSimulatorCG(const Aviation::CCallsign &cs);

        //! Remove pending timestamp
        //! \threadsafe
        //! \remark useful if we ignore the probe response on sim. side
        void removePendingElevationRequest(const Aviation::CCallsign &cs);

        //! Min.range considered as single point
        static PhysicalQuantities::CLength minRange(const PhysicalQuantities::CLength &range);

    private:
        CSimulatorPluginInfo m_simulatorPluginInfo; //!< info object
        Settings::CSimulatorSettings m_settings; //!< simulator settings
        QString m_simulatorName; //!< name of simulator
        QString m_simulatorDetails; //!< describes version etc.
        QString m_simulatorVersion; //!< simulator version
        CAircraftModel m_defaultModel; //!< default model

        // idea: the elevations on gnd are likely taxiways and runways, so we keep those
        int m_maxElevations = 100; //!< How many elevations we keep
        int m_maxElevationsGnd = 400; //!< How many elevations we keep for elevations on gnd.
        Geo::CCoordinateGeodeticList m_elvCoordinates; //!< elevation cache
        Geo::CCoordinateGeodeticList m_elvCoordinatesGnd; //!< elevation cache for on ground situations

        Aviation::CTimestampPerCallsign m_pendingElevationRequests; //!< pending elevation requests for aircraft callsign
        Aviation::CLengthPerCallsign m_cgsPerCallsign; //!< CGs per callsign
        Aviation::CLengthPerCallsign m_cgsPerCallsignOverridden; //!< CGs per callsign overridden (manually forced)
        QHash<QString, PhysicalQuantities::CLength> m_cgsPerModel; //!< CGs per model string
        QHash<QString, PhysicalQuantities::CLength> m_cgsPerModelOverridden; //!< CGs per model string (manually forced)
        qint64 m_statsMaxElevRequestTimeMs = -1;
        qint64 m_statsCurrentElevRequestTimeMs = -1;

        bool m_enableElevation = true;
        bool m_enableCG = true;

        mutable int m_elvFound = 0; //!< statistics only
        mutable int m_elvMissed = 0; //!< statistics only

        mutable QReadWriteLock m_lockElvCoordinates { QReadWriteLock::Recursive }; //!< lock m_coordinates, m_pendingElevationRequests
        mutable QReadWriteLock m_lockCG { QReadWriteLock::Recursive }; //!< lock CGs
        mutable QReadWriteLock m_lockModel { QReadWriteLock::Recursive }; //!< lock models
        mutable QReadWriteLock m_lockSimInfo { QReadWriteLock::Recursive }; //!< lock plugin info
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

        //! \copydoc ISimulationEnvironmentProvider::averageElevationOfOnGroundAircraft
        Geo::CElevationPlane averageElevationOfOnGroundAircraft(const Aviation::CAircraftSituation &reference, const PhysicalQuantities::CLength &range, int minValues, int sufficientValues) const;

        //! \copydoc ISimulationEnvironmentProvider::highestElevation
        Aviation::CAltitude highestElevation() const;

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

        //! \copydoc ISimulationEnvironmentProvider::getSimulatorCG
        PhysicalQuantities::CLength getSimulatorCG(const Aviation::CCallsign &callsign) const;

        //! \copydoc ISimulationEnvironmentProvider::getSimulatorOrDbCG
        PhysicalQuantities::CLength getSimulatorOrDbCG(const Aviation::CCallsign &callsign, const PhysicalQuantities::CLength &dbCG) const;

        //! \copydoc ISimulationEnvironmentProvider::hasSimulatorCG
        bool hasSimulatorCG(const Aviation::CCallsign &callsign) const;

        //! \copydoc ISimulationEnvironmentProvider::cleanElevationValues
        bool cleanElevationValues(const Aviation::CAircraftSituation &reference, const PhysicalQuantities::CLength &range, bool forced = false);

    protected:
        //! Default constructor
        CSimulationEnvironmentAware() {}

        //! Constructor
        CSimulationEnvironmentAware(ISimulationEnvironmentProvider *simEnvProvider) : IProviderAware(simEnvProvider) { Q_ASSERT(simEnvProvider); }
    };
} // namespace

Q_DECLARE_INTERFACE(BlackMisc::Simulation::ISimulationEnvironmentProvider, "org.swift-project.blackmisc::simulation::isimulationenvironmentprovider")

#endif // guard
