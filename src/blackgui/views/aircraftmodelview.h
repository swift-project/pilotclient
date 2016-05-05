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
#include "viewdbobjects.h"

namespace BlackGui
{
    namespace Views
    {
        //! Aircraft view
        class BLACKGUI_EXPORT CAircraftModelView :
            public COrderableViewWithDbObjects<Models::CAircraftModelListModel, BlackMisc::Simulation::CAircraftModelList, BlackMisc::Simulation::CAircraftModel, int>
        {
            Q_OBJECT

        public:
            //! How to validate loaded JSON data
            enum JsonLoadFlag
            {
                NotSet                   = 0,
                AllowOnlySingleSimulator = 1 << 0,
                ReduceToOneSimulator     = 1 << 1
            };
            Q_DECLARE_FLAGS(JsonLoad, JsonLoadFlag)

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

            //! Apply to selected objects
            int applyToSelected(const BlackMisc::CPropertyIndexVariantMap &vm);

            //! Has any models to stash and it is allowed to stash
            bool hasSelectedModelsToStash() const;

            //! Add the technically supported metatypes allowed for drag and drop
            void setAcceptedMetaTypeIds();

            //! Add my own filter dialog
            void addFilterDialog();

            //! Filter dialog if any
            BlackGui::Filters::CAircraftModelFilterDialog *getFilterDialog() const;

            //! Remove models with model strings
            int removeModelsWithModelString(const QStringList &modelStrings, Qt::CaseSensitivity sensitivity = Qt::CaseInsensitive);

            //! Remove models with model strings
            int removeModelsWithModelString(const BlackMisc::Simulation::CAircraftModelList &models, Qt::CaseSensitivity sensitivity = Qt::CaseInsensitive);

            //! Replace models with sme model string, otherwise add
            int replaceOrAddModelsWithString(const BlackMisc::Simulation::CAircraftModelList &models, Qt::CaseSensitivity sensitivity  = Qt::CaseInsensitive);

            //! \copydoc BlackGui::Models::CAircraftModelListModel::setHighlightModelStrings(const QStringList &)
            void setHighlightModelStrings(const QStringList &highlightModels);

            //! \copydoc BlackGui::Models::CAircraftModelListModel::setHighlightModelStrings(bool)
            void setHighlightModelStrings(bool highlight);

            //! \copydoc BlackGui::Models::CAircraftModelListModel::setHighlightModelStringsColor
            void setHighlightModelStringsColor(const QBrush &brush);

            //! \copydoc BlackGui::Models::CAircraftModelListModel::highlightModelStrings
            bool highlightModelStrings() const;

            //! Load validation
            void setJsonLoad(JsonLoad jsonLoad) { m_jsonLoad = jsonLoad; }

        signals:
            //! Request to stash if applicable
            void requestStash(const BlackMisc::Simulation::CAircraftModelList &models);

            //! Highligh stashed models has been toggled
            void toggledHighlightStashedModels();

            //! Request further handling of drops I cannot handle on my own
            void requestHandlingOfStashDrop(const BlackMisc::Aviation::CAirlineIcaoCode &airlineIcao);

            //! Models for simulator loaded (JSON)
            void jsonModelsForSimulatorLoaded(const BlackMisc::Simulation::CSimulatorInfo &simulator);

        protected:
            //! \copydoc QTableView::dropEvent
            //! \sa BlackGui::Models::CListModelBase::dropMimeData
            virtual void dropEvent(QDropEvent *event) override;

            //! \name View base class overrides
            //! @{
            virtual void customMenu(BlackGui::Menus::CMenuActions &menuActions) override;
            virtual BlackMisc::CStatusMessage modifyLoadedJsonData(BlackMisc::Simulation::CAircraftModelList &models) const override;
            virtual BlackMisc::CStatusMessage validateLoadedJsonData(const BlackMisc::Simulation::CAircraftModelList &models) const override;
            virtual void jsonLoadedAndModelUpdated(const BlackMisc::Simulation::CAircraftModelList &models) override;
            //! @}

        private slots:
            //! Highlight stashed models
            void ps_toggleHighlightStashedModels();

            //! Toggle highlight invalid models
            void ps_toogleHighlightInvalidModels();

            //! Toggle if stashing unselects
            void ps_stashingClearsSelection();

            //! Stash shortcut pressed
            void ps_requestStash();

        private:
            bool     m_stashingClearsSelection = true;   //!< stashing unselects
            JsonLoad m_jsonLoad                = NotSet; //!< Loaded JSON validation
        };
    } // ns
} // ns

Q_DECLARE_METATYPE(BlackGui::Views::CAircraftModelView::JsonLoad)
Q_DECLARE_METATYPE(BlackGui::Views::CAircraftModelView::JsonLoadFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(BlackGui::Views::CAircraftModelView::JsonLoad)

#endif // guard
