// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackgui/components/internalscomponent.h"
#include "blackgui/components/remoteaircraftselector.h"
#include "blackgui/guiapplication.h"
#include "blackgui/uppercasevalidator.h"
#include "blackcore/context/contextapplication.h"
#include "blackcore/context/contextaudio.h"
#include "blackcore/context/contextnetwork.h"
#include "blackcore/context/contextownaircraft.h"
#include "blackcore/context/contextsimulator.h"
#include "blackmisc/simulation/interpolationlogger.h"
#include "blackmisc/simulation/interpolationrenderingsetup.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/network/client.h"
#include "blackmisc/network/textmessage.h"
#include "blackmisc/math/mathutils.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/crashhandler.h"
#include "blackconfig/buildconfig.h"

#include "ui_internalscomponent.h"

#include <QCheckBox>
#include <QPushButton>
#include <QSpinBox>
#include <QString>
#include <QTextEdit>
#include <Qt>
#include <QtGlobal>
#include <QDesktopServices>
#include <QDateTime>
#include <QMessageBox>

using namespace BlackConfig;
using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;
using namespace BlackMisc::Math;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Simulation;

namespace BlackGui::Components
{
    CInternalsComponent::CInternalsComponent(QWidget *parent) : QWidget(parent),
                                                                ui(new Ui::CInternalsComponent)
    {
        ui->setupUi(this);
        ui->tw_Internals->setCurrentIndex(0);

        ui->le_TxtMsgFrom->setValidator(new CUpperCaseValidator(ui->le_TxtMsgFrom));
        ui->le_TxtMsgTo->setValidator(new CUpperCaseValidator(ui->le_TxtMsgFrom));
        ui->le_AtisCallsign->setValidator(new CUpperCaseValidator(ui->le_AtisCallsign));

        connect(ui->cb_DebugContextAudio, &QCheckBox::stateChanged, this, &CInternalsComponent::enableDebug);
        connect(ui->cb_DebugContextApplication, &QCheckBox::stateChanged, this, &CInternalsComponent::enableDebug);
        connect(ui->cb_DebugContextNetwork, &QCheckBox::stateChanged, this, &CInternalsComponent::enableDebug);
        connect(ui->cb_DebugContextOwnAircraft, &QCheckBox::stateChanged, this, &CInternalsComponent::enableDebug);
        connect(ui->cb_DebugContextSimulator, &QCheckBox::stateChanged, this, &CInternalsComponent::enableDebug);

        connect(ui->pb_SendTextMessageDirectly, &QPushButton::released, this, &CInternalsComponent::sendTextMessage, Qt::QueuedConnection);
        connect(ui->pb_SendTextMessageDeferred, &QPushButton::released, this, &CInternalsComponent::sendTextMessage, Qt::QueuedConnection);

        connect(ui->tb_LogStatusMessage, &QPushButton::released, this, &CInternalsComponent::logStatusMessage);
        connect(ui->le_StatusMessage, &QLineEdit::returnPressed, this, &CInternalsComponent::logStatusMessage);

        connect(ui->pb_LatestInterpolationLog, &QPushButton::released, this, &CInternalsComponent::showLogFiles);
        connect(ui->pb_LatestPartsLog, &QPushButton::released, this, &CInternalsComponent::showLogFiles);
        connect(ui->pb_SendAtis, &QPushButton::released, this, &CInternalsComponent::sendAtis);

        connect(ui->pb_NetworkUpdateAndReset, &QPushButton::released, this, &CInternalsComponent::networkStatistics);
        connect(ui->pb_NetworkUpdate, &QPushButton::released, this, &CInternalsComponent::networkStatistics);
        connect(ui->cb_NetworkStatistics, &QCheckBox::stateChanged, this, &CInternalsComponent::onNetworkStatisticsToggled);

        if (sGui && sGui->isSupportingCrashpad())
        {
            ui->cb_CrashDumpUpload->setChecked(CCrashHandler::instance()->isCrashDumpUploadEnabled());
            connect(ui->pb_SimulateCrash, &QPushButton::released, this, &CInternalsComponent::simulateCrash, Qt::QueuedConnection);
            connect(ui->pb_SimulateAssert, &QPushButton::released, this, &CInternalsComponent::simulateAssert, Qt::QueuedConnection);
            connect(ui->cb_CrashDumpUpload, &QCheckBox::toggled, this, &CInternalsComponent::onCrashDumpUploadToggled);
        }
        else
        {
            ui->pb_SimulateCrash->setEnabled(false);
            ui->cb_CrashDumpUpload->setEnabled(false);
        }

        this->contextFlagsToGui();
    }

