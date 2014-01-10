/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*!
    \file
*/

#ifndef BLACKMISC_VOICEROOMLIST_H
#define BLACKMISC_VOICEROOMLIST_H

#include "vvoiceroom.h"
#include "sequence.h"
#include "collection.h"
#include <QObject>
#include <QString>
#include <QList>

namespace BlackMisc
{
    namespace Voice
    {
        /*!
         * Value object encapsulating a list of voice rooms.
         */
        class CVoiceRoomList : public CSequence<CVoiceRoom>
        {
        public:
            /*!
             * \brief Default constructor.
             */
            CVoiceRoomList();

            /*!
             * \brief Construct from a base class object.
             * \param other
             */
            CVoiceRoomList(const CSequence &other);

            /*!
             * \brief QVariant, required for DBus QVariant lists
             * \return
             */
            virtual QVariant asQVariant() const
            {
                return QVariant::fromValue(*this);
            }

            /*!
             * \brief Register metadata
             */
            static void registerMetadata();

        };

    } //namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Voice::CVoiceRoomList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Voice::CVoiceRoom>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::Voice::CVoiceRoom>)

#endif //guard
