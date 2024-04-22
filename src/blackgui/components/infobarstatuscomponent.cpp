// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackgui/components/infobarstatuscomponent.h"
#include "blackgui/components/transpondermodecomponent.h"
#include "blackgui/guiapplication.h"
#include "blackgui/led.h"
#include "blackgui/guiutility.h"
#include "blackcore/context/contextapplication.h"
#include "blackcore/context/contextaudio.h"
#include "blackcore/context/contextnetwork.h"
#include "blackcore/context/contextsimulator.h"
#include "blackcore/simulator.h"
#include "blackmisc/simulation/simulatorplugininfo.h"
#include "blackmisc/network/server.h"
#include "blackmisc/audio/audioutils.h"
#include "blackconfig/buildconfig.h"
#include "ui_infobarstatuscomponent.h"

#include <QLabel>
#include <QList>
#include <QMenu>
#include <QPoint>
#include <QWidget>
#include <Qt>
#include <QtGlobal>
#include <QPointer>
#include <QStringBuilder>
#include <QTimer>

using namespace BlackConfig;
using namespace BlackCore;
using namespace BlackCore::Context;
using namespace BlackGui;
using namespace BlackMisc;
using namespace BlackMisc::Audio;
using namespace BlackMisc::Network;

namespace BlackGui::Components
{
    CInfoBarStatusComponent::CInfoBarStatusComponent(QWidget *parent) : QFrame(parent), ui(new Ui::CInfoBarStatusComponent)
    {
        ui->setupUi(this);
        this->initLeds();
        this->adjustTextSize();

        ui->lbl_Audio->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(ui->lbl_Audio, &QLabel::customContextMenuRequested, this, &CInfoBarStatusComponent::onCustomAudioContextMenuRequested);
        connect(ui->comp_XpdrMode, &CTransponderModeComponent::changed, this, &CInfoBarStatusComponent::transponderModeChanged);

        Q_ASSERT_X(sGui, Q_FUNC_INFO, "Need sGui");
        if (sGui->getIContextSimulator())
        {
            connect(sGui->getIContextSimulator(), &IContextSimulator::simulatorStatusChanged, this, &CInfoBarStatusComponent::onSimulatorStatusChanged, Qt::QueuedConnection);
            connect(sGui->getIContextSimulator(), &IContextSimulator::modelSetChanged, this, &CInfoBarStatusComponent::onMapperReady);

            // initial values
            this->onMapperReady();
            this->onSimulatorStatusChanged(sGui->getIContextSimulator()->getSimulatorStatus());
        }

        if (sGui->getIContextNetwork())
        {
            connect(sGui->getIContextNetwork(), &IContextNetwork::connectionStatusChanged, this, &CInfoBarStatusComponent::onNetworkConnectionChanged, Qt::QueuedConnection);
        }

        if (sGui->getIContextApplication())
        {
            ui->led_DBus->setOn(sGui->getIContextApplication()->isUsingImplementingObject());
        }

        ui->led_Audio->setOn(CInfoBarStatusComponent::isAudioAvailableAndNotMuted());
        if (sGui->getCContextAudioBase())
        {
            connect(sGui->getCContextAudioBase(), &CContextAudioBase::changedMute, this, &CInfoBarStatusComponent::onMuteChanged, Qt::QueuedConnection);
            connect(sGui->getCContextAudioBase(), &CContextAudioBase::startedAudio, this, &CInfoBarStatusComponent::onAudioStarted, Qt::QueuedConnection);
            connect(sGui->getCContextAudioBase(), &CContextAudioBase::stoppedAudio, this, &CInfoBarStatusComponent::onAudioStopped, Qt::QueuedConnection);

            // PTT as received on audio
            // that also would need to be reconnected if audio is disabled/enabled
            // connect(sGui->getCContextAudioBase(), &CContextAudioBase::ptt, this, &CInfoBarStatusComponent::onPttChanged, Qt::QueuedConnection);
        }

        QPointer<CInfoBarStatusComponent> myself(this);
        QTimer::singleShot(5000, this, [=] {
            if (!sGui || sGui->isShuttingDown() || !myself) { return; }
            this->updateValues();
        });
    }

    CInfoBarStatusComponent::~CInfoBarStatusComponent()
    {}