    CInternalsComponent::~CInternalsComponent() {}

    void CInternalsComponent::showEvent(QShowEvent *event)
    {
        // force new data when visible
        this->contextFlagsToGui();
        QWidget::showEvent(event);
    }

    void CInternalsComponent::enableDebug(int state)
    {
        Q_ASSERT(sGui->getIContextApplication());
        Q_ASSERT(sGui->getIContextAudio());
        Q_ASSERT(sGui->getIContextNetwork());
        Q_ASSERT(sGui->getIContextOwnAircraft());
        Q_ASSERT(sGui->getIContextSimulator());

        const Qt::CheckState checkState = static_cast<Qt::CheckState>(state);
        const bool debug = (checkState == Qt::Checked);
        const QObject *sender = QObject::sender();

        if (sender == ui->cb_DebugContextApplication) { sGui->getIContextApplication()->setDebugEnabled(debug); }
        else if (sender == ui->cb_DebugContextAudio) { sGui->getIContextAudio()->setDebugEnabled(debug); }
        else if (sender == ui->cb_DebugContextNetwork) { sGui->getIContextNetwork()->setDebugEnabled(debug); }
        else if (sender == ui->cb_DebugContextOwnAircraft) { sGui->getIContextOwnAircraft()->setDebugEnabled(debug); }
        else if (sender == ui->cb_DebugContextSimulator) { sGui->getIContextSimulator()->setDebugEnabled(debug); }
    }

    void CInternalsComponent::sendTextMessage()
    {
        if (!sGui || !sGui->getIContextNetwork()) { return; }
        if (ui->le_TxtMsgFrom->text().isEmpty()) { return; }
        if (ui->pte_TxtMsg->toPlainText().isEmpty()) { return; }
        if (ui->le_TxtMsgTo->text().isEmpty() && ui->dsb_TxtMsgFrequency->text().isEmpty()) { return; }

        // send in some time
        const QObject *senderObj = QObject::sender();
        if (senderObj == ui->pb_SendTextMessageDeferred)
        {
            QTimer::singleShot(5000, this, &CInternalsComponent::sendTextMessage);
            return;
        }

        const CCallsign sender(ui->le_TxtMsgFrom->text().trimmed());
        const CCallsign recipient(ui->le_TxtMsgTo->text().trimmed());
        const QString msgTxt(ui->pte_TxtMsg->toPlainText().trimmed());
        const double freqMHz = CMathUtils::round(ui->dsb_TxtMsgFrequency->value(), 3);
        CTextMessage tm;
        if (recipient.isEmpty() && freqMHz >= 117.995) // 118.000 actually plus epsilon
        {
            // remark: the internal double value can be still something like 127.999997
            CFrequency f = CFrequency(freqMHz, CFrequencyUnit::MHz());
            CComSystem::roundToChannelSpacing(f, CComSystem::ChannelSpacing8_33KHz);
            tm = CTextMessage(msgTxt, f, sender);
        }
        else
        {
            tm = CTextMessage(msgTxt, sender, recipient);
        }
        tm.setCurrentUtcTime();
        sGui->getIContextNetwork()->testReceivedTextMessages(CTextMessageList({ tm }));
    }

    void CInternalsComponent::sendAtis()
    {
        if (!sGui || !sGui->getIContextNetwork()) { return; }
        if (ui->le_AtisCallsign->text().isEmpty()) { return; }
        if (ui->pte_Atis->toPlainText().isEmpty()) { return; }
        const CCallsign cs(ui->le_AtisCallsign->text());
        const QString text(ui->pte_Atis->toPlainText());

        const CInformationMessage im(CInformationMessage::ATIS, text);
        sGui->getIContextNetwork()->testReceivedAtisMessage(cs, im);
    }

