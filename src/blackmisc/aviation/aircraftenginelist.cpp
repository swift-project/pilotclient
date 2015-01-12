/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "aircraftenginelist.h"

namespace BlackMisc
{
    namespace Aviation
    {
        CAircraftEngineList::CAircraftEngineList(const CSequence<CAircraftEngine> &other) :
            CSequence<CAircraftEngine>(other)
        { }

        void CAircraftEngineList::registerMetadata()
        {
            qRegisterMetaType<BlackMisc::CSequence<CAircraftEngine>>();
            qDBusRegisterMetaType<BlackMisc::CSequence<CAircraftEngine>>();
            qRegisterMetaType<BlackMisc::CCollection<CAircraftEngine>>();
            qDBusRegisterMetaType<BlackMisc::CCollection<CAircraftEngine>>();
            qRegisterMetaType<CAircraftEngineList>();
            qDBusRegisterMetaType<CAircraftEngineList>();
            registerMetaValueType<CAircraftEngineList>();
        }
    } // namespace
} // namespace
