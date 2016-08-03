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
#include "blackmisc/datacache.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/aircraftmodelloader.h"
#include "blackmisc/simulation/data/modelcaches.h"
#include "blackmisc/simulation/simulatorinfo.h"
#include "blackmisc/statusmessage.h"

#include <QFrame>
#include <QList>
#include <QObject>
#include <QScopedPointer>
#include <QString>
#include <memory>

class QAction;
class QWidget;

namespace Ui { class CDbOwnModelsComponent; }

namespace BlackGui
{
    namespace Menus { class CMenuActions; }
    namespace Models { class CAircraftModelListModel; }
    namespace Views { class CAircraftModelView; }

    namespace Components
    {
        /*!
         * Handling of own models on disk (the models installed for the simulator)
         */
        class CDbOwnModelsComponent : public QFrame
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CDbOwnModelsComponent(QWidget *parent = nullptr);

            //! Destructor
            ~CDbOwnModelsComponent();

            //! Own (installed) model for given model string
            BlackMisc::Simulation::CAircraftModel getOwnModelForModelString(const QString &modelString) const;

            //! Own models
            BlackMisc::Simulation::CAircraftModelList getOwnModels() const;

            //! Own models selected in view
            BlackMisc::Simulation::CAircraftModelList getOwnSelectedModels() const;

            //! Own models for simulator
            const BlackMisc::Simulation::CSimulatorInfo getOwnModelsSimulator() const;

            //! Change current simulator for own models
            void setSimulator(const BlackMisc::Simulation::CSimulatorInfo &simulator);

            //! Number of own models
            int getOwnModelsCount() const;

            //! Update view and cache
            BlackMisc::CStatusMessage updateViewAndCache(const BlackMisc::Simulation::CAircraftModelList &models);

            //! Models view
            BlackGui::Views::CAircraftModelView *view() const;

            //! Access to aircraft model
            Models::CAircraftModelListModel *model() const;

            //! Access to model loader
            BlackMisc::Simulation::IAircraftModelLoader *modelLoader() const;

            //! Graceful shutdown
            void gracefulShutdown();

        private slots:
            //! Request own models
            void ps_requestOwnModelsUpdate();

            //! Load the models
            void ps_loadInstalledModels(const BlackMisc::Simulation::CSimulatorInfo &simulator, BlackMisc::Simulation::IAircraftModelLoader::LoadMode mode);

            //! Model loading finished
            void ps_onOwnModelsLoadingFinished(bool success, const BlackMisc::Simulation::CSimulatorInfo &simulator);

            //! Request simulator models
            void ps_requestSimulatorModels(const BlackMisc::Simulation::CSimulatorInfo &simulator, BlackMisc::Simulation::IAircraftModelLoader::LoadMode mode);

        private:
            QScopedPointer<Ui::CDbOwnModelsComponent> ui;
            std::unique_ptr<BlackMisc::Simulation::IAircraftModelLoader>           m_modelLoader;                //!< read own aircraft models
            BlackMisc::CData<BlackMisc::Simulation::Data::TModelCacheLastSelection> m_simulatorSelection {this }; //!< last selection

            //! Init or change model loader
            bool initModelLoader(const BlackMisc::Simulation::CSimulatorInfo &simulator);

            //! File name for savinf
            void setSaveFileName(const BlackMisc::Simulation::CSimulatorInfo &sim);

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
                QList<QAction *> m_loadActions;   //!< load actions
                QList<QAction *> m_reloadActions; //!< reload actions
            };
        };
    } // ns
} // ns
#endif // guard
