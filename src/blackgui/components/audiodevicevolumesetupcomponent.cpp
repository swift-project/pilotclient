// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackgui/components/audiodevicevolumesetupcomponent.h"
#include "blackgui/guiapplication.h"
#include "blackgui/guiutility.h"
#include "blackcore/afv/clients/afvclient.h"
#include "blackcore/context/contextsimulator.h"
#include "blackcore/context/contextaudioimpl.h"

#include "misc/simulation/settings/simulatorsettings.h"
#include "misc/audio/audiodeviceinfo.h"
#include "misc/audio/notificationsounds.h"
#include "misc/audio/audiosettings.h"
#include "misc/sequence.h"
#include "ui_audiodevicevolumesetupcomponent.h"

#include <QCheckBox>
#include <QComboBox>
#include <QToolButton>
#include <QtGlobal>
#include <QPointer>
#include <QFileDialog>
#include <QStringLiteral>

using namespace BlackCore;
using namespace BlackCore::Afv::Audio;
using namespace BlackCore::Afv::Clients;
using namespace BlackCore::Context;
using namespace swift::misc;
using namespace swift::misc::aviation;
using namespace swift::misc::audio;
using namespace swift::misc::physical_quantities;
using namespace swift::misc::simulation;

namespace BlackGui::Components
{
    CAudioDeviceVolumeSetupComponent::CAudioDeviceVolumeSetupComponent(QWidget *parent) : QFrame(parent),
                                                                                          ui(new Ui::CAudioDeviceVolumeSetupComponent)
    {
        ui->setupUi(this);
        connect(ui->hs_VolumeIn, &QSlider::valueChanged, this, &CAudioDeviceVolumeSetupComponent::onVolumeSliderChanged);
        connect(ui->hs_VolumeOut, &QSlider::valueChanged, this, &CAudioDeviceVolumeSetupComponent::onVolumeSliderChanged);
        connect(ui->hs_VolumeOutCom1, &QSlider::valueChanged, this, &CAudioDeviceVolumeSetupComponent::onVolumeSliderChanged);
        connect(ui->hs_VolumeOutCom2, &QSlider::valueChanged, this, &CAudioDeviceVolumeSetupComponent::onVolumeSliderChanged);
        connect(ui->tb_RefreshInDevice, &QToolButton::released, this, &CAudioDeviceVolumeSetupComponent::onReloadDevices, Qt::QueuedConnection);
        connect(ui->tb_RefreshOutDevice, &QToolButton::released, this, &CAudioDeviceVolumeSetupComponent::onReloadDevices, Qt::QueuedConnection);
        connect(ui->tb_ResetInVolume, &QToolButton::released, this, &CAudioDeviceVolumeSetupComponent::onResetVolumeIn, Qt::QueuedConnection);
        connect(ui->tb_ResetOutVolume, &QToolButton::released, this, &CAudioDeviceVolumeSetupComponent::onResetVolumeOut, Qt::QueuedConnection);
        connect(ui->tb_ResetOutVolumeCom1, &QToolButton::released, this, &CAudioDeviceVolumeSetupComponent::onResetVolumeOutCom1, Qt::QueuedConnection);
        connect(ui->tb_ResetOutVolumeCom2, &QToolButton::released, this, &CAudioDeviceVolumeSetupComponent::onResetVolumeOutCom2, Qt::QueuedConnection);

        connect(ui->cb_1Tx, &QCheckBox::toggled, this, &CAudioDeviceVolumeSetupComponent::onRxTxChanged, Qt::QueuedConnection);
        connect(ui->cb_2Tx, &QCheckBox::toggled, this, &CAudioDeviceVolumeSetupComponent::onRxTxChanged, Qt::QueuedConnection);
        connect(ui->cb_1Rec, &QCheckBox::toggled, this, &CAudioDeviceVolumeSetupComponent::onRxTxChanged, Qt::QueuedConnection);
        connect(ui->cb_2Rec, &QCheckBox::toggled, this, &CAudioDeviceVolumeSetupComponent::onRxTxChanged, Qt::QueuedConnection);

        ui->hs_VolumeIn->setMaximum(CSettings::InMax);
        ui->hs_VolumeIn->setMinimum(CSettings::InMin);
        ui->hs_VolumeOut->setMaximum(CSettings::OutMax);
        ui->hs_VolumeOut->setMinimum(CSettings::OutMin);
        ui->hs_VolumeOutCom1->setMaximum(CSettings::OutMax);
        ui->hs_VolumeOutCom1->setMinimum(CSettings::OutMin);
        ui->hs_VolumeOutCom2->setMaximum(CSettings::OutMax);
        ui->hs_VolumeOutCom2->setMinimum(CSettings::OutMin);

        const CSettings as(m_audioSettings.getThreadLocal());
        const int i = this->getInValue();
        const int o = this->getOutValue();
        const int o1 = this->getOutValueCom1();
        const int o2 = this->getOutValueCom2();
        ui->hs_VolumeIn->setValue(i);
        ui->hs_VolumeOut->setValue(o);
        ui->hs_VolumeOutCom1->setValue(o1);
        ui->hs_VolumeOutCom2->setValue(o2);
        ui->cb_SetupAudioLoopback->setChecked(false);
        ui->cb_DisableAudioEffects->setChecked(!as.isAudioEffectsEnabled());

        ui->led_AudioConnected->setToolTips("Voice on and authenticated", "Voice off");
        ui->led_AudioConnected->setShape(CLedWidget::Rounded);
        ui->led_Rx1->setToolTips("COM1 receiving", "COM1 idle");
        ui->led_Rx1->setShape(CLedWidget::Rounded);
        ui->led_Rx2->setToolTips("COM2 receiving", "COM2 idle");
        ui->led_Rx2->setShape(CLedWidget::Rounded);

        // deferred init, because in a distributed swift system
        // it takes a moment until the settings are sychronized
        // this is leading to undesired "save settings" messages and played sounds
        QPointer<CAudioDeviceVolumeSetupComponent> myself(this);
        QTimer::singleShot(2000, this, [=] {
            if (!myself || !sGui || sGui->isShuttingDown()) { return; }
            this->init();
        });

        this->setCheckBoxesReadOnly(this->isComIntegrated());
        this->setVolumeSlidersReadOnly(this->isComIntegrated());
    }

