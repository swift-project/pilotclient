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
#include "blackcore/context_network.h"

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
            CEnableForRuntime(nullptr, false),
            ui(new Ui::CRemoteAircraftSelector)
        {
            ui->setupUi(this);
        }

        CRemoteAircraftSelector::~CRemoteAircraftSelector() { }

        BlackMisc::Aviation::CCallsign CRemoteAircraftSelector::getSelectedCallsign() const
        {
            const CCallsign empty {};
            int index = ui->cb_RemoteAircraftSelector->currentIndex();
            if (index < 0 || index > this->m_aircraft.size()) { return empty; }
            return m_aircraft[index].getCallsign();
        }

        void CRemoteAircraftSelector::runtimeHasBeenSet()
        {
            Q_ASSERT(getIContextNetwork());
            bool s = connect(getIContextNetwork(), &IContextNetwork::removedAircraft, this, &CRemoteAircraftSelector::ps_onRemovedAircraft);
            Q_ASSERT(s);
            s = connect(getIContextNetwork(), &IContextNetwork::addedAircraft, this, &CRemoteAircraftSelector::ps_onAddedAircraft);
            Q_ASSERT(s);
            Q_UNUSED(s);
        }

        void CRemoteAircraftSelector::showEvent(QShowEvent *event)
        {
            // force new combobox when visible
            this->fillComboBox();
            QWidget::showEvent(event);
        }

        void CRemoteAircraftSelector::ps_onAddedAircraft(const CSimulatedAircraft &aircraft)
        {
            Q_UNUSED(aircraft);
            this->fillComboBox();
        }

        void CRemoteAircraftSelector::ps_onRemovedAircraft(const CCallsign &callsign)
        {
            Q_UNUSED(callsign);
            this->fillComboBox();
        }

        void CRemoteAircraftSelector::fillComboBox()
        {
            if (!this->isVisible()) { return; } // for performance reasons
            Q_ASSERT(getIContextNetwork());
            m_aircraft = getIContextNetwork()->getAircraftInRange();
            this->ui->cb_RemoteAircraftSelector->clear();
            if (m_aircraft.isEmpty()) { return; }

            CCallsign currentSelection(this->getSelectedCallsign());
            QStringList items;
            for (const CSimulatedAircraft &aircraft : m_aircraft)
            {
                if (aircraft.getCallsign().isEmpty()) { continue; }
                QString i(aircraft.getCallsign().toQString());
                if (aircraft.hasValidAircraftDesignator())
                {
                    i += " (";
                    i += aircraft.getIcaoInfo().toQString(false);
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
