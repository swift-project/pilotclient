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
#include "blackmisc/aviation/atcstationlist.h"
#include "blackmisc/aviation/liverylist.h"
#include "blackmisc/aviation/airlineicaocodelist.h"
#include "blackmisc/aviation/aircrafticaocodelist.h"
#include "blackmisc/network/serverlist.h"
#include "blackmisc/network/voicecapabilities.h"
#include "blackmisc/network/webdataservicesprovider.h"
#include "blackmisc/simulation/distributorlist.h"
#include "blackmisc/logcategorylist.h"
#include "blackmisc/countrylist.h"
#include <QObject>

namespace BlackCore
{
    class CVatsimBookingReader;
    class CVatsimDataFileReader;
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
        CWebDataServices(CWebReaderFlags::WebReader readerFlags, QObject *parent = nullptr);

        //! Shutdown
        void gracefulShutdown();

        //! Read ATC bookings
        void readAtcBookingsInBackground() const;

        //! Booking reader
        CVatsimBookingReader *getBookingReader() const { return m_vatsimBookingReader; }

        //! Data file reader
        CVatsimDataFileReader *getDataFileReader() const { return m_vatsimDataFileReader; }

        //! Reader flags
        CWebReaderFlags::WebReader getReaderFlags() const { return m_readerFlags; }

        //! Log categories
        static const BlackMisc::CLogCategoryList &getLogCategories();

        // ------------------------ provider functionality ------------------------------

        //! \copydoc IWebDataReaderProvider::connectVatsimDataSignals
        //! \ingroup webdatareaderprovider
        virtual QList<QMetaObject::Connection> connectVatsimDataSignals(
            QObject *receiver,
            std::function<void(int)> bookingsRead, std::function<void(int)> dataFileRead) override;

        //! \copydoc IWebDataReaderProvider::connectSwiftDatabaseSignals
        //! \ingroup webdatareaderprovider
        virtual QList<QMetaObject::Connection> connectSwiftDatabaseSignals(
            QObject *receiver,
            std::function<void (BlackMisc::Network::CDbFlags::Entity, BlackMisc::Network::CDbFlags::ReadState, int)> dataRead) override;

        //! \copydoc IWebDataReaderProvider::connectSwiftDatabaseSignals
        //! \ingroup webdatareaderprovider
        virtual BlackMisc::Network::CDbFlags::Entity triggerRead(BlackMisc::Network::CDbFlags::Entity whatToRead) override;

        //! \copydoc IWebDataReaderProvider::getVatsimFsdServers
        //! \ingroup webdatareaderprovider
        virtual BlackMisc::Network::CServerList getVatsimFsdServers() const override;

        //! \copydoc IWebDataReaderProvider::getVatsimVoiceServers
        //! \ingroup webdatareaderprovider
        virtual BlackMisc::Network::CServerList getVatsimVoiceServers() const override;

        //! \copydoc IWebDataReaderProvider::getDistributors
        //! \ingroup webdatareaderprovider
        virtual BlackMisc::Simulation::CDistributorList getDistributors() const override;

        //! \copydoc IWebDataReaderProvider::getDistributorsCount
        //! \ingroup webdatareaderprovider
        virtual int getDistributorsCount() const override;

        //! \copydoc IWebDataReaderProvider::smartDistributorSelector
        //! \ingroup webdatareaderprovider
        virtual BlackMisc::Simulation::CDistributor smartDistributorSelector(const BlackMisc::Simulation::CDistributor &distributor) const override;

        //! \copydoc IWebDataReaderProvider::getLiveries
        //! \ingroup webdatareaderprovider
        virtual BlackMisc::Aviation::CLiveryList getLiveries() const override;

        //! \copydoc IWebDataReaderProvider::getLiveriesCount
        //! \ingroup webdatareaderprovider
        virtual int getLiveriesCount() const override;

        //! \copydoc IWebDataReaderProvider::getLiveryForCombinedCode
        //! \ingroup webdatareaderprovider
        virtual BlackMisc::Aviation::CLivery getLiveryForCombinedCode(const QString &combinedCode) const override;

        //! \copydoc IWebDataReaderProvider::getLiveryForDbKey
        //! \ingroup webdatareaderprovider
        virtual BlackMisc::Aviation::CLivery getLiveryForDbKey(int id) const override;

        //! \copydoc IWebDataReaderProvider::smartLiverySelector
        //! \ingroup webdatareaderprovider
        virtual BlackMisc::Aviation::CLivery smartLiverySelector(const BlackMisc::Aviation::CLivery &livery) const override;

        //! \copydoc IWebDataReaderProvider::getModels
        //! \ingroup webdatareaderprovider
        virtual BlackMisc::Simulation::CAircraftModelList getModels() const override;

        //! \copydoc IWebDataReaderProvider::getModelsCount
        //! \ingroup webdatareaderprovider
        virtual int getModelsCount() const override;

        //! \copydoc IWebDataReaderProvider::getModelsForAircraftDesignatorAndLiveryCombinedCode
        //! \ingroup webdatareaderprovider
        virtual BlackMisc::Simulation::CAircraftModelList getModelsForAircraftDesignatorAndLiveryCombinedCode(const QString &aircraftDesignator, const QString &combinedCode) const override;

        //! \copydoc IWebDataReaderProvider::getModelForModelString
        //! \ingroup webdatareaderprovider
        virtual BlackMisc::Simulation::CAircraftModel getModelForModelString(const QString &modelString) const override;

        //! \copydoc IWebDataReaderProvider::getAircraftIcaoCodes
        //! \ingroup webdatareaderprovider
        virtual BlackMisc::Aviation::CAircraftIcaoCodeList getAircraftIcaoCodes() const override;

