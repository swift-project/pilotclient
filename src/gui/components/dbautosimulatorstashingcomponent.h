// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_DBAUTOSIMULATORSTASHINGCOMPONENT_H
#define SWIFT_GUI_COMPONENTS_DBAUTOSIMULATORSTASHINGCOMPONENT_H

#include <QDialog>
#include <QScopedPointer>

#include "core/progress.h"
#include "gui/components/dbmappingcomponentaware.h"
#include "gui/swiftguiexport.h"
#include "gui/views/aircraftmodelview.h"
#include "misc/simulation/aircraftmodel.h"
#include "misc/statusmessage.h"

namespace Ui
{
    class CDbAutoSimulatorStashingComponent;
}
namespace swift::gui::components
{
    /*!
     * Allows to automatically update models if found in own model set, but already existing
     * for a sibling simulator (ie. FSX/P3D/FS9)
     */
    class SWIFT_GUI_EXPORT CDbAutoSimulatorStashingComponent :
        public QDialog,
        public components::CDbMappingComponentAware,
        public swift::core::IProgressIndicator
    {
        Q_OBJECT
        Q_INTERFACES(swift::gui::components::CDbMappingComponentAware)

    public:
        //! Current state of this component
        enum State
        {
            Idle,
            Running,
            Completed
        };

        //! Constructor
        explicit CDbAutoSimulatorStashingComponent(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CDbAutoSimulatorStashingComponent() override;

        //! At least run once and completed
        bool isCompleted() const { return m_state == Completed; }

        //! \copydoc swift::core::IProgressIndicator::updateProgressIndicator
        virtual void updateProgressIndicator(int percent) override;

    public slots:
        //! \copydoc QDialog::accept
        virtual void accept() override;

        //! \copydoc QDialog::exec
        virtual int exec() override;

    private:
        //! Init the GUI
        void initGui();

        //! Model view to take models from
        swift::gui::views::CAircraftModelView *currentModelView() const;

        //! Add a status message
        void addStatusMessage(const swift::misc::CStatusMessage &msg);

        //! Add a status messages
        void addStatusMessages(const swift::misc::CStatusMessageList &msgs);

        //! Add a status message for a given model (prefixed)
        void addStatusMessage(const swift::misc::CStatusMessage &msg, const swift::misc::simulation::CAircraftModel &model);

        //! Try to stash updated models
        void tryToStash();

        QScopedPointer<Ui::CDbAutoSimulatorStashingComponent> ui;
        State m_state = Idle; //!< modus
        swift::misc::simulation::CAircraftModelList m_modelsToStash; //!< Models about to be stashed
    };
} // namespace swift::gui::components

#endif // guard
