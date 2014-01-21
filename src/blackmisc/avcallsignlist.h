/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*!
    \file
*/

#ifndef BLACKMISC_CALLSIGNLIST_H
#define BLACKMISC_CALLSIGNLIST_H

#include "avcallsign.h"
#include "collection.h"
#include "sequence.h"
#include <QObject>

namespace BlackMisc
{
    namespace Aviation
    {
        /*!
         * Value object encapsulating a list of callsign.
         */
        class CCallsignList : public CSequence<CCallsign>
        {
        public:
            /*!
             * \brief Default constructor.
             */
            CCallsignList();

            /*!
             * \brief Construct from a base class object.
             */
            CCallsignList(const CSequence<CCallsign> &other);

            /*!
             * \copydoc CValueObject::toQVariant()
             */
            virtual QVariant toQVariant() const
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

Q_DECLARE_METATYPE(BlackMisc::Aviation::CCallsignList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Aviation::CCallsign>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::Aviation::CCallsign>)

#endif //guard
