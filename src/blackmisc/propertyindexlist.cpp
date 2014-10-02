/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "propertyindexlist.h"

namespace BlackMisc
{
    /*
     * Default constructor
     */
    CPropertyIndexList::CPropertyIndexList() { }

    /*
     * Construct from base class object
     */
    CPropertyIndexList::CPropertyIndexList(const CSequence<CPropertyIndex> &other) :
        CSequence<CPropertyIndex>(other)
    { }

    /*
     * Register metadata
     */
    void CPropertyIndexList::registerMetadata()
    {
        qRegisterMetaType<BlackMisc::CSequence<CPropertyIndex>>();
        qDBusRegisterMetaType<BlackMisc::CSequence<CPropertyIndex>>();
        qRegisterMetaType<BlackMisc::CCollection<CPropertyIndex>>();
        qDBusRegisterMetaType<BlackMisc::CCollection<CPropertyIndex>>();
        qRegisterMetaType<CPropertyIndexList>();
        qDBusRegisterMetaType<CPropertyIndexList>();
    }

} // namespace