    void CAudioDeviceVolumeSetupComponent::init()
    {
        if (!sGui || sGui->isShuttingDown() || !sGui->getCContextAudioBase()) { return; }

        // audio is optional
        const bool audio = this->hasAudio();
        this->setEnabled(audio);
        this->reloadSettings();

        bool c = connect(ui->cb_SetupAudioLoopback, &QCheckBox::toggled, this, &CAudioDeviceVolumeSetupComponent::onLoopbackToggled);
        Q_ASSERT(c);
        c = connect(ui->cb_DisableAudioEffects, &QCheckBox::toggled, this, &CAudioDeviceVolumeSetupComponent::onDisableAudioEffectsToggled);
        Q_ASSERT(c);

        if (hasSimulator())
        {
            c = connect(sGui->getIContextSimulator(), &IContextSimulator::simulatorSettingsChanged, this, &CAudioDeviceVolumeSetupComponent::simulatorSettingsChanged);
            Q_ASSERT(c);
        }

        if (audio)
        {
            this->setAudioRunsWhere();
            this->initAudioDeviceLists();

            // default
            ui->cb_SetupAudioLoopback->setChecked(sGui->getCContextAudioBase()->isAudioLoopbackEnabled());

            // the connects depend on initAudioDeviceLists
            c = connect(ui->cb_SetupAudioInputDevice, qOverload<int>(&QComboBox::currentIndexChanged), this, &CAudioDeviceVolumeSetupComponent::onAudioDeviceSelected, Qt::QueuedConnection);
            Q_ASSERT(c);
            c = connect(ui->cb_SetupAudioOutputDevice, qOverload<int>(&QComboBox::currentIndexChanged), this, &CAudioDeviceVolumeSetupComponent::onAudioDeviceSelected, Qt::QueuedConnection);
            Q_ASSERT(c);

            // context
            c = connect(sGui->getCContextAudioBase(), &CContextAudioBase::changedLocalAudioDevices, this, &CAudioDeviceVolumeSetupComponent::onAudioDevicesChanged, Qt::QueuedConnection);
            Q_ASSERT(c);
            c = connect(sGui->getCContextAudioBase(), &CContextAudioBase::startedAudio, this, &CAudioDeviceVolumeSetupComponent::onAudioStarted, Qt::QueuedConnection);
            Q_ASSERT(c);
            c = connect(sGui->getCContextAudioBase(), &CContextAudioBase::stoppedAudio, this, &CAudioDeviceVolumeSetupComponent::onAudioStopped, Qt::QueuedConnection);
            Q_ASSERT(c);

            this->initWithAfvClient();
            m_init = true;
        }
        Q_UNUSED(c)
    }

