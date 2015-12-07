/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_WEB_DATASERVICES_H
#define BLACKCORE_WEB_DATASERVICES_H

#include "blackcore/blackcoreexport.h"
#include "blackcore/webreaderflags.h"
#include "blackcore/data/globalsetup.h"
#include "blackmisc/aviation/atcstationlist.h"
#include "blackmisc/aviation/liverylist.h"
#include "blackmisc/aviation/airlineicaocodelist.h"
#include "blackmisc/aviation/aircrafticaocodelist.h"
#include "blackmisc/network/serverlist.h"
#include "blackmisc/network/voicecapabilities.h"
#include "blackmisc/network/webdataservicesprovider.h"
#include "blackmisc/network/entityflags.h"
#include "blackmisc/simulation/distributorlist.h"
#include "blackmisc/weather/metarset.h"
#include "blackmisc/logcategorylist.h"
#include "blackmisc/countrylist.h"
#include "blackmisc/project.h"
#include <QObject>

namespace BlackCore
{
    class CVatsimBookingReader;
    class CVatsimDataFileReader;
    class CVatsimMetarReader;
    class CIcaoDataReader;
    class CModelDataReader;
    class CDatabaseWriter;

    /*!
     * Encapsulates reading data from web sources
     */
    class BLACKCORE_EXPORT CWebDataServices :
        public QObject,
        public BlackMisc::Network::IWebDataServicesProvider
    {
        Q_OBJECT
        Q_INTERFACES(BlackMisc::Network::IWebDataServicesProvider)

    public:
        //! Constructor
        CWebDataServices(CWebReaderFlags::WebReader readerFlags,
                         int autoReadAfterSetupSynchronizedMs, QObject *parent = nullptr);

        //! Shutdown
        void gracefulShutdown();

        //! Read ATC bookings (used to re-read)
        void readAtcBookingsInBackground() const;

        //! Booking reader
        CVatsimBookingReader *getBookingReader() const { return m_vatsimBookingReader; }

        //! Data file reader
        CVatsimDataFileReader *getDataFileReader() const { return m_vatsimDataFileReader; }

        //! Metar reader
        CVatsimMetarReader *getMetarReader() const { return m_vatsimMetarReader; }

        //! Reader flags
        CWebReaderFlags::WebReader getReaderFlags() const { return m_readerFlags; }

        //! Log categories
        static const BlackMisc::CLogCategoryList &getLogCategories();

        // ------------------------ provider functionality start ------------------------------

        //! \copydoc IWebDataServicesProvider::connectDataReadSignal
        //! \ingroup webdatareaderprovider
        virtual QList<QMetaObject::Connection> connectDataReadSignal(
            QObject *receiver,
            std::function<void (BlackMisc::Network::CEntityFlags::Entity, BlackMisc::Network::CEntityFlags::ReadState, int)> dataRead) override;

        //! \copydoc IWebDataServicesProvider::triggerRead
        //! \ingroup webdatareaderprovider
        virtual BlackMisc::Network::CEntityFlags::Entity triggerRead(BlackMisc::Network::CEntityFlags::Entity whatToRead) override;

        //! \copydoc IWebDataServicesProvider::getVatsimFsdServers
        //! \ingroup webdatareaderprovider
        virtual BlackMisc::Network::CServerList getVatsimFsdServers() const override;

        //! \copydoc IWebDataServicesProvider::getVatsimVoiceServers
        //! \ingroup webdatareaderprovider
        virtual BlackMisc::Network::CServerList getVatsimVoiceServers() const override;

        //! \copydoc IWebDataServicesProvider::getDistributors
        //! \ingroup webdatareaderprovider
        virtual BlackMisc::Simulation::CDistributorList getDistributors() const override;

        //! \copydoc IWebDataServicesProvider::getDistributorsCount
        //! \ingroup webdatareaderprovider
        virtual int getDistributorsCount() const override;

        //! \copydoc IWebDataServicesProvider::smartDistributorSelector
        //! \ingroup webdatareaderprovider
        virtual BlackMisc::Simulation::CDistributor smartDistributorSelector(const BlackMisc::Simulation::CDistributor &distributor) const override;

        //! \copydoc IWebDataServicesProvider::getLiveries
        //! \ingroup webdatareaderprovider
        virtual BlackMisc::Aviation::CLiveryList getLiveries() const override;

        //! \copydoc IWebDataServicesProvider::getLiveriesCount
        //! \ingroup webdatareaderprovider
        virtual int getLiveriesCount() const override;

        //! \copydoc IWebDataServicesProvider::getLiveryForCombinedCode
        //! \ingroup webdatareaderprovider
        virtual BlackMisc::Aviation::CLivery getLiveryForCombinedCode(const QString &combinedCode) const override;

        //! \copydoc IWebDataServicesProvider::getLiveryForDbKey
        //! \ingroup webdatareaderprovider
        virtual BlackMisc::Aviation::CLivery getLiveryForDbKey(int id) const override;

        //! \copydoc IWebDataServicesProvider::smartLiverySelector
        //! \ingroup webdatareaderprovider
        virtual BlackMisc::Aviation::CLivery smartLiverySelector(const BlackMisc::Aviation::CLivery &livery) const override;

        //! \copydoc IWebDataServicesProvider::getModels
        //! \ingroup webdatareaderprovider
        virtual BlackMisc::Simulation::CAircraftModelList getModels() const override;

        //! \copydoc IWebDataServicesProvider::getModelsCount
        //! \ingroup webdatareaderprovider
        virtual int getModelsCount() const override;

        //! \copydoc IWebDataServicesProvider::getModelsForAircraftDesignatorAndLiveryCombinedCode
        //! \ingroup webdatareaderprovider
        virtual BlackMisc::Simulation::CAircraftModelList getModelsForAircraftDesignatorAndLiveryCombinedCode(const QString &aircraftDesignator, const QString &combinedCode) const override;

        //! \copydoc IWebDataServicesProvider::getModelForModelString
        //! \ingroup webdatareaderprovider
        virtual BlackMisc::Simulation::CAircraftModel getModelForModelString(const QString &modelString) const override;

        //! \copydoc IWebDataServicesProvider::getAircraftIcaoCodes
        //! \ingroup webdatareaderprovider
        virtual BlackMisc::Aviation::CAircraftIcaoCodeList getAircraftIcaoCodes() const override;

        //! \copydoc IWebDataServicesProvider::getAircraftIcaoCodesCount
        //! \ingroup webdatareaderprovider
        virtual int getAircraftIcaoCodesCount() const override;

        //! \copydoc IWebDataServicesProvider::getAircraftIcaoCodeForDesignator
        //! \ingroup webdatareaderprovider
        virtual BlackMisc::Aviation::CAircraftIcaoCode getAircraftIcaoCodeForDesignator(const QString &designator) const override;

        //! \copydoc IWebDataServicesProvider::getAircraftIcaoCodeForDbKey
        //! \ingroup webdatareaderprovider
        virtual BlackMisc::Aviation::CAircraftIcaoCode getAircraftIcaoCodeForDbKey(int key) const override;

        //! \copydoc IWebDataServicesProvider::getAircraftIcaoCodeForDbKey
        //! \ingroup webdatareaderprovider
        virtual BlackMisc::Aviation::CAircraftIcaoCode smartAircraftIcaoSelector(const BlackMisc::Aviation::CAircraftIcaoCode &icao) const override;

        //! \copydoc IWebDataServicesProvider::getAirlineIcaoCodes
        //! \ingroup webdatareaderprovider
        virtual BlackMisc::Aviation::CAirlineIcaoCodeList getAirlineIcaoCodes() const override;

        //! \copydoc IWebDataServicesProvider::getAirlineIcaoCodesCount
        //! \ingroup webdatareaderprovider
        virtual int getAirlineIcaoCodesCount() const override;

        //! \copydoc IWebDataServicesProvider::getAirlineIcaoCodeForDbKey
        //! \ingroup webdatareaderprovider
        virtual BlackMisc::Aviation::CAirlineIcaoCode getAirlineIcaoCodeForDbKey(int key) const override;

        //! \copydoc IWebDataServicesProvider::getAirlineIcaoCodeForDesignator
        //! \ingroup webdatareaderprovider
        virtual BlackMisc::Aviation::CAirlineIcaoCode getAirlineIcaoCodeForDesignator(const QString &designator) const override;

        //! \copydoc IWebDataServicesProvider::getCountries
        //! \ingroup webdatareaderprovider
        virtual BlackMisc::CCountryList getCountries() const override;

        //! \copydoc IWebDataServicesProvider::getCountries
        //! \ingroup webdatareaderprovider
        virtual int getCountriesCount() const override;

        //! \copydoc IWebDataServicesProvider::getCountryForName
        //! \ingroup webdatareaderprovider
        virtual BlackMisc::CCountry getCountryForName(const QString &name) const override;

        //! \copydoc IWebDataServicesProvider::getCountryForIsoCode
        //! \ingroup webdatareaderprovider
        virtual BlackMisc::CCountry getCountryForIsoCode(const QString &iso) const override;

        //! \copydoc IWebDataServicesProvider::getMetars
        //! \ingroup webdatareaderprovider
        virtual BlackMisc::Weather::CMetarSet getMetars() const override;

        //! \copydoc IWebDataServicesProvider::getMetarForAirport
        //! \ingroup webdatareaderprovider
        virtual BlackMisc::Weather::CMetar getMetarForAirport(const BlackMisc::Aviation::CAirportIcaoCode &icao) const override;

        //! \copydoc IWebDataServicesProvider::getMetarForAirport
        //! \ingroup webdatareaderprovider
        virtual int getMetarsCount() const override;

        //! \copydoc IWebDataServicesProvider::getUsersForCallsign
        //! \ingroup webdatareaderprovider
        virtual BlackMisc::Network::CUserList getUsersForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const override;

        //! \copydoc IWebDataServicesProvider::getAtcStationsForCallsign
        //! \ingroup webdatareaderprovider
        virtual BlackMisc::Aviation::CAtcStationList getAtcStationsForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const override;

        //! \copydoc IWebDataServicesProvider::getVoiceCapabilityForCallsign
        //! \ingroup webdatareaderprovider
        virtual BlackMisc::Network::CVoiceCapabilities getVoiceCapabilityForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const override;

        //! \copydoc IWebDataServicesProvider::updateWithWebData
        //! \ingroup webdatareaderprovider
        virtual void updateWithVatsimDataFileData(BlackMisc::Simulation::CSimulatedAircraft &aircraftToBeUdpated) const override;

        //! \copydoc IWebDataServicesProvider::asyncWriteModel
        //! \ingroup webdatareaderprovider
        virtual BlackMisc::CStatusMessageList asyncWriteModel(BlackMisc::Simulation::CAircraftModel &model) const override;

        //! \copydoc IWebDataServicesProvider::canConnectSwiftDb
        //! \ingroup webdatareaderprovider
        virtual bool canConnectSwiftDb() const override;

        //! Save all DB data to JSON files
        //! \ingroup webdatareaderprovider
        virtual bool writeDbDataToDisk(const QString &dir) const override;

        //! Load DB data from JSON files
        //! \ingroup webdatareaderprovider
        virtual bool readDbDataFromDisk(const QString &dir, bool inBackground) override;

        // ------------------------ provider functionality end ----------------------------

        // ---------------------------------------------
        // Consider to use the connect method of the provider to connect by entity
        // ---------------------------------------------

    public slots:
        //! First read (allows to immediately read in background)
        void readInBackground(BlackMisc::Network::CEntityFlags::Entity entities = BlackMisc::Network::CEntityFlags::AllEntities, int delayMs = 0);

    private slots:
        //! ATC bookings received
        void ps_receivedBookings(const BlackMisc::Aviation::CAtcStationList &bookedStations);

        //! Received METAR data
        void ps_receivedMetars(const BlackMisc::Weather::CMetarSet &metars);

        //! Data file has been read
        void ps_dataFileRead(int lines);

        //! Read from model reader
        void ps_readFromSwiftDb(BlackMisc::Network::CEntityFlags::Entity entity, BlackMisc::Network::CEntityFlags::ReadState state, int number);

        //! Setup has been read
        void ps_setupRead(bool success);

        //! Setup has been changed
        void ps_setupChanged();

        //! Setup timed out
        void ps_setupTimedOut();

    private:
        //! Init the readers
        void initReaders(CWebReaderFlags::WebReader flags);

        //! Init the writers
        void initWriters();

        CWebReaderFlags::WebReader m_readerFlags = CWebReaderFlags::WebReaderFlag::None; //!< which readers are available
        int m_autoReadAfterSetupMs               = -1;                                   //!< directly read all known readers after setup was syncronized
        bool m_initialRead                       = false;                                //!< Initial read conducted
        BlackCore::CData<BlackCore::Data::GlobalSetup> m_setup {this, &CWebDataServices::ps_setupChanged}; //!< setup cache

        // for reading XML and VATSIM data files
        CVatsimBookingReader  *m_vatsimBookingReader  = nullptr;
        CVatsimDataFileReader *m_vatsimDataFileReader = nullptr;
        CVatsimMetarReader    *m_vatsimMetarReader    = nullptr;
        CIcaoDataReader       *m_icaoDataReader       = nullptr;
        CModelDataReader      *m_modelDataReader      = nullptr;

        // writing objects directly into DB
        CDatabaseWriter       *m_databaseWriter       = nullptr;
    };
} // namespace

#endif
