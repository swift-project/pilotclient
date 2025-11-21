// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_SELECTSIMULATORCOMPONENT_H
#define SWIFT_GUI_COMPONENTS_SELECTSIMULATORCOMPONENT_H

#include <QFrame>
#include <QScopedPointer>
#include <QWizardPage>

#include "core/actionbind.h"
#include "core/application/applicationsettings.h"
#include "gui/models/actionhotkeylistmodel.h"
#include "gui/swiftguiexport.h"
#include "misc/icons.h"
#include "misc/identifierlist.h"
#include "misc/settingscache.h"
#include "misc/simulation/data/modelcaches.h"

namespace Ui
{
    class CSelectSimulatorComponent;
}
namespace swift::gui::components
{
    /*!
     * Simulator configuration
     */
    class CSelectSimulatorComponent : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CSelectSimulatorComponent(QWidget *parent = nullptr);

        //! Destructor
        ~CSelectSimulatorComponent() override;

        //! Save data
        void save();

        //! Load data
        void load();

        //! Has unsaved changes
        bool hasUnsavedChanges() const;

        //! Reset the flag
        void resetUnsavedChanges();

        swift::misc::simulation::CSimulatorInfo getSelectedSimulators();

    private:
        //! Preselect simulators
        void preselectSimulators();

        void preselectOptions();

        //! Get the plugin ids
        QStringList selectedSimsToPluginIds();

        //! Get selected options
        QStringList selectedOptions();

        bool m_GenerateModelsets = false;
        bool m_PTT = false;
        bool m_SetExportMode = false;

        swift::misc::CSetting<swift::core::application::TEnabledSimulators> m_enabledSimulators { this };
        swift::misc::CSetting<swift::core::application::TEnabledConfigOptions> m_enabledConfigOptions { this };
        swift::misc::simulation::data::CModelSetCaches m_modelSets { true, this };
        QScopedPointer<Ui::CSelectSimulatorComponent> ui;
    };

    /*!
     * Wizard page for CConfigSimulatorComponent
     */
    class CSelectSimulatorWizardPage : public QWizardPage
    {
        Q_OBJECT

    public:
        //! Constructors
        using QWizardPage::QWizardPage;

        explicit CSelectSimulatorWizardPage(CSelectSimulatorComponent *config, QWidget *parent = nullptr)
            : QWizardPage(parent), m_config(config)
        {}

        //! Set config
        void setConfigComponent(CSelectSimulatorComponent *config) { m_config = config; }

        //! \copydoc QWizardPage::initializePage
        void initializePage() override;

        //! \copydoc QWizardPage::validatePage
        bool validatePage() override;

    private:
        CSelectSimulatorComponent *m_config = nullptr;
    };
} // namespace swift::gui::components
#endif // SWIFT_GUI_COMPONENTS_SELECTSIMULATORCOMPONENT_H
