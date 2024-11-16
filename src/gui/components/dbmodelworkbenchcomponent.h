// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_DBMODELSWORKBENCHCOMPONENT_H
#define SWIFT_GUI_COMPONENTS_DBMODELSWORKBENCHCOMPONENT_H

#include <QFrame>
#include <QList>
#include <QObject>
#include <QScopedPointer>
#include <QStringList>

#include "gui/overlaymessagesframe.h"
#include "misc/directories.h"
#include "misc/simulation/aircraftmodelinterfaces.h"
#include "misc/simulation/aircraftmodellist.h"
#include "misc/statusmessage.h"

namespace Ui
{
    class CDbModelWorkbenchComponent;
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
        class CDbModelWorkbenchComponent :
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
            explicit CDbModelWorkbenchComponent(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CDbModelWorkbenchComponent() override;

            //! Log categories
            static const QStringList &getLogCategories();

            //! Own (installed) model for given model string
            swift::misc::simulation::CAircraftModel getOwnModelForModelString(const QString &modelString) const;

            //! Own models selected in view
            swift::misc::simulation::CAircraftModelList getSelectedModels() const;

            //! Models
            swift::misc::simulation::CAircraftModelList getModels() const;

            //! Number of own models
            int getModelsCount() const;

            //! Models view
            swift::gui::views::CAircraftModelView *view() const;

            //! Access to aircraft model
            models::CAircraftModelListModel *model() const;

            //! \name Implementations of the models interfaces
            //! @{
            virtual void setModels(const swift::misc::simulation::CAircraftModelList &models) override
            {
                this->setModelsForSimulator(models, swift::misc::simulation::CSimulatorInfo());
            }
            virtual void setModelsForSimulator(const swift::misc::simulation::CAircraftModelList &models,
                                               const swift::misc::simulation::CSimulatorInfo &simulator) override;
            virtual int updateModels(const swift::misc::simulation::CAircraftModelList &models) override
            {
                return this->updateModelsForSimulator(models, swift::misc::simulation::CSimulatorInfo());
            }
            virtual int updateModelsForSimulator(const swift::misc::simulation::CAircraftModelList &models,
                                                 const swift::misc::simulation::CSimulatorInfo &simulator) override;
            //! @}

        private:
            QScopedPointer<Ui::CDbModelWorkbenchComponent> ui;
            swift::misc::CSetting<swift::misc::settings::TDirectorySettings> m_directorySettings {
                this
            }; //!< the swift directories
        };
    } // namespace components
} // namespace swift::gui
#endif // guard
