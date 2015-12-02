/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_NETWORK_WEBDATASERVICESPROVIDER_H
#define BLACKMISC_NETWORK_WEBDATASERVICESPROVIDER_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/aviation/aircrafticaocodelist.h"
#include "blackmisc/aviation/airlineicaocodelist.h"
#include "blackmisc/aviation/liverylist.h"
#include "blackmisc/aviation/atcstationlist.h"
#include "blackmisc/network/entityflags.h"
#include "blackmisc/network/serverlist.h"
#include "blackmisc/network/userlist.h"
#include "blackmisc/network/voicecapabilities.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/distributorlist.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/weather/metarset.h"
#include "blackmisc/countrylist.h"
#include "blackmisc/connectionguard.h"

#include <functional>

namespace BlackMisc
{
    namespace Network
    {
        //! Direct thread safe in memory access to objects read via web services
        //! \note Can not be derived from QObject (as for the signals), as this would create multiple
        //!       inheritance. Hence Q_DECLARE_INTERFACE is used.
        //! \ingroup webdatareaderprovider
        class BLACKMISC_EXPORT IWebDataServicesProvider
        {
        public:
            //! Destructor
            virtual ~IWebDataServicesProvider() {}

            //! FSD servers
            //! \threadsafe
            virtual BlackMisc::Network::CServerList getVatsimFsdServers() const = 0;

            //! Voice servers
            //! \threadsafe
            virtual BlackMisc::Network::CServerList getVatsimVoiceServers() const = 0;

            //! Users by callsign
            //! \threadsafe
            virtual BlackMisc::Network::CUserList getUsersForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const = 0;

            //! ATC stations by callsign
            //! \threadsafe
            virtual BlackMisc::Aviation::CAtcStationList getAtcStationsForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const = 0;

            //! Voice capabilities for given callsign
            //! \threadsafe
            virtual BlackMisc::Network::CVoiceCapabilities getVoiceCapabilityForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const = 0;

            //! Update with web data
            //! \threadsafe
            virtual void updateWithVatsimDataFileData(BlackMisc::Simulation::CSimulatedAircraft &aircraftToBeUdpated) const = 0;

            //! Distributors
            //! \threadsafe
            virtual BlackMisc::Simulation::CDistributorList getDistributors() const = 0;

            //! Distributors count
            //! \threadsafe
            virtual int getDistributorsCount() const = 0;

            //! Use distributor object to select the best complete distributor from DB
            //! \threadsafe
            virtual BlackMisc::Simulation::CDistributor smartDistributorSelector(const BlackMisc::Simulation::CDistributor &distributor) const = 0;

            //! Liveries
            //! \threadsafe
            virtual BlackMisc::Aviation::CLiveryList getLiveries() const = 0;

            //! Liveries count
            //! \threadsafe
            virtual int getLiveriesCount() const = 0;

            //! Liveries
            //! \threadsafe
            virtual BlackMisc::Aviation::CLivery getLiveryForCombinedCode(const QString &combinedCode) const = 0;

            //! Livery for id
            //! \threadsafe
            virtual BlackMisc::Aviation::CLivery getLiveryForDbKey(int id) const = 0;

            //! Use a livery as template and select the best complete livery from DB for it
            //! \threadsafe
            virtual BlackMisc::Aviation::CLivery smartLiverySelector(const BlackMisc::Aviation::CLivery &livery) const = 0;

            //! Models
            //! \threadsafe
            virtual BlackMisc::Simulation::CAircraftModelList getModels() const = 0;

            //! Models count
            //! \threadsafe
            virtual int getModelsCount() const = 0;

            //! Models for combined code and aircraft designator
            //! \threadsafe
            virtual BlackMisc::Simulation::CAircraftModelList getModelsForAircraftDesignatorAndLiveryCombinedCode(const QString &aircraftDesignator, const QString &combinedCode) const = 0;

