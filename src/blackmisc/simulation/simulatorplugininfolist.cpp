/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/iterator.h"
#include "blackmisc/metaclassprivate.h"
#include "blackmisc/range.h"
#include "blackmisc/simulation/simulatorplugininfo.h"
#include "blackmisc/simulation/simulatorplugininfolist.h"

#include <algorithm>
#include <tuple>

namespace BlackMisc
{
    namespace Simulation
    {

        CSimulatorPluginInfoList::CSimulatorPluginInfoList() { }

        bool CSimulatorPluginInfoList::supportsSimulator(const QString &simulator)
        {
            return std::find_if(begin(), end(), [&simulator](const CSimulatorPluginInfo & info)
            {
                return info.getSimulator() == simulator;
            }) != end();
        }

        QStringList CSimulatorPluginInfoList::toStringList(bool i18n) const
        {
            return this->transform([i18n](const CSimulatorPluginInfo & info) { return info.toQString(i18n); });
        }

    } // namespace
} // namespace
