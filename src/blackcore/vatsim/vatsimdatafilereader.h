/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKCORE_VATSIM_VATSIMDATAFILEREADER_H
#define BLACKCORE_VATSIM_VATSIMDATAFILEREADER_H

#include "blackcore/blackcoreexport.h"
#include "blackcore/data/vatsimsetup.h"
#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/aviation/airlineicaocode.h"
#include "blackmisc/aviation/atcstationlist.h"
#include "blackmisc/aviation/callsignset.h"
#include "blackmisc/aviation/flightplan.h"
#include "blackmisc/network/entityflags.h"
#include "blackmisc/network/serverlist.h"
#include "blackmisc/network/userlist.h"
#include "blackmisc/network/voicecapabilities.h"
#include "blackmisc/simulation/simulatedaircraftlist.h"
#include "blackmisc/datacache.h"
#include "blackcore/threadedreader.h"

#include <QMap>
#include <QObject>
#include <QString>
#include <QStringList>

class QNetworkReply;

namespace BlackMisc { namespace Simulation { class CSimulatedAircraft; } }
namespace BlackCore
{
    namespace Vatsim
    {
        //! Read vatsim data file
        //! \sa http://info.vroute.net/vatsim-data.txt
        class BLACKCORE_EXPORT CVatsimDataFileReader : public BlackCore::CThreadedReader
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CVatsimDataFileReader(QObject *owner);

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

            //! Get all VATSIM FSD servers
            //! \threadsafe
            BlackMisc::Network::CServerList getFsdServers() const;

            //! Users for callsign(s)
            //! \threadsafe
            BlackMisc::Network::CUserList getUsersForCallsigns(const BlackMisc::Aviation::CCallsignSet &callsigns) const;

            //! User for callsign
            //! \threadsafe
            BlackMisc::Network::CUserList getUsersForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const;

            //! Controllers for callsigns
            //! \threadsafe
            BlackMisc::Network::CUserList getControllersForCallsigns(const BlackMisc::Aviation::CCallsignSet &callsigns) const;

            //! Controllers for callsign
            //! \threadsafe
            BlackMisc::Network::CUserList getControllersForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const;

            //! Users for callsigns
            //! \threadsafe
            BlackMisc::Network::CUserList getPilotsForCallsigns(const BlackMisc::Aviation::CCallsignSet &callsigns) const;

            //! Users for callsign
            //! \threadsafe
            BlackMisc::Network::CUserList getPilotsForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const;

            //! Aircraft ICAO info for callsign
            //! \threadsafe
            BlackMisc::Aviation::CAircraftIcaoCode getAircraftIcaoCode(const BlackMisc::Aviation::CCallsign &callsign) const;

            //! Airline ICAO info for callsign
            //! \threadsafe
            BlackMisc::Aviation::CAirlineIcaoCode getAirlineIcaoCode(const BlackMisc::Aviation::CCallsign &callsign) const;

            //! Voice capability for callsign
            //! \threadsafe
            BlackMisc::Network::CVoiceCapabilities getVoiceCapabilityForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const;

            //! Flight plan remarks for callsign
            //! \threadsafe
            QString getFlightPlanRemarksForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const;

            //! Parsed flight plan remarks for callsign
            //! \threadsafe
            BlackMisc::Aviation::CFlightPlanRemarks getParsedFlightPlanRemarksForCallsign(const BlackMisc::Aviation::CCallsign &callsign) const;

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

        protected:
            //! \name BlackCore::CThreadedReader overrides
            //! @{
            virtual void doWorkImpl() override;
            //! @}

        private slots:
            //! Data have been read, parse VATSIM file
            void ps_parseVatsimFile(QNetworkReply *nwReply);

            //! Read / re-read data file
            void ps_read();

        private:
            //! Section in file
            enum Section
            {
                SectionNone,
                SectionFsdServers,
                SectionVoiceServers,
                SectionClients,
                SectionGeneral
            };

            BlackMisc::Aviation::CAtcStationList m_atcStations;
            BlackMisc::Simulation::CSimulatedAircraftList m_aircraft;
            BlackMisc::CData<BlackCore::Data::TVatsimSetup> m_lastGoodSetup { this };
            BlackMisc::CSettingReadOnly<BlackCore::Vatsim::TVatsimDataFile> m_settings { this, &CVatsimDataFileReader::reloadSettings };
            QMap<BlackMisc::Aviation::CCallsign, BlackMisc::Network::CVoiceCapabilities> m_voiceCapabilities; //!< voice capabilities
            QMap<BlackMisc::Aviation::CCallsign, QString> m_flightPlanRemarks; //!< cache for flight plan remarks

            //! Reload the reader settings
            void reloadSettings();

            //! Split line and assign values to their corresponding attribute names
            //! \remark attributes expected as lower case
            static const QMap<QString, QString> clientPartsToMap(const QString &currentLine, const QStringList &clientSectionAttributes);

            //! Get current section
            static Section currentLineToSection(const QString &currentLine);
        };
    } // ns
} // ns

#endif // guard
