/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#ifndef BLACKGUI_MENUS_AIRCRAFTMODELMENUS_H
#define BLACKGUI_MENUS_AIRCRAFTMODELMENUS_H

#include "blackgui/menus/menudelegate.h"
#include "blackgui/overlaymessagesframe.h"
#include "blackgui/views/aircraftmodelview.h"
#include "blackmisc/simulation/aircraftmodelinterfaces.h"

#include <QMenu>
#include <QObject>

class QAction;

namespace BlackGui
{
    namespace Components { class CDbMappingComponent; }
    namespace Menus
    {
        //! Menu base class for aircraft model view menus
        class IAircraftModelViewMenu : public IMenuDelegate
        {
            Q_OBJECT

            virtual void anchor();

        public:
            //! Constructor
            IAircraftModelViewMenu(BlackGui::Views::CAircraftModelView *modelView) :
                IMenuDelegate(modelView)
            {}

            //! Log.categories
            static const QStringList &getLogCategories();

        protected:
            //! Model view
            BlackGui::Views::CAircraftModelView *modelView() const;

            //! Get aircraft models
            const BlackMisc::Simulation::CAircraftModelList &getAircraftModels() const;

            //! Get aircraft models (all, or all filtered)
            const BlackMisc::Simulation::CAircraftModelList &getAllOrAllFilteredAircraftModels(bool *filtered = nullptr) const;

            //! Selected aircraft models
            BlackMisc::Simulation::CAircraftModelList getSelectedAircraftModels() const;
        };

        //! Open the simulator file (e.g. aircraft.cfg) in the standard text editor
        class CShowSimulatorFileMenu : public IAircraftModelViewMenu
        {
            Q_OBJECT

        public:
            //! Constructor
            CShowSimulatorFileMenu(Views::CAircraftModelView *modelView, COverlayMessagesFrame *messageFrame);

            //! Log.categories
            static const QStringList &getLogCategories();

            //! \copydoc IMenuDelegate::customMenu
            virtual void customMenu(CMenuActions &menuActions) override;

        private:
            void showSimulatorFile();    //!< simulator file
            void displayIcon();          //!< aircraft icon if any
            void startModelConverterX(); //!< show in model converter X

            BlackGui::COverlayMessagesFrame *m_messageFrame = nullptr;
            QAction *m_fileAction = nullptr;
            QAction *m_iconAction = nullptr;
            QAction *m_modelConverterX = nullptr;
        };

        //! Consolidate/merge with DB data
        class CConsolidateWithDbDataMenu : public IAircraftModelViewMenu
        {
            Q_OBJECT

        public:
            //! Constructor
            using IAircraftModelViewMenu::IAircraftModelViewMenu;

            //! Constructor
            CConsolidateWithDbDataMenu(BlackGui::Views::CAircraftModelView *modelView, QObject *modelsTarget);

            //! Log.categories
            static const QStringList &getLogCategories();

            //! \copydoc IMenuDelegate::customMenu
            virtual void customMenu(CMenuActions &menuActions) override;

            //! Change target
            void setModelsTarget(QObject *target) { m_modelsTarget = target; }

        private:
            void consolidateData();
            void consolidateSelectedData();

            BlackMisc::Simulation::IModelsSetable   *modelsTargetSetable() const;
            BlackMisc::Simulation::IModelsUpdatable *modelsTargetUpdatable() const;
            QObject *m_modelsTarget        = nullptr; //!< optional target for setting/updating the models
            QAction *m_consolidateAll      = nullptr; //!< consolidate data with DB (all)
            QAction *m_consolidateSelected = nullptr; //!< consolidate data with DB (selected)
        };

        //! Merge/update with simulator models (e.g. remove no longer existing models)
        class CConsolidateWithSimulatorModels : public IAircraftModelViewMenu
        {
            Q_OBJECT

        public:
            //! Constructor
            using IAircraftModelViewMenu::IAircraftModelViewMenu;

            //! Constructor
            CConsolidateWithSimulatorModels(Views::CAircraftModelView *modelView, QObject *modelsTarget);

            //! Log.categories
            static const QStringList &getLogCategories();

            //! \copydoc IMenuDelegate::customMenu
            virtual void customMenu(CMenuActions &menuActions) override;

            //! Change target
            void setModelsTarget(QObject *target) { m_modelsTarget = target; }

        private:
            void consolidateData();
            void consolidateSelectedData();
            void updateDirectoryData();
            void updateDirectorySelectedData();

            //! Get models
            BlackMisc::Simulation::CAircraftModelList getSimulatorModels() const;

            //! Simulator
            BlackMisc::Simulation::CSimulatorInfo getSimulator() const;

            BlackMisc::Simulation::IModelsForSimulatorSetable   *modelsTargetSetable() const;
            BlackMisc::Simulation::IModelsForSimulatorUpdatable *modelsTargetUpdatable() const;
            BlackMisc::Simulation::ISimulatorSelectable         *simulatorSelectable() const;
            BlackGui::Components::CDbMappingComponent           *getMappingComponent() const;

            QObject *m_modelsTarget        = nullptr; //!< optional target for setting/updating the models
            QAction *m_consolidateAll      = nullptr; //!< consolidate data with DB (all)
            QAction *m_consolidateSelected = nullptr; //!< consolidate data with DB (selected)
            QAction *m_updateDirsAll       = nullptr; //!< consolidate file name/dir (all)
            QAction *m_updateDirsSelected  = nullptr; //!< consolidate file name/dir (selected)
        };
    } // ns
} // ns

#endif // guard