    void CAudioDeviceVolumeSetupComponent::initWithAfvClient()
    {
        if (!afvClient()) { return; }
        m_afvConnections.disconnectAll();

        //! \todo Workaround to avoid context signals
        CAfvClient *afv = afvClient();
        const Qt::ConnectionType ct = Qt::QueuedConnection;
        QMetaObject::Connection c;
        c = connect(afv, &CAfvClient::outputVolumePeakVU, this, &CAudioDeviceVolumeSetupComponent::onOutputVU, ct);
        Q_ASSERT(c);
        m_afvConnections.append(c);
        c = connect(afv, &CAfvClient::inputVolumePeakVU, this, &CAudioDeviceVolumeSetupComponent::onInputVU, ct);
        Q_ASSERT(c);
        m_afvConnections.append(c);
        c = connect(afv, &CAfvClient::receivedCallsignsChanged, this, &CAudioDeviceVolumeSetupComponent::onReceivingCallsignsChanged, ct);
        Q_ASSERT(c);
        m_afvConnections.append(c);
        c = connect(afv, &CAfvClient::updatedFromOwnAircraftCockpit, this, &CAudioDeviceVolumeSetupComponent::onUpdatedClientWithCockpitData, ct);
        Q_ASSERT(c);
        m_afvConnections.append(c);

        // default values for RX/TX
        afv->setRxTx(true, true, true, false);

        QPointer<CAudioDeviceVolumeSetupComponent> myself(this);
        c = connect(
            afv, &CAfvClient::connectionStatusChanged, this, [=](CAfvClient::ConnectionStatus status) {
                if (!myself || !sGui || sGui->isShuttingDown()) { return; }
                myself->setTransmitReceiveInUiFromVoiceClient();
                Q_UNUSED(status)
            },
            ct);
        Q_ASSERT(c);
        m_afvConnections.append(c);

        this->setTransmitReceiveInUiFromVoiceClient();
    }

    CAudioDeviceVolumeSetupComponent::~CAudioDeviceVolumeSetupComponent()
    {}

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

    int CAudioDeviceVolumeSetupComponent::getOutValueCom1(int from, int to) const
    {
        const double r = ui->hs_VolumeOutCom1->maximum() - ui->hs_VolumeOutCom1->minimum();
        const double tr = to - from;
        return qRound(ui->hs_VolumeOutCom1->value() / r * tr);
    }

    int CAudioDeviceVolumeSetupComponent::getOutValueCom2(int from, int to) const
    {
        const double r = ui->hs_VolumeOutCom2->maximum() - ui->hs_VolumeOutCom2->minimum();
        const double tr = to - from;
        return qRound(ui->hs_VolumeOutCom2->value() / r * tr);
    }

    void CAudioDeviceVolumeSetupComponent::setInValue(int value, int from, int to)
    {
        if (value > to) { value = to; }
        else if (value < from) { value = from; }
        const double r = ui->hs_VolumeIn->maximum() - ui->hs_VolumeIn->minimum();
        const double tr = to - from;
        ui->hs_VolumeIn->setValue(qRound(value / tr * r));
    }

    void CAudioDeviceVolumeSetupComponent::setOutValue(int value, int from, int to)
    {
        if (value > to) { value = to; }
        else if (value < from) { value = from; }
        const double r = ui->hs_VolumeOut->maximum() - ui->hs_VolumeOut->minimum();
        const double tr = to - from;
        ui->hs_VolumeOut->setValue(qRound(value / tr * r));
    }

    void CAudioDeviceVolumeSetupComponent::setOutValueCom1(int value, int from, int to)
    {
        if (value > to) { value = to; }
        else if (value < from) { value = from; }
        const double r = ui->hs_VolumeOutCom1->maximum() - ui->hs_VolumeOutCom1->minimum();
        const double tr = to - from;
        ui->hs_VolumeOutCom1->setValue(qRound(value / tr * r));
    }

    void CAudioDeviceVolumeSetupComponent::setOutValueCom2(int value, int from, int to)
    {
        if (value > to) { value = to; }
        else if (value < from) { value = from; }
        const double r = ui->hs_VolumeOutCom2->maximum() - ui->hs_VolumeOutCom2->minimum();
        const double tr = to - from;
        ui->hs_VolumeOutCom2->setValue(qRound(value / tr * r));
    }

    void CAudioDeviceVolumeSetupComponent::setInLevel(double value)
    {
        if (value > 1.0) { value = 1.0; }
        else if (value < 0.0) { value = 0.0; }
        ui->wip_InLevelMeter->levelChanged(value);
    }

