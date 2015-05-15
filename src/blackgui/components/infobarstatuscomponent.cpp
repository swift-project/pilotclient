/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "infobarstatuscomponent.h"
#include "ui_infobarstatuscomponent.h"
#include "blackcore/context_simulator.h"
#include "blackcore/context_network.h"
#include "blackcore/context_application.h"
#include "blackcore/context_audio.h"
#include "blackcore/simulator.h"
#include "blackmisc/project.h"
#include "blackmisc/icons.h"

#include <QPoint>
#include <QMenu>
#include <QProcess>

using namespace BlackCore;
using namespace BlackGui;
using namespace BlackMisc;

namespace BlackGui
{
    namespace Components
    {
        CInfoBarStatusComponent::CInfoBarStatusComponent(QWidget *parent) :
            QFrame(parent), ui(new Ui::CInfoBarStatusComponent)
        {
            ui->setupUi(this);
            this->initLeds();

            this->ui->lbl_Audio->setContextMenuPolicy(Qt::CustomContextMenu);
            connect(this->ui->lbl_Audio, &QLabel::customContextMenuRequested, this, &CInfoBarStatusComponent::ps_customAudioContextMenuRequested);
        }

        CInfoBarStatusComponent::~CInfoBarStatusComponent()
        { }

        void CInfoBarStatusComponent::initLeds()
        {
            CLedWidget::LedShape shape = CLedWidget::Circle;
            this->ui->led_DBus->setValues(CLedWidget::Yellow, CLedWidget::Black, shape, "DBus connected", "DBus disconnected", 14);
            this->ui->led_Network->setValues(CLedWidget::Yellow, CLedWidget::Black, shape, "Network connected", "Network disconnected", 14);
            this->ui->led_Simulator->setValues(CLedWidget::Yellow, CLedWidget::Black, CLedWidget::Blue, shape, "Simulator running", "Simulator disconnected", "Simulator connected", 14);
            this->ui->led_MapperReady->setValues(CLedWidget::Yellow, CLedWidget::Black, CLedWidget::Blue, shape, "Mapper ready", "Mappings loading", "Mappings loading", 14);

            shape = CLedWidget::Rounded;
            this->ui->led_Ptt->setValues(CLedWidget::Yellow, CLedWidget::Black, shape, "Ptt", "Silence", 18);
            this->ui->led_Audio->setValues(CLedWidget::Yellow, CLedWidget::Black, shape, "On", "Muted", 18);
        }

        void CInfoBarStatusComponent::setDBusStatus(bool dbus)
        {
            this->ui->led_DBus->setOn(dbus);
        }

        void CInfoBarStatusComponent::setDBusTooltip(const QString &tooltip)
        {
            this->ui->led_DBus->setOnToolTip(tooltip);
        }

        void CInfoBarStatusComponent::runtimeHasBeenSet()
        {
            if (getIContextApplication()->isEmptyObject()) return;

            // TODO: remove checks when empty contexts are fully introduced
            Q_ASSERT(getIContextSimulator());
            Q_ASSERT(getIContextAudio());
            Q_ASSERT(getIContextNetwork());

            if (this->getIContextSimulator())
            {
                connect(this->getIContextSimulator(), &IContextSimulator::simulatorStatusChanged, this, &CInfoBarStatusComponent::ps_onSimulatorStatusChanged);
                connect(this->getIContextSimulator(), &IContextSimulator::installedAircraftModelsChanged, this, &CInfoBarStatusComponent::ps_onMapperReady);

                // initial values
                this->ps_onMapperReady();
                bool connected = this->getIContextSimulator()->isConnected();
                this->ps_onSimulatorStatusChanged(connected ? ISimulator::Connected : ISimulator::Disconnected);
            }

            if (this->getIContextNetwork())
            {
                connect(this->getIContextNetwork(), &IContextNetwork::connectionStatusChanged, this, &CInfoBarStatusComponent::ps_onNetworkConnectionChanged);
            }

            if (this->getIContextApplication())
            {
                this->ui->led_DBus->setOn(this->getIContextApplication()->isUsingImplementingObject());
            }

            if (this->getIContextAudio())
            {
                this->ui->led_Audio->setOn(!this->getIContextAudio()->isMuted());
                connect(getIContextAudio(), &IContextAudio::changedMute, this, &CInfoBarStatusComponent::ps_onMuteChanged);
            }
        }

