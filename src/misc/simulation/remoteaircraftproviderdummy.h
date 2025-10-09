// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SIMULATION_REMOTEAIRCRAFTPROVIDERDUMMY_H
#define SWIFT_MISC_SIMULATION_REMOTEAIRCRAFTPROVIDERDUMMY_H

#include "misc/aviation/aircraftpartslist.h"
#include "misc/aviation/aircraftsituationlist.h"
#include "misc/aviation/callsignset.h"
#include "misc/simulation/airspaceaircraftsnapshot.h"
#include "misc/simulation/remoteaircraftprovider.h"
#include "misc/simulation/simulatedaircraftlist.h"
#include "misc/swiftmiscexport.h"

namespace swift::misc::simulation
{
    //! Dummy implementation for testing purpose
    class SWIFT_MISC_EXPORT CRemoteAircraftProviderDummy : public CRemoteAircraftProvider
    {
        Q_OBJECT

    public:
        //! Singleton
        static CRemoteAircraftProviderDummy *instance();

        //! Constructor
        CRemoteAircraftProviderDummy(QObject *parent = nullptr);

        //! @{
        //! For testing, add new situation and fire signals
        void insertNewSituation(const aviation::CAircraftSituation &situation);
        void insertNewSituations(const aviation::CAircraftSituationList &situations);
        void insertNewAircraftParts(const aviation::CCallsign &callsign, const aviation::CAircraftParts &parts,
                                    bool removeOutdatedParts);
        void insertNewAircraftParts(const aviation::CCallsign &callsign, const aviation::CAircraftPartsList &partsList,
                                    bool removeOutdatedParts);
        //! @}

        //! @{
        //! Members not implenented or fully implenented by CRemoteAircraftProvider
        //! \ingroup remoteaircraftprovider
        QObject *asQObject() override { return this; }
        CAirspaceAircraftSnapshot getLatestAirspaceAircraftSnapshot() const override;
        //! @}
    };
} // namespace swift::misc::simulation

#endif // SWIFT_MISC_SIMULATION_REMOTEAIRCRAFTPROVIDERDUMMY_H
