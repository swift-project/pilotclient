/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKSIM_SIMSETUP_H
#define BLACKSIM_SIMSETUP_H

#include "blackmisc/indexvariantmap.h"
#include <QMap>
#include <QString>

namespace BlackSim
{
    /*!
     * \brief Simulator settings for MS Flight sims
     */
    class CSimulatorSetup
    {
    protected:
        BlackMisc::CIndexVariantMap m_setup; //!< values describing the simulator setup (path, config files)

    protected:
        //! Default constructor
        CSimulatorSetup() {}

        //! Constructor
        CSimulatorSetup(const BlackMisc::CIndexVariantMap &map) : m_setup(map) {}

    public:

        enum
        {
            SetupSimPath
        };

        //! Settings
        BlackMisc::CIndexVariantMap getSettings() const { return this->m_setup;}

        //! Settings
        void setSettings(const BlackMisc::CIndexVariantMap &map);

        //! Init, to be used where simulator runs
        void init();

    };
} // namespace

#endif // guard
