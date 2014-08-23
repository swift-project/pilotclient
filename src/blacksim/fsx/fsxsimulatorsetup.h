/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSIM_FSX_SIMSETUP_H
#define BLACKSIM_FSX_SIMSETUP_H

#include "../simulatorsetup.h"
#include <QString>

namespace BlackSim
{
    namespace Fsx
    {

        /*!
         * \brief Simulator settings for FSX Flight sims
         */
        class CFsxSimulatorSetup : public BlackSim::CSimulatorSetup
        {

        public:

            enum
            {
                SetupSimConnectCfgFile = 1000, //!< location of simconnect.cfg file
            };


            //! Default constructor
            CFsxSimulatorSetup() : BlackSim::CSimulatorSetup() {}

            //! Constructor
            CFsxSimulatorSetup(const BlackMisc::CIndexVariantMap &map) : BlackSim::CSimulatorSetup(map) {}

            //! Init, to be used where FSX runs
            void init();
        };
    } // namespace
} // namespace

#endif // guard