    void CInternalsComponent::logStatusMessage()
    {
        if (ui->le_StatusMessage->text().isEmpty()) { return; }
        CStatusMessage::StatusSeverity s = CStatusMessage::SeverityDebug;
        if (ui->rb_StatusMessageError->isChecked())
        {
            s = CStatusMessage::SeverityError;
        }
        else if (ui->rb_StatusMessageWarning->isChecked())
        {
            s = CStatusMessage::SeverityWarning;
        }
        else if (ui->rb_StatusMessageInfo->isChecked())
        {
            s = CStatusMessage::SeverityInfo;
        }
        const CStatusMessage sm = CStatusMessage(this, s, ui->le_StatusMessage->text().trimmed());
        CLogMessage::preformatted(sm);
    }

    void CInternalsComponent::showLogFiles()
    {
        QString file;
        const QObject *sender = QObject::sender();
        if (sender == ui->pb_LatestInterpolationLog)
        {
            file = CInterpolationLogger::getLatestLogFiles().first();
        }
        else if (sender == ui->pb_LatestPartsLog)
        {
            file = CInterpolationLogger::getLatestLogFiles().last();
        }

        if (file.isEmpty()) { return; }
        QDesktopServices::openUrl(QUrl::fromLocalFile(file));
    }

    void CInternalsComponent::contextFlagsToGui()
    {
        ui->cb_DebugContextApplication->setChecked(sGui->getIContextApplication()->isDebugEnabled());
        ui->cb_DebugContextNetwork->setChecked(sGui->getIContextNetwork()->isDebugEnabled());
        ui->cb_DebugContextOwnAircraft->setChecked(sGui->getIContextOwnAircraft()->isDebugEnabled());
        ui->cb_DebugContextSimulator->setChecked(sGui->getIContextSimulator()->isDebugEnabled());
    }

    void CInternalsComponent::simulateCrash()
    {
        if (CBuildConfig::isReleaseBuild())
        {
            QMessageBox::information(this, "crash simulation", "Not possible in release builds!");
            return;
        }

        const QMessageBox::StandardButton reply = QMessageBox::question(this, "crash simulation", "Really simulate crash?", QMessageBox::Yes | QMessageBox::No);
        if (!sGui || reply != QMessageBox::Yes) { return; }
        sGui->simulateCrash();
    }

    void CInternalsComponent::simulateAssert()
    {
        if (CBuildConfig::isReleaseBuild())
        {
            QMessageBox::information(this, "ASSERT simulation", "Not possible in release builds!");
            return;
        }

        const QMessageBox::StandardButton reply = QMessageBox::question(this, "ASSERT simulation", "Really create an ASSERT?", QMessageBox::Yes | QMessageBox::No);
        if (!sGui || reply != QMessageBox::Yes) { return; }
        sGui->simulateAssert();
    }

    void CInternalsComponent::onCrashDumpUploadToggled(bool checked)
    {
        if (sGui && sGui->isSupportingCrashpad())
        {
            const bool current = CCrashHandler::instance()->isCrashDumpUploadEnabled();
            if (current == checked) { return; }
            sGui->enableCrashDumpUpload(checked);
        }
    }

    void CInternalsComponent::networkStatistics()
    {
        if (!sGui || sGui->isShuttingDown() || !sGui->getIContextNetwork()) { return; }
        const bool reset = (QObject::sender() == ui->pb_NetworkUpdateAndReset);
        const QString statistics = sGui->getIContextNetwork()->getNetworkStatistics(reset, "\n");
        ui->pte_NetworkCalls->setPlainText(statistics);
    }

    void CInternalsComponent::onNetworkStatisticsToggled(bool checked)
    {
        if (!sGui || sGui->isShuttingDown() || !sGui->getIContextNetwork()) { return; }
        const bool on = sGui->getIContextNetwork()->setNetworkStatisticsEnable(checked);
        CLogMessage(this).info(u"Network statistics is %1") << boolToOnOff(on);
    }
} // namespace
