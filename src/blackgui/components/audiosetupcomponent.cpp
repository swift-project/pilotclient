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
            this->reloadSettings();

            bool c = connect(ui->cb_SetupAudioLoopback, &QCheckBox::toggled, this, &CAudioSetupComponent::onLoopbackToggled);
            Q_ASSERT(c);

            if (audio)
            {
                ui->le_ExtraInfo->setText(audio ? sGui->getIContextAudio()->audioRunsWhereInfo() : "No audio, cannot change.");

                this->initAudioDeviceLists();

                // default
                ui->cb_SetupAudioLoopback->setChecked(sGui->getIContextAudio()->isAudioLoopbackEnabled());

                // the connects depend on initAudioDeviceLists
                c = connect(ui->cb_SetupAudioInputDevice,  qOverload<int>(&QComboBox::currentIndexChanged), this, &CAudioSetupComponent::onAudioDeviceSelected);
                Q_ASSERT(c);
                c = connect(ui->cb_SetupAudioOutputDevice, qOverload<int>(&QComboBox::currentIndexChanged), this, &CAudioSetupComponent::onAudioDeviceSelected);
                Q_ASSERT(c);

                // context
                c = connect(sGui->getIContextAudio(), &IContextAudio::changedAudioDevices, this, &CAudioSetupComponent::onAudioDevicesChanged, Qt::QueuedConnection);
                Q_ASSERT(c);
                c = connect(sGui->getIContextAudio(), &IContextAudio::changedSelectedAudioDevices, this, &CAudioSetupComponent::onCurrentAudioDevicesChanged, Qt::QueuedConnection);
                Q_ASSERT(c);

                // checkboxes for notifications
                c = connect(ui->cb_SetupAudioPTTClick, &QCheckBox::toggled, this, &CAudioSetupComponent::onNotificationsToggled);
                Q_ASSERT(c);
                c = connect(ui->cb_SetupAudioNotificationVoiceRoomLeft, &QCheckBox::toggled, this, &CAudioSetupComponent::onNotificationsToggled);
                Q_ASSERT(c);
                c = connect(ui->cb_SetupAudioNotificationVoiceRoomJoined, &QCheckBox::toggled, this, &CAudioSetupComponent::onNotificationsToggled);
                Q_ASSERT(c);
                c = connect(ui->cb_SetupAudioNotificationTextMessagePrivate, &QCheckBox::toggled, this, &CAudioSetupComponent::onNotificationsToggled);
                Q_ASSERT(c);
                c = connect(ui->cb_SetupAudioNotificationTextCallsignMentioned, &QCheckBox::toggled, this, &CAudioSetupComponent::onNotificationsToggled);
                Q_ASSERT(c);
            }
            Q_UNUSED(c);
        }

        CAudioSetupComponent::~CAudioSetupComponent()
        { }

        bool CAudioSetupComponent::playNotificationSounds() const
        {
            if (!this->hasAudio()) { return false; }
            return ui->cb_SetupAudioPTTClick->isChecked() || ui->cb_SetupAudioNotificationTextMessagePrivate->isChecked() ||
                   ui->cb_SetupAudioNotificationVoiceRoomLeft->isChecked() || ui->cb_SetupAudioNotificationVoiceRoomJoined->isChecked() ||
                   ui->cb_SetupAudioNotificationTextCallsignMentioned->isChecked();
        }

        void CAudioSetupComponent::reloadSettings()
        {
            const CSettings as(m_audioSettings.getThreadLocal());
            ui->cb_SetupAudioPTTClick->setChecked(as.isNotificationFlagSet(CNotificationSounds::PTTClick));
            ui->cb_SetupAudioNotificationVoiceRoomLeft->setChecked(as.isNotificationFlagSet(CNotificationSounds::NotificationVoiceRoomLeft));
            ui->cb_SetupAudioNotificationVoiceRoomJoined->setChecked(as.isNotificationFlagSet(CNotificationSounds::NotificationVoiceRoomJoined));
            ui->cb_SetupAudioNotificationTextMessagePrivate->setChecked(as.isNotificationFlagSet(CNotificationSounds::NotificationTextMessagePrivate));
            ui->cb_SetupAudioNotificationTextCallsignMentioned->setChecked(as.isNotificationFlagSet(CNotificationSounds::NotificationTextCallsignMentioned));
        }

        void CAudioSetupComponent::initAudioDeviceLists()
        {
            if (!this->hasAudio()) { return; }
            this->onAudioDevicesChanged(sGui->getIContextAudio()->getAudioDevices());
            this->onCurrentAudioDevicesChanged(sGui->getIContextAudio()->getCurrentAudioDevices());
        }

        bool CAudioSetupComponent::hasAudio() const
        {
            return sGui && sGui->getIContextAudio() && !sGui->getIContextAudio()->isEmptyObject();
        }

        CNotificationSounds::NotificationFlag CAudioSetupComponent::checkBoxToFlag(const QCheckBox *cb) const
        {
            if (!cb) { return CNotificationSounds::NoNotifications; }

            if (cb == ui->cb_SetupAudioPTTClick) { return CNotificationSounds::PTTClick; }
            if (cb == ui->cb_SetupAudioNotificationVoiceRoomJoined) { return CNotificationSounds::NotificationVoiceRoomJoined; }
            if (cb == ui->cb_SetupAudioNotificationVoiceRoomLeft) { return CNotificationSounds::NotificationVoiceRoomLeft; }
            if (cb == ui->cb_SetupAudioNotificationTextCallsignMentioned) { return CNotificationSounds::NotificationTextCallsignMentioned; }
            if (cb == ui->cb_SetupAudioNotificationTextMessagePrivate) { return CNotificationSounds::NotificationTextMessagePrivate; }
            return CNotificationSounds::NoNotifications;
        }

        void CAudioSetupComponent::onAudioDeviceSelected(int index)
        {
            if (!sGui->getIContextAudio()) return;
            if (index < 0) { return; }

            CAudioDeviceInfoList devices = sGui->getIContextAudio()->getAudioDevices();
            if (devices.isEmpty()) { return; }
            CAudioDeviceInfo selectedDevice;
            const QObject *sender = QObject::sender();
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

        void CAudioSetupComponent::onCurrentAudioDevicesChanged(const CAudioDeviceInfoList &devices)
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

        void CAudioSetupComponent::onAudioDevicesChanged(const CAudioDeviceInfoList &devices)
        {
            ui->cb_SetupAudioOutputDevice->clear();
            ui->cb_SetupAudioInputDevice->clear();

            for (const CAudioDeviceInfo &device : devices)
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

        void CAudioSetupComponent::onLoopbackToggled(bool loopback)
        {
            Q_ASSERT(sGui->getIContextAudio());
            if (sGui->getIContextAudio()->isAudioLoopbackEnabled() == loopback) { return; }
            sGui->getIContextAudio()->enableAudioLoopback(loopback);
        }

        void CAudioSetupComponent::onNotificationsToggled(bool checked)
        {
            CSettings as(m_audioSettings.getThreadLocal());
            as.setNotificationFlag(CNotificationSounds::PTTClick, ui->cb_SetupAudioPTTClick->isChecked());
            as.setNotificationFlag(CNotificationSounds::NotificationVoiceRoomLeft, ui->cb_SetupAudioNotificationVoiceRoomLeft->isChecked());
            as.setNotificationFlag(CNotificationSounds::NotificationVoiceRoomJoined, ui->cb_SetupAudioNotificationVoiceRoomJoined->isChecked());
            as.setNotificationFlag(CNotificationSounds::NotificationTextMessagePrivate, ui->cb_SetupAudioNotificationTextMessagePrivate->isChecked());
            as.setNotificationFlag(CNotificationSounds::NotificationTextCallsignMentioned, ui->cb_SetupAudioNotificationTextCallsignMentioned->isChecked());
            const CStatusMessage msg = m_audioSettings.set(as);
            CLogMessage(this).preformatted(msg);

            const QCheckBox *sender = qobject_cast<const QCheckBox *>(QObject::sender());
            if (checked && this->hasAudio() && sender)
            {
                const CNotificationSounds::NotificationFlag f = this->checkBoxToFlag(sender);
                sGui->getIContextAudio()->playNotification(f, false);
            }
        }
    } // namespace
} // namespace
