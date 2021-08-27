/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_DBOWNMODELSCOMPONENT_H
#define BLACKGUI_COMPONENTS_DBOWNMODELSCOMPONENT_H

#include "blackgui/components/simulatorselector.h"
#include "blackgui/menus/menudelegate.h"
#include "blackgui/overlaymessagesframe.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/aircraftmodelloader.h"
#include "blackmisc/simulation/aircraftmodelinterfaces.h"
#include "blackmisc/simulation/data/modelcaches.h"
#include "blackmisc/simulation/settings/modelsettings.h"
#include "blackmisc/simulation/simulatorinfo.h"
#include "blackmisc/datacache.h"
#include "blackmisc/directories.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/connectionguard.h"

#include <QFrame>
#include <QList>
#include <QObject>
#include <QScopedPointer>
#include <QStringList>
#include <memory>

class QAction;

namespace Ui { class CDbOwnModelsComponent; }
namespace BlackGui
{
    namespace Menus  { class CMenuActions; }
    namespace Models { class CAircraftModelListModel; }
    namespace Views  { class CAircraftModelView; }
    namespace Components
    {
        /*!
         * Handling of own models on disk (the models installed for the simulator)
         */
        class CDbOwnModelsComponent :
            public COverlayMessagesFrame,
            public BlackMisc::Simulation::IModelsSetable,
            public BlackMisc::Simulation::IModelsUpdatable,
            public BlackMisc::Simulation::IModelsForSimulatorSetable,
            public BlackMisc::Simulation::IModelsForSimulatorUpdatable
        {
            Q_OBJECT
            Q_INTERFACES(BlackMisc::Simulation::IModelsSetable)
            Q_INTERFACES(BlackMisc::Simulation::IModelsUpdatable)
            Q_INTERFACES(BlackMisc::Simulation::IModelsForSimulatorSetable)
            Q_INTERFACES(BlackMisc::Simulation::IModelsForSimulatorUpdatable)

        public:
            //! Constructor
            explicit CDbOwnModelsComponent(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CDbOwnModelsComponent() override;

            //! Log categories
            static const QStringList &getLogCategories();

            //! Own (installed) model for given model string
            BlackMisc::Simulation::CAircraftModel getOwnModelForModelString(const QString &modelString) const;

            //! Own models
            BlackMisc::Simulation::CAircraftModelList getOwnModels() const;

            //! Own cached models from loader
            BlackMisc::Simulation::CAircraftModelList getOwnCachedModels(const BlackMisc::Simulation::CSimulatorInfo &simulator) const;

            //! Own models selected in view
            BlackMisc::Simulation::CAircraftModelList getOwnSelectedModels() const;

            //! Own models for simulator
            BlackMisc::Simulation::CSimulatorInfo getOwnModelsSimulator() const;

            //! Change current simulator for own models
            bool setSimulator(const BlackMisc::Simulation::CSimulatorInfo &simulator, bool forced = false);

            //! \copydoc BlackGui::Components::CSimulatorSelector::setMode
            void setSimulatorSelectorMode(CSimulatorSelector::Mode mode);

            //! Number of own models
            int getOwnModelsCount() const;

            //! \copydoc BlackMisc::Simulation::Data::CModelCaches::getInfoString
            QString getInfoString() const;

            //! \copydoc BlackMisc::Simulation::Data::CModelCaches::getInfoStringFsFamily
            QString getInfoStringFsFamily() const;

            //! Update view and cache
            BlackMisc::CStatusMessage updateViewAndCache(const BlackMisc::Simulation::CAircraftModelList &models);

            //! Clear the view
            void clearView();

            //! Models view
            BlackGui::Views::CAircraftModelView *view() const;

            //! Access to aircraft model
            Models::CAircraftModelListModel *model() const;

            //! Access to model loader
            BlackMisc::Simulation::IAircraftModelLoader *modelLoader() const { return m_modelLoader; }

            //! Forced read for given simulator
            bool requestModelsInBackground(const BlackMisc::Simulation::CSimulatorInfo &simulator, bool onlyIfNotEmpty);

            //! Graceful shutdown
            void gracefulShutdown();

            //! \name Implementations of the models interfaces
            //! @{
            virtual void setModels(const BlackMisc::Simulation::CAircraftModelList &models) override  { this->setModelsForSimulator(models, this->getOwnModelsSimulator()); }
            virtual void setModelsForSimulator(const BlackMisc::Simulation::CAircraftModelList &models, const BlackMisc::Simulation::CSimulatorInfo &simulator) override;
            virtual int updateModels(const BlackMisc::Simulation::CAircraftModelList &models) override  { return this->updateModelsForSimulator(models, this->getOwnModelsSimulator()); }
            virtual int updateModelsForSimulator(const BlackMisc::Simulation::CAircraftModelList &models, const BlackMisc::Simulation::CSimulatorInfo &simulator) override;
            //! @}

        signals:
            //! Models have been successfully loaded
            void successfullyLoadedModels(const BlackMisc::Simulation::CSimulatorInfo &simulator, int count);

            //! Own models simulator has changed
            void ownModelsSimulatorChanged(const BlackMisc::Simulation::CSimulatorInfo &simulator);

        private:
            QScopedPointer<Ui::CDbOwnModelsComponent> ui;
            BlackMisc::Simulation::IAircraftModelLoader *m_modelLoader = nullptr; //!< read own aircraft models, aka models on disk
            BlackMisc::Simulation::CSimulatorInfo m_simulator; //!< currently init to simulator
            BlackMisc::CSetting<BlackMisc::Settings::TDirectorySettings> m_directorySettings { this }; //!< the swift directories
            BlackMisc::Simulation::Settings::CMultiSimulatorSettings m_simulatorSettings     { this }; //!< for directories

            //! Request own models
            void requestOwnModelsUpdate();

            //! Load the models
            void loadInstalledModels(const BlackMisc::Simulation::CSimulatorInfo &simulator, BlackMisc::Simulation::IAircraftModelLoader::LoadMode mode, const QStringList &modelDirectories = {});

            //! On disk loading started
            void onModelLoaderDiskLoadingStarted(const BlackMisc::Simulation::CSimulatorInfo &simulator, BlackMisc::Simulation::IAircraftModelLoader::LoadMode mode);

            //! Loading in progress
            void onModelLoadingProgress(const BlackMisc::Simulation::CSimulatorInfo &simulator, const QString &message, int progress);

            //! Model loading finished
            void onModelLoaderLoadingFinished(const BlackMisc::CStatusMessageList &statusMessages, const BlackMisc::Simulation::CSimulatorInfo &simulator, BlackMisc::Simulation::IAircraftModelLoader::LoadFinishedInfo info);

            //! Loading from disk (via view context menu)
            void onViewDiskLoadingFinished(const BlackMisc::CStatusMessage &status);

            //! Cache has been changed
            void onCacheChanged(const BlackMisc::Simulation::CSimulatorInfo &simulator);

            //! Request simulator models
            void requestSimulatorModels(const BlackMisc::Simulation::CSimulatorInfo &simulator, BlackMisc::Simulation::IAircraftModelLoader::LoadMode mode, const QStringList &modelDirectories = {});

            //! Request simulator models from cache
            void requestSimulatorModelsWithCacheInBackground(const BlackMisc::Simulation::CSimulatorInfo &simulator);

            //! Request simulator models from cache
            void clearSimulatorCache(const BlackMisc::Simulation::CSimulatorInfo &simulator);

            //! Own simulator changed
            void onSimulatorSelectorChanged();

            //! Init or change model loader
            bool initModelLoader(const BlackMisc::Simulation::CSimulatorInfo &simulator, BlackMisc::Simulation::IAircraftModelLoader::LoadMode load = BlackMisc::Simulation::IAircraftModelLoader::NotSet);

            //! File name for saving as file
            void setSaveFileName(const BlackMisc::Simulation::CSimulatorInfo &sim);

            //! Directory selector for given simulator
            QString directorySelector(const BlackMisc::Simulation::CSimulatorInfo &simulatorInfo);

            //! Simulator string
            void setUiSimulatorString(const BlackMisc::Simulation::CSimulatorInfo &simulatorInfo);

            //! Confirmed forced reload
            void confirmedForcedReloadCurrentSimulator();

            //! Confirmed forced reload
            void confirmedForcedReload(const BlackMisc::Simulation::CSimulatorInfo &simulator);

            //! Run CSL2XSB script
            void runScriptCSL2XSB();

            //! The menu for loading and handling own models for mapping tasks
            //! \note This is specific for that very component
            //! \fixme MS 2018-12 Move to namespace scope and add Q_OBJECT
            class CLoadModelsMenu : public Menus::IMenuDelegate
            {
            public:
                //! Constructor
                CLoadModelsMenu(CDbOwnModelsComponent *ownModelsComponent) :
                    BlackGui::Menus::IMenuDelegate(ownModelsComponent)
                {}

                //! \copydoc IMenuDelegate::customMenu
                virtual void customMenu(Menus::CMenuActions &menuActions) override;

            private:
                QList<QAction *> m_loadActions;       //!< load actions
                QList<QAction *> m_reloadActions;     //!< reload actions
                QList<QAction *> m_clearCacheActions; //!< clear own models cahce if ever needed
                QAction *m_csl2xsbAction = nullptr;   //!< run csl2xsb script
            };
        };
    } // ns
} // ns
#endif // guard