    void CAudioDeviceVolumeSetupComponent::setOutLevel(double value)
    {
        if (value > 1.0) { value = 1.0; }
        else if (value < 0.0) { value = 0.0; }
        ui->wip_OutLevelMeter->levelChanged(value);
    }

    void CAudioDeviceVolumeSetupComponent::setInfo(const QString &info)
    {
        ui->le_Info->setText(info);
        ui->le_Info->setToolTip(info);
    }

    void CAudioDeviceVolumeSetupComponent::setTransmitReceiveInUi(bool tx1, bool rec1, bool tx2, bool rec2, bool integrated)
    {
        this->setRxTxCheckboxes(rec1, tx1, rec2, tx2);
        this->setCheckBoxesReadOnly(integrated);
        this->setVolumeSlidersReadOnly(integrated);
    }

    void CAudioDeviceVolumeSetupComponent::setTransmitReceiveInUiFromVoiceClient()
    {
        if (!this->hasAudio())
        {
            ui->led_AudioConnected->setOn(false);
            return;
        }

        const bool on = sGui->getCContextAudioBase()->isAudioConnected();
        ui->led_AudioConnected->setOn(on);

        const bool com1Enabled = sGui->getCContextAudioBase()->isEnabledComUnit(CComSystem::Com1);
        const bool com2Enabled = sGui->getCContextAudioBase()->isEnabledComUnit(CComSystem::Com2);

        const bool com1Tx = com1Enabled && sGui->getCContextAudioBase()->isTransmittingComUnit(CComSystem::Com1);
        const bool com2Tx = com2Enabled && sGui->getCContextAudioBase()->isTransmittingComUnit(CComSystem::Com2);

        // we do not have receiving, so we use enable
        const bool com1Rx = com1Enabled;
        const bool com2Rx = com2Enabled;

        const bool integrated = this->isComIntegrated();
        this->setTransmitReceiveInUi(com1Tx, com1Rx, com2Tx, com2Rx, integrated);

        // Set transmit volume in GUI
        if (integrated)
        {
            const int vol1 = sGui->getCContextAudioBase()->getComOutputVolume(CComSystem::Com1);
            const int vol2 = sGui->getCContextAudioBase()->getComOutputVolume(CComSystem::Com2);
            ui->hs_VolumeOutCom1->setValue(vol1);
            ui->hs_VolumeOutCom2->setValue(vol2);
        }
    }

    void CAudioDeviceVolumeSetupComponent::setCheckBoxesReadOnly(bool readonly)
    {
        // all tx/rec checkboxes
        CGuiUtility::checkBoxReadOnly(ui->cb_1Tx, readonly);
        CGuiUtility::checkBoxReadOnly(ui->cb_2Tx, readonly);
        CGuiUtility::checkBoxReadOnly(ui->cb_1Rec, readonly);
        CGuiUtility::checkBoxReadOnly(ui->cb_2Rec, readonly);
    }

    void CAudioDeviceVolumeSetupComponent::setVolumeSlidersReadOnly(bool readonly)
    {
        ui->hs_VolumeOutCom1->setDisabled(readonly);
        ui->hs_VolumeOutCom2->setDisabled(readonly);
        if (readonly)
        {
            // \fixme hardcoded stylesheet setting, should come from stylesheet")
            ui->hs_VolumeOutCom1->setStyleSheet("background: rgb(40,40,40)");
            ui->hs_VolumeOutCom2->setStyleSheet("background: rgb(40,40,40)");
        }
        else
        {
            ui->hs_VolumeOutCom1->setStyleSheet("");
            ui->hs_VolumeOutCom2->setStyleSheet("");
        }
    }

    CAfvClient *CAudioDeviceVolumeSetupComponent::afvClient()
    {
        if (!sGui || sGui->isShuttingDown() || !sGui->getCContextAudioBase()) { return nullptr; }
        return sGui->getCContextAudioBase()->afvClient();
    }

    void CAudioDeviceVolumeSetupComponent::reloadSettings()
    {
        const CSettings as(m_audioSettings.getThreadLocal());
        ui->cb_DisableAudioEffects->setChecked(!as.isAudioEffectsEnabled());
        this->setInValue(as.getInVolume());
        this->setOutValue(as.getOutVolume());
        this->setOutValueCom1(as.getOutVolumeCom1());
        this->setOutValueCom2(as.getOutVolumeCom2());
    }

