/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_CDBOWNMODELSETCOMPONENT_H
#define BLACKGUI_COMPONENTS_CDBOWNMODELSETCOMPONENT_H

#include "blackgui/components/dbmappingcomponentaware.h"
#include "blackgui/menus/menudelegate.h"
#include "blackmisc/simulation/settings/modelsettings.h"
#include "blackmisc/simulation/aircraftmodelinterfaces.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/aircraftmodelsetloader.h"
#include "blackmisc/simulation/simulatorinfo.h"
#include "blackmisc/statusmessage.h"

#include <QFrame>
#include <QList>
#include <QObject>
#include <QScopedPointer>

class QAction;
class QWidget;

namespace Ui { class CDbOwnModelSetComponent; }
namespace BlackMisc { namespace Simulation { class CAircraftModel; } }
namespace BlackGui
{
    namespace Menus { class CMenuActions; }
    namespace Views { class CAircraftModelView; }
    namespace Components
    {
        class CDbMappingComponent;
        class CDbOwnModelSetFormDialog;
        class CCopyModelsFromOtherSwiftVersionsDialog;
        class CFirstModelSetDialog;

        /*!
         * Handling of the own model set
         */
        class CDbOwnModelSetComponent :
            public QFrame,
            public CDbMappingComponentAware,
            public BlackMisc::Simulation::IModelsSetable,
            public BlackMisc::Simulation::IModelsUpdatable,
            public BlackMisc::Simulation::IModelsPerSimulatorSetable,
            public BlackMisc::Simulation::IModelsPerSimulatorUpdatable,
            public BlackMisc::Simulation::ISimulatorSelectable
        {
            Q_OBJECT
            Q_INTERFACES(BlackGui::Components::CDbMappingComponentAware)
            Q_INTERFACES(BlackMisc::Simulation::IModelsSetable)
            Q_INTERFACES(BlackMisc::Simulation::IModelsUpdatable)
            Q_INTERFACES(BlackMisc::Simulation::IModelsPerSimulatorSetable)
            Q_INTERFACES(BlackMisc::Simulation::IModelsPerSimulatorUpdatable)
            Q_INTERFACES(BlackMisc::Simulation::ISimulatorSelectable)

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
            //! \remark this the set from the container, which can be different from cache while updating
            const BlackMisc::Simulation::CAircraftModelList &getModelSetFromView() const;

            //! Current sount of model set for simulator CDbOwnModelSetComponent::getModelSetSimulator
            //! \remark this the set from the container, which can be different from cache while updating
            int getModelSetCountFromView() const;

            //! \copydoc BlackMisc::Simulation::CAircraftModelSetLoader::getCachedModels
            BlackMisc::Simulation::CAircraftModelList getModelSetFromLoader() const { return m_modelSetLoader.getCachedModels(this->getModelSetSimulator()); }

            //! Model set is for simulator
            BlackMisc::Simulation::CSimulatorInfo getModelSetSimulator() const;

            //! Simulator
            void setSimulator(const BlackMisc::Simulation::CSimulatorInfo &simulator);

            //! Used model set loader
            const BlackMisc::Simulation::CAircraftModelSetLoader &modelSetLoader() const { return m_modelSetLoader; }

            //! \copydoc CDbMappingComponentAware::setMappingComponent
            virtual void setMappingComponent(CDbMappingComponent *component) override;

            //! \name Implementations of the models interfaces
            //! @{
            virtual void setModels(const BlackMisc::Simulation::CAircraftModelList &models) override  { this->setModelSet(models, this->getModelSetSimulator()); }
            virtual void updateModels(const BlackMisc::Simulation::CAircraftModelList &models) override  { this->replaceOrAddModelSet(models, this->getModelSetSimulator()); }
            virtual void setModels(const BlackMisc::Simulation::CAircraftModelList &models, const BlackMisc::Simulation::CSimulatorInfo &simulator) override  { this->setModelSet(models, simulator); }
            virtual void updateModels(const BlackMisc::Simulation::CAircraftModelList &models, const BlackMisc::Simulation::CSimulatorInfo &simulator) override  { this->replaceOrAddModelSet(models, simulator); }
            virtual BlackMisc::Simulation::CSimulatorInfo getSelectedSimulator() const override { return this->getModelSetSimulator(); }
            //! @}

            //! Enable buttons
            void enableButtons(bool firstSet);

            //! Set the model set for a given simulator
            void setModelSet(const BlackMisc::Simulation::CAircraftModelList &models, const BlackMisc::Simulation::CSimulatorInfo &simulator);

            //! Replace or add models provided for a given simulator
            void replaceOrAddModelSet(const BlackMisc::Simulation::CAircraftModelList &models, const BlackMisc::Simulation::CSimulatorInfo &simulator);

        private:
            //! Tab has been changed
            void tabIndexChanged(int index);

            //! Button was clicked
            void buttonClicked();

            //! View has changed row count
            void onRowCountChanged(int count, bool withFilter);

            //! JSON data have been loaded from disk
            void onJsonDataLoaded(const BlackMisc::Simulation::CSimulatorInfo &simulator);

            //! Model (of view) has been changed
            void viewModelChanged();

            //! Preferences changed
            void distributorPreferencesChanged();

            //! Default file name
            void setSaveFileName(const BlackMisc::Simulation::CSimulatorInfo &simulator);

            //! Update view to current models
            void updateViewToCurrentModels();

            //! Create new set
            void createNewSet();

            //! First set wizard
            void firstSet();

            //! Copy from another swift version
            void copyFromAnotherSwift();

            //! Show the airline/aircraft matrix
            void showAirlineAircraftMatrix() const;

            //! Update distributor order
            void updateDistributorOrder(const BlackMisc::Simulation::CSimulatorInfo &simulator);

            QScopedPointer<Ui::CDbOwnModelSetComponent> ui;
            QScopedPointer<CDbOwnModelSetFormDialog>    m_modelSetFormDialog;
            QScopedPointer<CFirstModelSetDialog>        m_firstModelSet;
            QScopedPointer<CCopyModelsFromOtherSwiftVersionsDialog> m_copyFromAnotherSwift;
            BlackMisc::Simulation::CSimulatorInfo m_simulator; //!< currently set simulator
            BlackMisc::Simulation::CAircraftModelSetLoader m_modelSetLoader { this };
            BlackMisc::CSettingReadOnly<BlackMisc::Simulation::Settings::TDistributorListPreferences> m_distributorPreferences { this, &CDbOwnModelSetComponent::distributorPreferencesChanged }; //!< distributor preferences
            BlackMisc::CSettingReadOnly<BlackMisc::Simulation::Settings::TModel> m_modelSettings { this }; //!< settings for models

            // -------------------------- custom menus -----------------------------------

            //! The menu for loading models from cache or create a new model set
            //! \note This is specific for that very model set component
            class CLoadModelSetMenu : public Menus::IMenuDelegate
            {
            public:
                //! Constructor
                CLoadModelSetMenu(CDbOwnModelSetComponent *ownModelSetComponent, bool separator = true) :
                    Menus::IMenuDelegate(ownModelSetComponent, separator)
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
