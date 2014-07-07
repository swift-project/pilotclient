/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*!
    \file
*/

#ifndef BLACKMISC_SERVERLIST_H
#define BLACKMISC_SERVERLIST_H

#include "nwserver.h"
#include "collection.h"
#include "sequence.h"
#include <QObject>
#include <QString>
#include <QList>

namespace BlackMisc
{
    namespace Network
    {
        /*!
         * Value object encapsulating a list of servers.
         */
        class CServerList : public CSequence<CServer>
        {
        public:
            //! Default constructor.
            CServerList();

            //! Construct from a base class object.
            CServerList(const CSequence<CServer> &other);

            //! \copydoc CValueObject::toQVariant
            virtual QVariant toQVariant() const override { return QVariant::fromValue(*this); }

            //! Register metadata
            static void registerMetadata();

        };

    } //namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Network::CServerList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Network::CServer>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::Network::CServer>)

#endif //guard
