/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/context/contextaudio.h"
#include "blackgui/components/audiosetupcomponent.h"
#include "blackgui/guiapplication.h"
#include "blackmisc/audio/audiodeviceinfo.h"
#include "blackmisc/audio/notificationsounds.h"
#include "blackmisc/audio/audiosettings.h"
#include "blackmisc/sequence.h"
#include "ui_audiosetupcomponent.h"

#include <QCheckBox>
#include <QComboBox>
#include <QToolButton>
#include <QtGlobal>

using namespace BlackCore;
using namespace BlackCore::Context;
using namespace BlackMisc;
using namespace BlackGui;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Audio;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackGui
{
    namespace Components
    {
        CAudioSetupComponent::CAudioSetupComponent(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CAudioSetupComponent)
        {
            ui->setupUi(this);

            Q_ASSERT_X(sGui, Q_FUNC_INFO, "Missing sGui");
            Q_ASSERT_X(sGui->getIContextAudio(), Q_FUNC_INFO, "Missing Audio context");

            // audio is optional
            const bool audio = this->hasAudio();
            this->setEnabled(audio);
            ui->lbl_ExtraInfo->setText(audio ? sGui->getIContextAudio()->audioRunsWhereInfo() : "No audio, cannot change.");

            bool c = connect(ui->tb_ExpandNotificationSounds, &QToolButton::toggled, this, &CAudioSetupComponent::ps_onToggleNotificationSoundsVisibility);
            Q_ASSERT(c);
            c = connect(ui->cb_SetupAudioLoopback, &QCheckBox::toggled, this, &CAudioSetupComponent::ps_onLoopbackToggled);
            Q_ASSERT(c);

            if (audio)
            {
                this->initAudioDeviceLists();

                // default
                ui->cb_SetupAudioLoopback->setChecked(sGui->getIContextAudio()->isAudioLoopbackEnabled());

                // the connects depend on initAudioDeviceLists
                c = connect(ui->cb_SetupAudioInputDevice,  static_cast<void (QComboBox::*)(int)> (&QComboBox::currentIndexChanged), this, &CAudioSetupComponent::ps_audioDeviceSelected);
                Q_ASSERT(c);
                c = connect(ui->cb_SetupAudioOutputDevice, static_cast<void (QComboBox::*)(int)> (&QComboBox::currentIndexChanged), this, &CAudioSetupComponent::ps_audioDeviceSelected);
                Q_ASSERT(c);

                // context
                c = connect(sGui->getIContextAudio(), &IContextAudio::changedAudioDevices, this, &CAudioSetupComponent::ps_onAudioDevicesChanged);
                Q_ASSERT(c);
                c = connect(sGui->getIContextAudio(), &IContextAudio::changedSelectedAudioDevices, this, &CAudioSetupComponent::ps_onCurrentAudioDevicesChanged);
                Q_ASSERT(c);
            }
            this->ps_reloadSettings();
            ui->tb_ExpandNotificationSounds->setChecked(false); // collapse
            Q_UNUSED(c);
        }

        CAudioSetupComponent::~CAudioSetupComponent()
        { }

        void CAudioSetupComponent::ps_reloadSettings()
        {
            CSettings as(m_audioSettings.getThreadLocal());
            ui->cb_SetupAudioPlayNotificationSounds->setChecked(true);
            ui->cb_SetupAudioNotificationTextMessage->setChecked(as.getNotificationFlag(CNotificationSounds::NotificationTextMessagePrivate));
            ui->cb_SetupAudioNotificationVoiceRoom->setChecked(as.getNotificationFlag(CNotificationSounds::NotificationVoiceRoomJoined));
        }

        void CAudioSetupComponent::ps_onToggleNotificationSoundsVisibility(bool checked)
        {
            ui->fr_NotificationSoundsInner->setVisible(checked);
        }

        void CAudioSetupComponent::initAudioDeviceLists()
        {
            if (!this->hasAudio()) { return; }
            this->ps_onAudioDevicesChanged(sGui->getIContextAudio()->getAudioDevices());
            this->ps_onCurrentAudioDevicesChanged(sGui->getIContextAudio()->getCurrentAudioDevices());
        }

        bool CAudioSetupComponent::hasAudio() const
        {
            return sGui && sGui->getIContextAudio() && !sGui->getIContextAudio()->isEmptyObject();
        }

        bool CAudioSetupComponent::playNotificationSounds() const
        {
            return ui->cb_SetupAudioPlayNotificationSounds->isChecked();
        }

        void CAudioSetupComponent::ps_audioDeviceSelected(int index)
        {
            if (!sGui->getIContextAudio()) return;
            if (index < 0) { return; }

            CAudioDeviceInfoList devices = sGui->getIContextAudio()->getAudioDevices();
            if (devices.isEmpty()) { return; }
            CAudioDeviceInfo selectedDevice;
            QObject *sender = QObject::sender();
            if (sender == ui->cb_SetupAudioInputDevice)
            {
                CAudioDeviceInfoList inputDevices = devices.getInputDevices();
                if (index >= inputDevices.size()) { return; }
                selectedDevice = inputDevices[index];
                sGui->getIContextAudio()->setCurrentAudioDevice(selectedDevice);
            }
            else if (sender == ui->cb_SetupAudioOutputDevice)
            {
                CAudioDeviceInfoList outputDevices = devices.getOutputDevices();
                if (index >= outputDevices.size()) { return; }
                selectedDevice = outputDevices[index];
                sGui->getIContextAudio()->setCurrentAudioDevice(selectedDevice);
            }
        }

        void CAudioSetupComponent::ps_onCurrentAudioDevicesChanged(const CAudioDeviceInfoList &devices)
        {
            for (auto &device : devices)
            {
                if (device.getType() == CAudioDeviceInfo::InputDevice)
                {
                    ui->cb_SetupAudioInputDevice->setCurrentText(device.toQString(true));
                }
                else if (device.getType() == CAudioDeviceInfo::OutputDevice)
                {
                    ui->cb_SetupAudioOutputDevice->setCurrentText(device.toQString(true));
                }
            }
        }

        void CAudioSetupComponent::ps_onAudioDevicesChanged(const CAudioDeviceInfoList &devices)
        {
            ui->cb_SetupAudioOutputDevice->clear();
            ui->cb_SetupAudioInputDevice->clear();

            for (auto &device : devices)
            {
                if (device.getType() == CAudioDeviceInfo::InputDevice)
                {
                    ui->cb_SetupAudioInputDevice->addItem(device.toQString(true));
                }
                else if (device.getType() == CAudioDeviceInfo::OutputDevice)
                {
                    ui->cb_SetupAudioOutputDevice->addItem(device.toQString(true));
                }
            }
        }

        void CAudioSetupComponent::ps_onLoopbackToggled(bool loopback)
        {
            Q_ASSERT(sGui->getIContextAudio());
            if (sGui->getIContextAudio()->isAudioLoopbackEnabled() == loopback) { return; }
            sGui->getIContextAudio()->enableAudioLoopback(loopback);
        }
    } // namespace
} // namespace
