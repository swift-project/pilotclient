/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVIATION_AIRCRAFTLIST_H
#define BLACKMISC_AVIATION_AIRCRAFTLIST_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/aviation/aircraft.h"
#include "blackmisc/aviation/callsignset.h"
#include "blackmisc/aviation/callsignobjectlist.h"
#include "blackmisc/geo/geoobjectlist.h"
#include "blackmisc/network/userlist.h"
#include "blackmisc/collection.h"
#include "blackmisc/propertyindexvariantmap.h"
#include <QObject>
#include <QString>
#include <QList>

namespace BlackMisc
{
    namespace Aviation
    {
        //! Value object encapsulating a list of aircraft.
        //! \deprecated consider using CSimulatedAircraftList
        class BLACKMISC_EXPORT CAircraftList :
            public CSequence<CAircraft>,
            public ICallsignObjectList<CAircraft, CAircraftList>,
            public BlackMisc::Geo::IGeoObjectWithRelativePositionList<CAircraft, CAircraftList>,
            public BlackMisc::Mixin::MetaType<CAircraftList>
        {
        public:
            BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CAircraftList)

            //! Default constructor.
            CAircraftList();

            //! Construct from a base class object.
            CAircraftList(const CSequence<CAircraft> &other);

            //! All pilots (with valid data)
            BlackMisc::Network::CUserList getPilots() const;

            //! Update aircraft with data from VATSIM data file
            //! \remarks The list used ("this") needs to contain the VATSIM data file objects
            bool updateWithVatsimDataFileData(CAircraft &aircraftToBeUpdated) const;

        };

    } //namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CAircraftList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Aviation::CAircraft>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::Aviation::CAircraft>)

#endif //guard
