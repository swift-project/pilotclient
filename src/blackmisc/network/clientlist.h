/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_NETWORK_CLIENTLIST_H
#define BLACKMISC_NETWORK_CLIENTLIST_H

#include "blackmisc/aviation/callsignobjectlist.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/collection.h"
#include "blackmisc/network/client.h"
#include "blackmisc/sequence.h"
#include "blackmisc/variant.h"
#include <QMetaType>

namespace BlackMisc
{
    namespace Network
    {
        //! Value object encapsulating a list of voice rooms.
        class BLACKMISC_EXPORT CClientList :
            public CSequence<CClient>,
            public BlackMisc::Aviation::ICallsignObjectList<CClient, CClientList>,
            public BlackMisc::Mixin::MetaType<CClientList>
        {
        public:
            BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CClientList)

            //! Default constructor.
            CClientList();

            //! Construct from a base class object.
            CClientList(const CSequence &other);
        };
    } //namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Network::CClientList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Network::CClient>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::Network::CClient>)

#endif //guard
