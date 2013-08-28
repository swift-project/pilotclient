/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/*!
    \file
*/

#ifndef BLACKMISC_ATCLIST_H
#define BLACKMISC_ATCLIST_H

#include "pqfrequency.h"
#include "pqlength.h"
#include "coordinategeodetic.h"
#include "context.h"
#include <QObject>
#include <QString>
#include <QMap>

namespace BlackMisc
{

    /*!
     * Value object encapsulating information about an ATC station.
     */
    class CAtcListEntry
    {
    public:
        /*!
         * Default constructor.
         */
        CAtcListEntry() {}

        /*!
         * Constructor.
         */
        CAtcListEntry(const QString &callsign, const BlackMisc::PhysicalQuantities::CFrequency &freq,
            const BlackMisc::Geo::CCoordinateGeodetic &pos, const BlackMisc::PhysicalQuantities::CLength &range)
            : m_callsign(callsign), m_freq(freq), m_pos(pos), m_range(range)
        {}

        /*!
         * Get callsign.
         * \return
         */
        const QString &getCallsign() const { return m_callsign; }

        /*!
         * Get frequency.
         * \return
         */
        const BlackMisc::PhysicalQuantities::CFrequency &getFrequency() const { return m_freq; }

        /*!
         * Get the position of the center of the controller's area of visibility.
         * \return
         */
        const BlackMisc::Geo::CCoordinateGeodetic &getPosition() const { return m_pos; }

        /*!
         * Get the radius of the controller's area of visibility.
         * \return
         */
        const BlackMisc::PhysicalQuantities::CLength &getAtcRange() const { return m_range; }

    private:
        QString m_callsign;
        BlackMisc::PhysicalQuantities::CFrequency m_freq;
        BlackMisc::Geo::CCoordinateGeodetic m_pos;
        BlackMisc::PhysicalQuantities::CLength m_range;
    };

    /*!
     * Value object encapsulating a list of ATC stations.
     */
    class CAtcList
    {
    public:
        /*!
         * Immutable getter for the internal map.
         * \return
         */
        const QMap<QString, CAtcListEntry> &constMap() const { return m_map; }

        /*!
         * Insert an ATC station into the list.
         * \param entry
         */
        void insert(const CAtcListEntry &entry) { m_map.insert(entry.getCallsign(), entry); }

        /*!
         * Remove an ATC station from the list.
         * \param callsign
         */
        void remove(const QString &callsign) { m_map.remove(callsign); }

        /*!
         * Remove all ATC stations from the list.
         */
        void clear() { m_map.clear(); }

    private:
        QMap<QString, CAtcListEntry> m_map;
    };

} //namespace BlackMisc

Q_DECLARE_METATYPE(BlackMisc::CAtcList)

#endif //BLACKMISC_ATCLIST_H