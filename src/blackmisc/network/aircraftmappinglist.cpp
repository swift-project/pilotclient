/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/network/aircraftmappinglist.h"
#include "blackmisc/predicates.h"

using namespace BlackMisc::Network;
using namespace BlackMisc::Aviation;

namespace BlackMisc
{
    namespace Network
    {
        CAircraftMappingList::CAircraftMappingList() { }

        CAircraftMappingList::CAircraftMappingList(const CSequence<CAircraftMapping> &other) :
            CSequence<CAircraftMapping>(other)
        { }

        CAircraftMappingList CAircraftMappingList::findByIcaoCodeWildcard(const CAircraftIcaoData &searchIcao) const
        {
            return this->findBy([ = ](const CAircraftMapping & mapping)
            {
                return mapping.matchesWildcardIcao(searchIcao);
            });
        }

        CAircraftMappingList CAircraftMappingList::findByIcaoAircraftDesignator(const CAircraftIcaoData &searchIcao) const
        {
            const QString aircraftIcao = searchIcao.getAircraftDesignator();
            if (aircraftIcao.isEmpty()) { return CAircraftMappingList(); }
            return this->findBy([ = ](const CAircraftMapping & mapping)
            {
                return mapping.getIcao().getAircraftDesignator() == aircraftIcao;
            });
        }

        CAircraftMappingList CAircraftMappingList::findByIcaoAirlineDesignator(const CAircraftIcaoData &searchIcao) const
        {
            const QString airlineIcao = searchIcao.getAircraftDesignator();
            if (airlineIcao.isEmpty()) { return CAircraftMappingList(); }
            return this->findBy([ = ](const CAircraftMapping & mapping)
            {
                return mapping.getIcao().getAirlineDesignator() == airlineIcao;
            });
        }

        CAircraftMappingList CAircraftMappingList::findByIcaoAircraftAndAirlineDesignator(const CAircraftIcaoData &searchIcao, bool allowRelaxedAirline) const
        {
            CAircraftMappingList aircraftSearch = findByIcaoAircraftDesignator(searchIcao);
            if (aircraftSearch.isEmpty()) { return aircraftSearch; }

            CAircraftMappingList aircraftAndAirlineSearch = aircraftSearch.findByIcaoAirlineDesignator(searchIcao);
            if (!aircraftAndAirlineSearch.isEmpty()) { return aircraftAndAirlineSearch; }
            return allowRelaxedAirline ? aircraftSearch : aircraftAndAirlineSearch;
        }

        CAircraftMappingList CAircraftMappingList::findByIcaoCodeExact(const CAircraftIcaoData &searchIcao) const
        {
            return this->findBy(&CAircraftMapping::getIcao, searchIcao);
        }

        CAircraftMappingList CAircraftMappingList::findByModelString(const QString &modelString, Qt::CaseSensitivity sensitivity) const
        {
            return this->findBy([ = ](const CAircraftMapping & mapping)
            {
                return mapping.matchesModelString(modelString, sensitivity);
            });
        }

        void CAircraftMappingList::registerMetadata()
        {
            qRegisterMetaType<BlackMisc::CSequence<CAircraftMapping>>();
            qDBusRegisterMetaType<BlackMisc::CSequence<CAircraftMapping>>();
            qRegisterMetaType<BlackMisc::CCollection<CAircraftMapping>>();
            qDBusRegisterMetaType<BlackMisc::CCollection<CAircraftMapping>>();
            qRegisterMetaType<CAircraftMappingList>();
            qDBusRegisterMetaType<CAircraftMappingList>();
            registerMetaValueType<CAircraftMappingList>();
        }

    } // namespace
} // namespace
