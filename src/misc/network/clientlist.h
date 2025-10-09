// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_NETWORK_CLIENTLIST_H
#define SWIFT_MISC_NETWORK_CLIENTLIST_H

#include <QMap>
#include <QMetaType>

#include "misc/aviation/callsignobjectlist.h"
#include "misc/collection.h"
#include "misc/network/client.h"
#include "misc/sequence.h"
#include "misc/swiftmiscexport.h"

SWIFT_DECLARE_SEQUENCE_MIXINS(swift::misc::network, CClient, CClientList)

namespace swift::misc::network
{
    //! Value object encapsulating a list of voice rooms.
    class SWIFT_MISC_EXPORT CClientList :
        public CSequence<CClient>,
        public aviation::ICallsignObjectList<CClient, CClientList>,
        public mixin::MetaType<CClientList>
    {
    public:
        SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CClientList)
        using CSequence::CSequence;

        //! Default constructor.
        CClientList() = default;

        //! Construct from a base class object.
        CClientList(const CSequence &other);
    };

    //! Client per callsign
    using CClientPerCallsign = QHash<aviation::CCallsign, CClient>;
} // namespace swift::misc::network

Q_DECLARE_METATYPE(swift::misc::network::CClientList)
Q_DECLARE_METATYPE(swift::misc::CCollection<swift::misc::network::CClient>)
Q_DECLARE_METATYPE(swift::misc::network::CClientPerCallsign)

#endif // SWIFT_MISC_NETWORK_CLIENTLIST_H
