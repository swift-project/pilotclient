/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "remoteaircraftselector.h"
#include "ui_remoteaircraftselector.h"
#include "blackgui/guiapplication.h"
#include "blackcore/contextnetwork.h"

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;
using namespace BlackCore;

namespace BlackGui
{
    namespace Components
    {
        CRemoteAircraftSelector::CRemoteAircraftSelector(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CRemoteAircraftSelector)
        {
            ui->setupUi(this);
            bool s = connect(sGui->getIContextNetwork(), &IContextNetwork::removedAircraft, this, &CRemoteAircraftSelector::ps_onRemovedAircraft);
            Q_ASSERT(s);
            s = connect(sGui->getIContextNetwork(), &IContextNetwork::addedAircraft, this, &CRemoteAircraftSelector::ps_onAddedAircraft);
            Q_ASSERT(s);
            Q_UNUSED(s);
        }

        CRemoteAircraftSelector::~CRemoteAircraftSelector() { }

        BlackMisc::Aviation::CCallsign CRemoteAircraftSelector::getSelectedCallsign() const
        {
            const CCallsign empty {};
            int index = ui->cb_RemoteAircraftSelector->currentIndex();
            if (index < 0 || index > this->m_aircraft.size()) { return empty; }
            return m_aircraft[index].getCallsign();
        }

        void CRemoteAircraftSelector::showEvent(QShowEvent *event)
        {
            // force new combobox when visible
            this->fillComboBox();
            QWidget::showEvent(event);
        }

        void CRemoteAircraftSelector::ps_onAddedAircraft(const CSimulatedAircraft &aircraft)
        {
            CCallsign cs(aircraft.getCallsign());
            if (cs.isEmpty()) { return; }
            if (this->m_aircraft.containsCallsign(cs)) { return; }
            this->fillComboBox();
        }

        void CRemoteAircraftSelector::ps_onRemovedAircraft(const CCallsign &callsign)
        {
            if (callsign.isEmpty()) { return; }
            if (this->m_aircraft.containsCallsign(callsign))
            {
                this->fillComboBox();
            }
        }

        void CRemoteAircraftSelector::fillComboBox()
        {
            if (!this->isVisible()) { return; } // for performance reasons
            m_aircraft = sGui->getIContextNetwork()->getAircraftInRange();
            this->ui->cb_RemoteAircraftSelector->clear();
            if (m_aircraft.isEmpty()) { return; }

            CCallsign currentSelection(this->getSelectedCallsign());
            QStringList items;
            for (const CSimulatedAircraft &aircraft : m_aircraft)
            {
                if (aircraft.getCallsign().isEmpty()) { continue; }
                QString i(aircraft.getCallsign().toQString());
                if (aircraft.hasAircraftDesignator())
                {
                    i += " (";
                    i += aircraft.getAircraftIcaoCode().toQString(false);
                    i += ")";
                }
                if (aircraft.hasValidRealName())
                {
                    i += " - ";
                    i += aircraft.getPilotRealname();
                }
                items.append(i);
            }

            // new combobox
            this->ui->cb_RemoteAircraftSelector->addItems(items);

            // set old selection if possible
            if (currentSelection.isEmpty()) { return; }
            int index = m_aircraft.firstIndexOfCallsign(currentSelection);
            if (index >= 0 && index < this->ui->cb_RemoteAircraftSelector->count())
            {
                this->ui->cb_RemoteAircraftSelector->setCurrentIndex(index);
            }
        }

    } // namespace
} // namespace
