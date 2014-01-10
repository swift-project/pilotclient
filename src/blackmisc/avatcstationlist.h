/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*!
    \file
*/

#ifndef BLACKMISC_ATCSTATIONLIST_H
#define BLACKMISC_ATCSTATIONLIST_H

#include "avatcstation.h"
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
         * Value object encapsulating a list of ATC stations.
         */
        class CAtcStationList : public CSequence<CAtcStation>
        {
        public:
            /*!
             * \brief Empty constructor.
             */
            CAtcStationList();

            /*!
             * \brief Construct from a base class object.
             * \param other
             */
            CAtcStationList(const CSequence<CAtcStation> &other);

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
            CAtcStationList findByCallsign(const CCallsign &callsign) const;

            /*!
             * \brief Find 0..n stations within range of given coordinate
             * \param coordinate
             * \param range
             * \return
             */
            CAtcStationList findWithinRange(const BlackMisc::Geo::ICoordinateGeodetic &coordinate, const BlackMisc::PhysicalQuantities::CLength &range) const;

            /*!
             * \brief Update distances to own plane
             * \param position
             */
            void calculateDistancesToPlane(const BlackMisc::Geo::CCoordinateGeodetic &position);

            /*!
             * \brief Register metadata
             */
            static void registerMetadata();

            /*!
             * \brief Merge with ATC station representing booking information
             * \param bookedAtcStation
             * \return
             */
            int mergeWithBooking(CAtcStation &bookedAtcStation);
        };

    } //namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CAtcStationList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Aviation::CAtcStation>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::Aviation::CAtcStation>)

#endif //guard
