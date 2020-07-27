/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_NETWORK_USERLIST_H
#define BLACKMISC_NETWORK_USERLIST_H

#include "blackmisc/aviation/callsignobjectlist.h"
#include "blackmisc/network/user.h"
#include "blackmisc/collection.h"
#include "blackmisc/sequence.h"
#include "blackmisc/variant.h"
#include "blackmisc/blackmiscexport.h"

#include <QMetaType>

namespace BlackMisc
{
    namespace Network
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
    } //namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Network::CUserList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Network::CUser>)

#endif //guard
