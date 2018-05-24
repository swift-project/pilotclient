/* Copyright (C) 2018
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "simulatorplugincommon.h"
#include "blackgui/components/interpolationlogdisplaydialog.h"
#include "blackgui/guiapplication.h"
#include "blackmisc/simplecommandparser.h"

using namespace BlackGui::Components;
using namespace BlackCore;
using namespace BlackMisc;
using namespace BlackMisc::Network;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Weather;

namespace BlackSimPlugin
{
    namespace Common
    {
        CSimulatorPluginCommon::CSimulatorPluginCommon(
            const CSimulatorPluginInfo &info,
            IOwnAircraftProvider *ownAircraftProvider,
            IRemoteAircraftProvider *renderedAircraftProvider,
            IWeatherGridProvider *weatherGridProvider,
            IClientProvider *clientProvider,
            QObject *parent) :
            CSimulatorCommon(info, ownAircraftProvider, renderedAircraftProvider, weatherGridProvider, clientProvider, parent)
        {
            CSimulatorPluginCommon::registerHelp();
        }

        CSimulatorPluginCommon::~CSimulatorPluginCommon()
        {
            if (m_interpolationDisplay)
            {
                m_interpolationDisplay->deleteLater();
            }
        }

        void CSimulatorPluginCommon::showInterpolationDisplay()
        {
            if (!m_interpolationDisplay)
            {
                QWidget *parentWidget = sGui ? sGui->mainApplicationWidget() : nullptr;
                CInterpolationLogDisplayDialog *dialog = new CInterpolationLogDisplayDialog(this, nullptr, parentWidget);
                m_interpolationDisplay = dialog;
            }
            m_interpolationDisplay->show();
        }

        bool CSimulatorPluginCommon::parseDetails(const CSimpleCommandParser &parser)
        {
            // .driver fsuipc on|off
            if (parser.matchesPart(1, "intdisplay"))
            {
                this->showInterpolationDisplay();
                return true;
            }
            return CSimulatorCommon::parseDetails(parser);
        }

        void CSimulatorPluginCommon::unload()
        {
            if (m_interpolationDisplay)
            {
                m_interpolationDisplay->hide();
                m_interpolationDisplay->deleteLater();
            }
            CSimulatorCommon::unload();
        }

        void CSimulatorPluginCommon::registerHelp()
        {
            if (CSimpleCommandParser::registered("BlackSimPlugin::Common::CSimulatorPluginCommon")) { return; }
            CSimpleCommandParser::registerCommand({".drv intdisplay", "interpolation display"});
        }
    } // namespace
} // namespace
