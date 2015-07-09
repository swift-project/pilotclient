/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_WEB_DATAREADER_H
#define BLACKCORE_WEB_DATAREADER_H

#include "blackcore/blackcoreexport.h"
#include "blackmisc/logcategorylist.h"
#include "blackmisc/aviation/atcstationlist.h"
#include "blackmisc/aviation/liverylist.h"
#include "blackmisc/aviation/airlineicaocodelist.h"
#include "blackmisc/aviation/aircrafticaocodelist.h"
#include "blackmisc/network/serverlist.h"
#include "blackmisc/simulation/distributorlist.h"
#include <QObject>

namespace BlackCore
{
    class CVatsimBookingReader;
    class CVatsimDataFileReader;
    class CIcaoDataReader;
    class CModelDataReader;

    /**
     * Encapsulates reading data from web sources
     */
    class BLACKCORE_EXPORT CWebDataReader : public QObject
    {
        Q_OBJECT

    public:
        //! \todo Qt5.5: Make use of QFlags
        enum WebReaderFlags
        {
            None = 0,
            VatsimBookingReader  = 1 << 0,
            VatsimDataReader     = 1 << 1,
            IcaoDataReader       = 1 << 2,
            ModelReader          = 1 << 3,
            AllVatsimReaders     = VatsimBookingReader | VatsimDataReader,
            AllSwiftDbReaders    = IcaoDataReader | ModelReader,
            AllReaders           = 0xFFFF
        };
        Q_DECLARE_FLAGS(WebReader, WebReaderFlags)

        //! Constructor
        CWebDataReader(WebReader readerFlags, QObject *parent = nullptr);

        //! Shutdown
        void gracefulShutdown();

        //! Relay signals for VATSIM data
        QList<QMetaObject::Connection> connectVatsimDataSignals(std::function<void(int)> bookingsRead, std::function<void(int)> dataFileRead);

        //! Relay signals for swift data
        QList<QMetaObject::Connection> connectSwiftDatabaseSignals(
            QObject *receiver,
            std::function<void(int)> aircraftIcaoCodeRead, std::function<void(int)> airlineIcaoCodeRead,
            std::function<void(int)> liveriesRead, std::function<void(int)> distributorsRead,
            std::function<void(int)> modelsRead);

        //! \copydoc CVatsimDataFileReader::getVatsimFsdServers
        BlackMisc::Network::CServerList getVatsimFsdServers() const;

        //! \copydoc CVatsimDataFileReader::getVatsimVoiceServers
        BlackMisc::Network::CServerList getVatsimVoiceServers() const;

        //! \copydoc CModelDataReader::getDistributors
        BlackMisc::Simulation::CDistributorList getDistributors() const;

        //! \copydoc CModelDataReader::getLiveries
        BlackMisc::Aviation::CLiveryList getLiveries() const;

        //! \copydoc CModelDataReader::getModels
        BlackMisc::Simulation::CAircraftModelList getModels() const;

        //! \copydoc CIcaoDataReader::getAircraftIcaoCodes
        BlackMisc::Aviation::CAircraftIcaoCodeList getAircraftIcaoCodes() const;

        //! \copydoc CIcaoDataReader::getAirlineIcaoCodes
        BlackMisc::Aviation::CAirlineIcaoCodeList getAirlineIcaoCodes() const;

        //! Read ATC bookings
        void readAtcBookingsInBackground() const;

        //! Read liveries
        void readModelDataInBackground() const;

        //! Booking reader
        CVatsimBookingReader *getBookingReader() const { return m_vatsimBookingReader; }

        //! Data file reader
        CVatsimDataFileReader *getDataFileReader() const { return m_vatsimDataFileReader; }

        //! Reader flags
        WebReader getReaderFlags() const { return m_readerFlags; }

        //! Log categories
        static const BlackMisc::CLogCategoryList &getLogCategories();

    public slots:
        //! First read (allows to immediately read in background)
        void readAllInBackground(int delayMs);

    signals:
        //! Data file read
        void vatsimDataFileRead(int lines);

        //! Bookings read
        void vatsimBookingsRead(int number);

        //! ICAO codes read
        void aircraftIcaoCodeRead(int number);

        //! ICAO codes read
        void airlineIcaoCodeRead(int number);

        //! Liveries read
        void liveriesRead(int number);

        //! Distributors read
        void distributorsRead(int number);

        //! Models read
        void modelsRead(int number);

    private slots:
        //! ATC bookings received
        void ps_receivedBookings(const BlackMisc::Aviation::CAtcStationList &bookedStations);

        //! Data file has been read
        void ps_dataFileRead(int lines);

        //! Read ICAO codes
        void ps_readAircraftIcaoCodes(int number);

        //! Read ICAO codes
        void ps_readAirlinesIcaoCodes(int number);

        //! Read liveries
        void ps_readLiveries(int number);

        //! Read distributors
        void ps_readDistributors(int number);

        //! Read models
        void ps_readModels(int number);

    private:
        //! Init the readers
        void initReaders(WebReader flags);

        WebReader m_readerFlags = None; //!< which readers are available

        // for reading XML and VATSIM data files
        CVatsimBookingReader  *m_vatsimBookingReader  = nullptr;
        CVatsimDataFileReader *m_vatsimDataFileReader = nullptr;
        CIcaoDataReader       *m_icaoDataReader       = nullptr;
        CModelDataReader      *m_modelDataReader      = nullptr;
    };
} // namespace

Q_DECLARE_OPERATORS_FOR_FLAGS(BlackCore::CWebDataReader::WebReader)

#endif
