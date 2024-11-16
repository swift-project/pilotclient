// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_FIRSTMODELSETCOMPONENT_H
#define SWIFT_GUI_COMPONENTS_FIRSTMODELSETCOMPONENT_H

#include <QFrame>
#include <QWizardPage>

#include "core/application/applicationsettings.h"
#include "gui/overlaymessagesframe.h"
#include "gui/swiftguiexport.h"
#include "misc/logcategories.h"
#include "misc/simulation/aircraftmodelloader.h"
#include "misc/simulation/settings/simulatorsettings.h"
#include "misc/simulation/simulatorinfo.h"

namespace Ui
{
    class CFirstModelSetComponent;
}
namespace swift::gui::components
{
    class CDbOwnModelsDialog;
    class CDbOwnModelsComponent;
    class CDbOwnModelSetDialog;
    class CDbOwnModelSetComponent;

    //! Create a first model set
    class CFirstModelSetComponent : public COverlayMessagesFrame
    {
        Q_OBJECT

    public:
        //! Log categories
        static const QStringList &getLogCategories();

        //! Constructor
        explicit CFirstModelSetComponent(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CFirstModelSetComponent();

    private:
        QScopedPointer<Ui::CFirstModelSetComponent> ui;
        QScopedPointer<CDbOwnModelsDialog> m_modelsDialog;
        QScopedPointer<CDbOwnModelSetDialog> m_modelSetDialog;
        swift::misc::simulation::settings::CMultiSimulatorSettings m_simulatorSettings { this };

        //! Simulator has been changed
        void onSimulatorChanged(const swift::misc::simulation::CSimulatorInfo &simulator);

        //! Simulator settings changed
        void onSettingsChanged(const swift::misc::simulation::CSimulatorInfo &simulator);

        //! Models have been loaded
        void onModelsLoaded(const swift::misc::simulation::CSimulatorInfo &simulator, int count);

        //! Asynchronously call onSettingsChanged
        void triggerSettingsChanged(const swift::misc::simulation::CSimulatorInfo &simulator);

        //! Direct access to component
        const CDbOwnModelsComponent *modelsComponent() const;

        //! Direct access to component
        const CDbOwnModelSetComponent *modelSetComponent() const;

        //! Direct access to component's loader
        swift::misc::simulation::IAircraftModelLoader *modelLoader() const;

        //! Open own models dialog
        void openOwnModelsDialog();

        //! Own model set dialog
        void openOwnModelSetDialog();

        //! Change model directory
        void changeModelDirectory();

        //! Create the model set
        void createModelSet();

        //! Main window or this
        QWidget *mainWindow();
    };

    //! Wizard page for CFirstModelSetComponent
    class CFirstModelSetWizardPage : public QWizardPage
    {
        Q_OBJECT

    public:
        //! Constructors
        using QWizardPage::QWizardPage;

        //! Set config
        void setFirstModelSet(CFirstModelSetComponent *firstModelSet) { m_firstModelSet = firstModelSet; }

        //! \copydoc QWizardPage::validatePage
        virtual bool validatePage() override;

    private:
        CFirstModelSetComponent *m_firstModelSet = nullptr;
    };
} // namespace swift::gui::components
#endif // guard
