/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackcore/context/contextaudio.h"
#include "blackgui/components/audiodevicevolumesetupcomponent.h"
#include "blackgui/guiapplication.h"
#include "blackmisc/audio/audiodeviceinfo.h"
#include "blackmisc/audio/notificationsounds.h"
#include "blackmisc/audio/audiosettings.h"
#include "blackmisc/sequence.h"
#include "ui_audiodevicevolumesetupcomponent.h"

#include <QCheckBox>
#include <QComboBox>
#include <QToolButton>
#include <QtGlobal>
#include <QPointer>
#include <QFileDialog>

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
        CAudioDeviceVolumeSetupComponent::CAudioDeviceVolumeSetupComponent(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CAudioDeviceVolumeSetupComponent)
        {
            ui->setupUi(this);

            // deferred init, because in a distributed swift system
            // it takes a moment until the settings are sychronized
            // this is leading to undesired "save settings" messages and played sounds
            QPointer<CAudioDeviceVolumeSetupComponent> myself(this);
            QTimer::singleShot(2500, this, [ = ]
            {
                if (!myself || !sGui || sGui->isShuttingDown()) { return; }
                this->init();
            });
        }

        void CAudioDeviceVolumeSetupComponent::init()
        {
            if (!sGui || sGui->isShuttingDown() || !sGui->getIContextAudio()) { return; }

            // audio is optional
            const bool audio = this->hasAudio();
            this->setEnabled(audio);
            this->reloadSettings();

            bool c = connect(ui->cb_SetupAudioLoopback, &QCheckBox::toggled, this, &CAudioDeviceVolumeSetupComponent::onLoopbackToggled);
            Q_ASSERT(c);

            if (audio)
            {
                ui->le_ExtraInfo->setText(audio ? sGui->getIContextAudio()->audioRunsWhereInfo() : "No audio, cannot change.");

                this->initAudioDeviceLists();

                // default
                ui->cb_SetupAudioLoopback->setChecked(sGui->getIContextAudio()->isAudioLoopbackEnabled());

                // the connects depend on initAudioDeviceLists
                c = connect(ui->cb_SetupAudioInputDevice,  qOverload<int>(&QComboBox::currentIndexChanged), this, &CAudioDeviceVolumeSetupComponent::onAudioDeviceSelected);
                Q_ASSERT(c);
                c = connect(ui->cb_SetupAudioOutputDevice, qOverload<int>(&QComboBox::currentIndexChanged), this, &CAudioDeviceVolumeSetupComponent::onAudioDeviceSelected);
                Q_ASSERT(c);

                // context
                c = connect(sGui->getIContextAudio(), &IContextAudio::changedAudioDevices, this, &CAudioDeviceVolumeSetupComponent::onAudioDevicesChanged, Qt::QueuedConnection);
                Q_ASSERT(c);
                c = connect(sGui->getIContextAudio(), &IContextAudio::changedSelectedAudioDevices, this, &CAudioDeviceVolumeSetupComponent::onCurrentAudioDevicesChanged, Qt::QueuedConnection);
                Q_ASSERT(c);
            }
            Q_UNUSED(c);
        }

        CAudioDeviceVolumeSetupComponent::~CAudioDeviceVolumeSetupComponent()
        { }

        void CAudioDeviceVolumeSetupComponent::reloadSettings()
        {
            const CSettings as(m_audioSettings.getThreadLocal());
        }

        void CAudioDeviceVolumeSetupComponent::initAudioDeviceLists()
        {
            if (!this->hasAudio()) { return; }
            this->onAudioDevicesChanged(sGui->getIContextAudio()->getAudioDevices());
            this->onCurrentAudioDevicesChanged(sGui->getIContextAudio()->getCurrentAudioDevices());
        }

        bool CAudioDeviceVolumeSetupComponent::hasAudio() const
        {
            return sGui && sGui->getIContextAudio() && !sGui->getIContextAudio()->isEmptyObject();
        }

        void CAudioDeviceVolumeSetupComponent::onAudioDeviceSelected(int index)
        {
            if (!sGui || sGui->isShuttingDown() || !sGui->getIContextAudio()) { return; }
            if (index < 0) { return; }

            CAudioDeviceInfoList devices = sGui->getIContextAudio()->getAudioDevices();
            if (devices.isEmpty()) { return; }
            CAudioDeviceInfo selectedDevice;
            const QObject *sender = QObject::sender();
            if (sender == ui->cb_SetupAudioInputDevice)
            {
                const CAudioDeviceInfoList inputDevices = devices.getInputDevices();
                if (index >= inputDevices.size()) { return; }
                selectedDevice = inputDevices[index];
                sGui->getIContextAudio()->setCurrentAudioDevice(selectedDevice);
            }
            else if (sender == ui->cb_SetupAudioOutputDevice)
            {
                const CAudioDeviceInfoList outputDevices = devices.getOutputDevices();
                if (index >= outputDevices.size()) { return; }
                selectedDevice = outputDevices[index];
                sGui->getIContextAudio()->setCurrentAudioDevice(selectedDevice);
            }
        }

        void CAudioDeviceVolumeSetupComponent::onCurrentAudioDevicesChanged(const CAudioDeviceInfoList &devices)
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

        void CAudioDeviceVolumeSetupComponent::onAudioDevicesChanged(const CAudioDeviceInfoList &devices)
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

        void CAudioDeviceVolumeSetupComponent::onLoopbackToggled(bool loopback)
        {
            if (!sGui || sGui->isShuttingDown() || !sGui->getIContextAudio()) { return; }
            if (sGui->getIContextAudio()->isAudioLoopbackEnabled() == loopback) { return; }
            sGui->getIContextAudio()->enableAudioLoopback(loopback);
        }
    } // namespace
} // namespace
