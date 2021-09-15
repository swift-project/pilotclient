/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_DBMODELKEYCOMPLETER_H
#define BLACKGUI_COMPONENTS_DBMODELKEYCOMPLETER_H

#include "blackmisc/simulation/simulatorinfo.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include <QLineEdit>

namespace BlackGui::Components
{
    /**
     * Completer for model keys
     */
    class CDbModelKeyCompleter : public QLineEdit
    {
        Q_OBJECT

    public:
        //! Ctor
        CDbModelKeyCompleter(QWidget *parent = nullptr);

        //! Limit models to simulator, or add unspecified
        void setSimulator(const BlackMisc::Simulation::CSimulatorInfo &simulator);

        //! Aircraft model for current string (searched in DB data)
        BlackMisc::Simulation::CAircraftModel getAircraftModel() const;

    private:
        //! Models have been read
        void onModelsRead();

        //! Init completer
        void initCompleter();

        BlackMisc::Simulation::CSimulatorInfo m_simulator { BlackMisc::Simulation::CSimulatorInfo::All };
    };
} // ns

#endif // guard
