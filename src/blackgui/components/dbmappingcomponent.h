/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_DBMAPPINGCOMPONENT_H
#define BLACKGUI_COMPONENTS_DBMAPPINGCOMPONENT_H

#include "blackgui/enableforviewbasedindicator.h"
#include "blackgui/menus/menudelegate.h"
#include "blackgui/overlaymessagesframe.h"
#include "blackgui/enablefordockwidgetinfoarea.h"
#include "blackgui/blackguiexport.h"
#include "blackcore/data/authenticateduser.h"
#include "blackmisc/aviation/airlineicaocode.h"
#include "blackmisc/network/entityflags.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/distributor.h"
#include "blackmisc/simulation/fscommon/vpilotrulesreader.h"
#include "blackmisc/simulation/simulatorinfo.h"
#include "blackmisc/datacache.h"
#include "blackmisc/digestsignal.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/statusmessagelist.h"

#include <QList>
#include <QObject>
#include <QScopedPointer>
#include <QString>
#include <QStringList>

class QAction;
class QModelIndex;
class QPoint;

namespace BlackMisc
{
    namespace Aviation
    {
        class CAircraftIcaoCode;
        class CLivery;
    }
}
namespace Ui { class CDbMappingComponent; }
namespace BlackGui
{
    namespace Menus { class CMenuActions; }
    namespace Views { class CAircraftModelView; }

    namespace Components
    {
        class CDbAutoStashingComponent;
        class CDbAutoSimulatorStashingComponent;
        class CDbModelMappingModifyDialog;