        void CInfoBarStatusComponent::ps_onSimulatorStatusChanged(int status)
        {
            if (status > 0 && (status & ISimulator::Connected))
            {
                QString s(
                    getIContextSimulator()->getSimulatorPluginInfo().getDescription() + ": " +
                    ISimulator::statusToString(status));
                // at least connected
                if (status & ISimulator::Paused)
                {
                    // in paused state
                    this->ui->led_Simulator->setTriState();
                    this->ui->led_Simulator->setTriStateToolTip(s);
                }
                else if (status & ISimulator::Running)
                {
                    this->ui->led_Simulator->setOn(true);
                    this->ui->led_Simulator->setOnToolTip(s);
                }
                else
                {
                    this->ui->led_Simulator->setTriState();
                    this->ui->led_Simulator->setTriStateToolTip(s);
                }
            }
            else
            {
                this->ui->led_Simulator->setOn(false);
            }
        }

        void CInfoBarStatusComponent::ps_onNetworkConnectionChanged(int from, int to)
        {
            INetwork::ConnectionStatus fromStatus = static_cast<INetwork::ConnectionStatus>(from);
            INetwork::ConnectionStatus toStatus = static_cast<INetwork::ConnectionStatus>(to);
            Q_UNUSED(fromStatus);

            switch (toStatus)
            {
            case INetwork::Disconnected:
            case INetwork::DisconnectedError:
            case INetwork::DisconnectedFailed:
            case INetwork::DisconnectedLost:
                this->ui->led_Network->setOn(false);
                break;
            case INetwork::Connected:
                this->ui->led_Network->setOn(true);
                this->ui->led_Network->setOnToolTip("Connected: " + getIContextNetwork()->getConnectedServer().getName());
                break;
            case INetwork::Connecting:
                this->ui->led_Network->setTriStateColor(CLedWidget::Yellow);
                break;
            default:
                this->ui->led_Network->setOn(false);
                break;
            }
        }

        void CInfoBarStatusComponent::ps_customAudioContextMenuRequested(const QPoint &position)
        {
            QWidget *sender = qobject_cast<QWidget *>(QWidget::sender());
            Q_ASSERT(sender);
            QPoint globalPosition = sender->mapToGlobal(position);

            QMenu menuAudio(this);
            menuAudio.addAction("Toogle mute");

            if (CProject::isRunningOnWindowsNtPlatform())
            {
                menuAudio.addAction("Mixer");
            }

            QAction *selectedItem = menuAudio.exec(globalPosition);
            if (selectedItem)
            {
                // http://forum.technical-assistance.co.uk/sndvol32exe-command-line-parameters-vt1348.html
                const QList<QAction *> actions = menuAudio.actions();
                if (selectedItem == actions.at(0))
                {
                    this->getIContextAudio()->setMute(!this->getIContextAudio()->isMuted());
                }
                else if (actions.size() > 1 && selectedItem == actions.at(1))
                {
                    BlackMisc::Audio::startWindowsMixer();
                }
            }
        }

        void CInfoBarStatusComponent::ps_onMuteChanged(bool muted)
        {
            this->ui->led_Audio->setOn(!muted);
        }

        void CInfoBarStatusComponent::ps_onMapperReady()
        {
            if (!getIContextSimulator())
            {
                this->ui->led_MapperReady->setOn(false);
                return;
            }

            int models = this->getIContextSimulator()->getInstalledModelsCount();
            bool on = (models > 0);
            this->ui->led_MapperReady->setOn(on);
            if (on)
            {
                QString m = QString("Mapper with %1 models").arg(models);
                this->ui->led_MapperReady->setToolTip(m);
            }
        }
    } // namespace
} // namespace
