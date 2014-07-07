/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*!
    \file
*/

#ifndef BLACKMISC_AIRCRAFTMAPPINGLIST_H
#define BLACKMISC_AIRCRAFTMAPPINGLIST_H

#include "nwaircraftmapping.h"
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
         * Value object encapsulating a list of aircraft mappings
         */
        class CAircraftMappingList : public CSequence<CAircraftMapping>
        {
        public:
            //! Empty constructor.
            CAircraftMappingList();

            //! Construct from a base class object.
            CAircraftMappingList(const CSequence<CAircraftMapping> &other);

            //! QVariant, required for DBus QVariant lists
            virtual QVariant toQVariant() const override { return QVariant::fromValue(*this); }

            //! Find by frequency
            CAircraftMappingList findByIcaoCode(const BlackMisc::Aviation::CAircraftIcao &searchIcao, bool emptyMeansWildcard = true) const;

            //! Find by model string
            CAircraftMappingList findByModelString(const QString modelString, Qt::CaseSensitivity sensitivity) const;

            //! Register metadata
            static void registerMetadata();

        };

    } //namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Network::CAircraftMappingList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Network::CAircraftMapping>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::Network::CAircraftMapping>)

#endif //guard
