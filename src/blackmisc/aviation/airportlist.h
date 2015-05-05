/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVIATION_AIRPORTLIST_H
#define BLACKMISC_AVIATION_AIRPORTLIST_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/aviation/airport.h"
#include "blackmisc/geo/geoobjectlist.h"
#include "blackmisc/collection.h"
#include "blackmisc/sequence.h"
#include "blackmisc/geo/geoobjectlist.h"
#include <QObject>
#include <QString>
#include <QList>

namespace BlackMisc
{
    namespace Aviation
    {
        //! Value object for a list of airports.
        class BLACKMISC_EXPORT CAirportList :
            public CSequence<CAirport>,
            public BlackMisc::Geo::IGeoObjectWithRelativePositionList<CAirport, CAirportList>
        {
        public:
            //! Default constructor.
            CAirportList();

            //! Construct from a base class object.
            CAirportList(const CSequence<CAirport> &other);

            //! Find 0..n airports by ICAO code
            CAirportList findByIcao(const CAirportIcao &icao) const;

            //! Replace or add based on same ICAO code
            void replaceOrAddByIcao(const CAirport &addedOrReplacedAirport);

            //! Find first station by callsign, if not return given value / default
            CAirport findFirstByIcao(const CAirportIcao &icao, const CAirport &ifNotFound = CAirport()) const;

            //! \copydoc CValueObject::toQVariant
            QVariant toQVariant() const { return QVariant::fromValue(*this); }

            //! \copydoc CValueObject::convertFromQVariant
            void convertFromQVariant(const QVariant &variant) { BlackMisc::setFromQVariant(this, variant); }

            //! Register metadata
            static void registerMetadata();

        };
    } //namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CAirportList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Aviation::CAirport>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::Aviation::CAirport>)

#endif //guard
