/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "simulatorsetup.h"

namespace BlackMisc
{
    namespace Simulation
    {
        void CSimulatorSetup::setSettings(const BlackMisc::CPropertyIndexVariantMap &map)
        {
            this->m_setup = map;
        }

        void CSimulatorSetup::init()
        {
            // void
        }
    } // ns
} // ns
