/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_AIRCRAFTMODELVIEW_H
#define BLACKGUI_AIRCRAFTMODELVIEW_H

#include "blackgui/blackguiexport.h"
#include "blackgui/models/aircraftmodellistmodel.h"
#include "blackgui/views/viewdbobjects.h"
#include "blackmisc/aviation/airlineicaocode.h"
#include "blackmisc/propertyindexvariantmap.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/distributor.h"
#include "blackmisc/simulation/simulatorinfo.h"
#include "blackmisc/statusmessage.h"

#include <QBrush>
#include <QFlags>
#include <QMetaType>
#include <QObject>
#include <QStringList>
#include <Qt>

class QDropEvent;

namespace BlackMisc::Aviation
{
    class CAircraftIcaoCode;
    class CLivery;
}
namespace BlackMisc::Simulation { class CAircraftModel; }
namespace BlackGui
{
    namespace Filters { class CAircraftModelFilterDialog; }
    namespace Menus   { class CMenuActions; }
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

            //! \copydoc BlackGui::Models::CAircraftModelListModel::setHighlightModels
            void setHighlightModels(const BlackMisc::Simulation::CAircraftModelList &highlightModels);

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
            void setCorrespondingSimulator(const BlackMisc::Simulation::CSimulatorInfo &simulator, const QString &simDir);

            //! Enable the validation context menu
            void setValidationContextMenu(const bool enabled) { m_withValidationContextMenu = enabled; }

        signals:
            //! Request to stash if applicable
            void requestStash(const BlackMisc::Simulation::CAircraftModelList &models);

            //! Request further handling of drops I cannot handle on my own
            void requestHandlingOfStashDrop(const BlackMisc::Aviation::CAirlineIcaoCode &airlineIcao);

            //! Temp.disable model
            void requestTempDisableModelsForMatching(const BlackMisc::Simulation::CAircraftModelList &models);

            //! Highlight stashed models has been toggled
            void toggledHighlightStashedModels();

            //! Models for simulator loaded (JSON)
            void jsonModelsForSimulatorLoaded(const BlackMisc::Simulation::CSimulatorInfo &simulator);

        protected:
            //! \copydoc QTableView::dropEvent
            //! \sa BlackGui::Models::CListModelBase::dropMimeData
            virtual void dropEvent(QDropEvent *event) override;

            //! \name View base class overrides
            //! @{

            //! \copydoc BlackGui::Views::COrderableViewWithDbObjects::customMenu
            virtual void customMenu(Menus::CMenuActions &menuActions) override;

            //! \copydoc BlackGui::Views::COrderableViewWithDbObjects::modifyLoadedJsonData
            virtual BlackMisc::CStatusMessage modifyLoadedJsonData(BlackMisc::Simulation::CAircraftModelList &models) const override;

            //! \copydoc BlackGui::Views::COrderableViewWithDbObjects::validateLoadedJsonData
            virtual BlackMisc::CStatusMessage validateLoadedJsonData(const BlackMisc::Simulation::CAircraftModelList &models) const override;

            //! \copydoc BlackGui::Views::COrderableViewWithDbObjects::jsonLoadedAndModelUpdated
            virtual void jsonLoadedAndModelUpdated(const BlackMisc::Simulation::CAircraftModelList &models) override;
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

            bool m_stashingClearsSelection   = true; //!< stashing unselects
            bool m_withValidationContextMenu = true; //!< validation didalog context menu
            CAircraftModelStatisticsDialog       *m_statisticsDialog     = nullptr;
            CAircraftModelValidationDialog       *m_fileValidationDialog = nullptr;
            BlackMisc::Simulation::CSimulatorInfo m_correspondingSimulator; //!< validation, simulator required when loading
            QString m_correspondingSimulatorDir;
        };
    } // ns
} // ns

#endif // guard
