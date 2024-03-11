// SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKCORE_VATSIM_VATSIMWEBSERVICES_H
#define BLACKCORE_VATSIM_VATSIMWEBSERVICES_H

#include "blackcore/blackcoreexport.h"
#include "blackmisc/restricted.h"
#include "blackmisc/network/serverlist.h"
#include "blackmisc/network/url.h"
#include "blackmisc/network/voicecapabilities.h"
#include "blackmisc/weather/metarlist.h"
#include "blackmisc/weather/metar.h"
#include "blackmisc/network/userlist.h"
#include "blackmisc/aviation/atcstationlist.h"

namespace BlackMisc::Simulation
{
    class CSimulatedAircraft;
}

namespace BlackCore
{
    class CApplication;
}

namespace BlackCore::Vatsim
{

    class CVatsimStatusFileReader;
    class CVatsimDataFileReader;
    class CVatsimMetarReader;
    class CVatsimServerFileReader;

    /*!
     * Encapsulates all VATSIM-related web services
     * First, starts reading status.json (that contains all further URLs).
     * Once the file is received, all other readers are started:
     * Server file: single shot
     * Data file: periodically
     * METAR file: periodically
     */
    class BLACKCORE_EXPORT CVatsimWebServices : public QObject
    {
        Q_OBJECT

    public:
        //! Log categories
        static const QStringList &getLogCategories();

        //! Constructor, only allowed from BlackCore::CApplication
        explicit CVatsimWebServices(BlackMisc::Restricted<CApplication>, QObject *parent = nullptr);

        //! Destructor
        ~CVatsimWebServices() override;

        //! Shutdown
        void gracefulShutdown();

        //! Data file reader
        Vatsim::CVatsimDataFileReader *getVatsimDataFileReader() const { return m_vatsimDataFileReader; }

        //! Metar reader
        Vatsim::CVatsimMetarReader *getMetarReader() const { return m_vatsimMetarReader; }

        //! FSD servers
        //! \threadsafe
        BlackMisc::Network::CServerList getVatsimFsdServers() const;

        //! METAR URL (from status file)
        //! \threadsafe
        BlackMisc::Network::CUrl getVatsimMetarUrl() const;

        //! Data file location (from status file)
        //! \threadsafe
        BlackMisc::Network::CUrl getVatsimDataFileUrl() const;

        //! Users by callsign
        //! \threadsafe
        BlackMisc::Network::CUserList getUsersForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const;

        //! ATC stations by callsign
        //! \threadsafe
        BlackMisc::Aviation::CAtcStationList getAtcStationsForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const;

        //! Voice capabilities for given callsign
        //! \threadsafe
        BlackMisc::Network::CVoiceCapabilities getVoiceCapabilityForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const;

        //! Update with web data
        //! \threadsafe
        void updateWithVatsimDataFileData(BlackMisc::Simulation::CSimulatedAircraft &aircraftToBeUpdated) const;

        //! Get METAR for airport
        //! \threadsafe
        BlackMisc::Weather::CMetar getMetarForAirport(const BlackMisc::Aviation::CAirportIcaoCode &icao) const;

    private:
        //! Run a single shot of the the VATSIM status file reader in the background thread
        void runVatsimStatusReader();

        //! Start the (periodically) read of the VATSIM Data file
        void startVatsimDataFileReader();

        //! Start the (periodically) read of the VATSIM METAR file
        void startVatsimMetarReader();

        //! Received METAR data
        void receivedMetars(const BlackMisc::Weather::CMetarList &metars);

        //! VATSIM data file has been read
        void vatsimDataFileRead(int kB);

        //! VATSIM status file has been read
        //! This will start the remaining readers
        void vatsimStatusFileRead(double kB);

        //! VATSIM server file has been read
        void vatsimServerFileRead(double kB, const QUrl &url);

        //! Initialize and start VATSIM server file reader
        void startVatsimServerFileReader();

        std::atomic_bool m_shuttingDown { false }; //!< shutting down?

        CVatsimStatusFileReader *m_vatsimStatusReader = nullptr;
        CVatsimDataFileReader *m_vatsimDataFileReader = nullptr;
        CVatsimMetarReader *m_vatsimMetarReader = nullptr;
        CVatsimServerFileReader *m_vatsimServerFileReader = nullptr;
    };
}

#endif // BLACKCORE_VATSIM_VATSIMWEBSERVICES_H
