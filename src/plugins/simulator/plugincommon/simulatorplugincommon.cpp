/* Copyright (C) 2018
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "simulatorplugincommon.h"
#include "blackgui/components/interpolationlogdisplaydialog.h"
#include "blackgui/guiapplication.h"
#include "blackmisc/simplecommandparser.h"

using namespace BlackGui;
using namespace BlackGui::Components;
using namespace BlackCore;
using namespace BlackMisc;
using namespace BlackMisc::Network;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Weather;

namespace BlackSimPlugin::Common
{
    CSimulatorPluginCommon::CSimulatorPluginCommon(
        const CSimulatorPluginInfo &info,
        IOwnAircraftProvider    *ownAircraftProvider,
        IRemoteAircraftProvider *renderedAircraftProvider,
        IWeatherGridProvider    *weatherGridProvider,
        IClientProvider         *clientProvider,
        QObject *parent) :
        ISimulator(info, ownAircraftProvider, renderedAircraftProvider, weatherGridProvider, clientProvider, parent)
    {
        CSimulatorPluginCommon::registerHelp();
    }

    CSimulatorPluginCommon::~CSimulatorPluginCommon()
    {
        this->deleteInterpolationDisplay();
    }

    void CSimulatorPluginCommon::deleteInterpolationDisplay()
    {
        if (m_interpolationDisplayDialog)
        {
            // if there is no parent widget, we clean dialog up
            // otherwise the parent is supposed to clean up
            CInterpolationLogDisplayDialog *dialog = m_interpolationDisplayDialog;
            m_interpolationDisplayDialog = nullptr;
            dialog->close();
            delete dialog;
        }
    }

    void CSimulatorPluginCommon::showInterpolationDisplay()
    {
        if (!m_interpolationDisplayDialog)
        {
            QWidget *parentWidget = sGui ? sGui->mainApplicationWidget() : nullptr;
            CInterpolationLogDisplayDialog *dialog = new CInterpolationLogDisplayDialog(this, nullptr, parentWidget);
            m_interpolationDisplayDialog = dialog;
            m_interpolationDisplayDialog->setModal(false);
        }

        if (m_interpolationDisplayDialog) { m_interpolationDisplayDialog->show(); }
    }

    bool CSimulatorPluginCommon::parseDetails(const CSimpleCommandParser &parser)
    {
        // .driver fsuipc on|off
        if (parser.matchesPart(1, "intdisplay"))
        {
            this->showInterpolationDisplay();
            return true;
        }
        return false;
    }

    void CSimulatorPluginCommon::unload()
    {
        this->deleteInterpolationDisplay();
        ISimulator::unload();
    }

    bool CSimulatorPluginCommon::disconnectFrom()
    {
        this->deleteInterpolationDisplay();
        return ISimulator::disconnectFrom();
    }

    void CSimulatorPluginCommon::registerHelp()
    {
        if (CSimpleCommandParser::registered("BlackSimPlugin::Common::CSimulatorPluginCommon")) { return; }
        CSimpleCommandParser::registerCommand({".drv intdisplay", "interpolation display"});
    }
} // namespace
