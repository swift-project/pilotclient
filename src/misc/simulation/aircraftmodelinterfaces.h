// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_SIMULATION_AIRCRAFTMODELINTERFACES_H
#define SWIFT_MISC_SIMULATION_AIRCRAFTMODELINTERFACES_H

#include "misc/simulation/aircraftmodellist.h"
#include "misc/swiftmiscexport.h"

namespace swift::misc::simulation
{
    //! Interface to "something" backing models, which can be set
    class SWIFT_MISC_EXPORT IModelsSetable
    {
    public:
        //! Set models
        virtual void setModels(const CAircraftModelList &models) = 0;

        //! @{
        //! Ctor, dtor, copy operators (CLANG warnings)
        IModelsSetable() {}
        virtual ~IModelsSetable() {}
        IModelsSetable(const IModelsSetable &) = default;
        IModelsSetable &operator=(const IModelsSetable &) = default;
        //! @}
    };

    //! Interface to "something" backing models, which can be modified (updated)
    class SWIFT_MISC_EXPORT IModelsUpdatable
    {
    public:
        //! Update models
        virtual int updateModels(const CAircraftModelList &models) = 0;

        //! @{
        //! Ctor, dtor, copy operators (CLANG warnings)
        IModelsUpdatable() {}
        virtual ~IModelsUpdatable() {}
        IModelsUpdatable(const IModelsUpdatable &) = default;
        IModelsUpdatable &operator=(const IModelsUpdatable &) = default;
        //! @}
    };

    //! Interface to "something" backing models, which can be set
    class SWIFT_MISC_EXPORT IModelsForSimulatorSetable
    {
    public:
        //! Set models
        virtual void setModelsForSimulator(const CAircraftModelList &models, const CSimulatorInfo &simulator) = 0;

        //! @{
        //! Ctor, dtor, copy operators (CLANG warnings)
        IModelsForSimulatorSetable() {}
        virtual ~IModelsForSimulatorSetable() {}
        IModelsForSimulatorSetable(const IModelsForSimulatorSetable &) = default;
        IModelsForSimulatorSetable &operator=(const IModelsForSimulatorSetable &) = default;
        //! @}
    };

    //! Interface to "something" backing models, which can be modified (updated)
    class SWIFT_MISC_EXPORT IModelsForSimulatorUpdatable
    {
    public:
        //! Set models
        virtual int updateModelsForSimulator(const CAircraftModelList &models, const CSimulatorInfo &simulator) = 0;

        //! @{
        //! Ctor, dtor, copy operators (CLANG warnings)
        IModelsForSimulatorUpdatable() {}
        virtual ~IModelsForSimulatorUpdatable() {}
        IModelsForSimulatorUpdatable(const IModelsForSimulatorUpdatable &) = default;
        IModelsForSimulatorUpdatable &operator=(const IModelsForSimulatorUpdatable &) = default;
        //! @}
    };

    //! Interface to "something" allowing a simulator selection
    class SWIFT_MISC_EXPORT ISimulatorSelectable
    {
    public:
        //! Simulator
        virtual swift::misc::simulation::CSimulatorInfo getSelectedSimulator() const = 0;

        //! @{
        //! Ctor, dtor, copy operators (CLANG warnings)
        ISimulatorSelectable() {}
        virtual ~ISimulatorSelectable() {}
        ISimulatorSelectable(const ISimulatorSelectable &) = default;
        ISimulatorSelectable &operator=(const ISimulatorSelectable &) = default;
        //! @}
    };
} // namespace swift::misc::simulation

Q_DECLARE_INTERFACE(swift::misc::simulation::IModelsSetable, "org.swift-project.misc.simulation.imodelssetable")
Q_DECLARE_INTERFACE(swift::misc::simulation::IModelsUpdatable, "org.swift-project.misc.simulation.imodelsupdateable")
Q_DECLARE_INTERFACE(swift::misc::simulation::IModelsForSimulatorSetable, "org.swift-project.misc.simulation.IModelsForSimulatorSetable")
Q_DECLARE_INTERFACE(swift::misc::simulation::IModelsForSimulatorUpdatable, "org.swift-project.misc.simulation.imodelspersimulatorupdatabale")
Q_DECLARE_INTERFACE(swift::misc::simulation::ISimulatorSelectable, "org.swift-project.misc.simulation.isimulatorselectable")

#endif // guard
