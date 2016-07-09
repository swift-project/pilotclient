/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackconfig/buildconfig.h"
#include "blackcore/context/contextapplication.h"
#include "blackcore/context/contextaudio.h"
#include "blackcore/context/contextnetwork.h"
#include "blackcore/context/contextsimulator.h"
#include "blackcore/simulator.h"
#include "blackgui/components/infobarstatuscomponent.h"
#include "blackgui/guiapplication.h"
#include "blackgui/led.h"
#include "blackmisc/audio/audioutils.h"
#include "blackmisc/network/server.h"
#include "blackmisc/simulation/simulatorplugininfo.h"
#include "ui_infobarstatuscomponent.h"

#include <QLabel>
#include <QList>
#include <QMenu>
#include <QPoint>
#include <QWidget>
#include <Qt>
#include <QtGlobal>

using namespace BlackConfig;
using namespace BlackCore;
using namespace BlackCore::Context;
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

            if (sGui->getIContextSimulator())
            {
                connect(sGui->getIContextSimulator(), &IContextSimulator::simulatorStatusChanged, this, &CInfoBarStatusComponent::ps_onSimulatorStatusChanged);
                connect(sGui->getIContextSimulator(), &IContextSimulator::installedAircraftModelsChanged, this, &CInfoBarStatusComponent::ps_onMapperReady);

                // initial values
                this->ps_onMapperReady();
                this->ps_onSimulatorStatusChanged(sGui->getIContextSimulator()->getSimulatorStatus());
            }

            if (sGui->getIContextNetwork())
            {
                connect(sGui->getIContextNetwork(), &IContextNetwork::connectionStatusChanged, this, &CInfoBarStatusComponent::ps_onNetworkConnectionChanged);
            }

            if (sGui->getIContextApplication())
            {
                this->ui->led_DBus->setOn(sGui->getIContextApplication()->isUsingImplementingObject());
            }

            if (sGui->getIContextAudio())
            {
                this->ui->led_Audio->setOn(!sGui->getIContextAudio()->isMuted());
                connect(sGui->getIContextAudio(), &IContextAudio::changedMute, this, &CInfoBarStatusComponent::ps_onMuteChanged);
            }
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

        void CInfoBarStatusComponent::ps_onSimulatorStatusChanged(int status)
        {
            if (status > 0 && (status & ISimulator::Connected))
            {
                QString s(
                    sGui->getIContextSimulator()->getSimulatorPluginInfo().getDescription() + ": " +
                    ISimulator::statusToString(status));

                // at least connected
                if (status & ISimulator::Paused)
                {
                    // in paused state
                    this->ui->led_Simulator->setTriState();
                    this->ui->led_Simulator->setTriStateToolTip(s);
                }
                else if (status & ISimulator::Simulating)
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

        void CInfoBarStatusComponent::ps_onNetworkConnectionChanged(BlackCore::INetwork::ConnectionStatus from, BlackCore::INetwork::ConnectionStatus to)
        {
            Q_UNUSED(from);

            switch (to)
            {
            case INetwork::Disconnected:
            case INetwork::DisconnectedError:
            case INetwork::DisconnectedFailed:
            case INetwork::DisconnectedLost:
                this->ui->led_Network->setOn(false);
                break;
            case INetwork::Connected:
                this->ui->led_Network->setOn(true);
                this->ui->led_Network->setOnToolTip("Connected: " + sGui->getIContextNetwork()->getConnectedServer().getName());
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

            if (CBuildConfig::isRunningOnWindowsNtPlatform())
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
                    sGui->getIContextAudio()->setMute(!sGui->getIContextAudio()->isMuted());
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
            if (!sGui->getIContextSimulator())
            {
                this->ui->led_MapperReady->setOn(false);
                return;
            }

            int models = sGui->getIContextSimulator()->getInstalledModelsCount();
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
