// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_CONFIGSIMULATORCOMPONENT_H
#define SWIFT_GUI_COMPONENTS_CONFIGSIMULATORCOMPONENT_H

#include <QFrame>
#include <QScopedPointer>
#include <QWizardPage>

#include "core/application/applicationsettings.h"
#include "misc/simulation/data/modelcaches.h"

namespace Ui
{
    class CConfigSimulatorComponent;
}
namespace swift::gui::components
{
    /*!
     * Simulator configuration
     */
    class CConfigSimulatorComponent : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CConfigSimulatorComponent(QWidget *parent = nullptr);

        //! Destructor
        ~CConfigSimulatorComponent() override;

        //! Save data
        void save();

        //! Has unsaved changes
        bool hasUnsavedChanges() const;

        //! Reset the flag
        void resetUnsavedChanges();

    private:
        //! Preselect simulators
        void preselectSimulators();

        //! Get the plugin ids
        QStringList selectedSimsToPluginIds();

        swift::misc::CSetting<swift::core::application::TEnabledSimulators> m_enabledSimulators { this };
        swift::misc::simulation::data::CModelSetCaches m_modelSets { true, this };
        QScopedPointer<Ui::CConfigSimulatorComponent> ui;
    };

    /*!
     * Wizard page for CConfigSimulatorComponent
     */
    class CConfigSimulatorWizardPage : public QWizardPage
    {
        Q_OBJECT

    public:
        //! Constructors
        using QWizardPage::QWizardPage;

        //! Set config
        void setConfigComponent(CConfigSimulatorComponent *config) { m_config = config; }

        //! \copydoc QWizardPage::initializePage
        void initializePage() override;

        //! \copydoc QWizardPage::validatePage
        bool validatePage() override;

    private:
        CConfigSimulatorComponent *m_config = nullptr;
    };
} // namespace swift::gui::components
#endif // SWIFT_GUI_COMPONENTS_CONFIGSIMULATORCOMPONENT_H
