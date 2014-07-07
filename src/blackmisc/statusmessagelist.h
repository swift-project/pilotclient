/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_STATUSMESSAGELIST_H
#define BLACKMISC_STATUSMESSAGELIST_H

#include "valueobject.h"
#include "sequence.h"
#include "collection.h"
#include "statusmessage.h"

namespace BlackMisc
{

    /*!
     * \brief Status messages, e.g. from Core -> GUI
     */
    class CStatusMessageList : public CSequence<CStatusMessage>
    {
    public:
        //! \brief Constructor
        CStatusMessageList() {}

        //! \brief Construct from a base class object.
        CStatusMessageList(const CSequence<CStatusMessage> &other);

        //! \brief Find by type
        CStatusMessageList findByType(CStatusMessage::StatusType type) const;

        //! \brief Find by severity
        CStatusMessageList findBySeverity(CStatusMessage::StatusSeverity severity) const;

        //! \copydoc CValueObject::asQVariant
        virtual QVariant toQVariant() const override { return QVariant::fromValue(*this); }

        //! \brief Register metadata of unit and quantity
        static void registerMetadata();

    };

}

Q_DECLARE_METATYPE(BlackMisc::CStatusMessageList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::CStatusMessage>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::CStatusMessage>)

#endif // guard
