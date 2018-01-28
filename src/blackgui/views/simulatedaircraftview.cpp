/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackconfig/buildconfig.h"
#include "blackgui/models/simulatedaircraftlistmodel.h"
#include "blackgui/menus/menuaction.h"
#include "blackgui/views/simulatedaircraftview.h"
#include "blackgui/guiapplication.h"
#include "blackcore/context/contextsimulator.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/icons.h"
#include "blackmisc/simulation/simulatedaircraft.h"

#include <QtGlobal>

using namespace BlackConfig;
using namespace BlackMisc;
using namespace BlackMisc::Simulation;
using namespace BlackCore::Context;
using namespace BlackGui::Models;
using namespace BlackGui::Menus;

namespace BlackGui
{
    namespace Views
    {
        CSimulatedAircraftView::CSimulatedAircraftView(QWidget *parent) : CViewWithCallsignObjects(parent)
        {
            this->standardInit(new CSimulatedAircraftListModel(this));
            this->m_menus |= MenuRefresh;
        }

        void CSimulatedAircraftView::setAircraftMode(CSimulatedAircraftListModel::AircraftMode mode)
        {
            Q_ASSERT(this->m_model);
            this->m_model->setAircraftMode(mode);
            this->setSortIndicator();
        }

        void CSimulatedAircraftView::configureMenu(bool menuHighlight, bool menuEnable, bool menufastPositionUpdates)
        {
            this->m_withMenuEnable = menuEnable;
            this->m_withMenuFastPosition = menufastPositionUpdates;
            this->m_withMenuHighlight = menuHighlight;
        }

        void CSimulatedAircraftView::customMenu(CMenuActions &menuActions)
        {
            if (this->hasSelection())
            {
                CSimulatedAircraft aircraft(selectedObject());
                Q_ASSERT(!aircraft.getCallsign().isEmpty());
                menuActions.addAction(CIcons::appTextMessages16(), "Show text messages", CMenuAction::pathClientCom(), { this, &CSimulatedAircraftView::requestTextMessage });
                if (m_withMenuEnable)       { menuActions.addAction(CIcons::appAircraft16(), aircraft.isEnabled() ? "Disable aircraft" : "Enabled aircraft", CMenuAction::pathClientSimulation(), { this, &CSimulatedAircraftView::toogleEnabledAircraft }); }
                if (m_withMenuHighlight)    { menuActions.addAction(CIcons::appSimulator16(), "Highlight in simulator", CMenuAction::pathClientSimulation(), { this, &CSimulatedAircraftView::highlightInSimulator }); }
                if (m_withMenuFastPosition) { menuActions.addAction(CIcons::globe16(), aircraft.fastPositionUpdates() ? "Normal updates" : "Fast position updates",  CMenuAction::pathClientSimulation(), { this, &CSimulatedAircraftView::fastPositionUpdates }); }
                const bool any = m_withMenuEnable || m_withMenuFastPosition || m_withMenuHighlight;
                if (any && (sApp && sApp->isDeveloperFlagSet()))
                {
                    menuActions.addAction(CIcons::appSimulator16(), "Show position log.", CMenuAction::pathClientSimulation(), { this, &CSimulatedAircraftView::showPositionLogInSimulator });
                }
            }
            CViewBase::customMenu(menuActions);
        }

        void CSimulatedAircraftView::requestTextMessage()
        {
            const CSimulatedAircraft aircraft(selectedObject());
            if (aircraft.getCallsign().isEmpty()) { return; }
            emit this->requestTextMessageWidget(aircraft.getCallsign());
        }

        void CSimulatedAircraftView::toogleEnabledAircraft()
        {
            CSimulatedAircraft aircraft(selectedObject());
            if (aircraft.getCallsign().isEmpty()) { return; }
            aircraft.setEnabled(!aircraft.isEnabled());
            emit this->requestEnableAircraft(aircraft);
        }

        void CSimulatedAircraftView::fastPositionUpdates()
        {
            CSimulatedAircraft aircraft(selectedObject());
            if (aircraft.getCallsign().isEmpty()) { return; }
            aircraft.setFastPositionUpdates(!aircraft.fastPositionUpdates());
            emit this->requestFastPositionUpdates(aircraft);
        }

        void CSimulatedAircraftView::highlightInSimulator()
        {
            const CSimulatedAircraft aircraft(selectedObject());
            if (aircraft.getCallsign().isEmpty()) { return; }
            emit this->requestHighlightInSimulator(aircraft);
        }

        void CSimulatedAircraftView::showPositionLogInSimulator()
        {
            if (!sGui || sGui->isShuttingDown())  { return; }
            if (!sGui->getIContextSimulator())  { return; }
            const CSimulatedAircraft aircraft(selectedObject());
            if (aircraft.getCallsign().isEmpty()) { return; }

            const CIdentifier i(this->objectName());
            const QString dotCmd(".drv pos " + aircraft.getCallsignAsString());
            sGui->getIContextSimulator()->parseCommandLine(dotCmd, i);
        }
    } // ns
} // ns
