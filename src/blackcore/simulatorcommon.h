/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_SIMULATOR_COMMON_H
#define BLACKCORE_SIMULATOR_COMMON_H

#include <QObject>
#include <QTimer>
#include <QtGlobal>

#include "blackcore/aircraftmatcher.h"
#include "blackcore/blackcoreexport.h"
#include "blackcore/simulator.h"
#include "blackmisc/interpolationrenderingsetup.h"
#include "blackmisc/aviation/callsignset.h"
#include "blackmisc/connectionguard.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/pq/time.h"
#include "blackmisc/pq/units.h"
#include "blackmisc/simulation/aircraftmodelsetloader.h"
#include "blackmisc/simulation/ownaircraftprovider.h"
#include "blackmisc/simulation/remoteaircraftprovider.h"
#include "blackmisc/simulation/simulatedaircraftlist.h"
#include "blackmisc/simulation/simulatorinfo.h"
#include "blackmisc/simulation/simulatorplugininfo.h"
#include "blackmisc/simulation/simulatorinternals.h"
#include "blackmisc/weather/weathergridprovider.h"

namespace BlackMisc
{
    class IInterpolator;

    namespace Aviation
    {
        class CAircraftParts;
        class CAircraftSituation;
        class CCallsign;
    }
    namespace Simulation
    {
        class CAirspaceAircraftSnapshot;
        class CSimulatedAircraft;
    }
}

namespace BlackCore
{
    //! Common base class with providers, interface and some base functionality
    class BLACKCORE_EXPORT CSimulatorCommon :
        public BlackCore::ISimulator,
        public BlackMisc::Simulation::COwnAircraftAware,    // gain access to in memory own aircraft data
        public BlackMisc::Simulation::CRemoteAircraftAware, // gain access to in memory remote aircraft data
        public BlackMisc::Weather::CWeatherGridAware        // gain access to in memory weather grid
    {
        Q_OBJECT

    public:
        //! Log categories
        static const BlackMisc::CLogCategoryList &getLogCategories();

        //! Destructor
        virtual ~CSimulatorCommon();

        //! \name ISimulator implementations
        //! @{
        virtual BlackMisc::Simulation::CAircraftModel getDefaultModel() const override;
        virtual void setInterpolationAndRenderingSetup(const BlackMisc::CInterpolationAndRenderingSetup &setup) override;
        virtual BlackMisc::CInterpolationAndRenderingSetup getInterpolationAndRenderingSetup() const override;
        virtual void highlightAircraft(const BlackMisc::Simulation::CSimulatedAircraft &aircraftToHighlight, bool enableHighlight, const BlackMisc::PhysicalQuantities::CTime &displayTime) override;
        virtual const BlackMisc::Simulation::CSimulatorPluginInfo &getSimulatorPluginInfo() const override;
        virtual const BlackMisc::Simulation::CSimulatorInternals &getSimulatorInternals() const override;
        virtual void unload() override;
        virtual int physicallyRemoveMultipleRemoteAircraft(const BlackMisc::Aviation::CCallsignSet &callsigns) override;
        //! @}

    protected slots:
        //! \name Connected with remote aircraft provider signals
        //! @{
        //! Slow timer used to highlight aircraft, can be used for other things too
        virtual void ps_oneSecondTimer();

        //! Recalculate the rendered aircraft, this happens when restrictions are applied (max. aircraft, range)
        virtual void ps_recalculateRenderedAircraft(const BlackMisc::Simulation::CAirspaceAircraftSnapshot &snapshot);

        //! Provider added situation
        virtual void ps_remoteProviderAddAircraftSituation(const BlackMisc::Aviation::CAircraftSituation &situation);

        //! Provider added parts
        virtual void ps_remoteProviderAddAircraftParts(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Aviation::CAircraftParts &parts);

        //! Provider removed aircraft
        virtual void ps_remoteProviderRemovedAircraft(const BlackMisc::Aviation::CCallsign &callsign);
        //! @}

    protected:
        //! Constructor
        CSimulatorCommon(const BlackMisc::Simulation::CSimulatorPluginInfo &info,
                         BlackMisc::Simulation::IOwnAircraftProvider *ownAircraftProvider,
                         BlackMisc::Simulation::IRemoteAircraftProvider *remoteAircraftProvider,
                         BlackMisc::Weather::IWeatherGridProvider *weatherGridProvider,
                         QObject *parent);

        //! \copydoc ISimulator::logicallyAddRemoteAircraft
        virtual bool logicallyAddRemoteAircraft(const BlackMisc::Simulation::CSimulatedAircraft &remoteAircraft) override;

        //! \copydoc ISimulator::logicallyRemoveRemoteAircraft
        virtual bool logicallyRemoveRemoteAircraft(const BlackMisc::Aviation::CCallsign &callsign) override;

        //! Reset state
        virtual void reset();

        //! Clear all aircraft related data
        virtual void clearAllAircraft();

        //! Blink the highlighted aircraft
        void blinkHighlightedAircraft();

        //! Restore aircraft from the provider data
        void resetAircraftFromProvider(const BlackMisc::Aviation::CCallsign &callsign);

        //! Override situation from current interpolator values, if any!
        bool setInitialAircraftSituation(BlackMisc::Simulation::CSimulatedAircraft &aircraft) const;

        //! Set own model
        void reverseLookupAndUpdateOwnAircraftModel(const BlackMisc::Simulation::CAircraftModel &model);

        //! Set own model
        void reverseLookupAndUpdateOwnAircraftModel(const QString &modelString);

        BlackMisc::IInterpolator *m_interpolator = nullptr;      //!< interpolator instance
        bool m_pausedSimFreezesInterpolation = false;            //!< paused simulator will also pause interpolation (so AI aircraft will hold)
        BlackMisc::Simulation::CAircraftModel m_defaultModel;    //!< default model
        qint64 m_statsUpdateAircraftTimeTotalMs = 0;             //!< statistics update time
        qint64 m_statsUpdateAircraftTimeAvgMs = 0;               //!< statistics update time
        int    m_statsUpdateAircraftCountMs = 0;                 //!< statistics update time
        BlackMisc::Simulation::CSimulatorInternals m_simulatorInternals;          //!< setup object
        BlackMisc::CInterpolationAndRenderingSetup m_interpolationRenderingSetup; //!< debug messages, rendering etc.

        // some optional functionality which can be used by the sims as needed
        BlackMisc::Simulation::CSimulatedAircraftList m_aircraftToAddAgainWhenRemoved; //!< add this model again when removed, normally used to change model

        //! Lookup against DB data
        static BlackMisc::Simulation::CAircraftModel reverseLookupModel(const BlackMisc::Simulation::CAircraftModel &model);

    private slots:
        //! All swift data read from DB
        void ps_allSwiftDataRead();

    private:
        bool m_blinkCycle = false;             //!< use for highlighting
        qint64 m_highlightEndTimeMsEpoch = 0;  //!< end highlighting
        int m_timerCounter = 0;                //!< allows to calculate n seconds
        QTimer                                        m_oneSecondTimer {this};              //!< multi purpose timer
        BlackMisc::Simulation::CSimulatorPluginInfo   m_simulatorPluginInfo;                //!< info object
        BlackMisc::Simulation::CSimulatedAircraftList m_highlightedAircraft;                //!< all other aircraft are to be ignored
        BlackMisc::Aviation::CCallsignSet             m_callsignsToBeRendered;              //!< callsigns which will be rendered
        BlackMisc::CConnectionGuard                   m_remoteAircraftProviderConnections;  //!< connected signal/slots
    };
} // namespace

#endif // guard
