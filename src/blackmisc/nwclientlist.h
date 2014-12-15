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
#include "blackmisc/avcallsign.h"
#include <QObject>
#include <QString>
#include <QList>

namespace BlackMisc
{
    namespace Network
    {
        //! Value object encapsulating a list of voice rooms.
        class CClientList : public CSequence<CClient>
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

            //! Find by callsign
            CClientList findByCallsign(const BlackMisc::Aviation::CCallsign &callsign) const;

            //! First by callsign
            CClient findFirstByCallsign(const BlackMisc::Aviation::CCallsign &callsign, const CClient &ifNotFound = CClient()) const;
        };

    } //namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Network::CClientList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Network::CClient>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::Network::CClient>)

#endif //guard