            //! Model for key if any
            //! \threadsafe
            virtual BlackMisc::Simulation::CAircraftModel getModelForModelString(const QString &modelKey) const = 0;

            //! Aircraft ICAO codes
            //! \threadsafe
            virtual BlackMisc::Aviation::CAircraftIcaoCodeList getAircraftIcaoCodes() const = 0;

            //! Aircraft ICAO codes count
            //! \threadsafe
            virtual int getAircraftIcaoCodesCount() const = 0;

            //! ICAO code for designator
            //! \threadsafe
            virtual BlackMisc::Aviation::CAircraftIcaoCode getAircraftIcaoCodeForDesignator(const QString &designator) const = 0;

            //! ICAO code for id
            //! \threadsafe
            virtual BlackMisc::Aviation::CAircraftIcaoCode getAircraftIcaoCodeForDbKey(int id) const = 0;

            //! Use an ICAO object to select the best complete ICAO object from DB for it
            //! \threadsafe
            virtual BlackMisc::Aviation::CAircraftIcaoCode smartAircraftIcaoSelector(const BlackMisc::Aviation::CAircraftIcaoCode &icao) const = 0;

            //! Airline ICAO codes
            //! \threadsafe
            virtual BlackMisc::Aviation::CAirlineIcaoCodeList getAirlineIcaoCodes() const = 0;

            //! Airline ICAO codes count
            //! \threadsafe
            virtual int getAirlineIcaoCodesCount() const = 0;

            //! ICAO code for designator
            //! \threadsafe
            virtual BlackMisc::Aviation::CAirlineIcaoCode getAirlineIcaoCodeForDesignator(const QString &designator) const = 0;

            //! ICAO code for id
            //! \threadsafe
            virtual BlackMisc::Aviation::CAirlineIcaoCode getAirlineIcaoCodeForDbKey(int id) const = 0;

            //! Countries
            //! \threadsafe
            virtual BlackMisc::CCountryList getCountries() const = 0;

            //! Country count
            //! \threadsafe
            virtual int getCountriesCount() const = 0;

            //! Country by ISO code (GB, US...)
            //! \threadsafe
            virtual BlackMisc::CCountry getCountryForIsoCode(const QString &iso) const = 0;

            //! Country by name (France, China ..)
            //! \threadsafe
            virtual BlackMisc::CCountry getCountryForName(const QString &name) const = 0;

            //! Get METARs
            //! \threadsafe
            virtual BlackMisc::Weather::CMetarSet getMetars() const = 0;

            //! Get METAR for airport
            //! \threadsafe
            virtual BlackMisc::Weather::CMetar getMetarForAirport(const BlackMisc::Aviation::CAirportIcaoCode &icao) const = 0;

            //! Get METARs count
            //! \threadsafe
            virtual int getMetarsCount() const = 0;

            //! Write directly to database
            virtual BlackMisc::CStatusMessageList asyncWriteModel(BlackMisc::Simulation::CAircraftModel &model) const = 0;

            //! Relay signals for swift data
            //! Connect signals to slot receiver. As the interface is no QObject, slots can not be connected directly.
            //! In order to disconnect a list of connections is provided, which have to be disconnected manually.
            //! \note receiver is required for connection type
            virtual QList<QMetaObject::Connection> connectDataReadSignal(
                QObject *receiver,
                std::function<void(BlackMisc::Network::CEntityFlags::Entity, BlackMisc::Network::CEntityFlags::ReadState, int)> dataRead) = 0;

            //! Trigger read of new data
            virtual BlackMisc::Network::CEntityFlags::Entity triggerRead(BlackMisc::Network::CEntityFlags::Entity whatToRead) = 0;

            //! Can connect to swift DB?
            virtual bool canConnectSwiftDb() const = 0;

            //! Write data to disk
            virtual bool writeDbDataToDisk(const QString &dir) const = 0;

            //! Load DB data from disk
            virtual bool readDbDataFromDisk(const QString &dir, bool inBackground) = 0;
        };

