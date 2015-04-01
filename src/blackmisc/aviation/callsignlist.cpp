/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/aviation/callsignlist.h"
#include "blackmisc/predicates.h"

namespace BlackMisc
{
    namespace Aviation
    {

        CCallsignList::CCallsignList() { }

        CCallsignList::CCallsignList(const CCollection<CCallsign> &other) :
            CCollection<CCallsign>(other)
        { }

        void CCallsignList::registerMetadata()
        {
            qRegisterMetaType<BlackMisc::CSequence<CCallsign>>();
            qDBusRegisterMetaType<BlackMisc::CSequence<CCallsign>>();
            qRegisterMetaType<BlackMisc::CCollection<CCallsign>>();
            qDBusRegisterMetaType<BlackMisc::CCollection<CCallsign>>();
            qRegisterMetaType<CCallsignList>();
            qDBusRegisterMetaType<CCallsignList>();
            registerMetaValueType<CCallsignList>();
        }

    } // namespace
} // namespace