    void CAudioDeviceVolumeSetupComponent::initAudioDeviceLists()
    {
        if (!this->hasAudio()) { return; }
        const bool changed = this->onAudioDevicesChanged(sGui->getCContextAudioBase()->getAudioDevicesPlusDefault());
        if (!changed) { return; }
        const CAudioDeviceInfoList currentDevices = sGui->getCContextAudioBase()->getCurrentAudioDevices();
        this->onAudioStarted(currentDevices.getInputDevices().frontOrDefault(), currentDevices.getOutputDevices().frontOrDefault());
    }

    bool CAudioDeviceVolumeSetupComponent::hasAudio() const
    {
        return sGui && sGui->getCContextAudioBase();
    }

    bool CAudioDeviceVolumeSetupComponent::hasSimulator() const
    {
        return sGui && sGui->getIContextSimulator();
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
        const int o1 = this->getOutValueCom1();
        const int o2 = this->getOutValueCom2();
        if (as.getInVolume() == i && o == as.getOutVolume() &&
            as.getOutVolumeCom1() == o1 && as.getOutVolumeCom2() == o2) { return; }
        as.setInVolume(i);
        as.setOutVolume(o);
        as.setOutVolumeCom1(o1);
        as.setOutVolumeCom2(o2);
        m_audioSettings.setAndSave(as);
    }

    void CAudioDeviceVolumeSetupComponent::onOutputVU(double vu)
    {
        this->setOutLevel(vu);
    }

    void CAudioDeviceVolumeSetupComponent::onInputVU(double vu)
    {
        this->setInLevel(vu);
    }

    void CAudioDeviceVolumeSetupComponent::onReloadDevices()
    {
        if (!hasAudio()) { return; }
        this->initAudioDeviceLists();
        const CAudioDeviceInfo i = this->getSelectedInputDevice();
        const CAudioDeviceInfo o = this->getSelectedOutputDevice();
        sGui->getCContextAudioBase()->setCurrentAudioDevices(i, o);
    }

    void CAudioDeviceVolumeSetupComponent::onResetVolumeIn()
    {
        ui->hs_VolumeIn->setValue((ui->hs_VolumeIn->maximum() - ui->hs_VolumeIn->minimum()) / 2);
    }

    void CAudioDeviceVolumeSetupComponent::onResetVolumeOut()
    {
        ui->hs_VolumeOut->setValue((ui->hs_VolumeOut->maximum() - ui->hs_VolumeOut->minimum()) / 2);
    }

    void CAudioDeviceVolumeSetupComponent::onResetVolumeOutCom1()
    {
        ui->hs_VolumeOutCom1->setValue(ui->hs_VolumeOutCom1->maximum());
    }

    void CAudioDeviceVolumeSetupComponent::onResetVolumeOutCom2()
    {
        ui->hs_VolumeOutCom2->setValue(ui->hs_VolumeOutCom2->maximum());
    }

    void CAudioDeviceVolumeSetupComponent::setAudioRunsWhere()
    {
        if (!this->hasAudio()) { return; }
        const QString ai = sGui->getCContextAudioBase()->audioRunsWhereInfo();
        ui->le_Info->setPlaceholderText(ai);
    }

    void CAudioDeviceVolumeSetupComponent::simulatorSettingsChanged()
    {
        const bool integrated = this->isComIntegrated();
        setCheckBoxesReadOnly(integrated);
        setVolumeSlidersReadOnly(integrated);
    }

    bool CAudioDeviceVolumeSetupComponent::isComIntegrated() const
    {
        if (!this->hasSimulator()) { return false; }
        const simulation::settings::CSimulatorSettings settings = sGui->getIContextSimulator()->getSimulatorSettings();
        const bool integrate = settings.isComIntegrated();
        return integrate;
    }

    void CAudioDeviceVolumeSetupComponent::onRxTxChanged(bool checked)
    {
        if (!this->hasAudio()) { return; }
        if (this->isComIntegrated()) { return; }

        Q_UNUSED(checked)
        const bool rx1 = ui->cb_1Rec->isChecked();
        const bool rx2 = ui->cb_2Rec->isChecked();

        // no transmit without receiving
        const bool tx1 = rx1 && ui->cb_1Tx->isChecked();
        const bool tx2 = rx2 && ui->cb_2Tx->isChecked();

        sGui->getCContextAudioBase()->setRxTx(rx1, tx1, rx2, tx2);

        QPointer<CAudioDeviceVolumeSetupComponent> myself(this);
        QTimer::singleShot(25, this, [=] {
            // in case UI values are not correct
            if (!myself) { return; }
            this->setRxTxCheckboxes(rx1, tx1, rx2, tx2);
        });
    }

