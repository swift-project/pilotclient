// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_MODELS_SIMULATEDAIRCRAFTLISTMODEL_H
#define BLACKGUI_MODELS_SIMULATEDAIRCRAFTLISTMODEL_H

#include "blackgui/blackguiexport.h"
#include "blackgui/models/listmodelcallsignobjects.h"
#include "blackmisc/simulation/simulatedaircraftlist.h"

namespace BlackMisc::Simulation
{
    class CSimulatedAircraft;
}
namespace BlackGui::Models
{
    //! Aircraft list model
    class BLACKGUI_EXPORT CSimulatedAircraftListModel :
        public CListModelCallsignObjects<BlackMisc::Simulation::CSimulatedAircraftList, true>
    {
        Q_OBJECT

    public:
        //! Model modes
        enum AircraftMode
        {
            NetworkMode, //!< like aircraft in range
            RenderedMode, //!< focusing on used model
            NotSet
        };

        //! Constructor
        explicit CSimulatedAircraftListModel(QObject *parent = nullptr);

        //! Destructor
        virtual ~CSimulatedAircraftListModel() override {}

        //! Mode
        void setAircraftMode(AircraftMode mode);

    private:
        AircraftMode m_mode = NotSet;
    };
} // ns
#endif // guard