        //! \copydoc IWebDataReaderProvider::getAircraftIcaoCodesCount
        //! \ingroup webdatareaderprovider
        virtual int getAircraftIcaoCodesCount() const override;

        //! \copydoc IWebDataReaderProvider::getAircraftIcaoCodeForDesignator
        //! \ingroup webdatareaderprovider
        virtual BlackMisc::Aviation::CAircraftIcaoCode getAircraftIcaoCodeForDesignator(const QString &designator) const override;

        //! \copydoc IWebDataReaderProvider::getAircraftIcaoCodeForDbKey
        //! \ingroup webdatareaderprovider
        virtual BlackMisc::Aviation::CAircraftIcaoCode getAircraftIcaoCodeForDbKey(int key) const override;

        //! \copydoc IWebDataReaderProvider::getAircraftIcaoCodeForDbKey
        //! \ingroup webdatareaderprovider
        virtual BlackMisc::Aviation::CAircraftIcaoCode smartAircraftIcaoSelector(const BlackMisc::Aviation::CAircraftIcaoCode &icao) const override;

        //! \copydoc IWebDataReaderProvider::getAirlineIcaoCodes
        //! \ingroup webdatareaderprovider
        virtual BlackMisc::Aviation::CAirlineIcaoCodeList getAirlineIcaoCodes() const override;

        //! \copydoc IWebDataReaderProvider::getAirlineIcaoCodesCount
        //! \ingroup webdatareaderprovider
        virtual int getAirlineIcaoCodesCount() const override;

        //! \copydoc IWebDataReaderProvider::getAirlineIcaoCodeForDbKey
        //! \ingroup webdatareaderprovider
        virtual BlackMisc::Aviation::CAirlineIcaoCode getAirlineIcaoCodeForDbKey(int key) const override;

        //! \copydoc IWebDataReaderProvider::getAirlineIcaoCodeForDesignator
        //! \ingroup webdatareaderprovider
        virtual BlackMisc::Aviation::CAirlineIcaoCode getAirlineIcaoCodeForDesignator(const QString &designator) const override;

        //! \copydoc IWebDataReaderProvider::getCountries
        //! \ingroup webdatareaderprovider
        virtual BlackMisc::CCountryList getCountries() const override;

        //! \copydoc IWebDataReaderProvider::getCountries
        //! \ingroup webdatareaderprovider
        virtual int getCountriesCount() const override;

        //! \copydoc IWebDataReaderProvider::getCountryForName
        //! \ingroup webdatareaderprovider
        virtual BlackMisc::CCountry getCountryForName(const QString &name) const override;

        //! \copydoc IWebDataReaderProvider::getCountryForIsoCode
        //! \ingroup webdatareaderprovider
        virtual BlackMisc::CCountry getCountryForIsoCode(const QString &iso) const override;

        //! \copydoc IWebDataReaderProvider::getUsersForCallsign
        //! \ingroup webdatareaderprovider
        virtual BlackMisc::Network::CUserList getUsersForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const override;

        //! \copydoc IWebDataReaderProvider::getAtcStationsForCallsign
        //! \ingroup webdatareaderprovider
        virtual BlackMisc::Aviation::CAtcStationList getAtcStationsForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const override;

        //! \copydoc IWebDataReaderProvider::getVoiceCapabilityForCallsign
        //! \ingroup webdatareaderprovider
        virtual BlackMisc::Network::CVoiceCapabilities getVoiceCapabilityForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const override;

        //! \copydoc IWebDataReaderProvider::updateWithWebData
        //! \ingroup webdatareaderprovider
        virtual void updateWithVatsimDataFileData(BlackMisc::Simulation::CSimulatedAircraft &aircraftToBeUdpated) const override;

        //! \copydoc IWebDataReaderProvider::asyncWriteModel
        //! \ingroup webdatareaderprovider
        virtual BlackMisc::CStatusMessageList asyncWriteModel(BlackMisc::Simulation::CAircraftModel &model) const override;

        //! \copydoc IWebDataReaderProvider::canConnectSwiftDb
        //! \ingroup webdatareaderprovider
        virtual bool canConnectSwiftDb() const override;

    public slots:
        //! First read (allows to immediately read in background)
        void readAllInBackground(int delayMs);

    signals:
        //! Data file read
        void vatsimDataFileRead(int lines);

        //! Bookings read
        void vatsimBookingsRead(int number);

        //! Combined read signal
        void readSwiftDbData(BlackMisc::Network::CDbFlags::Entity entity, BlackMisc::Network::CDbFlags::ReadState state, int number);

        //! Model has been written
        void modelWritten(const BlackMisc::Simulation::CAircraftModel &model);

    private slots:
        //! ATC bookings received
        void ps_receivedBookings(const BlackMisc::Aviation::CAtcStationList &bookedStations);

        //! Data file has been read
        void ps_dataFileRead(int lines);

        //! Read from model reader
        void ps_readFromSwiftDb(BlackMisc::Network::CDbFlags::Entity entity, BlackMisc::Network::CDbFlags::ReadState state, int number);

    private:
        //! Init the readers
        void initReaders(CWebReaderFlags::WebReader flags);

        //! Init the writers
        void initWriters();

        CWebReaderFlags::WebReader m_readerFlags = CWebReaderFlags::WebReaderFlags::None; //!< which readers are available

        // for reading XML and VATSIM data files
        CVatsimBookingReader  *m_vatsimBookingReader  = nullptr;
        CVatsimDataFileReader *m_vatsimDataFileReader = nullptr;
        CIcaoDataReader       *m_icaoDataReader       = nullptr;
        CModelDataReader      *m_modelDataReader      = nullptr;

        // writing objects directly into DB
        CDatabaseWriter       *m_databaseWriter       = nullptr;
    };
} // namespace

#endif