    void CInfoBarStatusComponent::initLeds()
    {
        this->updateSpacing();
        CLedWidget::LedShape shape = CLedWidget::Circle;
        ui->led_DBus->setValues(CLedWidget::Yellow, CLedWidget::Black, shape, "DBus connected", "DBus disconnected", 14);
        ui->led_Network->setValues(CLedWidget::Yellow, CLedWidget::Black, shape, "Network connected", "Network disconnected", 14);
        ui->led_Simulator->setValues(CLedWidget::Yellow, CLedWidget::Black, CLedWidget::Blue, shape, "Simulator running", "Simulator disconnected", "Simulator connected", 14);
        ui->led_MapperReady->setValues(CLedWidget::Yellow, CLedWidget::Black, CLedWidget::Blue, shape, "Mapper ready", "Mappings not yet loaded", "Mappings not yet loaded", 14);

        shape = CLedWidget::Rounded;
        ui->led_Ptt->setValues(CLedWidget::Yellow, CLedWidget::Black, shape, "Ptt", "Silence", 18);
        ui->led_Audio->setValues(CLedWidget::Yellow, CLedWidget::Black, shape, "On", "Muted", 18);
    }

    void CInfoBarStatusComponent::adjustTextSize()
    {
        CGuiUtility::setElidedText(ui->lbl_Audio, QStringLiteral("audio"), Qt::ElideRight);
        CGuiUtility::setElidedText(ui->lbl_Network, QStringLiteral("network"), Qt::ElideRight);
        CGuiUtility::setElidedText(ui->lbl_DBus, QStringLiteral("DBus"), {}, Qt::ElideRight);
        CGuiUtility::setElidedText(ui->lbl_MapperReady, QStringLiteral("mapper ready"), Qt::ElideRight);
        CGuiUtility::setElidedText(ui->lbl_Ptt, QStringLiteral("PTT"), Qt::ElideRight);
        CGuiUtility::setElidedText(ui->lbl_Simulator, QStringLiteral("simulator"), Qt::ElideRight);
    }

    void CInfoBarStatusComponent::setDBusStatus(bool dbus)
    {
        ui->led_DBus->setOn(dbus);
    }

    void CInfoBarStatusComponent::setDBusTooltip(const QString &tooltip)
    {
        ui->led_DBus->setOnToolTip(tooltip);
    }

    void CInfoBarStatusComponent::setSpacing(int spacing)
    {
        if (this->layout())
        {
            this->layout()->setSpacing(spacing);
        }
    }

    void CInfoBarStatusComponent::resizeEvent(QResizeEvent *event)
    {
        QFrame::resizeEvent(event);
        m_dsResize.inputSignal();
    }

    void CInfoBarStatusComponent::onSimulatorStatusChanged(int status)
    {
        if (!sGui || sGui->isShuttingDown()) { return; }
        const ISimulator::SimulatorStatus simStatus = static_cast<ISimulator::SimulatorStatus>(status);
        if (simStatus.testFlag(ISimulator::Connected))
        {
            // at least connected
            const QString s(
                sGui->getIContextSimulator()->getSimulatorPluginInfo().getDescription() % u": " %
                ISimulator::statusToString(simStatus));

            if (simStatus.testFlag(ISimulator::Paused))
            {
                ui->led_Simulator->setTriState();
                ui->led_Simulator->setTriStateToolTip(s);
            }
            else if (simStatus.testFlag(ISimulator::Simulating))
            {
                ui->led_Simulator->setOn(true);
                ui->led_Simulator->setOnToolTip(s);
            }
            else
            {
                // connected only
                ui->led_Simulator->setTriState();
                ui->led_Simulator->setTriStateToolTip(s);
            }
        }
        else
        {
            ui->led_Simulator->setOn(false);
        }

        // simulator status has impact on model set available
        this->onMapperReady();
    }

    void CInfoBarStatusComponent::onNetworkConnectionChanged(const CConnectionStatus &from, const CConnectionStatus &to)
    {
        Q_UNUSED(from)
        switch (to.getConnectionStatus())
        {
        case CConnectionStatus::Disconnected:
            ui->led_Network->setOn(false);
            break;
        case CConnectionStatus::Connected:
            ui->led_Network->setOn(true);
            ui->led_Network->setOnToolTip(u"Connected: " % sGui->getIContextNetwork()->getConnectedServer().getName());
            break;
        case CConnectionStatus::Connecting:
            ui->led_Network->setTriStateColor(CLedWidget::Yellow);
            break;
        default:
            ui->led_Network->setOn(false);
            break;
        }
    }

