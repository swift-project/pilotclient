/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_NETWORK_USERLIST_H
#define BLACKMISC_NETWORK_USERLIST_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/network/user.h"
#include "blackmisc/sequence.h"
#include "blackmisc/collection.h"
#include <QObject>
#include <QString>
#include <QList>

namespace BlackMisc
{
    namespace Network
    {
        //! Value object encapsulating a list of voice rooms.
        class BLACKMISC_EXPORT CUserList : public CSequence<CUser>
        {
        public:
            //! Default constructor.
            CUserList();

            //! Construct from a base class object.
            CUserList(const CSequence &other);

            //! \copydoc CValueObject::toQVariant
            QVariant toQVariant() const { return QVariant::fromValue(*this); }

            //! \copydoc CValueObject::convertFromQVariant
            void convertFromQVariant(const QVariant &variant) { BlackMisc::setFromQVariant(this, variant); }

            //! Register metadata
            static void registerMetadata();

        };

    } //namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Network::CUserList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Network::CUser>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::Network::CUser>)

#endif //guard
