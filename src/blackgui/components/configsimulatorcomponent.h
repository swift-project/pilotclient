// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_COMPONENTS_CONFIGSIMULATORCOMPONENT_H
#define BLACKGUI_COMPONENTS_CONFIGSIMULATORCOMPONENT_H

#include "misc/simulation/data/modelcaches.h"
#include "blackcore/application/applicationsettings.h"
#include <QWizardPage>
#include <QFrame>
#include <QScopedPointer>

namespace Ui
{
    class CConfigSimulatorComponent;
}
namespace BlackGui::Components
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
        virtual ~CConfigSimulatorComponent();

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

        swift::misc::CSetting<BlackCore::Application::TEnabledSimulators> m_enabledSimulators { this };
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
        virtual void initializePage() override;

        //! \copydoc QWizardPage::validatePage
        virtual bool validatePage() override;

    private:
        CConfigSimulatorComponent *m_config = nullptr;
    };
} // ns
#endif // guard
