/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_CLIENTLIST_H
#define BLACKMISC_CLIENTLIST_H

#include "nwclient.h"
#include "sequence.h"
#include "collection.h"
#include "blackmisc/avcallsignobjectlist.h"
#include "blackmisc/avcallsign.h"

#include <QObject>
#include <QString>
#include <QList>

namespace BlackMisc
{
    namespace Network
    {
        //! Value object encapsulating a list of voice rooms.
        class CClientList :
            public CSequence<CClient>,
            public BlackMisc::Aviation::ICallsignObjectList<BlackMisc::Network::CClient, BlackMisc::Network::CClientList>
        {
        public:
            //! Default constructor.
            CClientList();

            //! Construct from a base class object.
            CClientList(const CSequence &other);

            //! QVariant, required for DBus QVariant lists
            virtual QVariant toQVariant() const override { return QVariant::fromValue(*this); }

            //! Register metadata
            static void registerMetadata();

        protected:
            //! Myself
            virtual const CClientList &container() const { return *this; }

            //! Myself
            virtual CClientList &container() { return *this; }
        };

    } //namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Network::CClientList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Network::CClient>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::Network::CClient>)

#endif //guard
