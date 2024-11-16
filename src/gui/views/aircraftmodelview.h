// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_AIRCRAFTMODELVIEW_H
#define SWIFT_GUI_AIRCRAFTMODELVIEW_H

#include <QBrush>
#include <QFlags>
#include <QMetaType>
#include <QObject>
#include <QStringList>
#include <Qt>

#include "gui/models/aircraftmodellistmodel.h"
#include "gui/swiftguiexport.h"
#include "gui/views/viewdbobjects.h"
#include "misc/aviation/airlineicaocode.h"
#include "misc/propertyindexvariantmap.h"
#include "misc/simulation/aircraftmodellist.h"
#include "misc/simulation/distributor.h"
#include "misc/simulation/simulatorinfo.h"
#include "misc/statusmessage.h"

class QDropEvent;

namespace swift::misc::aviation
{
    class CAircraftIcaoCode;
    class CLivery;
} // namespace swift::misc::aviation
namespace swift::misc::simulation
{
    class CAircraftModel;
}
namespace swift::gui
{
    namespace filters
    {
        class CAircraftModelFilterDialog;
    }
    namespace menus
    {
        class CMenuActions;
    }
    namespace views
    {
        class CAircraftModelStatisticsDialog;
        class CAircraftModelValidationDialog;

        //! Aircraft view
        class SWIFT_GUI_EXPORT CAircraftModelView :
            public COrderableViewWithDbObjects<models::CAircraftModelListModel>
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CAircraftModelView(QWidget *parent = nullptr);

            //! Set display mode
            void setAircraftModelMode(models::CAircraftModelListModel::AircraftModelMode mode);

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
            swift::gui::filters::CAircraftModelFilterDialog *getFilterDialog() const;

            //! Remove models with model strings
            int removeModelsWithModelString(const QStringList &modelStrings, Qt::CaseSensitivity sensitivity = Qt::CaseInsensitive);

            //! Remove models with model strings
            int removeModelsWithModelString(const swift::misc::simulation::CAircraftModelList &models, Qt::CaseSensitivity sensitivity = Qt::CaseInsensitive);

            //! Replace models with sme model string, otherwise add
            int replaceOrAddModelsWithString(const swift::misc::simulation::CAircraftModelList &models, Qt::CaseSensitivity sensitivity = Qt::CaseInsensitive);

            //! \copydoc swift::gui::models::CAircraftModelListModel::setHighlightModels
            void setHighlightModels(const swift::misc::simulation::CAircraftModelList &highlightModels);

            //! \copydoc swift::gui::models::CAircraftModelListModel::setHighlightModelStrings(const QStringList &)
            void setHighlightModelStrings(const QStringList &highlightModels);

            //! Set the highlight models based on the saved file
            void setHighlightModelsForInvalidModels();

            //! \copydoc swift::gui::models::CAircraftModelListModel::setHighlight
            void setHighlight(bool highlight);

            //! \copydoc swift::gui::models::CAircraftModelListModel::setHighlightColor
            void setHighlightColor(const QBrush &brush);

            //! \copydoc swift::gui::models::CAircraftModelListModel::highlightModels
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
            //! \sa swift::gui::models::CListModelBase::dropMimeData
            virtual void dropEvent(QDropEvent *event) override;

            //! \name View base class overrides
            //! @{

            //! \copydoc swift::gui::views::COrderableViewWithDbObjects::customMenu
            virtual void customMenu(menus::CMenuActions &menuActions) override;

            //! \copydoc swift::gui::views::COrderableViewWithDbObjects::modifyLoadedJsonData
            virtual swift::misc::CStatusMessage modifyLoadedJsonData(swift::misc::simulation::CAircraftModelList &models) const override;

            //! \copydoc swift::gui::views::COrderableViewWithDbObjects::validateLoadedJsonData
            virtual swift::misc::CStatusMessage validateLoadedJsonData(const swift::misc::simulation::CAircraftModelList &models) const override;

            //! \copydoc swift::gui::views::COrderableViewWithDbObjects::jsonLoadedAndModelUpdated
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
    } // namespace views
} // namespace swift::gui

#endif // guard
