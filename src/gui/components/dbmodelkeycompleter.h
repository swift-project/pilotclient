// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_DBMODELKEYCOMPLETER_H
#define SWIFT_GUI_COMPONENTS_DBMODELKEYCOMPLETER_H

#include <QLineEdit>

#include "misc/simulation/aircraftmodel.h"
#include "misc/simulation/simulatorinfo.h"

namespace swift::gui::components
{
    /*!
     * Completer for model keys
     */
    class CDbModelKeyCompleter : public QLineEdit
    {
        Q_OBJECT

    public:
        //! Ctor
        CDbModelKeyCompleter(QWidget *parent = nullptr);

        //! Limit models to simulator, or add unspecified
        void setSimulator(const swift::misc::simulation::CSimulatorInfo &simulator);

        //! Aircraft model for current string (searched in DB data)
        swift::misc::simulation::CAircraftModel getAircraftModel() const;

    private:
        //! Models have been read
        void onModelsRead();

        //! Init completer
        void initCompleter();

        swift::misc::simulation::CSimulatorInfo m_simulator { swift::misc::simulation::CSimulatorInfo::All };
    };
} // namespace swift::gui::components

#endif // guard
