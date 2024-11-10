// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_AIRCRAFTMODELVIEW_H
#define BLACKGUI_AIRCRAFTMODELVIEW_H

#include "blackgui/blackguiexport.h"
#include "blackgui/models/aircraftmodellistmodel.h"
#include "blackgui/views/viewdbobjects.h"
#include "misc/aviation/airlineicaocode.h"
#include "misc/propertyindexvariantmap.h"
#include "misc/simulation/aircraftmodellist.h"
#include "misc/simulation/distributor.h"
#include "misc/simulation/simulatorinfo.h"
#include "misc/statusmessage.h"

#include <QBrush>
#include <QFlags>
#include <QMetaType>
#include <QObject>
#include <QStringList>
#include <Qt>

class QDropEvent;

namespace swift::misc::aviation
{
    class CAircraftIcaoCode;
    class CLivery;
}
namespace swift::misc::simulation
{
    class CAircraftModel;
}
namespace BlackGui
{
    namespace Filters
    {
        class CAircraftModelFilterDialog;
    }
    namespace Menus
    {
        class CMenuActions;
    }
    namespace Views
    {
        class CAircraftModelStatisticsDialog;
        class CAircraftModelValidationDialog;

        //! Aircraft view
        class BLACKGUI_EXPORT CAircraftModelView :
            public COrderableViewWithDbObjects<Models::CAircraftModelListModel>
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CAircraftModelView(QWidget *parent = nullptr);

            //! Set display mode
            void setAircraftModelMode(Models::CAircraftModelListModel::AircraftModelMode mode);

            //! Apply to selected objects
            int applyToSelected(const swift::misc::aviation::CLivery &livery);

            //! Apply to selected objects
            int applyToSelected(const swift::misc::aviation::CAircraftIcaoCode &icao);

            //! Apply to selected objects
            int applyToSelected(const swift::misc::simulation::CDistributor &distributor);

            //! Apply to selected objects
            int applyToSelected(const swift::misc::CPropertyIndexVariantMap &vm);

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
            int removeModelsWithModelString(const swift::misc::simulation::CAircraftModelList &models, Qt::CaseSensitivity sensitivity = Qt::CaseInsensitive);

            //! Replace models with sme model string, otherwise add
            int replaceOrAddModelsWithString(const swift::misc::simulation::CAircraftModelList &models, Qt::CaseSensitivity sensitivity = Qt::CaseInsensitive);

            //! \copydoc BlackGui::Models::CAircraftModelListModel::setHighlightModels
            void setHighlightModels(const swift::misc::simulation::CAircraftModelList &highlightModels);

            //! \copydoc BlackGui::Models::CAircraftModelListModel::setHighlightModelStrings(const QStringList &)
            void setHighlightModelStrings(const QStringList &highlightModels);

            //! Set the highlight models based on the saved file
            void setHighlightModelsForInvalidModels();

            //! \copydoc BlackGui::Models::CAircraftModelListModel::setHighlight
            void setHighlight(bool highlight);

            //! \copydoc BlackGui::Models::CAircraftModelListModel::setHighlightColor
            void setHighlightColor(const QBrush &brush);

            //! \copydoc BlackGui::Models::CAircraftModelListModel::highlightModels
            bool highlightModels() const;

            //! Corresponding simulator
            //! \remark used for validation or loading data will be restricted to simulator
            void setCorrespondingSimulator(const swift::misc::simulation::CSimulatorInfo &simulator, const QString &simDir);

            //! Enable the validation context menu
            void setValidationContextMenu(const bool enabled) { m_withValidationContextMenu = enabled; }

        signals:
            //! Request to stash if applicable
            void requestStash(const swift::misc::simulation::CAircraftModelList &models);

            //! Request further handling of drops I cannot handle on my own
            void requestHandlingOfStashDrop(const swift::misc::aviation::CAirlineIcaoCode &airlineIcao);

            //! Temp.disable model
            void requestTempDisableModelsForMatching(const swift::misc::simulation::CAircraftModelList &models);

            //! Highlight stashed models has been toggled
            void toggledHighlightStashedModels();

            //! Models for simulator loaded (JSON)
            void jsonModelsForSimulatorLoaded(const swift::misc::simulation::CSimulatorInfo &simulator);

        protected:
            //! \copydoc QTableView::dropEvent
            //! \sa BlackGui::Models::CListModelBase::dropMimeData
            virtual void dropEvent(QDropEvent *event) override;

            //! \name View base class overrides
            //! @{

            //! \copydoc BlackGui::Views::COrderableViewWithDbObjects::customMenu
            virtual void customMenu(Menus::CMenuActions &menuActions) override;

            //! \copydoc BlackGui::Views::COrderableViewWithDbObjects::modifyLoadedJsonData
            virtual swift::misc::CStatusMessage modifyLoadedJsonData(swift::misc::simulation::CAircraftModelList &models) const override;

            //! \copydoc BlackGui::Views::COrderableViewWithDbObjects::validateLoadedJsonData
            virtual swift::misc::CStatusMessage validateLoadedJsonData(const swift::misc::simulation::CAircraftModelList &models) const override;

            //! \copydoc BlackGui::Views::COrderableViewWithDbObjects::jsonLoadedAndModelUpdated
            virtual void jsonLoadedAndModelUpdated(const swift::misc::simulation::CAircraftModelList &models) override;
            //! @}

        private:
            //! Toggle highlight invalid models
            void toggleHighlightInvalidModels();

            //! Toggle if stashing unselects
            void stashingClearsSelection();

            //! Highlight stashed models
            void toggleHighlightStashedModels();

            //! Stash shortcut pressed
            void requestedStash();

            //! Requested a temporary disabled model
            void requestTempDisable();

            //! Dialog about model statistics
            void displayModelStatisticsDialog();

            //! File validation dialog
            void displayModelValidationDialog();

            bool m_stashingClearsSelection = true; //!< stashing unselects
            bool m_withValidationContextMenu = true; //!< validation didalog context menu
            CAircraftModelStatisticsDialog *m_statisticsDialog = nullptr;
            CAircraftModelValidationDialog *m_fileValidationDialog = nullptr;
            swift::misc::simulation::CSimulatorInfo m_correspondingSimulator; //!< validation, simulator required when loading
            QString m_correspondingSimulatorDir;
        };
    } // ns
} // ns

#endif // guard