    void CAudioDeviceVolumeSetupComponent::setRxTxCheckboxes(bool rx1, bool tx1, bool rx2, bool tx2)
    {
        if (ui->cb_1Tx->isChecked() != tx1) { ui->cb_1Tx->setChecked(tx1); }
        if (ui->cb_2Tx->isChecked() != tx2) { ui->cb_2Tx->setChecked(tx2); }
        if (ui->cb_1Rec->isChecked() != rx1) { ui->cb_1Rec->setChecked(rx1); }
        if (ui->cb_2Rec->isChecked() != rx2) { ui->cb_2Rec->setChecked(rx2); }
    }

    void CAudioDeviceVolumeSetupComponent::onReceivingCallsignsChanged(const CCallsignSet &com1Callsigns, const CCallsignSet &com2Callsigns)
    {
        const QString info = (com1Callsigns.isEmpty() ? QString() : QStringLiteral("COM1: ") % com1Callsigns.getCallsignsAsString()) %
                             (!com1Callsigns.isEmpty() && !com2Callsigns.isEmpty() ? QStringLiteral(" | ") : QString()) %
                             (com2Callsigns.isEmpty() ? QString() : QStringLiteral("COM2: ") % com2Callsigns.getCallsignsAsString());

        ui->led_Rx1->setOn(!com1Callsigns.isEmpty());
        ui->led_Rx2->setOn(!com2Callsigns.isEmpty());
        this->setInfo(info);
    }

    void CAudioDeviceVolumeSetupComponent::onUpdatedClientWithCockpitData()
    {
        this->setTransmitReceiveInUiFromVoiceClient();
    }

    CAudioDeviceInfo CAudioDeviceVolumeSetupComponent::getSelectedInputDevice() const
    {
        if (!hasAudio()) { return CAudioDeviceInfo(); }
        const CAudioDeviceInfoList devices = sGui->getCContextAudioBase()->getAudioInputDevicesPlusDefault();
        return devices.findByName(ui->cb_SetupAudioInputDevice->currentText());
    }

    CAudioDeviceInfo CAudioDeviceVolumeSetupComponent::getSelectedOutputDevice() const
    {
        if (!hasAudio()) { return CAudioDeviceInfo(); }
        const CAudioDeviceInfoList devices = sGui->getCContextAudioBase()->getAudioOutputDevicesPlusDefault();
        return devices.findByName(ui->cb_SetupAudioOutputDevice->currentText());
    }

    void CAudioDeviceVolumeSetupComponent::onAudioDeviceSelected(int index)
    {
        if (!sGui || sGui->isShuttingDown() || !sGui->getIContextAudio()) { return; }
        if (index < 0) { return; }

        const CAudioDeviceInfo in = this->getSelectedInputDevice();
        const CAudioDeviceInfo out = this->getSelectedOutputDevice();
        sGui->getCContextAudioBase()->setCurrentAudioDevices(in, out);
    }

    void CAudioDeviceVolumeSetupComponent::onAudioStarted(const CAudioDeviceInfo &input, const CAudioDeviceInfo &output)
    {
        if (!afvClient()) { return; }
        if (m_afvConnections.isEmpty() && m_init)
        {
            this->initWithAfvClient();
        }

        ui->cb_SetupAudioInputDevice->setCurrentText(input.toQString(true));
        ui->cb_SetupAudioOutputDevice->setCurrentText(output.toQString(true));
        this->setAudioRunsWhere();
    }

    void CAudioDeviceVolumeSetupComponent::onAudioStopped()
    {
        this->setAudioRunsWhere();
        if (!afvClient())
        {
            m_afvConnections.disconnectAll();
        }
    }

    bool CAudioDeviceVolumeSetupComponent::onAudioDevicesChanged(const CAudioDeviceInfoList &devices)
    {
        if (m_cbDevices.hasSameDevices(devices)) { return false; } // avoid numerous follow up actions
        m_cbDevices = devices;

        this->setAudioRunsWhere();
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

        return true;
    }

    void CAudioDeviceVolumeSetupComponent::onLoopbackToggled(bool loopback)
    {
        if (!sGui || sGui->isShuttingDown() || !sGui->getIContextAudio()) { return; }
        if (sGui->getCContextAudioBase()->isAudioLoopbackEnabled() == loopback) { return; }
        sGui->getCContextAudioBase()->enableAudioLoopback(loopback);
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
