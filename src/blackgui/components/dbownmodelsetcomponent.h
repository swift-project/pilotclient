// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_CDBOWNMODELSETCOMPONENT_H
#define BLACKGUI_COMPONENTS_CDBOWNMODELSETCOMPONENT_H

#include "blackgui/components/dbmappingcomponentaware.h"
#include "blackgui/components/simulatorselector.h"
#include "blackgui/menus/menudelegate.h"
#include "misc/simulation/data/modelcaches.h"
#include "misc/simulation/settings/modelsettings.h"
#include "misc/simulation/settings/simulatorsettings.h"
#include "misc/simulation/aircraftmodelinterfaces.h"
#include "misc/simulation/aircraftmodellist.h"
#include "misc/simulation/simulatorinfo.h"
#include "misc/statusmessage.h"

#include <QFrame>
#include <QList>
#include <QObject>
#include <QScopedPointer>

class QAction;

namespace Ui
{
    class CDbOwnModelSetComponent;
}
namespace swift::misc::simulation
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
            public swift::misc::simulation::data::CCentralMultiSimulatorModelSetCachesAware,
            public swift::misc::simulation::IModelsSetable,
            public swift::misc::simulation::IModelsUpdatable,
            public swift::misc::simulation::ISimulatorSelectable
        {
            Q_OBJECT
            Q_INTERFACES(BlackGui::Components::CDbMappingComponentAware)
            Q_INTERFACES(swift::misc::simulation::IModelsSetable)
            Q_INTERFACES(swift::misc::simulation::IModelsUpdatable)
            Q_INTERFACES(swift::misc::simulation::IModelsForSimulatorSetable)
            Q_INTERFACES(swift::misc::simulation::IModelsForSimulatorUpdatable)
            Q_INTERFACES(swift::misc::simulation::ISimulatorSelectable)

        public:
            //! Constructor
            explicit CDbOwnModelSetComponent(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CDbOwnModelSetComponent() override;

            //! Corresponding view
            Views::CAircraftModelView *view() const;

            //! Add to model set
            swift::misc::CStatusMessage addToModelSet(const swift::misc::simulation::CAircraftModelList &models, const swift::misc::simulation::CSimulatorInfo &simulator);

            //! Add to model set
            swift::misc::CStatusMessage addToModelSet(const swift::misc::simulation::CAircraftModel &model, const swift::misc::simulation::CSimulatorInfo &simulator);

            //! Current model set for simulator CDbOwnModelSetComponent::getModelSetSimulator
            //! \remark this the set from the container, which can be different from cache while updating
            const swift::misc::simulation::CAircraftModelList &getModelSetFromView() const;

            //! Current sount of model set for simulator CDbOwnModelSetComponent::getModelSetSimulator
            //! \remark this the set from the container, which can be different from cache while updating
            int getModelSetCountFromView() const;

            //! Cached models for current simulator
            swift::misc::simulation::CAircraftModelList getModelSet() const { return this->getCachedModels(m_simulator); }

            //! Cached models count for current simulator
            int getModelSetCount() const { return this->getCachedModelsCount(m_simulator); }

            //! Count and cache timestamp
            QString getModelCacheCountAndTimestamp() const { return QString::number(getModelSetCount()); }

            //! Simulator
            void setSimulator(const swift::misc::simulation::CSimulatorInfo &simulator);

            //! Deferred init of simulator
            void triggerSetSimulatorDeferred(const swift::misc::simulation::CSimulatorInfo &simulator);

            //! \copydoc BlackGui::Components::CSimulatorSelector::setMode
            void setSimulatorSelectorMode(CSimulatorSelector::Mode mode);

            //! \copydoc CDbMappingComponentAware::setMappingComponent
            virtual void setMappingComponent(CDbMappingComponent *component) override;

            //! Model set is for simulator
            swift::misc::simulation::CSimulatorInfo getModelSetSimulator() const { return m_simulator; }

            //! \name Implementations of the models interfaces
            //! @{
            virtual void setModels(const swift::misc::simulation::CAircraftModelList &models) override { this->setModelSet(models, this->getModelSetSimulator()); }
            virtual int updateModels(const swift::misc::simulation::CAircraftModelList &models) override { return this->replaceOrAddModelSet(models, this->getModelSetSimulator()); }
            virtual swift::misc::simulation::CSimulatorInfo getSelectedSimulator() const override { return this->getModelSetSimulator(); }
            //! @}

            //! Enable buttons
            void enableButtons(bool firstSet, bool newSet);

            //! Set the model set for a given simulator
            void setModelSet(const swift::misc::simulation::CAircraftModelList &models, const swift::misc::simulation::CSimulatorInfo &simulator);

            //! Replace or add models provided for a given simulator
            int replaceOrAddModelSet(const swift::misc::simulation::CAircraftModelList &models, const swift::misc::simulation::CSimulatorInfo &simulator);

        private:
            //! Tab has been changed
            void tabIndexChanged(int index);

            //! Button was clicked
            void buttonClicked();

            //! View has changed row count
            void onRowCountChanged(int count, bool withFilter);

            //! JSON data have been loaded from disk
            void onJsonDataLoaded(const swift::misc::simulation::CSimulatorInfo &simulator);

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
            void setSaveFileName(const swift::misc::simulation::CSimulatorInfo &simulator);

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
            void updateDistributorOrder(const swift::misc::simulation::CSimulatorInfo &simulator);

            //! Is that component running in a dialog
            bool runsInDialog();

            QScopedPointer<Ui::CDbOwnModelSetComponent> ui;
            QScopedPointer<CDbOwnModelSetFormDialog> m_modelSetFormDialog;
            QScopedPointer<CFirstModelSetDialog> m_firstModelSetDialog;
            QScopedPointer<CDbReduceModelDuplicates> m_reduceModelsDialog;
            QScopedPointer<CCopyModelsFromOtherSwiftVersionsDialog> m_copyFromAnotherSwiftDialog;
            QScopedPointer<Views::CAircraftModelStatisticsDialog> m_modelStatisticsDialog;

            swift::misc::simulation::CSimulatorInfo m_simulator; //!< currently set simulator
            swift::misc::CSettingReadOnly<swift::misc::simulation::settings::TDistributorListPreferences> m_distributorPreferences { this, &CDbOwnModelSetComponent::distributorPreferencesChanged }; //!< distributor preferences
            swift::misc::CSettingReadOnly<swift::misc::simulation::settings::TModel> m_modelSettings { this }; //!< settings for models
            swift::misc::simulation::settings::CMultiSimulatorSettings m_simulatorSettings { this }; //!< for directories

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
