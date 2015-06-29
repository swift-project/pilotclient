/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "cloudlayerlist.h"

using namespace BlackMisc::Aviation;

namespace BlackMisc
{
    namespace Weather
    {
        CCloudLayerList::CCloudLayerList(const CSequence<CCloudLayer> &other) :
            CSequence<CCloudLayer>(other)
        { }

        bool CCloudLayerList::containsCeiling(const CAltitude &ceiling) const
        {
            return contains(&CCloudLayer::getCeiling, ceiling);
        }

        CCloudLayer CCloudLayerList::findByCeiling(const CAltitude &ceiling) const
        {
            return findFirstByOrDefault(&CCloudLayer::getCeiling, ceiling);
        }

    } // namespace
} // namespace
