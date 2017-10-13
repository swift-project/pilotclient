/* Copyright (C) 2017
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "callsigncompleter.h"
#include "ui_callsigncompleter.h"
#include "blackcore/context/contextnetwork.h"
#include "blackgui/guiapplication.h"
#include "blackgui/led.h"
#include "blackgui/uppercasevalidator.h"
#include <QStringListModel>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackCore;
using namespace BlackCore::Context;

namespace BlackGui
{
    namespace Components
    {
        CCallsignCompleter::CCallsignCompleter(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CCallsignCompleter)
        {
            Q_ASSERT_X(sGui, Q_FUNC_INFO, "Need sGui");
            Q_ASSERT_X(sGui->getIContextNetwork(), Q_FUNC_INFO, "Need network context");
            ui->setupUi(this);
            CUpperCaseValidator *ucv = new CUpperCaseValidator(ui->le_Callsign);
            ui->le_Callsign->setValidator(ucv);
            ui->led_Status->setToolTips("connected", "disconnected");
            ui->led_Status->setShape(CLedWidget::Rounded);
            ui->led_Status->setToolTips("network connected", "network disconnected", "data");
            ui->led_Status->setShape(CLedWidget::Rounded);
            connect(ui->le_Callsign, &QLineEdit::editingFinished, this, &CCallsignCompleter::onEditingFinished);
            connect(sGui->getIContextNetwork(), &IContextNetwork::changedAircraftInRange, &m_dsAircraftsInRangeChanged, &CDigestSignal::inputSignal);
            connect(sGui->getIContextNetwork(), &IContextNetwork::connectionStatusChanged, this, &CCallsignCompleter::onChangedConnectionStatus, Qt::QueuedConnection);
            m_dsAircraftsInRangeChanged.inputSignal(); // trigger
        }

        CCallsignCompleter::~CCallsignCompleter()
        { }

        BlackMisc::Aviation::CCallsign CCallsignCompleter::getCallsign() const
        {
            const QString csString = ui->le_Callsign->text().trimmed().toUpper();
            if (!this->isValidKnownCallsign(csString)) { return CCallsign(); }
            return CCallsign(csString);
        }

        QString CCallsignCompleter::getRawCallsignString() const
        {
            return ui->le_Callsign->text();
        }

        bool CCallsignCompleter::hasValidCallsign() const
        {
            const QString csString = ui->le_Callsign->text().trimmed().toUpper();
            return this->isValidKnownCallsign(csString);
        }

        void CCallsignCompleter::updateCallsignsFromContext()
        {
            if (!sGui || sGui->isShuttingDown()) { return; }
            if (!sGui->getIContextNetwork()) { return; }
            m_validCallsigns = sGui->getIContextNetwork()->getAircraftInRangeCallsigns();
            const QStringList modelData = m_validCallsigns.getCallsignStrings(true);
            if (!m_currentCompleter)
            {
                m_currentCompleter = new QCompleter(modelData, ui->le_Callsign);
                ui->le_Callsign->setCompleter(m_currentCompleter);
            }
            else
            {
                QStringListModel *model = qobject_cast<QStringListModel *>(m_currentCompleter->model());
                Q_ASSERT(model);
                model->setStringList(modelData);
            }
            ui->led_Status->setTriState(500);
        }

        void CCallsignCompleter::onEditingFinished()
        {
            const CCallsign cs = this->getCallsign();
            emit this->editingFinished();
            if (cs.isValid())
            {
                emit this->validCallsignEntered();
            }
        }

        void CCallsignCompleter::onChangedAircraftInRange()
        {
            this->updateCallsignsFromContext();
        }

        void CCallsignCompleter::onChangedConnectionStatus(INetwork::ConnectionStatus from, INetwork::ConnectionStatus to)
        {
            Q_UNUSED(from);
            const bool on = (INetwork::Connected == to);
            ui->led_Status->setOn(on);
            ui->le_Callsign->clear();
            ui->le_Callsign->setEnabled(on);
            if (!on)
            {
                QStringListModel *model = qobject_cast<QStringListModel *>(m_currentCompleter->model());
                Q_ASSERT(model);
                model->setStringList(QStringList());
            }
        }

        bool CCallsignCompleter::isValidKnownCallsign(const QString &callsignString) const
        {
            if (m_validCallsigns.isEmpty()) { return false; }
            if (!CCallsign::isValidAircraftCallsign(callsignString)) { return false; }
            return m_validCallsigns.contains(CCallsign(callsignString));
        }
    } // ns
} // ns
