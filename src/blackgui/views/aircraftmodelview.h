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
#include "blackgui/filters/aircraftmodelfilterdialog.h"
#include "blackgui/models/aircraftmodellistmodel.h"
#include "viewbase.h"

namespace BlackGui
{
    namespace Views
    {
        //! Aircraft view
        class BLACKGUI_EXPORT CAircraftModelView :
            public CViewBase<Models::CAircraftModelListModel, BlackMisc::Simulation::CAircraftModelList, BlackMisc::Simulation::CAircraftModel>
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CAircraftModelView(QWidget *parent = nullptr);

            //! Set display mode
            void setAircraftModelMode(Models::CAircraftModelListModel::AircraftModelMode mode);

            //! Display automatically (when models are loaded)
            bool displayAutomatically() const;

            //! Display automatically (when models are loaded)
            void setDisplayAutomatically(bool automatically) { m_displayAutomatically = automatically; }

        signals:
            //! Request to load VPilot data
            void requestVPilotRules();

        protected:
            //! \copydoc CViewBase::customMenu
            virtual void customMenu(QMenu &menu) const override;

        private slots:
            //! Toggle auto display
            void ps_toggleAutoDisplay();

            //! Highlight DB models
            void ps_toggleHighlightDbModels();

        private:
            bool m_displayAutomatically = false; //!< display automatically (when models are loaded)
        };
    } // ns
} // ns
#endif // guard
