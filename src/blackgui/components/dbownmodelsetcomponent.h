// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_CDBOWNMODELSETCOMPONENT_H
#define BLACKGUI_COMPONENTS_CDBOWNMODELSETCOMPONENT_H

#include "blackgui/components/dbmappingcomponentaware.h"
#include "blackgui/components/simulatorselector.h"
#include "blackgui/menus/menudelegate.h"
#include "blackmisc/simulation/data/modelcaches.h"
#include "blackmisc/simulation/settings/modelsettings.h"
#include "blackmisc/simulation/settings/simulatorsettings.h"
#include "blackmisc/simulation/aircraftmodelinterfaces.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/simulation/simulatorinfo.h"
#include "blackmisc/statusmessage.h"

#include <QFrame>
#include <QList>
#include <QObject>
#include <QScopedPointer>

class QAction;

namespace Ui
{
    class CDbOwnModelSetComponent;
}
namespace BlackMisc::Simulation
{
    class CAircraftModel;
}
namespace BlackGui
{
    namespace Menus
    {
        class CMenuActions;
    }
    namespace Views
    {
        class CAircraftModelView;
        class CAircraftModelStatisticsDialog;
    }
    namespace Components
    {
        class CDbMappingComponent;
        class CDbOwnModelSetFormDialog;
        class CDbReduceModelDuplicates;
        class CCopyModelsFromOtherSwiftVersionsDialog;
        class CFirstModelSetDialog;

        /*!
         * Handling of the own model set
         */
        class CDbOwnModelSetComponent :
            public QFrame,
            public CDbMappingComponentAware,
            public BlackMisc::Simulation::Data::CCentralMultiSimulatorModelSetCachesAware,
            public BlackMisc::Simulation::IModelsSetable,
            public BlackMisc::Simulation::IModelsUpdatable,
            public BlackMisc::Simulation::ISimulatorSelectable
        {
            Q_OBJECT
            Q_INTERFACES(BlackGui::Components::CDbMappingComponentAware)
            Q_INTERFACES(BlackMisc::Simulation::IModelsSetable)
            Q_INTERFACES(BlackMisc::Simulation::IModelsUpdatable)
            Q_INTERFACES(BlackMisc::Simulation::IModelsForSimulatorSetable)
            Q_INTERFACES(BlackMisc::Simulation::IModelsForSimulatorUpdatable)
            Q_INTERFACES(BlackMisc::Simulation::ISimulatorSelectable)

        public:
            //! Constructor
            explicit CDbOwnModelSetComponent(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CDbOwnModelSetComponent() override;

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

            //! Cached models for current simulator
            BlackMisc::Simulation::CAircraftModelList getModelSet() const { return this->getCachedModels(m_simulator); }

            //! Cached models count for current simulator
            int getModelSetCount() const { return this->getCachedModelsCount(m_simulator); }

            //! Count and cache timestamp
            QString getModelCacheCountAndTimestamp() const { return QString::number(getModelSetCount()); }

            //! Simulator
            void setSimulator(const BlackMisc::Simulation::CSimulatorInfo &simulator);

            //! Deferred init of simulator
            void triggerSetSimulatorDeferred(const BlackMisc::Simulation::CSimulatorInfo &simulator);

            //! \copydoc BlackGui::Components::CSimulatorSelector::setMode
            void setSimulatorSelectorMode(CSimulatorSelector::Mode mode);

            //! \copydoc CDbMappingComponentAware::setMappingComponent
            virtual void setMappingComponent(CDbMappingComponent *component) override;

            //! Model set is for simulator
            BlackMisc::Simulation::CSimulatorInfo getModelSetSimulator() const { return m_simulator; }

            //! \name Implementations of the models interfaces
            //! @{
            virtual void setModels(const BlackMisc::Simulation::CAircraftModelList &models) override { this->setModelSet(models, this->getModelSetSimulator()); }
            virtual int updateModels(const BlackMisc::Simulation::CAircraftModelList &models) override { return this->replaceOrAddModelSet(models, this->getModelSetSimulator()); }
            virtual BlackMisc::Simulation::CSimulatorInfo getSelectedSimulator() const override { return this->getModelSetSimulator(); }
            //! @}

            //! Enable buttons
            void enableButtons(bool firstSet, bool newSet);

            //! Set the model set for a given simulator
            void setModelSet(const BlackMisc::Simulation::CAircraftModelList &models, const BlackMisc::Simulation::CSimulatorInfo &simulator);

            //! Replace or add models provided for a given simulator
            int replaceOrAddModelSet(const BlackMisc::Simulation::CAircraftModelList &models, const BlackMisc::Simulation::CSimulatorInfo &simulator);

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

            //! Reduce models
            void reduceModels();

            //! Remove excluded models
            void removeExcludedModels();

            //! Remove NON DB models
            void removeNonDBModels();

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
            void showModelStatistics();

            //! Update distributor order
            void updateDistributorOrder(const BlackMisc::Simulation::CSimulatorInfo &simulator);

            //! Is that component running in a dialog
            bool runsInDialog();

            QScopedPointer<Ui::CDbOwnModelSetComponent> ui;
            QScopedPointer<CDbOwnModelSetFormDialog> m_modelSetFormDialog;
            QScopedPointer<CFirstModelSetDialog> m_firstModelSetDialog;
            QScopedPointer<CDbReduceModelDuplicates> m_reduceModelsDialog;
            QScopedPointer<CCopyModelsFromOtherSwiftVersionsDialog> m_copyFromAnotherSwiftDialog;
            QScopedPointer<Views::CAircraftModelStatisticsDialog> m_modelStatisticsDialog;

            BlackMisc::Simulation::CSimulatorInfo m_simulator; //!< currently set simulator
            BlackMisc::CSettingReadOnly<BlackMisc::Simulation::Settings::TDistributorListPreferences> m_distributorPreferences { this, &CDbOwnModelSetComponent::distributorPreferencesChanged }; //!< distributor preferences
            BlackMisc::CSettingReadOnly<BlackMisc::Simulation::Settings::TModel> m_modelSettings { this }; //!< settings for models
            BlackMisc::Simulation::Settings::CMultiSimulatorSettings m_simulatorSettings { this }; //!< for directories

            // -------------------------- custom menus -----------------------------------

            //! The menu for loading models from cache or create a new model set
            //! \note This is specific for that very model set component
            //! \fixme MS 2018-12 Move to namespace scope and add Q_OBJECT
            class CLoadModelSetMenu : public Menus::IMenuDelegate
            {
            public:
                //! Constructor
                CLoadModelSetMenu(CDbOwnModelSetComponent *ownModelSetComponent) : Menus::IMenuDelegate(ownModelSetComponent)
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
