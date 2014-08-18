/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_USERLIST_H
#define BLACKMISC_USERLIST_H

#include "nwuser.h"
#include "sequence.h"
#include "collection.h"
#include <QObject>
#include <QString>
#include <QList>

namespace BlackMisc
{
    namespace Network
    {
        //! Value object encapsulating a list of voice rooms.
        class CUserList : public CSequence<CUser>
        {
        public:
            //! Default constructor.
            CUserList();

            //! Construct from a base class object.
            CUserList(const CSequence &other);

            //! \copydoc CValueObject::toQVariant
            virtual QVariant toQVariant() const override { return QVariant::fromValue(*this); }

            //! \copydoc CValueObject::fromQVariant
            virtual void fromQVariant(const QVariant &variant) override { BlackMisc::setFromQVariant(this, variant); }

            //! Register metadata
            static void registerMetadata();

        };

    } //namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Network::CUserList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Network::CUser>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::Network::CUser>)

#endif //guard
