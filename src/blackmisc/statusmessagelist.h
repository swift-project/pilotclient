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
        /*!
         * \brief StatusMessage
         */
        CStatusMessageList() {}

        /*!
         * \brief Find by type
         * \param type
         * \return
         */
        CStatusMessageList findByType(CStatusMessage::StatusType type) const;

        /*!
         * \brief Find by type
         * \param type
         * \return
         */
        CStatusMessageList findBySeverity(CStatusMessage::StatusSeverity severity) const;

        /*!
         * \brief As QVariant
         * \return
         */
        virtual QVariant asQVariant() const
        {
            return QVariant::fromValue(*this);
        }

        /*!
         * \brief Register metadata of unit and quantity
         */
        static void registerMetadata();

    };

}

Q_DECLARE_METATYPE(BlackMisc::CStatusMessageList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::CStatusMessage>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::CStatusMessage>)

#endif // guard
