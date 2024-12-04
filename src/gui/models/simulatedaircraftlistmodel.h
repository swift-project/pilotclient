// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_MODELS_SIMULATEDAIRCRAFTLISTMODEL_H
#define SWIFT_GUI_MODELS_SIMULATEDAIRCRAFTLISTMODEL_H

#include "gui/models/listmodelcallsignobjects.h"
#include "gui/swiftguiexport.h"
#include "misc/simulation/simulatedaircraftlist.h"

namespace swift::misc::simulation
{
    class CSimulatedAircraft;
}
namespace swift::gui::models
{
    //! Aircraft list model
    class SWIFT_GUI_EXPORT CSimulatedAircraftListModel :
        public CListModelCallsignObjects<swift::misc::simulation::CSimulatedAircraftList, true>
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
} // namespace swift::gui::models
#endif // SWIFT_GUI_MODELS_SIMULATEDAIRCRAFTLISTMODEL_H
