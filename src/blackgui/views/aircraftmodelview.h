/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_AIRCRAFTMODELVIEW_H
#define BLACKGUI_AIRCRAFTMODELVIEW_H

#include "aircraftmodelfilterform.h"
#include "viewbase.h"
#include "../models/aircraftmodellistmodel.h"

namespace BlackGui
{
    namespace Views
    {
        //! Aircrafts view
        class CAircraftModelView : public CViewBase<Models::CAircraftModelListModel, BlackMisc::Simulation::CAircraftModelList, BlackMisc::Simulation::CAircraftModel>
        {

        public:

            //! Constructor
            explicit CAircraftModelView(QWidget *parent = nullptr);

            //! Set display mode
            void setAircraftModelMode(Models::CAircraftModelListModel::AircraftModelMode mode);

        protected slots:
            //! \copydoc CViewBaseNonTemplate::ps_filterDialogFinished
            virtual bool ps_filterDialogFinished(int status) override;

        private:
            //! Filter form
            CAircraftModelFilterForm *getFilterForm() const;
        };
    }
}
#endif // guard
