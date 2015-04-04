/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVIATION_ATCSTATIONLIST_H
#define BLACKMISC_AVIATION_ATCSTATIONLIST_H

#include "blackmisc/geo/geoobjectlist.h"
#include "blackmisc/nwuserlist.h"
#include "blackmisc/aviation/atcstation.h"
#include "blackmisc/aviation/callsignobjectlist.h"
#include "blackmisc/collection.h"
#include "blackmisc/sequence.h"
#include <QObject>
#include <QString>
#include <QList>

namespace BlackMisc
{
    namespace Aviation
    {
        //! Value object for a list of ATC stations.
        class CAtcStationList :
            public CSequence<CAtcStation>,
            public BlackMisc::Aviation::ICallsignObjectList<CAtcStation, CAtcStationList>,
            public BlackMisc::Geo::IGeoObjectWithRelativePositionList<CAtcStation, CAtcStationList>
        {
        public:
            //! Default constructor.
            CAtcStationList();

            //! Construct from a base class object.
            CAtcStationList(const CSequence<CAtcStation> &other);

            //! Find 0..n stations tune in frequency of COM unit (with 25kHt channel spacing
            CAtcStationList findIfComUnitTunedIn25KHz(const BlackMisc::Aviation::CComSystem &comUnit) const;

            //! Find 0..n stations with valid voice room
            //! \sa CAtcStation::hasValid
            CAtcStationList stationsWithValidVoiceRoom() const;

            //! All controllers (with valid data)
            BlackMisc::Network::CUserList getControllers() const;

            //! Merge with ATC station representing booking information.
            //! Both sides (booking, online station) will be updated.
            //! \remarks Can be used if the stored data in this list are online ATC stations
            int mergeWithBooking(CAtcStation &bookedAtcStation);

            //! Merge with the data from the VATSIM data file
            //! \remarks Can be used if the stored data in this list are VATSIM data file stations
            bool updateFromVatsimDataFileStation(CAtcStation &stationToBeUpdated) const;

            //! \copydoc CValueObject::toQVariant
            virtual QVariant toQVariant() const override { return QVariant::fromValue(*this); }

            //! \copydoc CValueObject::convertFromQVariant
            virtual void convertFromQVariant(const QVariant &variant) override { BlackMisc::setFromQVariant(this, variant); }

            //! Register metadata
            static void registerMetadata();

        protected:
            //! Myself
            virtual const CAtcStationList &container() const { return *this; }

            //! Myself
            virtual CAtcStationList &container() { return *this; }

        };

    } //namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CAtcStationList)
Q_DECLARE_METATYPE(BlackMisc::CCollection<BlackMisc::Aviation::CAtcStation>)
Q_DECLARE_METATYPE(BlackMisc::CSequence<BlackMisc::Aviation::CAtcStation>)

#endif //guard
