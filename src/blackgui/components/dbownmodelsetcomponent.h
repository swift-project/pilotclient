/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_DBOWNMODELSETCOMPONENT_H
#define BLACKGUI_COMPONENTS_DBOWNMODELSETCOMPONENT_H

#include "blackgui/menus/menudelegate.h"
#include "blackcore/settings/distributorpreferences.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/aircraftmodelsetloader.h"
#include "dbmappingcomponentaware.h"
#include <QFrame>
#include <QScopedPointer>
#include <QMenu>

namespace Ui { class CDbOwnModelSetComponent; }

namespace BlackGui
{
    namespace Views { class CAircraftModelView; }
    namespace Components
    {
        class CDbOwnModelSetDialog;

        /*!
         * Handling of the own model set
         */
        class CDbOwnModelSetComponent :
            public QFrame,
            public CDbMappingComponentAware,
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
            explicit CDbOwnModelSetComponent(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CDbOwnModelSetComponent();

            //! Corresponding view
            Views::CAircraftModelView *view() const;

            //! Add to model set
            BlackMisc::CStatusMessage addToModelSet(const BlackMisc::Simulation::CAircraftModelList &models, const BlackMisc::Simulation::CSimulatorInfo &simulator);

            //! Add to model set
            BlackMisc::CStatusMessage addToModelSet(const BlackMisc::Simulation::CAircraftModel &model, const BlackMisc::Simulation::CSimulatorInfo &simulator);

            //! Current model set for simulator CDbOwnModelSetComponent::getModelSetSimulator
            const BlackMisc::Simulation::CAircraftModelList &getModelSet() const;

            //! Model set is for simulator
            const Simulation::CSimulatorInfo getModelSetSimulator() const;

            //! \copydoc CDbMappingComponentAware::setMappingComponent
            virtual void setMappingComponent(CDbMappingComponent *component) override;

            //! \name Implementations of the models interfaces
            //! @{
            virtual void setModels(const BlackMisc::Simulation::CAircraftModelList &models) override  { this->setModelSet(models, this->getModelSetSimulator()); }
            virtual void updateModels(const BlackMisc::Simulation::CAircraftModelList &models) override  { this->replaceOrAddModelSet(models, this->getModelSetSimulator()); }
            virtual void setModels(const BlackMisc::Simulation::CAircraftModelList &models, const BlackMisc::Simulation::CSimulatorInfo &simulator) override  { this->setModelSet(models, simulator); }
            virtual void updateModels(const BlackMisc::Simulation::CAircraftModelList &models, const BlackMisc::Simulation::CSimulatorInfo &simulator) override  { this->replaceOrAddModelSet(models, simulator); }
            //! @}

        public slots:
            //! Set the model set
            void setModelSet(const BlackMisc::Simulation::CAircraftModelList &models, const BlackMisc::Simulation::CSimulatorInfo &simulator);

            //! Replace or add models provided
            void replaceOrAddModelSet(const BlackMisc::Simulation::CAircraftModelList &models, const BlackMisc::Simulation::CSimulatorInfo &simulator);

        private slots:
            //! Tab has been changed
            void ps_tabIndexChanged(int index);

            //! Button was clicked
            void ps_buttonClicked();

            //! Change current simulator
            void ps_changeSimulator(const BlackMisc::Simulation::CSimulatorInfo &simulator);

            //! Simulator has been changed (in loader)
            void ps_onSimulatorChanged(const BlackMisc::Simulation::CSimulatorInfo &simulator);

            //! View has changed row count
            void ps_onRowCountChanged(int count, bool withFilter);

            //! JSON data have been loaded from disk
            void ps_onJsonDataLoaded(const BlackMisc::Simulation::CSimulatorInfo &simulator);

            //! Preferences changed
            void ps_preferencesChanged();

            //! Model has been changed
            void ps_modelChanged();

        private:
            //! Default file name
            void setSaveFileName(const BlackMisc::Simulation::CSimulatorInfo &sim);

            //! Simulator
            void setSimulator(const BlackMisc::Simulation::CSimulatorInfo &simulator);

            //! Update distributor order
            void updateDistributorOrder(const BlackMisc::Simulation::CSimulatorInfo &simulator);

            QScopedPointer<Ui::CDbOwnModelSetComponent> ui;
            QScopedPointer<CDbOwnModelSetDialog>           m_modelSetDialog;
            BlackMisc::Simulation::CAircraftModelSetLoader m_modelSetLoader { BlackMisc::Simulation::CSimulatorInfo(BlackMisc::Simulation::CSimulatorInfo::FSX), this };
            BlackMisc::CSetting<BlackCore::Settings::Simulation::DistributorListPreferences> m_distributorPreferences { this, &CDbOwnModelSetComponent::ps_preferencesChanged };

            //! The menu for loading and handling own models for mapping tasks
            //! \note This is specific for that very component
            class CLoadModelsMenu : public BlackGui::Menus::IMenuDelegate
            {
            public:
                //! Constructor
                CLoadModelsMenu(CDbOwnModelSetComponent *ownModelSetComponent, bool separator = true) :
                    BlackGui::Menus::IMenuDelegate(ownModelSetComponent, separator)
                {}

                //! \copydoc IMenuDelegate::customMenu
                virtual void customMenu(BlackGui::Menus::CMenuActions &menuActions) override;

            private:
                QList<QAction *> m_setActions;
                QList<QAction *> m_setNewActions;
            };
        };
    } // ns
} // ns

#endif // guard
