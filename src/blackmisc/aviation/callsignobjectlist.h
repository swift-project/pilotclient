/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVIATION_CALLSIGNOBJECTLIST_H
#define BLACKMISC_AVIATION_CALLSIGNOBJECTLIST_H

#include "blackmisc/collection.h"
#include "blackmisc/sequence.h"
#include "blackmisc/aviation/callsignset.h"
#include "blackmisc/propertyindexlist.h"
#include "blackmisc/propertyindexvariantmap.h"
#include <QList>
#include <QHash>

namespace BlackMisc
{

    namespace Aviation
    {
        class CAircraftSituation;
        class CAircraftSituationList;
        class CAircraftParts;
        class CAircraftPartsList;
        class CAircraft;
        class CAircraftList;
        class CAtcStation;
        class CAtcStationList;
    }

    namespace Simulation
    {
        class CSimulatedAircraft;
        class CSimulatedAircraftList;
    }

    namespace Network
    {
        class CClient;
        class CClientList;
    }

    namespace Aviation
    {
        //! List of objects with callsign.
        template<class OBJ, class CONTAINER>
        class ICallsignObjectList
        {
        public:

            //! Contains callsign?
            bool containsCallsign(const BlackMisc::Aviation::CCallsign &callsign) const;

            //! Apply for given callsign
            int applyIfCallsign(const BlackMisc::Aviation::CCallsign &callsign, const BlackMisc::CPropertyIndexVariantMap &variantMap);

            //! All callsigns
            BlackMisc::Aviation::CCallsignSet getCallsigns() const;

            //! Find 0..n stations by callsign
            CONTAINER findByCallsign(const CCallsign &callsign) const;

            //! Find 0..n aircraft matching any of a set of callsigns
            CONTAINER findByCallsigns(const CCallsignSet &callsigns) const;

            //! Find the first aircraft by callsign, if none return given one
            OBJ findFirstByCallsign(const CCallsign &callsign, const OBJ &ifNotFound = {}) const;

            //! Find the back object by callsign, if none return given one
            OBJ findLastByCallsign(const CCallsign &callsign, const OBJ &ifNotFound = {}) const;

            //! All with given suffix, empty suffixes ignored
            CONTAINER findBySuffix(const QString &suffix) const;

            //! First found index of callsign, otherwise -1
            int firstIndexOfCallsign(const BlackMisc::Aviation::CCallsign &callsign);

            //! Remove all objects with callsign
            int removeByCallsign(const CCallsign &callsign);

            //! All suffixes with their respective count
            QMap<QString, int> getSuffixes() const;

            //! Split into 0..n containers as per callsign
            QHash<BlackMisc::Aviation::CCallsign, CONTAINER> splitPerCallsign() const;

            //! Sort by callsign
            void sortByCallsign();

            //! Incremental update or add object
            int incrementalUpdateOrAdd(const OBJ &objectBeforeChanged, const BlackMisc::CPropertyIndexVariantMap &changedValues);

        protected:
            //! Constructor
            ICallsignObjectList();

            //! Container
            const CONTAINER &container() const;

            //! Container
            CONTAINER &container();
        };

        extern template class BLACKMISC_EXPORT_TEMPLATE ICallsignObjectList<BlackMisc::Aviation::CAtcStation, BlackMisc::Aviation::CAtcStationList>;
        extern template class BLACKMISC_EXPORT_TEMPLATE ICallsignObjectList<BlackMisc::Aviation::CAircraft, BlackMisc::Aviation::CAircraftList>;
        extern template class BLACKMISC_EXPORT_TEMPLATE ICallsignObjectList<BlackMisc::Aviation::CAircraftSituation, BlackMisc::Aviation::CAircraftSituationList>;
        extern template class BLACKMISC_EXPORT_TEMPLATE ICallsignObjectList<BlackMisc::Aviation::CAircraftParts, BlackMisc::Aviation::CAircraftPartsList>;
        extern template class BLACKMISC_EXPORT_TEMPLATE ICallsignObjectList<BlackMisc::Simulation::CSimulatedAircraft, BlackMisc::Simulation::CSimulatedAircraftList>;
        extern template class BLACKMISC_EXPORT_TEMPLATE ICallsignObjectList<BlackMisc::Network::CClient, BlackMisc::Network::CClientList>;

    } //namespace
} // namespace

#endif //guard
