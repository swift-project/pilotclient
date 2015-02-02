/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "nwclientlist.h"
#include "predicates.h"

using namespace BlackMisc::Aviation;

namespace BlackMisc
{
    namespace Network
    {
        /*
         * Default constructor
         */
        CClientList::CClientList() { }

        /*
         * Construct from base class object
         */
        CClientList::CClientList(const CSequence &other) : CSequence<CClient>(other)
        { }

        /*
         * Register metadata
         */
        void CClientList::registerMetadata()
        {
            qRegisterMetaType<BlackMisc::CSequence<CClient>>();
            qDBusRegisterMetaType<BlackMisc::CSequence<CClient>>();
            qRegisterMetaType<BlackMisc::CCollection<CClient>>();
            qDBusRegisterMetaType<BlackMisc::CCollection<CClient>>();
            qRegisterMetaType<CClientList>();
            qDBusRegisterMetaType<CClientList>();
            registerMetaValueType<CClientList>();
        }

    } // namespace
} // namespace
