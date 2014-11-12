/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "nwaircraftmappinglist.h"
#include "predicates.h"

using namespace BlackMisc::Network;
using namespace BlackMisc::Aviation;

namespace BlackMisc
{
    namespace Network
    {
        /*
         * Empty constructor
         */
        CAircraftMappingList::CAircraftMappingList() { }

        /*
         * Construct from base class object
         */
        CAircraftMappingList::CAircraftMappingList(const CSequence<CAircraftMapping> &other) :
            CSequence<CAircraftMapping>(other)
        { }

        CAircraftMappingList CAircraftMappingList::findByIcaoCodeWildcard(const CAircraftIcao &searchIcao) const
        {
            return this->findBy([ = ](const CAircraftMapping &mapping)
            {
                return mapping.matchesWildcardIcao(searchIcao);
            });
        }

        CAircraftMappingList CAircraftMappingList::findByIcaoCodeExact(const CAircraftIcao &searchIcao) const
        {
            return this->findBy(&CAircraftMapping::getIcao, searchIcao);
        }

        CAircraftMappingList CAircraftMappingList::findByModelString(const QString modelString, Qt::CaseSensitivity sensitivity) const
        {
            return this->findBy([ = ](const CAircraftMapping &mapping)
            {
                return mapping.matchesModelString(modelString, sensitivity);
            });
        }

        /*
         * Register metadata
         */
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
