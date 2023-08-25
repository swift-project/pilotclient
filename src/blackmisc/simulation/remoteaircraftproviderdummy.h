// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_SIMULATION_REMOTEAIRCRAFTPROVIDERDUMMY_H
#define BLACKMISC_SIMULATION_REMOTEAIRCRAFTPROVIDERDUMMY_H

#include "blackmisc/simulation/airspaceaircraftsnapshot.h"
#include "blackmisc/simulation/remoteaircraftprovider.h"
#include "blackmisc/simulation/simulatedaircraftlist.h"
#include "blackmisc/aviation/aircraftpartslist.h"
#include "blackmisc/aviation/aircraftsituationlist.h"
#include "blackmisc/aviation/callsignset.h"
#include "blackmisc/blackmiscexport.h"

namespace BlackMisc::Simulation
{
    //! Dummy implementation for testing purpose
    class BLACKMISC_EXPORT CRemoteAircraftProviderDummy : public CRemoteAircraftProvider
    {
        Q_OBJECT

    public:
        //! Singleton
        static CRemoteAircraftProviderDummy *instance();

        //! Constructor
        CRemoteAircraftProviderDummy(QObject *parent = nullptr);

        //! @{
        //! For testing, add new situation and fire signals
        void insertNewSituation(const Aviation::CAircraftSituation &situation);
        void insertNewSituations(const Aviation::CAircraftSituationList &situations);
        void insertNewAircraftParts(const Aviation::CCallsign &callsign, const Aviation::CAircraftParts &parts, bool removeOutdatedParts);
        void insertNewAircraftParts(const Aviation::CCallsign &callsign, const Aviation::CAircraftPartsList &partsList, bool removeOutdatedParts);
        //! @}

        //! @{
        //! Members not implenented or fully implenented by CRemoteAircraftProvider
        //! \ingroup remoteaircraftprovider
        virtual QObject *asQObject() override { return this; }
        virtual CAirspaceAircraftSnapshot getLatestAirspaceAircraftSnapshot() const override;
        //! @}
    };
} // namespace

#endif // guard