        //! Class which can be directly used to access an \sa IWebDataReaderProvider object
        class BLACKMISC_EXPORT CWebDataServicesAware
        {
        public:
            //! Destructor
            ~CWebDataServicesAware();

        public:
            //! \copydoc IWebDataReaderProvider::getMetarForAirport
            BlackMisc::Weather::CMetar getMetarForAirport(const BlackMisc::Aviation::CAirportIcaoCode &icao) const;

        protected:
            //! Constructor
            CWebDataServicesAware(IWebDataServicesProvider *webDataReaderProvider = nullptr) : m_webDataReaderProvider(webDataReaderProvider) { }

            //! \copydoc IWebDataReaderProvider::getVatsimFsdServers
            BlackMisc::Network::CServerList getVatsimFsdServers() const;

            //! \copydoc IWebDataReaderProvider::getVatsimVoiceServers
            BlackMisc::Network::CServerList getVatsimVoiceServers() const;

            //! \copydoc IWebDataReaderProvider::getUsersForCallsign
            BlackMisc::Network::CUserList getUsersForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const;

            //! \copydoc IWebDataReaderProvider::getAtcStationsForCallsign
            BlackMisc::Aviation::CAtcStationList getAtcStationsForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const;

            //! \copydoc IWebDataReaderProvider::getVoiceCapabilityForCallsign
            BlackMisc::Network::CVoiceCapabilities getVoiceCapabilityForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const;

            //! \copydoc IWebDataReaderProvider::getDistributors
            BlackMisc::Simulation::CDistributorList getDistributors() const;

            //! \copydoc IWebDataReaderProvider::smartDistributorSelector
            BlackMisc::Simulation::CDistributor smartDistributorSelector(const BlackMisc::Simulation::CDistributor &distributor) const;

            //! \copydoc IWebDataReaderProvider::getDistributorsCount
            int getDistributorsCount() const;

            //! \copydoc IWebDataReaderProvider::getLiveries
            BlackMisc::Aviation::CLiveryList getLiveries() const;

            //! \copydoc IWebDataReaderProvider::getLiveries
            int getLiveriesCount() const;

            //! \copydoc IWebDataReaderProvider::getLiveryForCombinedCode
            BlackMisc::Aviation::CLivery getLiveryForCombinedCode(const QString &combinedCode) const;

            //! \copydoc IWebDataReaderProvider::smartLiverySelector
            BlackMisc::Aviation::CLivery smartLiverySelector(const Aviation::CLivery &livery) const;

            //! \copydoc IWebDataReaderProvider::getModels
            BlackMisc::Simulation::CAircraftModelList getModels() const;

            //! \copydoc IWebDataReaderProvider::getModelsCount
            int getModelsCount() const;

            //! \copydoc IWebDataReaderProvider::getModelsForAircraftDesignatorAndLiveryCombinedCode
            BlackMisc::Simulation::CAircraftModelList getModelsForAircraftDesignatorAndLiveryCombinedCode(const QString &aircraftDesignator, const QString &combinedCode) const;

            //! \copydoc IWebDataReaderProvider::getModelForModelString
            BlackMisc::Simulation::CAircraftModel getModelForModelString(const QString &modelString) const;

            //! \copydoc IWebDataReaderProvider::getAircraftIcaoCodes
            BlackMisc::Aviation::CAircraftIcaoCodeList getAircraftIcaoCodes() const;

            //! \copydoc IWebDataReaderProvider::getAircraftIcaoCodesCount
            int getAircraftIcaoCodesCount() const;

            //! \copydoc IWebDataReaderProvider::getAircraftIcaoCodeForDesignator
            BlackMisc::Aviation::CAircraftIcaoCode getAircraftIcaoCodeForDesignator(const QString &designator) const;

