// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_NETWORK_USERLIST_H
#define SWIFT_MISC_NETWORK_USERLIST_H

#include <QMetaType>

#include "misc/aviation/callsignobjectlist.h"
#include "misc/collection.h"
#include "misc/network/user.h"
#include "misc/sequence.h"
#include "misc/swiftmiscexport.h"

SWIFT_DECLARE_SEQUENCE_MIXINS(swift::misc::network, CUser, CUserList)

namespace swift::misc::network
{
    //! Value object encapsulating a list of voice rooms.
    class SWIFT_MISC_EXPORT CUserList :
        public CSequence<CUser>,
        public aviation::ICallsignObjectList<CUser, CUserList>,
        public mixin::MetaType<CUserList>
    {
    public:
        SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CUserList)
        using CSequence::CSequence;

        //! Default constructor.
        CUserList();

        //! Construct from a base class object.
        CUserList(const CSequence &other);
    };
} // namespace swift::misc::network

Q_DECLARE_METATYPE(swift::misc::network::CUserList)
Q_DECLARE_METATYPE(swift::misc::CCollection<swift::misc::network::CUser>)

#endif
