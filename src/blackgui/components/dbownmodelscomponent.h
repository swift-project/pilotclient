/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_DBOWNMODELSCOMPONENT_H
#define BLACKGUI_COMPONENTS_DBOWNMODELSCOMPONENT_H

#include "blackgui/menus/menudelegate.h"
#include "blackgui/overlaymessagesframe.h"
#include "blackmisc/datacache.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/aircraftmodelloader.h"
#include "blackmisc/simulation/aircraftmodelinterfaces.h"
#include "blackmisc/simulation/data/modelcaches.h"
#include "blackmisc/simulation/simulatorinfo.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/connectionguard.h"

#include <QFrame>
#include <QList>
#include <QObject>
#include <QScopedPointer>
#include <QStringList>
#include <memory>

class QAction;
class QWidget;

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
            public BlackMisc::Simulation::IModelsPerSimulatorSetable,
            public BlackMisc::Simulation::IModelsPerSimulatorUpdatable
        {
            Q_OBJECT
            Q_INTERFACES(BlackMisc::Simulation::IModelsSetable)
            Q_INTERFACES(BlackMisc::Simulation::IModelsUpdatable)
            Q_INTERFACES(BlackMisc::Simulation::IModelsPerSimulatorSetable)
            Q_INTERFACES(BlackMisc::Simulation::IModelsPerSimulatorUpdatable)

        public:
            //! Constructor
            explicit CDbOwnModelsComponent(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CDbOwnModelsComponent();

            //! Log categories
            static const BlackMisc::CLogCategoryList &getLogCategories();

            //! Own (installed) model for given model string
            BlackMisc::Simulation::CAircraftModel getOwnModelForModelString(const QString &modelString) const;

            //! Own models
            BlackMisc::Simulation::CAircraftModelList getOwnModels() const;

            //! Own cached models from loader
            BlackMisc::Simulation::CAircraftModelList getOwnCachedModels(const BlackMisc::Simulation::CSimulatorInfo &simulator) const;

            //! Own models selected in view
            BlackMisc::Simulation::CAircraftModelList getOwnSelectedModels() const;

            //! Own models for simulator
            const BlackMisc::Simulation::CSimulatorInfo getOwnModelsSimulator() const;

            //! Change current simulator for own models
            void setSimulator(const BlackMisc::Simulation::CSimulatorInfo &simulator);

            //! Number of own models
            int getOwnModelsCount() const;

            //! \copydoc BlackMisc::Simulation::Data::CModelCaches::getInfoString
            QString getInfoString() const;

            //! \copydoc BlackMisc::Simulation::Data::CModelCaches::getInfoStringFsFamily
            QString getInfoStringFsFamily() const;

            //! Update view and cache
            BlackMisc::CStatusMessage updateViewAndCache(const BlackMisc::Simulation::CAircraftModelList &models);

            //! Models view
            BlackGui::Views::CAircraftModelView *view() const;

            //! Access to aircraft model
            Models::CAircraftModelListModel *model() const;

            //! Access to model loader
            BlackMisc::Simulation::IAircraftModelLoader *modelLoader() const;

            //! Forced read for given simulator
            bool requestModelsInBackground(const BlackMisc::Simulation::CSimulatorInfo &simulator, bool onlyIfNotEmpty);

            //! Graceful shutdown
            void gracefulShutdown();

            //! \name Implementations of the models interfaces
            //! @{
            virtual void setModels(const BlackMisc::Simulation::CAircraftModelList &models) override  { this->setModels(models, this->getOwnModelsSimulator()); }
            virtual void updateModels(const BlackMisc::Simulation::CAircraftModelList &models) override  { this->updateModels(models, this->getOwnModelsSimulator()); }
            virtual void setModels(const BlackMisc::Simulation::CAircraftModelList &models, const BlackMisc::Simulation::CSimulatorInfo &simulator) override;
            virtual void updateModels(const BlackMisc::Simulation::CAircraftModelList &models, const BlackMisc::Simulation::CSimulatorInfo &simulator) override;
            //! @}

        signals:
            //! Models have been successfully loaded
            void successfullyLoadedModels(const BlackMisc::Simulation::CSimulatorInfo &simulator);

        private:
            QScopedPointer<Ui::CDbOwnModelsComponent> ui;
            std::unique_ptr<BlackMisc::Simulation::IAircraftModelLoader> m_modelLoader; //!< read own aircraft models, aka models on disk
            BlackMisc::CConnectionGuard m_loaderConnections;
            BlackMisc::Simulation::CSimulatorInfo m_simulator; //!< currently init to simulator

            //! Request own models
            void requestOwnModelsUpdate();

            //! Load the models
            void loadInstalledModels(const BlackMisc::Simulation::CSimulatorInfo &simulator, BlackMisc::Simulation::IAircraftModelLoader::LoadMode mode, const QStringList &modelDirectories = {});

            //! Model loading finished
            void onOwnModelsLoadingFinished(const BlackMisc::CStatusMessageList &statusMessages, const BlackMisc::Simulation::CSimulatorInfo &simulator, BlackMisc::Simulation::IAircraftModelLoader::LoadFinishedInfo info);

            //! Request simulator models
            void requestSimulatorModels(const BlackMisc::Simulation::CSimulatorInfo &simulator, BlackMisc::Simulation::IAircraftModelLoader::LoadMode mode, const QStringList &modelDirectories = {});

            //! Request simulator models from cache
            void requestSimulatorModelsWithCacheInBackground(const BlackMisc::Simulation::CSimulatorInfo &simulator);

            //! Request simulator models from cache
            void clearSimulatorCache(const BlackMisc::Simulation::CSimulatorInfo &simulator);

            //! Own simulator changed
            void onSimulatorSelectorChanged();

            //! Init or change model loader
            bool initModelLoader(const BlackMisc::Simulation::CSimulatorInfo &simulator);

            //! File name for saving as file
            void setSaveFileName(const BlackMisc::Simulation::CSimulatorInfo &sim);

            //! Directory selector for given simulator
            static QString directorySelector(const BlackMisc::Simulation::CSimulatorInfo &simulatorInfo);

            //! The menu for loading and handling own models for mapping tasks
            //! \note This is specific for that very component
            class CLoadModelsMenu : public BlackGui::Menus::IMenuDelegate
            {
            public:
                //! Constructor
                CLoadModelsMenu(CDbOwnModelsComponent *ownModelsComponent, bool separator = true) :
                    BlackGui::Menus::IMenuDelegate(ownModelsComponent, separator)
                {}

                //! \copydoc IMenuDelegate::customMenu
                virtual void customMenu(BlackGui::Menus::CMenuActions &menuActions) override;

            private:
                QList<QAction *> m_loadActions;       //!< load actions
                QList<QAction *> m_reloadActions;     //!< reload actions
                QList<QAction *> m_clearCacheActions; //!< clear own models cahce if ever needed
            };
        };
    } // ns
} // ns
#endif // guard