        /*!
         * Mapping component
         */
        class BLACKGUI_EXPORT CDbMappingComponent :
            public COverlayMessagesFrame,
            public CEnableForDockWidgetInfoArea,
            public CEnableForViewBasedIndicator
        {
            Q_OBJECT

        public:
            //! Tab index.
            //! \remark Must match real tab index
            enum TabIndex
            {
                NoValidTab      =  -1,
                TabOwnModelSet  =   0,
                TabOwnModels    =   1,
                TabWorkbench    =   2,
                TabStash        =   3,
                TabModelMatcher =   4,
                TabVPilot      =    5
            };

            //! Constructor
            explicit CDbMappingComponent(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CDbMappingComponent() override;

            //! Graceful shutdown
            void gracefulShutdown();

            //! With vPilot rules?
            bool withVPilot() const { return m_vPilotEnabled; }

            //! Any models which can be stashed?
            bool hasSelectedModelsToStash() const;

            //! Models to be stashed from currently activated tab (table view)
            BlackMisc::Simulation::CAircraftModelList getSelectedModelsToStash() const;

            //! Current tab index
            TabIndex currentTabIndex() const;

            //! Is stashed view?
            bool isStashTab() const;

            //! Is workbench tab?
            bool isWorkbenchTab() const;

            //! Tab can contribute to model set
            bool canAddToModelSetTab() const;

            //! Current model view
            Views::CAircraftModelView *currentModelView() const;

            //! Current model view
            Views::CAircraftModelView *modelView(TabIndex tab) const;

            //! Unvalidated consolidated aircraft model from the editor subparts (icao, distributor)
            //! \note not guaranteed to be valid, just a snapshot of its current editor state
            BlackMisc::Simulation::CAircraftModel getEditorAircraftModel() const;

            // ---------------- stash -----------------

            //! \name Models from BlackGui::Components::CDbStashComponent
            //! @{
            //! Stashed models
            const BlackMisc::Simulation::CAircraftModelList &getStashedModels() const;

            //! Any stashed models?
            bool hasStashedModels() const;

            //! Stashed model strings
            QStringList getStashedModelStrings() const;
            //! @}

            // ---------------- own models -----------------

            //! \name Own models from BlackGui::Components::CDbOwnModelsComponent
            //! @{
            //! Own models
            BlackMisc::Simulation::CAircraftModelList getOwnModels() const;

            //! Own cached models
            BlackMisc::Simulation::CAircraftModelList getOwnCachedModels(const BlackMisc::Simulation::CSimulatorInfo &simulator) const;

            //! Own selected models
            BlackMisc::Simulation::CAircraftModelList getOwnSelectedModels() const;

            //! Own (installed) model for given model string
            BlackMisc::Simulation::CAircraftModel getOwnModelForModelString(const QString &modelString) const;

            //! Own models for simulator
            BlackMisc::Simulation::CSimulatorInfo getOwnModelsSimulator() const;

            //! Set simulator for own models
            void setOwnModelsSimulator(const BlackMisc::Simulation::CSimulatorInfo &simulator);

            //! Number of own models
            int getOwnModelsCount() const;

            //! \copydoc BlackMisc::Simulation::Data::CModelCaches::getInfoString
            QString getOwnModelsInfoString() const;

            //! \copydoc BlackMisc::Simulation::Data::CModelCaches::getInfoStringFsFamily
            QString getOwnModelsInfoStringFsFamily() const;
            //! @}

            // ---------------- own model set -----------------

            //! Set simulator for own models
            void setOwnModelSetSimulator(const BlackMisc::Simulation::CSimulatorInfo &simulator);

            //! Own model set
            BlackMisc::Simulation::CAircraftModelList getOwnModelSet() const;

        public slots:
            //! \copydoc CDbStashComponent::stashModel
            BlackMisc::CStatusMessage stashModel(const BlackMisc::Simulation::CAircraftModel &model, bool replace = false);

            //! \copydoc CDbStashComponent::stashModels
            BlackMisc::CStatusMessageList stashModels(const BlackMisc::Simulation::CAircraftModelList &models);

            //! \copydoc CDbOwnModelSetComponent::addToModelSet
            BlackMisc::CStatusMessage addToOwnModelSet(const BlackMisc::Simulation::CAircraftModelList &models, const BlackMisc::Simulation::CSimulatorInfo &simulator);

            //! \copydoc CDbStashComponent::consolidateModel
            BlackMisc::Simulation::CAircraftModel consolidateModel(const BlackMisc::Simulation::CAircraftModel &model) const;

            //! \copydoc CDbStashComponent::replaceModelsUnvalidated
            void replaceStashedModelsUnvalidated(const BlackMisc::Simulation::CAircraftModelList &models) const;

            //! Validate, empty list means OK
            BlackMisc::CStatusMessageList validateCurrentModel(bool withNestedForms) const;

            //! Resize so that selection is easy (larger table view)
            void resizeForSelect();

            //! Resize so that mapping is easier
            void resizeForMapping();

            //! Max. space for table view
            void maxTableView();

            //! Stash models
            void stashSelectedModels();

            //! Open model modify dialog
            void modifyModelDialog();

        signals:
            //! Request to filter by livery
            void filterByLivery(const BlackMisc::Aviation::CLivery &livery);

            //! Request to filter by aircraft ICAO
            void filterByAircraftIcao(const BlackMisc::Aviation::CAircraftIcaoCode &icao);

            //! Request to filter by distributor
            void filterByDistributor(const BlackMisc::Simulation::CDistributor &distributor);

            //! Request latest (incremental) data from backend
            void requestUpdatedData(BlackMisc::Network::CEntityFlags::Entity entities);

            //! Tab index has been changed
            void tabIndexChanged(int index);

        private slots:
            //! Add to own model set
            void ps_addToOwnModelSet(); // still used with QShortcut

        private:
            //! Data for vPilot have been loaded
            void onLoadVPilotDataFinished(bool success);

            //! Load removed models
            void loadRemovedModels();

            //! Merge with vPilot models
            void mergeWithVPilotModels();

            //! vPilot cached models changed
            void onVPilotCacheChanged();

            //! Load the vPilot rules
            void loadVPilotData();

            //! Request update of vPilot data
            void requestVPilotDataUpdate();

            //! Merge selected with vPilot models
            void mergeSelectedWithVPilotModels();

            //! vPilot data changed
            void onVPilotDataChanged(int count, bool withFilter);

            //! Workbench data changed
            void onWorkbenchDataChanged(int count, bool withFilter);

            //! Tab index changed
            void onTabIndexChanged(int index);

            //! Stashed models changed
            void onStashedModelsChangedTriggerDigest();

            //! Stashed models changed
            void onStashedModelsChangedDigest();

            //! Stash has been changed
            void onStashedModelsDataChangedDigest(int count, bool withFilter);

            //! Models have been published successfully
            void onModelsSuccessfullyPublished(const BlackMisc::Simulation::CAircraftModelList &models, bool directWrite);

            //! Stash drop request
            void handleStashDropRequest(const BlackMisc::Aviation::CAirlineIcaoCode &code) const;

            //! Model set has been changed
            void onModelSetChangedDigest(int count, bool withFilter);

            //! Own models have been changed
            void onOwnModelsChangedDigest(int count, bool withFilter);

            //! Row has been selected
            void onModelRowSelected(const QModelIndex &index);

            //! Simulator for own model has changed
            void onOwnModelsSimulatorChanged(const BlackMisc::Simulation::CSimulatorInfo simulator);

            //! User object changed
            void onUserChanged();

            //! Stash current model
            void stashCurrentModel();

            //! Display auto stashing dialog
            void displayAutoStashingDialog();

            //! Display auto simulator stashing dialog
            void displayAutoSimulatorStashingDialog();

            //! Remove DB models from current view
            void removeDbModelsFromView();

            //! Show changed attributes of stashed
            void showChangedAttributes();

            //! Toggle auto filtering
            void toggleAutoFiltering();

            //! Apply current DB data from form
            void applyFormLiveryData();

            //! Apply current DB data from form
            void applyFormAircraftIcaoData();

            //! Apply current DB data from form
            void applyFormDistributorData();

            //! Custom menu
            void onCustomContextMenu(const QPoint &point);

        private:
            QScopedPointer<Ui::CDbMappingComponent>             ui;
            QScopedPointer<CDbAutoStashingComponent>            m_autoStashDialog;     //!< dialog auto stashing
            QScopedPointer<CDbAutoSimulatorStashingComponent>   m_autoSimulatorDialog; //!< dialog auto simulator update
            QScopedPointer<CDbModelMappingModifyDialog>         m_modelModifyDialog;   //!< dialog when modifying models
            BlackMisc::Simulation::FsCommon::CVPilotRulesReader m_vPilotReader;        //!< read vPilot rules
            BlackMisc::CDigestSignal                            m_dsStashedModelsChanged { this, &CDbMappingComponent::onStashedModelsChangedDigest, 750, 25 };
            BlackMisc::CDataReadOnly<BlackCore::Data::TAuthenticatedDbUser> m_swiftDbUser { this, &CDbMappingComponent::onUserChanged };
            const bool vPilotSupport   = true;   //!< vPilot support (will be removed in future)
            bool m_vPilot1stInit       = true;   //!< vPilot extensions initalized?
            bool m_vPilotEnabled       = false;  //!< use vPilot extensions
            bool m_vPilotFormatted     = false;  //!< vPilot formatted (workaround)
            bool m_autoFilterInDbViews = false;  //!< automatically filter the DB view by the current model

            //! Init vPilot if rights and suitable
            void initVPilotLoading();

            //! Workaround to format vPilot view
            //! \remark presize does not work properly when data are loaded, reason is not understood. This here does a formatting when tab becomes visible.
            //! \fixme can be removed whenever a proper formatting is archived with data loading, vPilot support will be removed in the long term anyway
            void formatVPilotView();

            //! Model for given index from sender/current view
            BlackMisc::Simulation::CAircraftModel getModelFromView(const QModelIndex &index) const;

            //! Current tab text
            QString currentTabText() const;

            //! Data have been changed and the editor data might need an update
            void updateEditorsWhenApplicable();

            // -------------------- component specific menus --------------------------

            //! The menu for loading and handling VPilot rules for mapping tasks
            //! \note This is a specific menu for that very component
            //! \fixme MS 2018-12 Move to namespace scope and add Q_OBJECT
            class CMappingVPilotMenu : public Menus::IMenuDelegate
            {
            public:
                //! Constructor
                CMappingVPilotMenu(CDbMappingComponent *mappingComponent) : Menus::IMenuDelegate(mappingComponent)
                {}

                //! \copydoc IMenuDelegate::customMenu
                virtual void customMenu(Menus::CMenuActions &menuActions) override;

            private:
                //! Mapping component
                CDbMappingComponent *mappingComponent() const;
                QAction *m_menuAction = nullptr;
            };

            //! Menu for stashing related tools:
            //! -# removing DB models from stash view
            //! -# for auto stashing
            //! -# cross simulator updating (FSX, FS9, P3D family)
            //! -# toggle stash auto filtering
            //! -# show changed attributes
            //! \note This is a specific menu for the CDbMappingComponent component
            //! \fixme MS 2018-12 Move to namespace scope and add Q_OBJECT
            class CStashToolsMenu : public Menus::IMenuDelegate
            {
            public:
                //! Constructor
                CStashToolsMenu(CDbMappingComponent *mappingComponent);

                //! \copydoc IMenuDelegate::customMenu
                virtual void customMenu(Menus::CMenuActions &menuActions) override;

            private:
                //! Mapping component
                CDbMappingComponent *mappingComponent() const;

                //! Removel models existing in DB
                void addStashViewSpecificMenus(Menus::CMenuActions &menuActions);

                QAction *m_autoStashing = nullptr;
                QAction *m_autoSimulatorStashing = nullptr;
                QAction *m_stashFiltering = nullptr;
            };

            //! Menu for own model sets
            //! \fixme MS 2018-12 Move to namespace scope and add Q_OBJECT
            class COwnModelSetMenu : public Menus::IMenuDelegate
            {
            public:
                //! Constructor
                COwnModelSetMenu(CDbMappingComponent *mappingComponent) :
                    Menus::IMenuDelegate(mappingComponent)
                {}

                //! \copydoc IMenuDelegate::customMenu
                virtual void customMenu(Menus::CMenuActions &menuActions) override;

            private:
                //! Mapping component
                CDbMappingComponent *mappingComponent() const;

                QAction *m_menuAction = nullptr;
            };

            //! Menu for removed models
            //! \fixme MS 2018-12 Move to namespace scope and add Q_OBJECT
            class CRemovedModelsMenu : public Menus::IMenuDelegate
            {
            public:
                //! Constructor
                CRemovedModelsMenu(CDbMappingComponent *mappingComponent) :
                    Menus::IMenuDelegate(mappingComponent)
                {}

                //! \copydoc IMenuDelegate::customMenu
                virtual void customMenu(Menus::CMenuActions &menuActions) override;

            private:
                //! Mapping component
                CDbMappingComponent *mappingComponent() const;

                QAction *m_menuAction = nullptr;
            };

            //! Apply DB data to selected models
            //! \fixme MS 2018-12 Move to namespace scope and add Q_OBJECT
            class CApplyDbDataMenu : public Menus::IMenuDelegate
            {
            public:
                //! Constructor
                CApplyDbDataMenu(CDbMappingComponent *mappingComponent) :
                    Menus::IMenuDelegate(mappingComponent)
                {}

                //! \copydoc IMenuDelegate::customMenu
                virtual void customMenu(Menus::CMenuActions &menuActions) override;

            private:
                //! Mapping component
                CDbMappingComponent *mappingComponent() const;

                QList<QAction *> m_menuActions;  //!< actions, kept when once initialized
            };

            //! Merge with vPilot data
            //! \deprecated vPilot menus will be removed in the future
            //! \fixme MS 2018-12 Move to namespace scope and add Q_OBJECT
            class CMergeWithVPilotMenu : public Menus::IMenuDelegate
            {
            public:
                //! Constructor
                CMergeWithVPilotMenu(CDbMappingComponent *mappingComponent);

                //! \copydoc IMenuDelegate::customMenu
                virtual void customMenu(Menus::CMenuActions &menuActions) override;

                //! Mapping component
                CDbMappingComponent *mappingComponent() const;

            private:
                QList<QAction *> m_menuActions; //!< actions, kept when once initialized
            };
        };
    } // ns
} // ns
#endif // guard
