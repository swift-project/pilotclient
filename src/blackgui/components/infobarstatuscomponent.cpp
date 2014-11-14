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
#include "blackmisc/icons.h"
#include "blackcore/context_simulator.h"
#include "blackcore/context_network.h"
#include "blackcore/context_application.h"

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
        {
            delete ui;
        }

        void CInfoBarStatusComponent::initLeds()
        {
            CLedWidget::LedShape shape = CLedWidget::Circle;
            this->ui->led_DBus->setValues(CLedWidget::Yellow, CLedWidget::Black, shape, "DBus connected", "DBus disconnected", 14);
            this->ui->led_Network->setValues(CLedWidget::Yellow, CLedWidget::Black, shape, "Network connected", "Network disconnected", 14);
            this->ui->led_Simulator->setValues(CLedWidget::Yellow, CLedWidget::Black, shape, "Simulator connected", "Simulator disconnected", 14);

            shape = CLedWidget::Rounded;
            this->ui->led_Ptt->setValues(CLedWidget::Yellow, CLedWidget::Red, shape, "Ptt", "Silence", 18);
            this->ui->led_Audio->setValues(CLedWidget::Yellow, CLedWidget::Red, shape, "On", "Muted", 18);
        }

        void CInfoBarStatusComponent::setDBusStatus(bool dbus)
        {
            this->ui->led_DBus->setOn(dbus);
        }

        void CInfoBarStatusComponent::setDBusTooltip(const QString &tooltip)
        {
            this->ui->led_DBus->setOnToolTip(tooltip);
        }

        void CInfoBarStatusComponent::setVolume(int volume)
        {
            if (volume < 1)
            {
                this->ui->led_Audio->setOn(false);
            }
            else
            {
                this->ui->led_Audio->setOn(true);
            }
        }

        void CInfoBarStatusComponent::runtimeHasBeenSet()
        {
            Q_ASSERT(getIContextSimulator());
            Q_ASSERT(getIContextAudio());
            Q_ASSERT(getIContextNetwork());

            if (this->getIContextSimulator())
            {
                connect(this->getIContextSimulator(), &IContextSimulator::connectionChanged, this, &CInfoBarStatusComponent::ps_simulatorConnectionChanged);
            }

            if (this->getIContextNetwork())
            {
                connect(this->getIContextNetwork(), &IContextNetwork::connectionStatusChanged, this, &CInfoBarStatusComponent::ps_networkConnectionChanged);
            }

            if (this->getIContextApplication())
            {
                if (this->getIContextApplication()->isUsingImplementingObject())
                {
                    this->ui->led_DBus->setOn(false);
                }
                else
                {
                    this->ui->led_DBus->setOn(true);
                }
            }
        }

        void CInfoBarStatusComponent::ps_simulatorConnectionChanged(bool connected)
        {
            this->ui->led_Simulator->setOn(connected);
        }

        void CInfoBarStatusComponent::ps_networkConnectionChanged(uint from, uint to, const QString &message)
        {
            INetwork::ConnectionStatus fromStatus = static_cast<INetwork::ConnectionStatus>(from);
            INetwork::ConnectionStatus toStatus = static_cast<INetwork::ConnectionStatus>(to);
            Q_UNUSED(fromStatus);
            Q_UNUSED(message);

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
                break;
            case INetwork::Connecting:
                this->ui->led_Network->setTemporaryColor(CLedWidget::Yellow);
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

#if defined(Q_OS_WIN)
            // QSysInfo::WindowsVersion only available on Win platforms
            if (QSysInfo::WindowsVersion & QSysInfo::WV_NT_based)
            {
                menuAudio.addAction("Mixer");
            }
#endif

            QAction *selectedItem = menuAudio.exec(globalPosition);
            if (selectedItem)
            {
                // http://forum.technical-assistance.co.uk/sndvol32exe-command-line-parameters-vt1348.html
                const QList<QAction *> actions = menuAudio.actions();
                if (selectedItem == actions.at(0))
                {
                    // TODO: toogle mute
                }
                else if (actions.size() > 1 && selectedItem == actions.at(1))
                {
                    QStringList parameterlist;
                    QProcess::startDetached("SndVol.exe", parameterlist);
                }
            }
        } // custom menu
    }
}
