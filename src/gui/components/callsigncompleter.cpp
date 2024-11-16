// SPDX-FileCopyrightText: Copyright (C) 2017 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "callsigncompleter.h"

#include <QStringListModel>

#include "ui_callsigncompleter.h"

#include "core/context/contextnetwork.h"
#include "core/context/contextownaircraft.h"
#include "gui/guiapplication.h"
#include "gui/led.h"
#include "gui/uppercasevalidator.h"
#include "misc/simulation/simulatedaircraftlist.h"

using namespace swift::misc;
using namespace swift::misc::aviation;
using namespace swift::misc::network;
using namespace swift::misc::simulation;
using namespace swift::core;
using namespace swift::core::context;

namespace swift::gui::components
{
    CCallsignCompleter::CCallsignCompleter(QWidget *parent) : QFrame(parent), ui(new Ui::CCallsignCompleter)
    {
        Q_ASSERT_X(sGui, Q_FUNC_INFO, "Need sGui");
        Q_ASSERT_X(sGui->getIContextNetwork(), Q_FUNC_INFO, "Need network context");
        ui->setupUi(this);
        CUpperCaseValidator *ucv = new CUpperCaseValidator(ui->le_Callsign);
        ui->le_Callsign->setValidator(ucv);
        ui->le_Callsign->setCompleter(*completer());
        ui->led_Status->setToolTips("connected", "disconnected");
        ui->led_Status->setShape(CLedWidget::Rounded);
        ui->led_Status->setToolTips("network connected", "network disconnected", "data");
        ui->led_Status->setShape(CLedWidget::Rounded);
        connect(ui->le_Callsign, &QLineEdit::editingFinished, this, &CCallsignCompleter::onEditingFinished);
        connect(sGui->getIContextNetwork(), &IContextNetwork::changedAircraftInRange, &m_dsAircraftsInRangeChanged,
                &CDigestSignal::inputSignal);
        connect(sGui->getIContextNetwork(), &IContextNetwork::connectionStatusChanged, this,
                &CCallsignCompleter::onChangedConnectionStatus, Qt::QueuedConnection);
        m_dsAircraftsInRangeChanged.inputSignal(); // trigger
    }

    CCallsignCompleter::~CCallsignCompleter() {}

    CCallsign CCallsignCompleter::getCallsign(bool onlyKnownCallsign) const
    {
        const QString csString = ui->le_Callsign->text().trimmed().toUpper();
        const bool valid =
            onlyKnownCallsign ? this->isValidKnownCallsign(csString) : CCallsign::isValidAircraftCallsign(csString);
        if (!valid) { return CCallsign(); }
        return CCallsign(csString, CCallsign::Aircraft);
    }

    void CCallsignCompleter::setCallsign(const CCallsign &cs) { ui->le_Callsign->setText(cs.asString()); }

    QString CCallsignCompleter::getRawCallsignString() const { return ui->le_Callsign->text(); }

    bool CCallsignCompleter::hasValidCallsign() const
    {
        const QString csString = ui->le_Callsign->text().trimmed().toUpper();
        return this->isValidKnownCallsign(csString);
    }

    void CCallsignCompleter::setReadOnly(bool readOnly)
    {
        if (ui->le_Callsign->isReadOnly() == readOnly) { return; }
        ui->le_Callsign->setReadOnly(readOnly);
        this->setStyleSheet(this->styleSheet());
    }

    void CCallsignCompleter::updateCallsignsFromContext()
    {
        if (!sGui || sGui->isShuttingDown()) { return; }
        if (!sGui->getIContextNetwork()) { return; }
        if (completer()->wasUpdatedWithinTime(1500)) { return; } // avoid context call via DBus
        CCallsignSet validCallsigns;
        if (m_onlyWithParts)
        {
            //! \todo KB 2020-04 transfers allaircraft via DBus
            validCallsigns = sGui->getIContextNetwork()->getAircraftInRange().getCallsignsWithSynchronizedParts();
        }
        else { validCallsigns = sGui->getIContextNetwork()->getAircraftInRangeCallsigns(); }

        if (m_addOwnCallsign && sGui->getIContextOwnAircraft())
        {
            const CCallsign ownCs = sGui->getIContextOwnAircraft()->getOwnAircraft().getCallsign();
            if (!ownCs.isEmpty()) { validCallsigns.insert(ownCs); }
        }

        const QStringList modelData = validCallsigns.getCallsignStrings(true);
        completer()->updateData(modelData, 2000);
        ui->led_Status->setTriState(500);
    }

    void CCallsignCompleter::onEditingFinished()
    {
        const CCallsign cs = this->getCallsign();
        const bool changed = (cs.asString() == m_lastValue);
        m_lastValue = cs.asString();
        m_dsEditingFinished.inputSignal();
        if (cs.isValid())
        {
            m_dsValidCallsignEntered.inputSignal();
            if (changed) { emit this->validChangedCallsignEntered(); }
        }
    }

    void CCallsignCompleter::onChangedAircraftInRange() { this->updateCallsignsFromContext(); }

    void CCallsignCompleter::onChangedConnectionStatus(const CConnectionStatus &from, const CConnectionStatus &to)
    {
        Q_UNUSED(from)
        const bool connected = to.isConnected();
        ui->led_Status->setOn(connected);
        ui->le_Callsign->clear();
        ui->le_Callsign->setEnabled(connected);
        if (!connected) { completer()->clearData(); }
    }

    bool CCallsignCompleter::isValidKnownCallsign(const QString &callsignString) const
    {
        if (!CCallsign::isValidAircraftCallsign(callsignString)) { return false; }
        if (!completer()) { return false; }
        return completer()->contains(callsignString);
    }

    CSharedStringListCompleter *CCallsignCompleter::completer()
    {
        static CSharedStringListCompleter *c = new CSharedStringListCompleter();
        return c;
    }
} // namespace swift::gui::components
