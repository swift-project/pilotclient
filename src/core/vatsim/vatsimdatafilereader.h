// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_CORE_VATSIM_VATSIMDATAFILEREADER_H
#define SWIFT_CORE_VATSIM_VATSIMDATAFILEREADER_H

#include <QMap>
#include <QObject>
#include <QString>
#include <QStringList>

#include "core/data/vatsimsetup.h"
#include "core/swiftcoreexport.h"
#include "core/threadedreaderperiodic.h"
#include "misc/aviation/aircrafticaocode.h"
#include "misc/aviation/airlineicaocode.h"
#include "misc/aviation/atcstationlist.h"
#include "misc/aviation/callsignset.h"
#include "misc/aviation/flightplan.h"
#include "misc/datacache.h"
#include "misc/network/entityflags.h"
#include "misc/network/userlist.h"
#include "misc/network/voicecapabilities.h"
#include "misc/simulation/simulatedaircraftlist.h"

class QNetworkReply;

namespace swift::misc::simulation
{
    class CSimulatedAircraft;
}
namespace swift::core::vatsim
{
    //! Read vatsim data file
    class SWIFT_CORE_EXPORT CVatsimDataFileReader : public CThreadedReaderPeriodic
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CVatsimDataFileReader(QObject *owner);

        //! Get aircraft
        //! \threadsafe
        swift::misc::simulation::CSimulatedAircraftList getAircraft() const;

        //! Get ATC station
        //! \threadsafe
        swift::misc::aviation::CAtcStationList getAtcStations() const;

        //! Get ATC stations for callsign
        //! \threadsafe
        swift::misc::aviation::CAtcStationList
        getAtcStationsForCallsign(const swift::misc::aviation::CCallsign &callsign) const;

        //! Get ATC stations for callsigns
        //! \threadsafe
        swift::misc::aviation::CAtcStationList
        getAtcStationsForCallsigns(const swift::misc::aviation::CCallsignSet &callsigns) const;

        //! Users for callsign(s)
        //! \threadsafe
        swift::misc::network::CUserList
        getUsersForCallsigns(const swift::misc::aviation::CCallsignSet &callsigns) const;

        //! User for callsign
        //! \threadsafe
        swift::misc::network::CUserList getUsersForCallsign(const swift::misc::aviation::CCallsign &callsign) const;

        //! Controllers for callsigns
        //! \threadsafe
        swift::misc::network::CUserList
        getControllersForCallsigns(const swift::misc::aviation::CCallsignSet &callsigns) const;

        //! Controllers for callsign
        //! \threadsafe
        swift::misc::network::CUserList
        getControllersForCallsign(const swift::misc::aviation::CCallsign &callsign) const;

        //! Users for callsigns
        //! \threadsafe
        swift::misc::network::CUserList
        getPilotsForCallsigns(const swift::misc::aviation::CCallsignSet &callsigns) const;

        //! Users for callsign
        //! \threadsafe
        swift::misc::network::CUserList getPilotsForCallsign(const swift::misc::aviation::CCallsign &callsign) const;

        //! Aircraft ICAO info for callsign
        //! \threadsafe
        swift::misc::aviation::CAircraftIcaoCode
        getAircraftIcaoCode(const swift::misc::aviation::CCallsign &callsign) const;

        //! Airline ICAO info for callsign
        //! \threadsafe
        swift::misc::aviation::CAirlineIcaoCode
        getAirlineIcaoCode(const swift::misc::aviation::CCallsign &callsign) const;

        //! Voice capability for callsign
        //! \threadsafe
        swift::misc::network::CVoiceCapabilities
        getVoiceCapabilityForCallsign(const swift::misc::aviation::CCallsign &callsign) const;

        //! Flight plan remarks for callsign
        //! \threadsafe
        swift::misc::aviation::CFlightPlanRemarks
        getFlightPlanRemarksForCallsign(const swift::misc::aviation::CCallsign &callsign) const;

        //! Update aircraft with VATSIM aircraft data from data file
        //! \threadsafe
        void updateWithVatsimDataFileData(swift::misc::simulation::CSimulatedAircraft &aircraftToBeUdpated) const;

    signals:
        //! Data have been read
        void dataFileRead(int kB);

        //! Data have been read
        void dataRead(swift::misc::network::CEntityFlags::Entity entity,
                      swift::misc::network::CEntityFlags::ReadState state, int number, const QUrl &url);

    protected:
        // CThreadedReader overrides
        void doWorkImpl() override;

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

        swift::misc::aviation::CAtcStationList m_atcStations;
        swift::misc::simulation::CSimulatedAircraftList m_aircraft;
        swift::misc::CSettingReadOnly<swift::core::vatsim::TVatsimDataFile> m_settings {
            this, &CVatsimDataFileReader::reloadSettings
        };
        QMap<swift::misc::aviation::CCallsign, swift::misc::aviation::CFlightPlanRemarks>
            m_flightPlanRemarks; //!< cache for flight plan remarks

        //! Data have been read, parse VATSIM file
        void parseVatsimFile(QNetworkReply *nwReply);

        swift::misc::simulation::CSimulatedAircraft parsePilot(const QJsonObject &,
                                                               QStringList &o_illegalEquipmentCodes) const;
        swift::misc::aviation::CFlightPlanRemarks parseFlightPlanRemarks(const QJsonObject &) const;
        swift::misc::aviation::CAtcStation parseController(const QJsonObject &) const;

        //! Read / re-read data file
        void read();

        //! Reload the reader settings
        void reloadSettings();
    };
} // namespace swift::core::vatsim

#endif // SWIFT_CORE_VATSIM_VATSIMDATAFILEREADER_H
