/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackgui/components/audiodevicevolumesetupcomponent.h"
#include "blackgui/guiapplication.h"

#include "blackcore/afv/clients/afvclient.h"
#include "blackcore/context/contextaudioimpl.h"

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
using namespace BlackCore::Afv::Clients;
using namespace BlackCore::Afv::Audio;
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
            connect(ui->hs_VolumeIn,         &QSlider::valueChanged, this, &CAudioDeviceVolumeSetupComponent::onVolumeSliderChanged);
            connect(ui->hs_VolumeOut,        &QSlider::valueChanged, this, &CAudioDeviceVolumeSetupComponent::onVolumeSliderChanged);
            connect(ui->tb_RefreshInDevice,  &QToolButton::released, this, &CAudioDeviceVolumeSetupComponent::onReloadDevices, Qt::QueuedConnection);
            connect(ui->tb_RefreshOutDevice, &QToolButton::released, this, &CAudioDeviceVolumeSetupComponent::onReloadDevices, Qt::QueuedConnection);
            connect(ui->tb_ResetInVolume,    &QToolButton::released, this, &CAudioDeviceVolumeSetupComponent::onResetVolumeIn,  Qt::QueuedConnection);
            connect(ui->tb_ResetOutVolume,   &QToolButton::released, this, &CAudioDeviceVolumeSetupComponent::onResetVolumeOut, Qt::QueuedConnection);

            ui->hs_VolumeIn->setMaximum(CSettings::InMax);
            ui->hs_VolumeIn->setMinimum(CSettings::InMin);
            ui->hs_VolumeOut->setMaximum(CSettings::OutMax);
            ui->hs_VolumeOut->setMinimum(CSettings::OutMin);

            const CSettings as(m_audioSettings.getThreadLocal());
            const int i = this->getInValue();
            const int o = this->getOutValue();
            ui->hs_VolumeIn->setValue(i);
            ui->hs_VolumeOut->setValue(o);

            // deferred init, because in a distributed swift system
            // it takes a moment until the settings are sychronized
            // this is leading to undesired "save settings" messages and played sounds
            QPointer<CAudioDeviceVolumeSetupComponent> myself(this);
            QTimer::singleShot(1000, this, [ = ]
            {
                if (!myself || !sGui || sGui->isShuttingDown()) { return; }
                this->init();
            });

            ui->pb_LevelIn->setValue(ui->pb_LevelIn->minimum());
            ui->pb_LevelOut->setValue(ui->pb_LevelOut->minimum());
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
            c = connect(ui->cb_DisableAudioEffects, &QCheckBox::toggled, this, &CAudioDeviceVolumeSetupComponent::onDisableAudioEffectsToggled);
            Q_ASSERT(c);

            if (audio)
            {
                ui->le_Info->setText(audio ? sGui->getIContextAudio()->audioRunsWhereInfo() : "No audio, cannot change.");

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

                CAfvClient *afvClient = CAudioDeviceVolumeSetupComponent::afvClient();
                if (afvClient)
                {
                    connect(afvClient, &CAfvClient::outputVolumePeakVU, this, &CAudioDeviceVolumeSetupComponent::onOutputVU);
                    connect(afvClient, &CAfvClient::inputVolumePeakVU,  this, &CAudioDeviceVolumeSetupComponent::onInputVU);
                    connect(afvClient, &CAfvClient::receivingCallsignsChanged, this, &CAudioDeviceVolumeSetupComponent::onReceivingCallsignsChanged);
                }

            }
            Q_UNUSED(c)
        }

        CAudioDeviceVolumeSetupComponent::~CAudioDeviceVolumeSetupComponent()
        { }

        int CAudioDeviceVolumeSetupComponent::getInValue(int from, int to) const
        {
            const double r = ui->hs_VolumeIn->maximum() - ui->hs_VolumeIn->minimum();
            const double tr = to - from;
            return qRound(ui->hs_VolumeIn->value() / r * tr);
        }

        int CAudioDeviceVolumeSetupComponent::getOutValue(int from, int to) const
        {
            const double r = ui->hs_VolumeOut->maximum() - ui->hs_VolumeOut->minimum();
            const double tr = to - from;
            return qRound(ui->hs_VolumeOut->value() / r * tr);
        }

        void CAudioDeviceVolumeSetupComponent::setInValue(int value, int from, int to)
        {
            if (value > to)   { value = to; }
            if (value < from) { value = from; }
            const double r = ui->hs_VolumeIn->maximum() - ui->hs_VolumeIn->minimum();
            const double tr = to - from;
            ui->hs_VolumeIn->setValue(qRound(value / tr * r));
        }

        void CAudioDeviceVolumeSetupComponent::setOutValue(int value, int from, int to)
        {
            if (value > to)   { value = to; }
            if (value < from) { value = from; }
            const double r = ui->hs_VolumeOut->maximum() - ui->hs_VolumeOut->minimum();
            const double tr = to - from;
            ui->hs_VolumeOut->setValue(qRound(value / tr * r));
        }

        void CAudioDeviceVolumeSetupComponent::setInLevel(int value, int from, int to)
        {
            if (value > to)   { value = to; }
            if (value < from) { value = from; }
            const double r = ui->pb_LevelIn->maximum() - ui->pb_LevelIn->minimum();
            const double tr = to - from;
            ui->pb_LevelIn->setValue(qRound(value / tr * r));
        }

        void CAudioDeviceVolumeSetupComponent::setOutLevel(int value, int from, int to)
        {
            if (value > to)   { value = to; }
            if (value < from) { value = from; }
            const double r = ui->pb_LevelOut->maximum() - ui->pb_LevelOut->minimum();
            const double tr = to - from;
            ui->pb_LevelOut->setValue(qRound(value / tr * r));
        }

        void CAudioDeviceVolumeSetupComponent::setInfo(const QString &info)
        {
            ui->le_Info->setText(info);
        }

        void CAudioDeviceVolumeSetupComponent::setTransmitReceive(bool tx1, bool rec1, bool tx2, bool rec2)
        {
            ui->cb_1Tx->setChecked(tx1);
            ui->cb_2Tx->setChecked(tx2);
            ui->cb_1Rec->setChecked(rec1);
            ui->cb_2Rec->setChecked(rec2);
        }

        void CAudioDeviceVolumeSetupComponent::reloadSettings()
        {
            const CSettings as(m_audioSettings.getThreadLocal());
            ui->cb_DisableAudioEffects->setChecked(as.isAudioEffectsEnabled());

            this->setInValue(as.getInVolume());
            this->setOutValue(as.getInVolume());
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

        void CAudioDeviceVolumeSetupComponent::onVolumeSliderChanged(int v)
        {
            Q_UNUSED(v)
            m_volumeSliderChanged.inputSignal();
        }

        void CAudioDeviceVolumeSetupComponent::saveVolumes()
        {
            CSettings as(m_audioSettings.getThreadLocal());
            const int i = this->getInValue();
            const int o = this->getOutValue();
            if (as.getInVolume() == i && as.getOutVolume() == o) { return; }
            as.setInVolume(i);
            as.setOutVolume(o);
            m_audioSettings.setAndSave(as);
        }

        void CAudioDeviceVolumeSetupComponent::onOutputVU(double vu)
        {
            this->setOutLevel(qRound(vu * 100.0), 0, 100);
        }

        void CAudioDeviceVolumeSetupComponent::onInputVU(double vu)
        {
            this->setInLevel(qRound(vu * 100.0), 0, 100);
        }

        void CAudioDeviceVolumeSetupComponent::onReloadDevices()
        {
            if (!hasAudio()) { return; }
            this->initAudioDeviceLists();
            const CAudioDeviceInfo i = this->getSelectedInputDevice();
            const CAudioDeviceInfo o = this->getSelectedInputDevice();
            sGui->getIContextAudio()->setCurrentAudioDevices(i, o);
        }

        void CAudioDeviceVolumeSetupComponent::onResetVolumeIn()
        {
            ui->hs_VolumeIn->setValue((ui->hs_VolumeIn->maximum() - ui->hs_VolumeIn->minimum()) / 2);
        }

        void CAudioDeviceVolumeSetupComponent::onResetVolumeOut()
        {
            ui->hs_VolumeOut->setValue((ui->hs_VolumeOut->maximum() - ui->hs_VolumeOut->minimum()) / 2);
        }

        void CAudioDeviceVolumeSetupComponent::onReceivingCallsignsChanged(const TransceiverReceivingCallsignsChangedArgs &args)
        {
            this->setInfo(args.receivingCallsigns.join(", "));
        }

        CAudioDeviceInfo CAudioDeviceVolumeSetupComponent::getSelectedInputDevice() const
        {
            if (!hasAudio()) { return CAudioDeviceInfo(); }
            const CAudioDeviceInfoList devices = sGui->getIContextAudio()->getAudioInputDevices();
            return devices.findByName(ui->cb_SetupAudioInputDevice->currentText());
        }

        CAudioDeviceInfo CAudioDeviceVolumeSetupComponent::getSelectedOutputDevice() const
        {
            if (!hasAudio()) { return CAudioDeviceInfo(); }
            const CAudioDeviceInfoList devices = sGui->getIContextAudio()->getAudioOutputDevices();
            return devices.findByName(ui->cb_SetupAudioOutputDevice->currentText());
        }

        CAfvClient *CAudioDeviceVolumeSetupComponent::afvClient()
        {
            if (!sGui || sGui->isShuttingDown() || !sGui->getIContextAudio()) { return nullptr; }
            if (!sGui->getIContextAudio()->isUsingImplementingObject()) { return nullptr; }

            CAfvClient &afvClient = sGui->getCoreFacade()->getCContextAudio()->voiceClient();
            return &afvClient;
        }

        void CAudioDeviceVolumeSetupComponent::onAudioDeviceSelected(int index)
        {
            if (!sGui || sGui->isShuttingDown() || !sGui->getIContextAudio()) { return; }
            if (index < 0) { return; }

            const CAudioDeviceInfo in  = this->getSelectedInputDevice();
            const CAudioDeviceInfo out = this->getSelectedOutputDevice();
            sGui->getIContextAudio()->setCurrentAudioDevices(in, out);
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

            const QString i = ui->cb_SetupAudioInputDevice->currentText();
            const QString o = ui->cb_SetupAudioOutputDevice->currentText();

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

            if (!i.isEmpty()) { ui->cb_SetupAudioInputDevice->setCurrentText(i); }
            if (!o.isEmpty()) { ui->cb_SetupAudioOutputDevice->setCurrentText(o); }
        }

        void CAudioDeviceVolumeSetupComponent::onLoopbackToggled(bool loopback)
        {
            if (!sGui || sGui->isShuttingDown() || !sGui->getIContextAudio()) { return; }
            if (sGui->getIContextAudio()->isAudioLoopbackEnabled() == loopback) { return; }
            sGui->getIContextAudio()->enableAudioLoopback(loopback);
        }

        void CAudioDeviceVolumeSetupComponent::onDisableAudioEffectsToggled(bool disabled)
        {
            if (!sGui || sGui->isShuttingDown() || !sGui->getIContextAudio()) { return; }
            CSettings as(m_audioSettings.getThreadLocal());
            const bool enabled = !disabled;
            if (as.isAudioEffectsEnabled() == enabled) { return; }
            as.setAudioEffectsEnabled(enabled);
            m_audioSettings.setAndSave(as);
        }
    } // namespace
} // namespace
