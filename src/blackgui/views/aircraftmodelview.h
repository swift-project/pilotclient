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

#include "blackgui/blackguiexport.h"
#include "aircraftmodelfilterform.h"
#include "viewbase.h"
#include "../models/aircraftmodellistmodel.h"

namespace BlackGui
{
    namespace Views
    {
        //! Aircrafts view
        class BLACKGUI_EXPORT CAircraftModelView : public CViewBase<Models::CAircraftModelListModel, BlackMisc::Simulation::CAircraftModelList, BlackMisc::Simulation::CAircraftModel>
        {
            Q_OBJECT

        public:

            //! Constructor
            explicit CAircraftModelView(QWidget *parent = nullptr);

            //! Set display mode
            void setAircraftModelMode(Models::CAircraftModelListModel::AircraftModelMode mode);

            //! Display automatically (when models are loaded)
            bool displayAutomatically() const;

        signals:
            //! Request reloading of backend models
            void requestModelReload();

        protected slots:
            //! \copydoc CViewBaseNonTemplate::ps_filterDialogFinished
            virtual bool ps_filterDialogFinished(int status) override;

        protected:
            //! \copydoc CViewBase::customMenu
            virtual void customMenu(QMenu &menu) const override;

        private slots:
            //! Toggle auto display
            void ps_toggleAutoDisplay();

        private:
            //! Filter form
            CAircraftModelFilterForm *getFilterForm() const;
            bool m_displayAutomatically = false;
        };
    } // ns
} // ns
#endif // guard
