/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "avcallsignlist.h"
#include "predicates.h"

namespace BlackMisc
{
    namespace Aviation
    {
        /*
         * Empty constructor
         */
        CCallsignList::CCallsignList() { }

        /*
         * Construct from base class object
         */
        CCallsignList::CCallsignList(const CSequence<CCallsign> &other) :
            CSequence<CCallsign>(other)
        { }

        /*
         * Register metadata
         */
        void CCallsignList::registerMetadata()
        {
            qRegisterMetaType<BlackMisc::CSequence<CCallsign>>();
            qDBusRegisterMetaType<BlackMisc::CSequence<CCallsign>>();
            qRegisterMetaType<BlackMisc::CCollection<CCallsign>>();
            qDBusRegisterMetaType<BlackMisc::CCollection<CCallsign>>();
            qRegisterMetaType<CCallsignList>();
            qDBusRegisterMetaType<CCallsignList>();
        }
    } // namespace
} // namespace
