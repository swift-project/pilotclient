/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_SIMSETUP_H
#define BLACKMISC_SIMULATION_SIMSETUP_H

#include "blackmisc/propertyindexvariantmap.h"
#include <QMap>
#include <QString>

namespace BlackMisc
{
    namespace Simulation
    {
        /*!
         * \brief Simulator settings for flight simulators
         * \details Represents the generic part of a simulator setup ("common denominator"),
         *          details kept in specialized classes
         */
        class CSimulatorSetup
        {
        protected:
            BlackMisc::CPropertyIndexVariantMap m_setup; //!< values describing the simulator setup (path, config files)

        protected:
            //! Default constructor
            CSimulatorSetup() {}

            //! Constructor
            CSimulatorSetup(const BlackMisc::CPropertyIndexVariantMap &map) : m_setup(map) {}

        public:
            //! Specific values
            enum
            {
                SetupSimPath = BlackMisc::CPropertyIndex::GlobalIndexAbuseMode
            };

            //! Settings
            BlackMisc::CPropertyIndexVariantMap getSettings() const { return this->m_setup;}

            //! Settings
            void setSettings(const BlackMisc::CPropertyIndexVariantMap &map);

            //! Init, to be used where simulator runs
            void init();

        };
    } // namespace
} // namespace

#endif // guard
