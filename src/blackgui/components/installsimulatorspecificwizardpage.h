/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_COMPONENTS_INSTALLSIMULATORSPECIFICCOMPONENT_H
#define BLACKGUI_COMPONENTS_INSTALLSIMULATORSPECIFICCOMPONENT_H

#include "blackgui/overlaymessagesframe.h"
#include <QWizard>

namespace BlackGui::Components
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
} // ns
#endif // guard
