// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/components/remoteaircraftselector.h"

#include <QComboBox>
#include <QString>
#include <QStringBuilder>
#include <QStringList>
#include <QWidget>
#include <QtGlobal>

#include "ui_remoteaircraftselector.h"

#include "core/context/contextnetwork.h"
#include "gui/guiapplication.h"
#include "misc/aviation/aircrafticaocode.h"
#include "misc/sequence.h"
#include "misc/simulation/simulatedaircraft.h"

using namespace swift::misc;
using namespace swift::misc::aviation;
using namespace swift::misc::simulation;
using namespace swift::core;
using namespace swift::core::context;

namespace swift::gui::components
{
    CRemoteAircraftSelector::CRemoteAircraftSelector(QWidget *parent) : QFrame(parent),
                                                                        ui(new Ui::CRemoteAircraftSelector)
    {
        ui->setupUi(this);
        bool s = connect(sGui->getIContextNetwork(), &IContextNetwork::removedAircraft, this, &CRemoteAircraftSelector::onRemovedAircraft, Qt::QueuedConnection);
        Q_ASSERT(s);
        s = connect(sGui->getIContextNetwork(), &IContextNetwork::addedAircraft, this, &CRemoteAircraftSelector::onAddedAircraft, Qt::QueuedConnection);
        Q_ASSERT(s);
        s = connect(ui->cb_RemoteAircraftSelector, &QComboBox::currentTextChanged, this, &CRemoteAircraftSelector::comboBoxChanged);
        Q_UNUSED(s);
    }

    CRemoteAircraftSelector::~CRemoteAircraftSelector() {}

    swift::misc::aviation::CCallsign CRemoteAircraftSelector::getSelectedCallsign() const
    {
        static const CCallsign empty {};
        const int index = ui->cb_RemoteAircraftSelector->currentIndex();
        if (index < 0 || index > m_aircraft.size()) { return empty; }
        return m_aircraft[index].getCallsign();
    }

    void CRemoteAircraftSelector::indicatePartsEnabled(bool indicate)
    {
        m_showPartsEnabled = indicate;
    }

    void CRemoteAircraftSelector::showEvent(QShowEvent *event)
    {
        // force new combobox when visible
        m_dsFillComboBox.inputSignal(); // fill combo box
        QWidget::showEvent(event);
    }

    void CRemoteAircraftSelector::onAddedAircraft(const CSimulatedAircraft &aircraft)
    {
        CCallsign cs(aircraft.getCallsign());
        if (cs.isEmpty()) { return; }
        if (m_aircraft.containsCallsign(cs)) { return; }
        m_dsFillComboBox.inputSignal(); // fill combo box
    }

    void CRemoteAircraftSelector::onRemovedAircraft(const CCallsign &callsign)
    {
        if (callsign.isEmpty()) { return; }
        if (m_aircraft.containsCallsign(callsign))
        {
            m_dsFillComboBox.inputSignal(); // fill combo box
        }
    }

    void CRemoteAircraftSelector::comboBoxChanged(const QString &text)
    {
        if (m_currentText == text) { return; }
        m_currentText = text;
        emit this->changedCallsign();
    }

    void CRemoteAircraftSelector::fillComboBox()
    {
        if (!this->isVisible()) { return; } // for performance reasons
        if (!sGui || sGui->isShuttingDown()) { return; }

        const CCallsign currentSelection(this->getSelectedCallsign());
        m_aircraft = sGui->getIContextNetwork()->getAircraftInRange().sortedByCallsign();
        ui->cb_RemoteAircraftSelector->clear();
        if (m_aircraft.isEmpty()) { return; }

        QStringList items;
        for (const CSimulatedAircraft &aircraft : m_aircraft)
        {
            if (aircraft.getCallsign().isEmpty()) { continue; }
            QString i(aircraft.getCallsign().toQString());
            if (aircraft.hasAircraftDesignator())
            {
                i += u" (" %
                     aircraft.getAircraftIcaoCode().toQString(false) %
                     u')';
            }
            if (aircraft.hasRealName())
            {
                i += u" - " % aircraft.getPilotRealName();
            }
            if (m_showPartsEnabled)
            {
                if (aircraft.isPartsSynchronized())
                {
                    i += QStringLiteral(" [parts]");
                }
            }
            items.append(i);
        }

        // new combobox
        ui->cb_RemoteAircraftSelector->addItems(items);

        // set old selection if possible
        if (currentSelection.isEmpty()) { return; }
        int index = m_aircraft.firstIndexOfCallsign(currentSelection);
        if (index >= 0 && index < ui->cb_RemoteAircraftSelector->count())
        {
            ui->cb_RemoteAircraftSelector->setCurrentIndex(index);
        }
    }
} // namespace swift::gui::components
