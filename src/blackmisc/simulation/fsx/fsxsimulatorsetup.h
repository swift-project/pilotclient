/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_FSX_SIMSETUP_H
#define BLACKMISC_SIMULATION_FSX_SIMSETUP_H

#include "blackmisc/blackmiscexport.h"
#include "../simulatorsetup.h"
#include <QString>

namespace BlackMisc
{
    namespace Simulation
    {
        namespace Fsx
        {
            //! Simulator settings for FSX Flight simulators
            class BLACKMISC_EXPORT CFsxSimulatorSetup
            {

            public:
                //! No constructor
                CFsxSimulatorSetup() = delete;

                //! Init, to be used where FSX runs
                static BlackMisc::Simulation::CSimulatorSetup getInitialSetup();

                //! Key
                static const QString &KeyLocalSimConnectCfgFilename();

            };
        } // namespace
    } // namespace
} // namespace

#endif // guard
