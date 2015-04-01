/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "aircraftpartslist.h"

namespace BlackMisc
{
    namespace Aviation
    {

        CAircraftPartsList::CAircraftPartsList() { }

        CAircraftPartsList::CAircraftPartsList(const CSequence<CAircraftParts> &other) :
            CSequence<CAircraftParts>(other)
        { }

        void CAircraftPartsList::registerMetadata()
        {
            qRegisterMetaType<BlackMisc::CSequence<CAircraftParts>>();
            qDBusRegisterMetaType<BlackMisc::CSequence<CAircraftParts>>();
            qRegisterMetaType<BlackMisc::CCollection<CAircraftParts>>();
            qDBusRegisterMetaType<BlackMisc::CCollection<CAircraftParts>>();
            qRegisterMetaType<CAircraftPartsList>();
            qDBusRegisterMetaType<CAircraftPartsList>();
            registerMetaValueType<CAircraftPartsList>();
        }

    } // namespace
} // namespace
