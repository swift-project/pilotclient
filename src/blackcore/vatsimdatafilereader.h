/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_VATSIMDATAFILEREADER_H
#define BLACKCORE_VATSIMDATAFILEREADER_H

#include "blackcoreexport.h"
#include "blackmisc/threadedreader.h"
#include "blackmisc/simulation/simulatedaircraftlist.h"
#include "blackmisc/aviation/atcstationlist.h"
#include "blackmisc/aviation/callsignset.h"
#include "blackmisc/network/serverlist.h"
#include "blackmisc/network/userlist.h"
#include "blackmisc/network/voicecapabilities.h"

#include <QObject>
#include <QTimer>
#include <QNetworkReply>
#include <QReadWriteLock>

namespace BlackCore
{
    //! Read bookings from VATSIM
    class BLACKCORE_EXPORT CVatsimDataFileReader : public BlackMisc::CThreadedReader
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CVatsimDataFileReader(QObject *owner, const QStringList &urls);

        //! Get aircraft
        //! \threadsafe
        BlackMisc::Simulation::CSimulatedAircraftList getAircraft() const;

        //! Get ATC station
        //! \threadsafe
        BlackMisc::Aviation::CAtcStationList getAtcStations() const;

        //! Get ATC stations for callsign
        //! \threadsafe
        BlackMisc::Aviation::CAtcStationList getAtcStationsForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const;

        //! Get ATC stations for callsigns
        //! \threadsafe
        BlackMisc::Aviation::CAtcStationList getAtcStationsForCallsigns(const BlackMisc::Aviation::CCallsignSet &callsigns) const;

        //! Get all voice servers
        //! \threadsafe
        BlackMisc::Network::CServerList getVoiceServers() const;

        //! Get all FSD servers
        //! \threadsafe
        BlackMisc::Network::CServerList getFsdServers() const;

        //! Users for callsign(s)
        //! \threadsafe
        BlackMisc::Network::CUserList getUsersForCallsigns(const BlackMisc::Aviation::CCallsignSet &callsigns);

        //! User for callsign
        //! \threadsafe
        BlackMisc::Network::CUserList getUsersForCallsign(const BlackMisc::Aviation::CCallsign &callsign);

        //! Controllers for callsigns
        //! \threadsafe
        BlackMisc::Network::CUserList getControllersForCallsigns(const BlackMisc::Aviation::CCallsignSet &callsigns);

        //! Controllers for callsign
        //! \threadsafe
        BlackMisc::Network::CUserList getControllersForCallsign(const BlackMisc::Aviation::CCallsign &callsign);

        //! Users for callsigns
        //! \threadsafe
        BlackMisc::Network::CUserList getPilotsForCallsigns(const BlackMisc::Aviation::CCallsignSet &callsigns);

        //! Users for callsign
        //! \threadsafe
        BlackMisc::Network::CUserList getPilotsForCallsign(const BlackMisc::Aviation::CCallsign &callsign);

        //! Aircraft ICAO info for callsign
        //! \threadsafe
        BlackMisc::Aviation::CAircraftIcaoCode getAircraftIcaoCode(const BlackMisc::Aviation::CCallsign &callsign);

        //! Airline ICAO info for callsign
        //! \threadsafe
        BlackMisc::Aviation::CAirlineIcaoCode getAirlineIcaoCode(const BlackMisc::Aviation::CCallsign &callsign);

        //! Voice capability for callsign
        //! \threadsafe
        BlackMisc::Network::CVoiceCapabilities getVoiceCapabilityForCallsign(const BlackMisc::Aviation::CCallsign &callsign);

        //! Update aircraft with VATSIM aircraft data from data file
        //! \threadsafe
        void updateWithVatsimDataFileData(BlackMisc::Simulation::CSimulatedAircraft &aircraftToBeUdpated) const;

        //! Start reading in own thread
        void readInBackgroundThread();

    signals:
        //! Data have been read
        void dataFileRead(int lines);

        //! Data have been read
        void dataRead(BlackMisc::Network::CEntityFlags::Entity entity, BlackMisc::Network::CEntityFlags::ReadState state, int number);

    private slots:
        //! Data have been read, parse VATSIM file
        void ps_parseVatsimFile(QNetworkReply *nwReply);

        //! Read / re-read data file
        void ps_read();

    private:
        QNetworkAccessManager *m_networkManager = nullptr;
        QStringList m_serviceUrls; //!< URL of the service
        int m_currentUrlIndex;
        BlackMisc::Network::CServerList               m_voiceServers;
        BlackMisc::Network::CServerList               m_fsdServers;
        BlackMisc::Aviation::CAtcStationList          m_atcStations;
        BlackMisc::Simulation::CSimulatedAircraftList m_aircraft;
        QMap<BlackMisc::Aviation::CCallsign, BlackMisc::Network::CVoiceCapabilities> m_voiceCapabilities;

        //! Split line and assign values to their corresponding attribute names
        static const QMap<QString, QString> clientPartsToMap(const QString &currentLine, const QStringList &clientSectionAttributes);

        //! Section in file
        enum Section
        {
            SectionNone,
            SectionFsdServers,
            SectionVoiceServers,
            SectionClients,
            SectionGeneral
        };

        //! Get current section
        static Section currentLineToSection(const QString &currentLine);

    };
} // ns

#endif // guard
