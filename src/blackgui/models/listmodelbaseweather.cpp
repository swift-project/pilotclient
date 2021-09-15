/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "listmodelbase.cpp"


namespace BlackGui::Models
{
    // see here for the reason of thess forward instantiations
    // https://isocpp.org/wiki/faq/templates#separate-template-fn-defn-from-decl
    template class CListModelBase<BlackMisc::Weather::CCloudLayerList, false>;
    template class CListModelBase<BlackMisc::Weather::CTemperatureLayerList, false>;
    template class CListModelBase<BlackMisc::Weather::CWindLayerList, false>;

} // namespace
