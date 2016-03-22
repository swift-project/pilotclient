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

#include "blackgui/models/aircraftmodellistmodel.h"
#include "blackgui/components/data/lastselections.h"
#include "blackgui/menus/menudelegate.h"
#include "blackmisc/simulation/aircraftmodelloader.h"
#include <QFrame>
#include <QScopedPointer>

namespace Ui { class CDbOwnModelsComponent; }

namespace BlackGui
{
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

            //! Own models for simulator
            const BlackMisc::Simulation::CSimulatorInfo &getOwnModelsSimulator() const;

            //! Number of own models
            int getOwnModelsCount() const;

            //! Models view
            BlackGui::Views::CAircraftModelView *view() const;

            //! Access to aircraft model
            Models::CAircraftModelListModel *model() const;

            //! Graceful shutdown
            void gracefulShutdown();

        private slots:
            //! Request own models
            void ps_requestOwnModelsUpdate();

            //! Load the models
            void ps_loadInstalledModels(const BlackMisc::Simulation::CSimulatorInfo &simInfo, BlackMisc::Simulation::IAircraftModelLoader::LoadMode mode);

            //! Model loading finished
            void ps_onOwnModelsLoadingFinished(bool success, const BlackMisc::Simulation::CSimulatorInfo &simInfo);

            //! Request simulator models
            void ps_requestSimulatorModels(const BlackMisc::Simulation::CSimulatorInfo &simInfo, BlackMisc::Simulation::IAircraftModelLoader::LoadMode mode);

        private:
            QScopedPointer<Ui::CDbOwnModelsComponent> ui;
            std::unique_ptr<BlackMisc::Simulation::IAircraftModelLoader>       m_modelLoader;              //!< read own aircraft models
            BlackMisc::CData<BlackGui::Components::Data::DbOwnModelsComponent> m_lastInteractions {this};  //!< last interactions

            //! Init model loader
            bool initModelLoader(const BlackMisc::Simulation::CSimulatorInfo &simInfo);

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
                virtual void customMenu(QMenu &menu) const override;
            };
        };
    } // ns
} // ns
#endif // guard
