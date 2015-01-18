/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_SIMULATEDAIRCRAFTLISTMODEL_H
#define BLACKGUI_SIMULATEDAIRCRAFTLISTMODEL_H

#include "blackmisc/simulation/simulatedaircraftlist.h"
#include "blackgui/models/listmodelbase.h"
#include <QAbstractItemModel>
#include <QDBusConnection>

namespace BlackGui
{
    namespace Models
    {

        //! Aircraft list model
        class CSimulatedAircraftListModel : public CListModelBase<BlackMisc::Simulation::CSimulatedAircraft, BlackMisc::Simulation::CSimulatedAircraftList>
        {

        public:
            //! Model modes
            enum AircraftMode
            {
                InfoMode,  //!< like aircraft in range
                ModelMode, //!< focusing on used model
                NotSet
            };

            //! Constructor
            explicit CSimulatedAircraftListModel(QObject *parent = nullptr);

            //! Destructor
            virtual ~CSimulatedAircraftListModel() {}

            //! Mode
            void setAircraftMode(AircraftMode mode);

        private:
            AircraftMode m_mode = NotSet;
        };
    }
}
#endif // guard
