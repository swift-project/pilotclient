/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of Swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_CONTEXTNETWORK_IMPL_H
#define BLACKCORE_CONTEXTNETWORK_IMPL_H

#include "blackcore/context_network.h"
#include "blackcore/context_settings.h"
#include "blackcore/context_runtime.h"
#include "blackmisc/simulation/remoteaircraftprovider.h"
#include "blackcore/dbus_server.h"
#include "blackcore/network.h"
#include "blackcore/airspace_monitor.h"
#include "blackmisc/aviation/atcstationlist.h"
#include "blackmisc/setnetwork.h"
#include "blackmisc/nwclientlist.h"
#include "blackmisc/digestsignal.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/aviation/aircraftsituationlist.h"

#include <QMap>
#include <QTimer>

namespace BlackCore
{
    class CVatsimBookingReader;
    class CVatsimDataFileReader;

    //! Network context implementation
    class CContextNetwork :
        public IContextNetwork,
        public BlackMisc::Simulation::IRemoteAircraftProvider
    {
        Q_OBJECT
        Q_INTERFACES(BlackMisc::Simulation::IRemoteAircraftProvider)
        Q_CLASSINFO("D-Bus Interface", BLACKCORE_CONTEXTNETWORK_INTERFACENAME)

        friend class IContextNetwork;
        friend class CRuntime;

    public:
        //! Destructor
        virtual ~CContextNetwork();

        //! \copydoc IRemoteAircraftProviderReadOnly::renderedAircraft
        virtual const BlackMisc::Simulation::CSimulatedAircraftList &remoteAircraft() const override;

        //! \copydoc IRenderedAircraftProvider::renderedAircraft
        virtual BlackMisc::Simulation::CSimulatedAircraftList &remoteAircraft() override;

        //! \copydoc IRemoteAircraftProviderReadOnly::renderedAircraftSituations
        virtual const BlackMisc::Aviation::CAircraftSituationList &remoteAircraftSituations() const override;

        //! \copydoc IRemoteAircraftProviderReadOnly::renderedAircraftSituations
        virtual BlackMisc::Aviation::CAircraftSituationList &remoteAircraftSituations() override;

        //! \copydoc IRemoteAircraftProviderReadOnly::renderedAircraftParts
        virtual const BlackMisc::Aviation::CAircraftPartsList &remoteAircraftParts() const override;

        //! \copydoc IRenderedAircraftProvider::renderedAircraftParts
        virtual BlackMisc::Aviation::CAircraftPartsList &remoteAircraftParts() override;

        //! \copydoc IRemoteAircraftProviderReadOnly::connectSignals
        virtual bool connectRemoteAircraftProviderSignals(
            std::function<void(const BlackMisc::Aviation::CAircraftSituation &)> situationSlot,
            std::function<void(const BlackMisc::Aviation::CAircraftParts &)> partsSlot,
            std::function<void(const BlackMisc::Aviation::CCallsign &)> removedAircraftSlot
        ) override;

    signals:
        //! \copydoc IRemoteAircraftProviderReadOnly::addedRemoteAircraftSituation
        void addedRemoteAircraftSituation(const BlackMisc::Aviation::CAircraftSituation &situation);

        //! \copydoc IRemoteAircraftProviderReadOnly::addedRemoteAircraftPart
        void addedRemoteAircraftParts(const BlackMisc::Aviation::CAircraftParts &parts);

        //! \copydoc IRemoteAircraftProviderReadOnly::removedAircraft
        //! \sa IContextNetwork::removedAircraft() which is the equivalent when using IContextNetwork
        void removedRemoteAircraft(const BlackMisc::Aviation::CCallsign &callsign);

    public slots:
        //! \copydoc IContextNetwork::readAtcBookingsFromSource()
        virtual void readAtcBookingsFromSource() const override;

        //! \copydoc IContextNetwork::getAtcStationsOnline()
        virtual BlackMisc::Aviation::CAtcStationList getAtcStationsOnline() const override;

        //! \copydoc IContextNetwork::getAtcStationsBooked()
        virtual BlackMisc::Aviation::CAtcStationList getAtcStationsBooked() const override;

        //! \copydoc IContextNetwork::getAircraftInRange()
        virtual BlackMisc::Simulation::CSimulatedAircraftList getAircraftInRange() const override;

        //! \copydoc IContextNetwork::getAircraftForCallsign
        virtual BlackMisc::Simulation::CSimulatedAircraft getAircraftForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const override;

        //! \copydoc IContextNetwork::getOnlineStationForCallsign
        virtual BlackMisc::Aviation::CAtcStation getOnlineStationForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const override;

        //! \copydoc IContextNetwork::connectToNetwork()
        virtual BlackMisc::CStatusMessage connectToNetwork(const BlackMisc::Network::CServer &server, uint mode) override;

        //! \copydoc IContextNetwork::disconnectFromNetwork()
        virtual BlackMisc::CStatusMessage disconnectFromNetwork() override;

        //! \copydoc IContextNetwork::isConnected()
        virtual bool isConnected() const override;

        /*!
         * In transition state, e.g. connecting, disconnecting.
         * \details In such a state it is advisable to wait until an end state (connected/disconnected) is reached
         * \remarks Intentionally only running locally, not in interface
         */
        bool isPendingConnection() const;

        //! \addtogroup commandline
        //! @{
        //! <pre>
        //! .m  .msg   message text
        //! </pre>
        //! @}
        //! \copydoc IContextNetwork::parseCommandLine
        virtual bool parseCommandLine(const QString &commandLine, const QString &originator) override;

        //! \copydoc IContextNetwork::sendTextMessages()
        virtual void sendTextMessages(const BlackMisc::Network::CTextMessageList &textMessages) override;

        //! \copydoc IContextNetwork::sendFlightPlan()
        virtual void sendFlightPlan(const BlackMisc::Aviation::CFlightPlan &flightPlan) override;

        //! \copydoc IContextNetwork::loadFlightPlanFromNetwork()
        virtual BlackMisc::Aviation::CFlightPlan loadFlightPlanFromNetwork(const BlackMisc::Aviation::CCallsign &callsign) const override;

        //! \copydoc IContextNetwork::getMetar
        virtual BlackMisc::Aviation::CInformationMessage getMetar(const BlackMisc::Aviation::CAirportIcao &airportIcaoCode) override;

        //! \copydoc IContextNetwork::getSelectedVoiceRooms()
        virtual BlackMisc::Audio::CVoiceRoomList getSelectedVoiceRooms() const override;

        //! \copydoc IContextNetwork::getSelectedAtcStations
        virtual BlackMisc::Aviation::CAtcStationList getSelectedAtcStations() const override;

        //! \copydoc IContextNetwork::getUsers()
        virtual BlackMisc::Network::CUserList getUsers() const override;

        //! \copydoc IContextNetwork::getUsersForCallsigns
        virtual BlackMisc::Network::CUserList getUsersForCallsigns(const BlackMisc::Aviation::CCallsignList &callsigns) const override;

        //! \copydoc IContextNetwork::getUsersForCallsign
        virtual BlackMisc::Network::CUser getUserForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const override;

        //! \copydoc IContextNetwork::getOtherClients
        virtual BlackMisc::Network::CClientList getOtherClients() const override;

        //! \copydoc IContextNetwork::getOtherClientForCallsigns
        virtual BlackMisc::Network::CClientList getOtherClientsForCallsigns(const BlackMisc::Aviation::CCallsignList &callsigns) const override;

        //! \copydoc IContextNetwork::getVatsimFsdServers
        virtual BlackMisc::Network::CServerList getVatsimFsdServers() const override;

        //! \copydoc IContextNetwork::getVatsimVoiceServers
        virtual BlackMisc::Network::CServerList getVatsimVoiceServers() const override;

        //! \copydoc IContextNetwork::requestDataUpdates
        virtual void requestDataUpdates()override;

        //! \copydoc IContextNetwork::requestAtisUpdates
        virtual void requestAtisUpdates() override;

        //! \copydoc IContextNetwork::updateAircraftEnabled
        virtual bool updateAircraftEnabled(const BlackMisc::Aviation::CCallsign &callsign, bool enabledForRedering, const QString &originator) override;

        //! \copydoc IContextNetwork::updateAircraftModel
        virtual bool updateAircraftModel(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::Simulation::CAircraftModel &model, const QString &originator) override;

        //! \copydoc IContextNetwork::updateFastPositionUpdates
        virtual bool updateFastPositionUpdates(const BlackMisc::Aviation::CCallsign &callsign, bool enableFastPositonUpdates, const QString &originator) override;

        //! \copydoc IContextNetwork::isFastPositionSendingEnabled
        virtual bool isFastPositionSendingEnabled() const override;

        //! \copydoc IContextNetwork::enableFastPositionSending
        virtual void enableFastPositionSending(bool enable) override;

        //! \copydoc IContextNetwork::setFastPositionEnabledCallsigns
        virtual void setFastPositionEnabledCallsigns(BlackMisc::Aviation::CCallsignList &callsigns) override;

        //! \copydoc IContextNetwork::getFastPositionEnabledCallsigns
        virtual BlackMisc::Aviation::CCallsignList getFastPositionEnabledCallsigns() override;

        //! \copydoc IContextNetwork::testCreateDummyOnlineAtcStations
        virtual void testCreateDummyOnlineAtcStations(int number) override;

        //! \copydoc IContextNetwork::testAddAircraftParts
        virtual void testAddAircraftParts(const BlackMisc::Aviation::CAircraftParts &parts, bool incremental) override;

        //! Gracefully shut down, e.g. for thread safety
        void gracefulShutdown();

    protected:
        //! Constructor, with link to runtime
        CContextNetwork(CRuntimeConfig::ContextMode, CRuntime *runtime);

        //! Register myself in DBus
        CContextNetwork *registerWithDBus(CDBusServer *server)
        {
            if (!server || this->m_mode != CRuntimeConfig::LocalInDbusServer) return this;
            server->addObject(IContextNetwork::ObjectPath(), this);
            return this;
        }

    private:
        CAirspaceMonitor    *m_airspace = nullptr;
        BlackCore::INetwork *m_network  = nullptr;
        INetwork::ConnectionStatus m_currentStatus = INetwork::Disconnected; //!< used to detect pending connections

        // Digest signals, only sending after some time
        BlackMisc::CDigestSignal m_dsAtcStationsBookedChanged { this, &IContextNetwork::changedAtcStationsBooked, &IContextNetwork::changedAtcStationsBookedDigest, 750, 2 };
        BlackMisc::CDigestSignal m_dsAtcStationsOnlineChanged { this, &IContextNetwork::changedAtcStationsOnline, &IContextNetwork::changedAtcStationsOnlineDigest, 750, 4 };
        BlackMisc::CDigestSignal m_dsAircraftsInRangeChanged  { this, &IContextNetwork::changedAircraftInRange, &IContextNetwork::changedAircraftInRangeDigest, 750, 4 };

        // for reading XML and VATSIM data files
        CVatsimBookingReader  *m_vatsimBookingReader  = nullptr;
        CVatsimDataFileReader *m_vatsimDataFileReader = nullptr;
        QTimer *m_dataUpdateTimer = nullptr; //!< general updates such as ATIS, frequencies, see requestDataUpdates()

        //! Get network settings
        BlackMisc::Settings::CSettingsNetwork getNetworkSettings() const
        {
            Q_ASSERT(this->getRuntime());
            Q_ASSERT(this->getRuntime()->getIContextSettings());
            return this->getRuntime()->getIContextSettings()->getNetworkSettings();
        }

        //! Own aircraft from \sa CContextOwnAircraft
        const BlackMisc::Aviation::CAircraft &ownAircraft() const;

    private slots:
        //! ATC bookings received
        void ps_receivedBookings(const BlackMisc::Aviation::CAtcStationList &bookedStations);

        //! Data file has been read
        void ps_dataFileRead();

        //! Check if a supervisor message was received
        void ps_checkForSupervisiorTextMessage(const BlackMisc::Network::CTextMessageList &messages);

        /*!
         * \brief Connection status changed?
         * \param from  old status
         * \param to    new status
         */
        void ps_fsdConnectionStatusChanged(INetwork::ConnectionStatus from, INetwork::ConnectionStatus to);

    };
}

#endif // guard
