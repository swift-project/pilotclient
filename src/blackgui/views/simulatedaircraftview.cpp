/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackconfig/buildconfig.h"
#include "blackgui/views/simulatedaircraftview.h"
#include "blackgui/models/simulatedaircraftlistmodel.h"
#include "blackgui/menus/menuaction.h"
#include "blackgui/guiapplication.h"
#include "blackcore/context/contextsimulator.h"
#include "blackmisc/simulation/simulatedaircraftlist.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/icons.h"

#include <QPointer>
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
            m_menus |= MenuRefresh;
        }

        void CSimulatedAircraftView::setAircraftMode(CSimulatedAircraftListModel::AircraftMode mode)
        {
            Q_ASSERT(m_model);
            m_model->setAircraftMode(mode);
            this->setSortIndicator();
        }

        void CSimulatedAircraftView::configureMenu(bool menuHighlightAndFollow, bool menuEnableAircraft, bool menuFastPositionUpdates, bool menuGndFlag)
        {
            m_withMenuEnableAircraft = menuEnableAircraft;
            m_withMenuFastPosition = menuFastPositionUpdates;
            m_withMenuHighlightAndFollow = menuHighlightAndFollow;
            m_withMenuEnableGndFlag = menuGndFlag;
        }

        void CSimulatedAircraftView::customMenu(CMenuActions &menuActions)
        {
            if (m_withMenuEnableAircraft)
            {
                menuActions.addAction(CIcons::appAircraft16(), "Enable all aircraft", CMenuAction::pathClientSimulation(), { this, &CSimulatedAircraftView::enableAllDisabledAircraft });
                menuActions.addAction(CIcons::appAircraft16(), "Disable all aircraft", CMenuAction::pathClientSimulation(), { this, &CSimulatedAircraftView::disableAllEnabledAircraft });
                menuActions.addAction(CIcons::appAircraft16(), "Re-enable unrendered aircraft", CMenuAction::pathClientSimulation(), { this, &CSimulatedAircraftView::reEnableAllUnrenderedAircraft });
            }

            if (this->hasSelection())
            {
                CSimulatedAircraft aircraft(selectedObject());
                Q_ASSERT(!aircraft.getCallsign().isEmpty());
                menuActions.addAction(CIcons::appTextMessages16(), "Show text messages", CMenuAction::pathClientCom(), { this, &CSimulatedAircraftView::requestTextMessage });
                if (m_withMenuEnableAircraft)
                {
                    menuActions.addAction(CIcons::appAircraft16(), aircraft.isEnabled() ? "Disable aircraft" : "Enabled aircraft", CMenuAction::pathClientSimulation(), { this, &CSimulatedAircraftView::toggleEnabledAircraft });
                }
                if (m_withMenuHighlightAndFollow)
                {
                    menuActions.addAction(CIcons::appAircraft16(), "Follow in simulator", CMenuAction::pathClientSimulation(), { this, &CSimulatedAircraftView::reqFollowInSimulator });
                    menuActions.addAction(CIcons::appSimulator16(), "Highlight in simulator", CMenuAction::pathClientSimulation(), { this, &CSimulatedAircraftView::reqHighlightInSimulator });
                }
                if (m_withMenuEnableGndFlag)
                {
                    menuActions.addAction(CIcons::geoPosition16(), aircraft.isSupportingGndFlag() ? "Disable gnd.flag" : "Enabled gnd.flag", CMenuAction::pathClientSimulation(), { this, &CSimulatedAircraftView::toggleSupportingGndFlag });
                }
                if (m_withMenuFastPosition)
                {
                    menuActions.addAction(CIcons::globe16(), aircraft.fastPositionUpdates() ? "Normal updates" : "Fast position updates",  CMenuAction::pathClientSimulation(), { this, &CSimulatedAircraftView::toggleFastPositionUpdates });
                }
                const bool any = m_withMenuEnableAircraft || m_withMenuFastPosition || m_withMenuHighlightAndFollow || m_withMenuEnableGndFlag;
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

        void CSimulatedAircraftView::toggleEnabledAircraft()
        {
            CSimulatedAircraft aircraft(selectedObject());
            if (aircraft.getCallsign().isEmpty()) { return; }
            aircraft.setEnabled(!aircraft.isEnabled());
            emit this->requestEnableAircraft(aircraft);
        }

        void CSimulatedAircraftView::toggleFastPositionUpdates()
        {
            CSimulatedAircraft aircraft(selectedObject());
            if (aircraft.getCallsign().isEmpty()) { return; }
            aircraft.setFastPositionUpdates(!aircraft.fastPositionUpdates());
            emit this->requestFastPositionUpdates(aircraft);
        }

        void CSimulatedAircraftView::toggleSupportingGndFlag()
        {
            CSimulatedAircraft aircraft(selectedObject());
            if (aircraft.getCallsign().isEmpty()) { return; }
            aircraft.setSupportingGndFlag(!aircraft.isSupportingGndFlag());
            emit this->requestSupportingGndFlag(aircraft);
        }

        void CSimulatedAircraftView::reqHighlightInSimulator()
        {
            const CSimulatedAircraft aircraft(selectedObject());
            if (aircraft.getCallsign().isEmpty()) { return; }
            emit this->requestHighlightInSimulator(aircraft);
        }

        void CSimulatedAircraftView::reqFollowInSimulator()
        {
            const CSimulatedAircraft aircraft(selectedObject());
            if (aircraft.getCallsign().isEmpty()) { return; }
            emit this->requestFollowInSimulator(aircraft);
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

        void CSimulatedAircraftView::enableAllDisabledAircraft()
        {
            if (!sGui || sGui->isShuttingDown())  { return; }
            const CSimulatedAircraftList aircraft = this->container().findByEnabled(false);
            this->enableOrDisableAircraft(aircraft, true);
        }

        void CSimulatedAircraftView::disableAllEnabledAircraft()
        {
            if (!sGui || sGui->isShuttingDown())  { return; }
            const CSimulatedAircraftList aircraft = this->container().findByEnabled(true);
            this->enableOrDisableAircraft(aircraft, false);
        }

        void CSimulatedAircraftView::reEnableAllUnrenderedAircraft()
        {
            if (!sGui || sGui->isShuttingDown())  { return; }
            const CSimulatedAircraftList aircraft = this->container().findByRendered(false);
            this->enableOrDisableAircraft(aircraft, true);
        }

        void CSimulatedAircraftView::enableOrDisableAircraft(const CSimulatedAircraftList &aircraft, bool newEnabled)
        {
            if (aircraft.isEmpty())  { return; }

            const QPointer<CSimulatedAircraftView> myself(this);
            for (const CSimulatedAircraft &sa : aircraft)
            {
                QTimer::singleShot(10, this, [ = ]
                {
                    if (!myself) { return; }
                    if (!sGui || sGui->isShuttingDown()) { return; }
                    CSimulatedAircraft enabledAircraft(sa);
                    enabledAircraft.setEnabled(newEnabled);
                    emit this->requestEnableAircraft(enabledAircraft);
                });
            }
        }
    } // ns
} // ns
