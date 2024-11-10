// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_DBAUTOSIMULATORSTASHINGCOMPONENT_H
#define BLACKGUI_COMPONENTS_DBAUTOSIMULATORSTASHINGCOMPONENT_H

#include "blackgui/blackguiexport.h"
#include "blackgui/components/dbmappingcomponentaware.h"
#include "blackgui/views/aircraftmodelview.h"
#include "blackcore/progress.h"
#include "misc/simulation/aircraftmodel.h"
#include "misc/statusmessage.h"
#include <QDialog>
#include <QScopedPointer>

namespace Ui
{
    class CDbAutoSimulatorStashingComponent;
}
namespace BlackGui::Components
{
    /*!
     * Allows to automatically update models if found in own model set, but already existing
     * for a sibling simulator (ie. FSX/P3D/FS9)
     */
    class BLACKGUI_EXPORT CDbAutoSimulatorStashingComponent :
        public QDialog,
        public Components::CDbMappingComponentAware,
        public BlackCore::IProgressIndicator
    {
        Q_OBJECT
        Q_INTERFACES(BlackGui::Components::CDbMappingComponentAware)

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

        //! \copydoc BlackCore::IProgressIndicator::updateProgressIndicator
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
        BlackGui::Views::CAircraftModelView *currentModelView() const;

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
} // ns

#endif // guard
