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

            //! Apply to selected objects
            int applyToSelected(const BlackMisc::Aviation::CLivery &livery);

            //! Apply to selected objects
            int applyToSelected(const BlackMisc::Aviation::CAircraftIcaoCode &icao);

            //! Apply to selected objects
            int applyToSelected(const BlackMisc::Simulation::CDistributor &distributor);

            //! Allow to stash
            void setAllowStash(bool stash) { m_allowStash = stash; }

            //! Has any models to stash amd is allowed to stash
            bool hasModelsToStash() const;

            //! Add the technically supported metatypes as allows
            void setImplementedMetaTypeIds();

            //! Add my own filter dialog
            void addFilterDialog();

        signals:
            //! Request to stash if applicable
            void requestStash();

            //! Request further handling of drops I cannot handle on my own
            void requestHandlingOfStashDrop(const BlackMisc::Aviation::CAirlineIcaoCode &airlineIcao);

        protected:
            //! \copydoc QTableView::dropEvent
            virtual void dropEvent(QDropEvent *event) override;

        private slots:
            //! Highlight DB models
            void ps_toggleHighlightDbModels();

            //! Highlight stashed models
            void ps_toggleHighlightStashedModels();

            //! Stash shortcut pressed
            void ps_stashShortcut();

        private:
            bool m_allowStash = false; //!< allow to stash

            //! Custom menu for the models which have been loaded
            class CHighlightDbModelsMenu : public BlackGui::IMenuDelegate
            {
            public:
                //! Constructor
                CHighlightDbModelsMenu(CAircraftModelView *parent, bool separatorAtEnd) : IMenuDelegate(parent, separatorAtEnd) {}

                //! \copydoc IMenuDelegate::customMenu
                virtual void customMenu(QMenu &menu) const override;
            };

            //! Custom menu for the models which have been loaded
            class CHighlightStashedModelsMenu : public BlackGui::IMenuDelegate
            {
            public:
                //! Constructor
                CHighlightStashedModelsMenu(CAircraftModelView *parent, bool separatorAtEnd) : IMenuDelegate(parent, separatorAtEnd) {}

                //! \copydoc IMenuDelegate::customMenu
                virtual void customMenu(QMenu &menu) const override;
            };
        };
    } // ns
} // ns
#endif // guard
