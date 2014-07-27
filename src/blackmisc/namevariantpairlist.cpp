/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "namevariantpairlist.h"
#include "predicates.h"

namespace BlackMisc
{
    /*
     * Empty constructor
     */
    CNameVariantPairList::CNameVariantPairList() { }

    /*
     * Construct from base class object
     */
    CNameVariantPairList::CNameVariantPairList(const CSequence<CNameVariantPair> &other) :
        CSequence<CNameVariantPair>(other)
    { }

    /*
     * Register metadata
     */
    void CNameVariantPairList::registerMetadata()
    {
        qRegisterMetaType<BlackMisc::CSequence<CNameVariantPair>>();
        qDBusRegisterMetaType<BlackMisc::CSequence<CNameVariantPair>>();
        qRegisterMetaType<BlackMisc::CCollection<CNameVariantPair>>();
        qDBusRegisterMetaType<BlackMisc::CCollection<CNameVariantPair>>();
        qRegisterMetaType<CNameVariantPairList>();
        qDBusRegisterMetaType<CNameVariantPairList>();
    }
} // namespace
