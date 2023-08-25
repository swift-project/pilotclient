// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKMISC_NETWORK_USERLIST_H
#define BLACKMISC_NETWORK_USERLIST_H

#include "blackmisc/aviation/callsignobjectlist.h"
#include "blackmisc/network/user.h"
#include "blackmisc/collection.h"
#include "blackmisc/sequence.h"
#include "blackmisc/blackmiscexport.h"

#include <QMetaType>

BLACK_DECLARE_SEQUENCE_MIXINS(BlackMisc::Network, CUser, CUserList)

namespace BlackMisc::Network
{
    //! Value object encapsulating a list of voice rooms.
    class BLACKMISC_EXPORT CUserList :
        public CSequence<CUser>,
        public Aviation::ICallsignObjectList<CUser, CUserList>,
        public Mixin::MetaType<CUserList>
    {
    public:
        BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CUserList)
        using CSequence::CSequence;

        //! Default constructor.
        CUserList();

        //! Construct from a base class object.
        CUserList(const CSequence &other);
    };
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Network::CUserList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Network::CUser>)

#endif // guard
