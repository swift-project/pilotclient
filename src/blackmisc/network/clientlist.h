// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_NETWORK_CLIENTLIST_H
#define BLACKMISC_NETWORK_CLIENTLIST_H

#include "blackmisc/aviation/callsignobjectlist.h"
#include "blackmisc/collection.h"
#include "blackmisc/network/client.h"
#include "blackmisc/sequence.h"
#include "blackmisc/blackmiscexport.h"
#include <QMap>
#include <QMetaType>

BLACK_DECLARE_SEQUENCE_MIXINS(BlackMisc::Network, CClient, CClientList)

namespace BlackMisc::Network
{
    //! Value object encapsulating a list of voice rooms.
    class BLACKMISC_EXPORT CClientList :
        public CSequence<CClient>,
        public Aviation::ICallsignObjectList<CClient, CClientList>,
        public Mixin::MetaType<CClientList>
    {
    public:
        BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CClientList)
        using CSequence::CSequence;

        //! Default constructor.
        CClientList();

        //! Construct from a base class object.
        CClientList(const CSequence &other);

        //! Has client for callsign the given capability?
        bool hasCapability(const Aviation::CCallsign &callsign, CClient::Capability capability) const;

        //! Capabilities of client for callsign
        CClient::Capabilities getCapabilities(const Aviation::CCallsign &callsign) const;

        //! Filter pilots by callsign in set
        CClientList filterPilotsByCallsign(const Aviation::CCallsignSet &callsigns) const;
    };

    //! Client per callsign
    using CClientPerCallsign = QHash<Aviation::CCallsign, CClient>;
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Network::CClientList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Network::CClient>)
Q_DECLARE_METATYPE(BlackMisc::Network::CClientPerCallsign)

#endif // guard
