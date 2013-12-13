/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*!
    \file
*/

#ifndef BLACKMISC_AIRCRAFTLIST_H
#define BLACKMISC_AIRCRAFTLIST_H

#include "avaircraft.h"
#include "collection.h"
#include "sequence.h"
#include <QObject>
#include <QString>
#include <QList>

namespace BlackMisc
{
    namespace Aviation
    {
        /*!
         * Value object encapsulating a list of aircraft stations.
         */
        class CAircraftList : public CSequence<CAircraft>
        {
        public:
            /*!
             * \brief Empty constructor.
             */
            CAircraftList();

            /*!
             * \brief Construct from a base class object.
             * \param other
             */
            CAircraftList(const CSequence<CAircraft> &other);

            /*!
             * \brief QVariant, required for DBus QVariant lists
             * \return
             */
            virtual QVariant toQVariant() const
            {
                return QVariant::fromValue(*this);
            }

            /*!
             * \brief Find 0..n stations by callsign
             * \param callsign
             * \return
             */
            CAircraftList findByCallsign(const CCallsign &callsign) const;

            /*!
             * \brief Find 0..n stations within range of given coordinate
             * \param coordinate
             * \param range
             * \return
             */
            CAircraftList findWithinRange(const BlackMisc::Geo::ICoordinateGeodetic &coordinate, const BlackMisc::PhysicalQuantities::CLength &range) const;

            /*!
             * \brief Register metadata
             */
            static void registerMetadata();

        };

    } //namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CAircraftList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Aviation::CAircraft>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::Aviation::CAircraft>)

#endif //guard
