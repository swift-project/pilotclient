/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_CONTEXTOWNAIRCRAFT_IMPL_H
#define BLACKCORE_CONTEXTOWNAIRCRAFT_IMPL_H

#include "blackcoreexport.h"
#include "blackcore/context_ownaircraft.h"
#include "blackcore/context_runtime.h"
#include "blackcore/dbus_server.h"
#include "blackcore/settings/network.h"
#include "blackmisc/aviation/atcstation.h"
#include "blackmisc/simulation/ownaircraftprovider.h"
#include "blackmisc/identifiable.h"

namespace BlackCore
{

    //! Own aircraft context implementation.
    //! Central instance of data for \sa IOwnAircraftProvider .
    class BLACKCORE_EXPORT CContextOwnAircraft :
        public IContextOwnAircraft,
        public BlackMisc::Simulation::IOwnAircraftProvider,
        public BlackMisc::CIdentifiable
    {
        Q_OBJECT
        Q_CLASSINFO("D-Bus Interface", BLACKCORE_CONTEXTOWNAIRCRAFT_INTERFACENAME)
        Q_INTERFACES(BlackMisc::Simulation::IOwnAircraftProvider)
        friend class CRuntime;
        friend class IContextOwnAircraft;

    public:
        //! Destructor
        virtual ~CContextOwnAircraft();

        //! \copydoc IOwnAircraftProvider::getOwnAircraftPosition
        //! \ingroup ownaircraftprovider
        virtual BlackMisc::Geo::CCoordinateGeodetic getOwnAircraftPosition() const override;

        //! \copydoc IOwnAircraftProvider::getOwnAircraftParts
        //! \ingroup ownaircraftprovider
        virtual BlackMisc::Aviation::CAircraftParts getOwnAircraftParts() const;

        //! \copydoc IOwnAircraftProvider::getOwnAircraftModel
        //! \ingroup ownaircraftprovider
        virtual BlackMisc::Simulation::CAircraftModel getOwnAircraftModel() const;

        //! \copydoc IOwnAircraftProvider::getDistanceToOwnAircraft
        //! \ingroup ownaircraftprovider
        virtual BlackMisc::PhysicalQuantities::CLength getDistanceToOwnAircraft(const BlackMisc::Geo::ICoordinateGeodetic &position) const override;

        //! \copydoc IOwnAircraftProvider::updateOwnModel
        //! \ingroup ownaircraftprovider
        virtual bool updateOwnModel(const BlackMisc::Simulation::CAircraftModel &model) override;

        //! \copydoc IOwnAircraftProvider::updateOwnSituation
        //! \ingroup ownaircraftprovider
        virtual bool updateOwnSituation(const BlackMisc::Aviation::CAircraftSituation &situation) override;

        //! \copydoc IOwnAircraftProvider::updateOwnParts
        //! \ingroup ownaircraftprovider
        virtual bool updateOwnParts(const BlackMisc::Aviation::CAircraftParts &parts) override;

    public slots:

        //! \copydoc IContextOwnAircraft::getOwnAircraft()
        //! \ingroup ownaircraftprovider
        virtual BlackMisc::Simulation::CSimulatedAircraft getOwnAircraft() const override;

        //! \copydoc IContextOwnAircraft::updateOwnCallsign
        //! \ingroup ownaircraftprovider
        virtual bool updateOwnCallsign(const BlackMisc::Aviation::CCallsign &callsign) override;

        //! \copydoc IContextOwnAircraft::updateOwnIcaoCodes
        //! \ingroup ownaircraftprovider
        virtual bool updateOwnIcaoCodes(const BlackMisc::Aviation::CAircraftIcaoCode &aircraftIcaoCode, const BlackMisc::Aviation::CAirlineIcaoCode &airlineIcaoCode) override;

        //! \copydoc IContextOwnAircraft::updatePosition
        virtual bool updateOwnPosition(const BlackMisc::Geo::CCoordinateGeodetic &position, const BlackMisc::Aviation::CAltitude &altitude) override;

        //! \copydoc IContextOwnAircraft::updateCockpit
        virtual bool updateCockpit(const BlackMisc::Aviation::CComSystem &com1, const BlackMisc::Aviation::CComSystem &com2, const BlackMisc::Aviation::CTransponder &transponder, const BlackMisc::CIdentifier &originator) override;

        //! \copydoc IContextOwnAircraft::updateSelcal
        virtual bool updateSelcal(const BlackMisc::Aviation::CSelcal &selcal, const BlackMisc::CIdentifier &originator) override;

        //! \copydoc IContextOwnAircraft::updateComFrequency
        virtual bool updateActiveComFrequency(const BlackMisc::PhysicalQuantities::CFrequency &frequency, BlackMisc::Aviation::CComSystem::ComUnit comUnit, const BlackMisc::CIdentifier &originator) override;

        //! \copydoc IContextOwnAircraft::updatePilot
        virtual bool updateOwnAircraftPilot(const BlackMisc::Network::CUser &pilot) override;

        //! \copydoc IContextOwnAircraft::setAudioOutputVolume
        virtual void setAudioOutputVolume(int outputVolume) override;

        //! \copydoc IContextOwnAircraft::setAudioVoiceRoomOverrideUrls
        virtual void setAudioVoiceRoomOverrideUrls(const QString &voiceRoom1Url, const QString &voiceRoom2Url) override;

        //! \copydoc IContextOwnAircraft::enableAutomaticVoiceRoomResolution
        virtual void enableAutomaticVoiceRoomResolution(bool enable) override;

        //! \addtogroup commandline
        //! @{
        //! <pre>
        //! .x    .xpdr  code       set transponder code    CContextOwnAircraft
        //! .com1 .com2  frequency  set COM1/2 frequency    CContextOwnAircraft
        //! .selcal      code       set SELCAL code         CContextOwnAircraft
        //! </pre>
        //! @}
        //! \copydoc IContextOwnAircraft::parseCommandLine
        virtual bool parseCommandLine(const QString &commandLine, const BlackMisc::CIdentifier &originator) override;

    protected:
        //! Constructor, with link to runtime
        CContextOwnAircraft(CRuntimeConfig::ContextMode, CRuntime *runtime);

        //! Register myself in DBus
        CContextOwnAircraft *registerWithDBus(CDBusServer *server)
        {
            if (!server || this->m_mode != CRuntimeConfig::LocalInDbusServer) return this;
            server->addObject(IContextOwnAircraft::ObjectPath(), this);
            return this;
        }

    private slots:
        //! Station has been changed, needed to tune in/out voice room
        //! \note Connected in runtime
        void ps_changedAtcStationOnlineConnectionStatus(const BlackMisc::Aviation::CAtcStation &atcStation, bool connected);

        //! Simulator model has been changed
        //! \note Connected in runtime
        void ps_changedSimulatorModel(const BlackMisc::Simulation::CSimulatedAircraft &ownAircraft);

    private:
        BlackMisc::Simulation::CSimulatedAircraft m_ownAircraft; //!< my aircraft
        bool m_automaticVoiceRoomResolution = true;  //!< automatic voice room resolution, or disable for override
        QString m_voiceRoom1UrlOverride;             //!< overridden voice room url
        QString m_voiceRoom2UrlOverride;             //!< overridden voice room url
        mutable QReadWriteLock m_lockAircraft;       //!< lock aircraft

        CSetting<Settings::Network::CurrentTrafficServer> m_currentNetworkServer { this };

        //! Init my very own aircraft with some defaults, before overridden by simulator
        void initOwnAircraft();

        //! Resolve voice rooms
        void resolveVoiceRooms();
    };
}

#endif // guard
