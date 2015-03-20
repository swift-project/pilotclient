/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "simulatedaircraftview.h"
#include "blackmisc/project.h"
#include <QHeaderView>

using namespace BlackMisc;
using namespace BlackMisc::Simulation;
using namespace BlackGui::Models;

namespace BlackGui
{
    namespace Views
    {
        CSimulatedAircraftView::CSimulatedAircraftView(QWidget *parent) : CViewBase(parent)
        {
            this->standardInit(new CSimulatedAircraftListModel(this));
            this->m_withMenuItemRefresh = true;
        }

        void CSimulatedAircraftView::setAircraftMode(CSimulatedAircraftListModel::AircraftMode mode)
        {
            Q_ASSERT(this->m_model);
            this->m_model->setAircraftMode(mode);
            this->setSortIndicator();
        }

        void CSimulatedAircraftView::customMenu(QMenu &menu) const
        {
            if (BlackMisc::CProject::isDebugBuild())
            {
                // tbd
            }

            if (this->hasSelection())
            {
                CSimulatedAircraft aircraft(selectedObject());
                Q_ASSERT(!aircraft.getCallsign().isEmpty());
                menu.addAction(CIcons::appTextMessages16(), "Show text messages", this, SLOT(ps_requestTextMessage()));
                menu.addAction(CIcons::appAircrafts16(), aircraft.isEnabled() ? "Disable aircraft" : "Enabled aircraft", this, SLOT(ps_enableAircraft()));
                menu.addAction(CIcons::globe16(), aircraft.fastPositionUpdates() ? "Normal updates" : "Fast position updates", this, SLOT(ps_fastPositionUpdates()));
                menu.addSeparator();
            }
            CViewBase::customMenu(menu);
        }

        void CSimulatedAircraftView::ps_requestTextMessage()
        {
            CSimulatedAircraft aircraft(selectedObject());
            if (aircraft.getCallsign().isEmpty()) { return; }
            emit requestTextMessageWidget(aircraft.getCallsign());
        }

        void CSimulatedAircraftView::ps_enableAircraft()
        {
            CSimulatedAircraft aircraft(selectedObject());
            if (aircraft.getCallsign().isEmpty()) { return; }
            aircraft.setEnabled(!aircraft.isEnabled());
            emit requestEnableAircraft(aircraft);
        }

        void CSimulatedAircraftView::ps_fastPositionUpdates()
        {
            CSimulatedAircraft aircraft(selectedObject());
            if (aircraft.getCallsign().isEmpty()) { return; }
            aircraft.setFastPositionUpdates(!aircraft.fastPositionUpdates());
            emit requestFastPositionUpdates(aircraft);
        }

    } // ns
} // ns
