// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_DBOWNMODELSCOMPONENT_H
#define SWIFT_GUI_COMPONENTS_DBOWNMODELSCOMPONENT_H

#include <memory>

#include <QFrame>
#include <QList>
#include <QObject>
#include <QScopedPointer>
#include <QStringList>

#include "gui/components/simulatorselector.h"
#include "gui/menus/menudelegate.h"
#include "gui/overlaymessagesframe.h"
#include "misc/connectionguard.h"
#include "misc/datacache.h"
#include "misc/directories.h"
#include "misc/simulation/aircraftmodelinterfaces.h"
#include "misc/simulation/aircraftmodellist.h"
#include "misc/simulation/aircraftmodelloader.h"
#include "misc/simulation/data/modelcaches.h"
#include "misc/simulation/settings/modelsettings.h"
#include "misc/simulation/simulatorinfo.h"
#include "misc/statusmessage.h"

class QAction;

namespace Ui
{
    class CDbOwnModelsComponent;
}
namespace swift::gui
{
    namespace menus
    {
        class CMenuActions;
    }
    namespace models
    {
        class CAircraftModelListModel;
    }
    namespace views
    {
        class CAircraftModelView;
    }
    namespace components
    {
        /*!
         * Handling of own models on disk (the models installed for the simulator)
         */
        class CDbOwnModelsComponent :
            public COverlayMessagesFrame,
            public swift::misc::simulation::IModelsSetable,
            public swift::misc::simulation::IModelsUpdatable,
            public swift::misc::simulation::IModelsForSimulatorSetable,
            public swift::misc::simulation::IModelsForSimulatorUpdatable
        {
            Q_OBJECT
            Q_INTERFACES(swift::misc::simulation::IModelsSetable)
            Q_INTERFACES(swift::misc::simulation::IModelsUpdatable)
            Q_INTERFACES(swift::misc::simulation::IModelsForSimulatorSetable)
            Q_INTERFACES(swift::misc::simulation::IModelsForSimulatorUpdatable)

        public:
            //! Constructor
            explicit CDbOwnModelsComponent(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CDbOwnModelsComponent() override;

            //! Log categories
            static const QStringList &getLogCategories();

            //! Own (installed) model for given model string
            swift::misc::simulation::CAircraftModel getOwnModelForModelString(const QString &modelString) const;

            //! Own models
            swift::misc::simulation::CAircraftModelList getOwnModels() const;

            //! Own cached models from loader
            swift::misc::simulation::CAircraftModelList
            getOwnCachedModels(const swift::misc::simulation::CSimulatorInfo &simulator) const;

            //! Own models selected in view
            swift::misc::simulation::CAircraftModelList getOwnSelectedModels() const;

            //! Own models for simulator
            swift::misc::simulation::CSimulatorInfo getOwnModelsSimulator() const;

            //! Change current simulator for own models
            bool setSimulator(const swift::misc::simulation::CSimulatorInfo &simulator, bool forced = false);

            //! \copydoc swift::gui::components::CSimulatorSelector::setMode
            void setSimulatorSelectorMode(CSimulatorSelector::Mode mode);

            //! Number of own models
            int getOwnModelsCount() const;

            //! \copydoc swift::misc::simulation::data::CModelCaches::getInfoString
            QString getInfoString() const;

            //! \copydoc swift::misc::simulation::data::CModelCaches::getInfoStringFsFamily
            QString getInfoStringFsFamily() const;

            //! Update view and cache
            swift::misc::CStatusMessage updateViewAndCache(const swift::misc::simulation::CAircraftModelList &models);

            //! Clear the view
            void clearView();

            //! Models view
            swift::gui::views::CAircraftModelView *view() const;

            //! Access to aircraft model
            models::CAircraftModelListModel *model() const;

            //! Access to model loader
            swift::misc::simulation::IAircraftModelLoader *modelLoader() const { return m_modelLoader; }

            //! Forced read for given simulator
            bool requestModelsInBackground(const swift::misc::simulation::CSimulatorInfo &simulator,
                                           bool onlyIfNotEmpty);

            //! Graceful shutdown
            void gracefulShutdown();

            //! \name Implementations of the models interfaces
            //! @{
            virtual void setModels(const swift::misc::simulation::CAircraftModelList &models) override
            {
                this->setModelsForSimulator(models, this->getOwnModelsSimulator());
            }
            virtual void setModelsForSimulator(const swift::misc::simulation::CAircraftModelList &models,
                                               const swift::misc::simulation::CSimulatorInfo &simulator) override;
            virtual int updateModels(const swift::misc::simulation::CAircraftModelList &models) override
            {
                return this->updateModelsForSimulator(models, this->getOwnModelsSimulator());
            }
            virtual int updateModelsForSimulator(const swift::misc::simulation::CAircraftModelList &models,
                                                 const swift::misc::simulation::CSimulatorInfo &simulator) override;
            //! @}

        signals:
            //! Models have been successfully loaded
            void successfullyLoadedModels(const swift::misc::simulation::CSimulatorInfo &simulator, int count);

            //! Own models simulator has changed
            void ownModelsSimulatorChanged(const swift::misc::simulation::CSimulatorInfo &simulator);

        private:
            QScopedPointer<Ui::CDbOwnModelsComponent> ui;
            swift::misc::simulation::IAircraftModelLoader *m_modelLoader =
                nullptr; //!< read own aircraft models, aka models on disk
            swift::misc::simulation::CSimulatorInfo m_simulator; //!< currently init to simulator
            swift::misc::CSetting<swift::misc::settings::TDirectorySettings> m_directorySettings {
                this
            }; //!< the swift directories
            swift::misc::simulation::settings::CMultiSimulatorSettings m_simulatorSettings {
                this
            }; //!< for directories

            //! Request own models
            void requestOwnModelsUpdate();

            //! Load the models
            void loadInstalledModels(const swift::misc::simulation::CSimulatorInfo &simulator,
                                     swift::misc::simulation::IAircraftModelLoader::LoadMode mode,
                                     const QStringList &modelDirectories = {});

            //! On disk loading started
            void onModelLoaderDiskLoadingStarted(const swift::misc::simulation::CSimulatorInfo &simulator,
                                                 swift::misc::simulation::IAircraftModelLoader::LoadMode mode);

            //! Loading in progress
            void onModelLoadingProgress(const swift::misc::simulation::CSimulatorInfo &simulator,
                                        const QString &message, int progress);

            //! Model loading finished
            void onModelLoaderLoadingFinished(const swift::misc::CStatusMessageList &statusMessages,
                                              const swift::misc::simulation::CSimulatorInfo &simulator,
                                              swift::misc::simulation::IAircraftModelLoader::LoadFinishedInfo info);

            //! Loading from disk (via view context menu)
            void onViewDiskLoadingFinished(const swift::misc::CStatusMessage &status);

            //! Cache has been changed
            void onCacheChanged(const swift::misc::simulation::CSimulatorInfo &simulator);

            //! Request simulator models
            void requestSimulatorModels(const swift::misc::simulation::CSimulatorInfo &simulator,
                                        swift::misc::simulation::IAircraftModelLoader::LoadMode mode,
                                        const QStringList &modelDirectories = {});

            //! Request simulator models from cache
            void requestSimulatorModelsWithCacheInBackground(const swift::misc::simulation::CSimulatorInfo &simulator);

            //! Request simulator models from cache
            void clearSimulatorCache(const swift::misc::simulation::CSimulatorInfo &simulator);

            //! Own simulator changed
            void onSimulatorSelectorChanged();

            //! Init or change model loader
            bool initModelLoader(const swift::misc::simulation::CSimulatorInfo &simulator,
                                 swift::misc::simulation::IAircraftModelLoader::LoadMode load =
                                     swift::misc::simulation::IAircraftModelLoader::NotSet);

            //! File name for saving as file
            void setSaveFileName(const swift::misc::simulation::CSimulatorInfo &sim);

            //! Directory selector for given simulator
            QString directorySelector(const swift::misc::simulation::CSimulatorInfo &simulatorInfo);

            //! Simulator string
            void setUiSimulatorString(const swift::misc::simulation::CSimulatorInfo &simulatorInfo);

            //! Confirmed forced reload
            void confirmedForcedReloadCurrentSimulator();

            //! Confirmed forced reload
            void confirmedForcedReload(const swift::misc::simulation::CSimulatorInfo &simulator);

            //! Run CSL2XSB script
            void runScriptCSL2XSB();

            //! The menu for loading and handling own models for mapping tasks
            //! \note This is specific for that very component
            //! \fixme MS 2018-12 Move to namespace scope and add Q_OBJECT
            class CLoadModelsMenu : public menus::IMenuDelegate
            {
            public:
                //! Constructor
                CLoadModelsMenu(CDbOwnModelsComponent *ownModelsComponent)
                    : swift::gui::menus::IMenuDelegate(ownModelsComponent)
                {}

                //! \copydoc IMenuDelegate::customMenu
                virtual void customMenu(menus::CMenuActions &menuActions) override;

            private:
                QList<QAction *> m_loadActions; //!< load actions
                QList<QAction *> m_reloadActions; //!< reload actions
                QList<QAction *> m_clearCacheActions; //!< clear own models cahce if ever needed
                QAction *m_csl2xsbAction = nullptr; //!< run csl2xsb script
            };
        };
    } // namespace components
} // namespace swift::gui
#endif // SWIFT_GUI_COMPONENTS_DBOWNMODELSCOMPONENT_H
