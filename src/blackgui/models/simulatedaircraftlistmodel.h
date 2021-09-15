/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_MODELS_SIMULATEDAIRCRAFTLISTMODEL_H
#define BLACKGUI_MODELS_SIMULATEDAIRCRAFTLISTMODEL_H

#include "blackgui/blackguiexport.h"
#include "blackgui/models/listmodelcallsignobjects.h"
#include "blackmisc/simulation/simulatedaircraftlist.h"

namespace BlackMisc::Simulation { class CSimulatedAircraft; }
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
            NetworkMode,  //!< like aircraft in range
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
