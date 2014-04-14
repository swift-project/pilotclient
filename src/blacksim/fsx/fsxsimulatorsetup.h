/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
                SetupSimConnectCfgFile = 1000
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
