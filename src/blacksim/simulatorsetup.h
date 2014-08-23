/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKSIM_SIMSETUP_H
#define BLACKSIM_SIMSETUP_H

#include "blackmisc/indexvariantmap.h"
#include <QMap>
#include <QString>

namespace BlackSim
{
    /*!
     * \brief Simulator settings for flight simulators
     * \details Represents the generic part of a simulator setup ("common denominator"),
     *          details kept in specialized classes
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
