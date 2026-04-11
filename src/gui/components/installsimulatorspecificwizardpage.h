// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_COMPONENTS_INSTALLSIMULATORSPECIFICCOMPONENT_H
#define SWIFT_GUI_COMPONENTS_INSTALLSIMULATORSPECIFICCOMPONENT_H

#include <QWizard>

#include "gui/overlaymessagesframe.h"

namespace swift::gui::components
{
    class CInstallXSwiftBusComponent;

    /*!
     * Simulator specific installation
     */
    class CInstallSimulatorSpecificWizardPage : public COverlayMessagesWizardPage
    {
        Q_OBJECT

    public:
        //! Constructors
        using COverlayMessagesWizardPage::COverlayMessagesWizardPage;

        //! Set config
        void setConfigComponent(CInstallXSwiftBusComponent *config) { m_xSwiftBusConfig = config; }

        //! \copydoc QWizardPage::validatePage
        bool validatePage() override { return true; }

    private:
        CInstallXSwiftBusComponent *m_xSwiftBusConfig = nullptr;
    };
} // namespace swift::gui::components
#endif // SWIFT_GUI_COMPONENTS_INSTALLSIMULATORSPECIFICCOMPONENT_H
