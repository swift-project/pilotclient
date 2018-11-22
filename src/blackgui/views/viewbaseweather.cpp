/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "viewbase.cpp"

namespace BlackGui
{
    namespace Views
    {
        template class CViewBase<BlackGui::Models::CCloudLayerListModel, BlackMisc::Weather::CCloudLayerList, BlackMisc::Weather::CCloudLayer>;
        template class CViewBase<BlackGui::Models::CTemperatureLayerListModel, BlackMisc::Weather::CTemperatureLayerList, BlackMisc::Weather::CTemperatureLayer>;
        template class CViewBase<BlackGui::Models::CWindLayerListModel, BlackMisc::Weather::CWindLayerList, BlackMisc::Weather::CWindLayer>;
    } // namespace
} // namespace
