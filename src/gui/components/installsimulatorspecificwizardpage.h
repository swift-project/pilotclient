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
    class CInstallFsxTerrainProbeComponent;

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
        void setConfigComponent(CInstallXSwiftBusComponent *config, CInstallFsxTerrainProbeComponent *fsxTerrainProbe)
        {
            m_xSwiftBusConfig = config;
            m_fsxTerrainProbe = fsxTerrainProbe;
        }

        //! \copydoc QWizardPage::validatePage
        virtual bool validatePage() override { return true; }

    private:
        CInstallXSwiftBusComponent *m_xSwiftBusConfig = nullptr;
        CInstallFsxTerrainProbeComponent *m_fsxTerrainProbe = nullptr;
    };
} // namespace swift::gui::components
#endif // SWIFT_GUI_COMPONENTS_INSTALLSIMULATORSPECIFICCOMPONENT_H
