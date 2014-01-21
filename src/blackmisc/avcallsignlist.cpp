/* Copyright (C) 2013 VATSIM Community / authors
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
