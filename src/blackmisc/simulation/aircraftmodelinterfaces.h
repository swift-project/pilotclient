/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_SIMULATION_AIRCRAFTMODELINTERFACES_H
#define BLACKMISC_SIMULATION_AIRCRAFTMODELINTERFACES_H

#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/blackmiscexport.h"

namespace BlackMisc
{
    namespace Simulation
    {
        //! Interface to "something" backing models, which can be set
        class BLACKMISC_EXPORT IModelsSetable
        {
        public:
            //! Set models
            virtual void setModels(const CAircraftModelList &models) = 0;
        };

        //! Interface to "something" backing models, which can be modified (updated)
        class BLACKMISC_EXPORT IModelsUpdatable
        {
        public:
            //! Update models
            virtual int updateModels(const CAircraftModelList &models) = 0;
        };

        //! Interface to "something" backing models, which can be set
        class BLACKMISC_EXPORT IModelsForSimulatorSetable
        {
        public:
            //! Set models
            virtual void setModelsForSimulator(const CAircraftModelList &models, const CSimulatorInfo &simulator) = 0;
        };

        //! Interface to "something" backing models, which can be modified (updated)
        class BLACKMISC_EXPORT IModelsForSimulatorUpdatable
        {
        public:
            //! Set models
            virtual int updateModelsForSimulator(const CAircraftModelList &models, const CSimulatorInfo &simulator) = 0;
        };

        //! Interface to "something" allowing a simulator selection
        class BLACKMISC_EXPORT ISimulatorSelectable
        {
        public:
            //! Simulator
            virtual BlackMisc::Simulation::CSimulatorInfo getSelectedSimulator() const = 0;
        };
    } // namespace
} // namespace

Q_DECLARE_INTERFACE(BlackMisc::Simulation::IModelsSetable, "org.swift-project.blackmisc.simulation.imodelssetable")
Q_DECLARE_INTERFACE(BlackMisc::Simulation::IModelsUpdatable, "org.swift-project.blackmisc.simulation.imodelsupdateable")
Q_DECLARE_INTERFACE(BlackMisc::Simulation::IModelsForSimulatorSetable, "org.swift-project.blackmisc.simulation.IModelsForSimulatorSetable")
Q_DECLARE_INTERFACE(BlackMisc::Simulation::IModelsForSimulatorUpdatable, "org.swift-project.blackmisc.simulation.imodelspersimulatorupdatabale")
Q_DECLARE_INTERFACE(BlackMisc::Simulation::ISimulatorSelectable, "org.swift-project.blackmisc.simulation.isimulatorselectable")

#endif // guard
