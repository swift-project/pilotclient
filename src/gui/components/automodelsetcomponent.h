// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_AUTOMODELSETCOMPONENT_H
#define SWIFT_GUI_COMPONENTS_AUTOMODELSETCOMPONENT_H

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
    class CAutoModelSetComponent;
}
namespace swift::gui::components
{
    class CDbOwnModelsDialog;
    class CDbOwnModelsComponent;
    class CDbOwnModelSetDialog;
    class CDbOwnModelSetComponent;

    //! Create a first model set
    class CAutoModelSetComponent : public COverlayMessagesFrame
    {
        Q_OBJECT

    public:
        //! Log categories
        static const QStringList &getLogCategories();

        //! Constructor
        explicit CAutoModelSetComponent(QWidget *parent = nullptr);

        //! Destructor
        ~CAutoModelSetComponent() override;

    private:
        QScopedPointer<Ui::CAutoModelSetComponent> ui;
        QScopedPointer<CDbOwnModelsDialog> m_modelsDialog;
        QScopedPointer<CDbOwnModelSetDialog> m_modelSetDialog;
        swift::misc::simulation::settings::CMultiSimulatorSettings m_simulatorSettings { this };
        swift::misc::CSetting<swift::core::application::TEnabledConfigOptions> m_enabledConfigOptions { this };
        swift::misc::CSetting<swift::core::application::TEnabledSimulators> m_enabledSimulators { this };

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

        //! Create automatic model sets
        void createAutoModelsets();

        //! Set expert mode for modelset creation
        void toggleExportMode();

        void preselectOptions();

        bool m_expertmode;
        bool m_GenerateModelsets = false;
        bool m_SetExportMode = false;
        swift::misc::simulation::CSimulatorInfo m_simulator;

        //! Main window or this
        QWidget *mainWindow();
    };

    //! Wizard page for CAutoModelSetComponent
    class CAutoModelSetWizardPage : public QWizardPage
    {
        Q_OBJECT

    public:
        //! Constructors
        using QWizardPage::QWizardPage;

        //! Set config
        void setAutoModelSet(CAutoModelSetComponent *firstModelSet) { m_firstModelSet = firstModelSet; }

        //! \copydoc QWizardPage::validatePage
        bool validatePage() override;

    private:
        CAutoModelSetComponent *m_firstModelSet = nullptr;
    };
} // namespace swift::gui::components
#endif // SWIFT_GUI_COMPONENTS_AUTOMODELSETCOMPONENT_H