    void CInfoBarStatusComponent::onCustomAudioContextMenuRequested(const QPoint &position)
    {
        const QWidget *sender = qobject_cast<QWidget *>(QWidget::sender());
        Q_ASSERT_X(sender, Q_FUNC_INFO, "Missing sender");
        const QPoint globalPosition = sender->mapToGlobal(position);

        QMenu menuAudio(this);
        menuAudio.addAction("Toogle mute");

        if (CBuildConfig::isRunningOnWindowsNtPlatform())
        {
            menuAudio.addAction("Mixer");
        }

        const QAction *selectedItem = menuAudio.exec(globalPosition);
        if (selectedItem)
        {
            // http://forum.technical-assistance.co.uk/sndvol32exe-command-line-parameters-vt1348.html
            const QList<QAction *> actions = menuAudio.actions();
            if (selectedItem == actions.at(0))
            {
                // toggle MUTED
                sGui->getCContextAudioBase()->setMute(!sGui->getCContextAudioBase()->isMuted());
            }
            else if (actions.size() > 1 && selectedItem == actions.at(1))
            {
                startWindowsMixer();
            }
        }
    }

    void CInfoBarStatusComponent::onMuteChanged(bool muted)
    {
        const bool on = !muted && isAudioAvailableAndNotMuted(); // make sure audio is started
        ui->led_Audio->setOn(on);
    }

    void CInfoBarStatusComponent::onAudioStarted(const CAudioDeviceInfo &input, const CAudioDeviceInfo &output)
    {
        Q_UNUSED(input)
        Q_UNUSED(output)
        this->updateValues();
    }

    void CInfoBarStatusComponent::onAudioStopped()
    {
        this->updateValues();
    }

    void CInfoBarStatusComponent::onMapperReady()
    {
        if (sGui && sGui->isShuttingDown()) { return; }
        if (!sGui || !sGui->getIContextSimulator())
        {
            ui->led_MapperReady->setOn(false);
            return;
        }

        const int models = sGui->getIContextSimulator()->getModelSetCount();
        const bool on = (models > 0);
        ui->led_MapperReady->setOn(on);
        if (on)
        {
            const QString m = QStringLiteral("Mapper with %1 models").arg(models);
            ui->led_MapperReady->setToolTip(m);
        }
    }

    void CInfoBarStatusComponent::onPttChanged(bool enabled)
    {
        ui->led_Ptt->setOn(enabled);
    }

    void CInfoBarStatusComponent::updateValues()
    {
        if (!sGui || sGui->isShuttingDown()) { return; }

        const IContextSimulator *simCon = sGui->getIContextSimulator();
        if (simCon)
        {

            this->onSimulatorStatusChanged(simCon->getSimulatorStatus());
            if (simCon->getModelSetCount() > 0)
            {
                this->onMapperReady();
            }
        }

        if (sGui->getIContextNetwork())
        {
            ui->led_Network->setOn(sGui->getIContextNetwork()->isConnected());
        }

        if (sGui->getIContextApplication())
        {
            ui->led_DBus->setOn(sGui->getIContextApplication()->isUsingImplementingObject());
        }

        // audio context can be empty depending on which side it is called
        ui->led_Audio->setOn(CInfoBarStatusComponent::isAudioAvailableAndNotMuted());
    }

    void CInfoBarStatusComponent::updateSpacing()
    {
        if (!sGui || sGui->isShuttingDown() || !sGui->mainApplicationWidget()) { return; }
        const int w = sGui->mainApplicationWidget()->width();
        const int s = (w >= 400) ? 6 : 2;
        this->setSpacing(s);
    }

    bool CInfoBarStatusComponent::isAudioAvailableAndNotMuted()
    {
        if (!sGui || !sGui->getCContextAudioBase() || sGui->isShuttingDown()) { return false; }
        if (!sGui->getCContextAudioBase()->isAudioStarted()) { return false; }
        return !sGui->getCContextAudioBase()->isMuted();
    }
} // namespace
