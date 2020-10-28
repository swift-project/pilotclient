/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_DBMODELSWORKBENCHCOMPONENT_H
#define BLACKGUI_COMPONENTS_DBMODELSWORKBENCHCOMPONENT_H

#include "blackgui/overlaymessagesframe.h"
#include "blackmisc/simulation/aircraftmodelinterfaces.h"
#include "blackmisc/simulation/aircraftmodellist.h"
#include "blackmisc/directories.h"
#include "blackmisc/statusmessage.h"

#include <QFrame>
#include <QList>
#include <QObject>
#include <QScopedPointer>
#include <QStringList>

namespace Ui { class CDbModelWorkbenchComponent; }
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
        class CDbModelWorkbenchComponent :
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
            explicit CDbModelWorkbenchComponent(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CDbModelWorkbenchComponent() override;

            //! Log categories
            static const QStringList &getLogCategories();

            //! Own (installed) model for given model string
            BlackMisc::Simulation::CAircraftModel getOwnModelForModelString(const QString &modelString) const;

            //! Own models selected in view
            BlackMisc::Simulation::CAircraftModelList getSelectedModels() const;

            //! Models
            BlackMisc::Simulation::CAircraftModelList getModels() const;

            //! Number of own models
            int getModelsCount() const;

            //! Models view
            BlackGui::Views::CAircraftModelView *view() const;

            //! Access to aircraft model
            Models::CAircraftModelListModel *model() const;

            //! \name Implementations of the models interfaces
            //! @{
            virtual void setModels(const BlackMisc::Simulation::CAircraftModelList &models) override  { this->setModelsForSimulator(models, BlackMisc::Simulation::CSimulatorInfo()); }
            virtual void setModelsForSimulator(const BlackMisc::Simulation::CAircraftModelList &models, const BlackMisc::Simulation::CSimulatorInfo &simulator) override;
            virtual int updateModels(const BlackMisc::Simulation::CAircraftModelList &models) override  { return this->updateModelsForSimulator(models, BlackMisc::Simulation::CSimulatorInfo()); }
            virtual int updateModelsForSimulator(const BlackMisc::Simulation::CAircraftModelList &models, const BlackMisc::Simulation::CSimulatorInfo &simulator) override;
            //! @}

        private:
            QScopedPointer<Ui::CDbModelWorkbenchComponent> ui;
            BlackMisc::CSetting<BlackMisc::Settings::TDirectorySettings> m_directorySettings { this }; //!< the swift directories
        };
    } // ns
} // ns
#endif // guard