            //! \copydoc IWebDataReaderProvider::getAircraftIcaoCodeForId
            BlackMisc::Aviation::CAircraftIcaoCode getAircraftIcaoCodeForDbKey(int id) const;

            //! \copydoc IWebDataReaderProvider::getAirlineIcaoCodes
            BlackMisc::Aviation::CAirlineIcaoCodeList getAirlineIcaoCodes() const;

            //! \copydoc IWebDataReaderProvider::getAirlineIcaoCodesCount
            int getAirlineIcaoCodesCount() const;

            //! \copydoc IWebDataReaderProvider::getAirlineIcaoCodeForDesignator
            BlackMisc::Aviation::CAirlineIcaoCode getAirlineIcaoCodeForDesignator(const QString &designator) const;

            //! \copydoc IWebDataReaderProvider::getAirlineIcaoCodeForId
            BlackMisc::Aviation::CAirlineIcaoCode getAirlineIcaoCodeForDbKey(int id) const;

            //! \copydoc IWebDataReaderProvider::smartAircraftIcaoSelector
            BlackMisc::Aviation::CAircraftIcaoCode smartAircraftIcaoSelector(const BlackMisc::Aviation::CAircraftIcaoCode &icao) const;

            //! \copydoc IWebDataReaderProvider::getCountries
            BlackMisc::CCountryList getCountries() const;

            //! \copydoc IWebDataReaderProvider::getCountriesCount
            int getCountriesCount() const;

            //! \copydoc IWebDataReaderProvider::getCountryForIsoCode
            BlackMisc::CCountry getCountryForIsoCode(const QString &iso) const;

            //! \copydoc IWebDataReaderProvider::getCountryForName
            BlackMisc::CCountry getCountryForName(const QString &name) const;

            //! \copydoc IWebDataReaderProvider::getMetars
            BlackMisc::Weather::CMetarSet getMetars() const;

            //! \copydoc IWebDataReaderProvider::getMetarCount
            int getMetarCount() const;

            //! \copydoc IWebDataReaderProvider::updateWithWebData
            void updateWithVatsimDataFileData(BlackMisc::Simulation::CSimulatedAircraft &aircraftToBeUdpated) const;

            //! \copydoc IWebDataReaderProvider::asyncWriteModel
            BlackMisc::CStatusMessageList asyncWriteModelToDb(BlackMisc::Simulation::CAircraftModel &model) const;

            //! Set the provider
            virtual void setProvider(IWebDataServicesProvider *webDataReaderProvider);

            //! Is provider avialable?
            bool hasProvider() const;

            //! Called when provider should no longer be used
            void gracefulShutdown();

            //! \copydoc IWebDataReaderProvider::connectDataReadSignal
            void connectDataReadSignal(
                QObject *receiver,
                std::function<void(BlackMisc::Network::CEntityFlags::Entity, BlackMisc::Network::CEntityFlags::ReadState, int)> dataRead);

            //! \copydoc IWebDataReaderProvider::triggerRead
            BlackMisc::Network::CEntityFlags::Entity triggerRead(BlackMisc::Network::CEntityFlags::Entity whatToRead);

            //! \copydoc IWebDataReaderProvider::canConnectSwiftDb
            bool canConnectSwiftDb() const;

            //! \copydoc IWebDataReaderProvider::writeDbDataToDisk
            bool writeDbDataToDisk(const QString &dir) const;

            //! \copydoc IWebDataReaderProvider::readDbDataFromDisk
            bool readDbDataFromDisk(const QString &dir, bool inBackround);

        private:
            IWebDataServicesProvider    *m_webDataReaderProvider = nullptr; //!< access to object
            BlackMisc::CConnectionGuard m_swiftConnections;                 //!< signal connection with swift

            //! Disconnect all signals
            void disconnectSignals();
        };

    } // namespace
} // namespace

Q_DECLARE_INTERFACE(BlackMisc::Network::IWebDataServicesProvider, "BlackMisc::Network::IWebDataServicesProvider")

#endif // guard
