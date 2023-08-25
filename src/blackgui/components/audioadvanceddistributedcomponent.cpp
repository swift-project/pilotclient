// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "audioadvanceddistributedcomponent.h"
#include "ui_audioadvanceddistributedcomponent.h"

#include "blackgui/guiapplication.h"
#include "blackcore/context/contextaudio.h"
#include "blackcore/context/contextnetwork.h"
#include "blackcore/afv/clients/afvclient.h"

using namespace BlackMisc::Audio;
using namespace BlackCore::Context;

namespace BlackGui::Components
{
    CAudioAdvancedDistributedComponent::CAudioAdvancedDistributedComponent(QWidget *parent) : QFrame(parent),
                                                                                              ui(new Ui::CAudioAdvancedDistributedComponent)
    {
        ui->setupUi(this);

        connect(sGui->getCContextAudioBase(), &CContextAudioBase::startedAudio, this, &CAudioAdvancedDistributedComponent::onAudioStarted, Qt::QueuedConnection);
        connect(sGui->getCContextAudioBase(), &CContextAudioBase::stoppedAudio, this, &CAudioAdvancedDistributedComponent::onAudioStoppend, Qt::QueuedConnection);
        connect(ui->pb_EnableDisable, &QPushButton::pressed, this, &CAudioAdvancedDistributedComponent::toggleAudioEnableDisable, Qt::QueuedConnection);
        connect(ui->pb_StartStop, &QPushButton::pressed, this, &CAudioAdvancedDistributedComponent::toggleAudioStartStop, Qt::QueuedConnection);
        connect(ui->pb_ReloadRegistered, &QPushButton::pressed, this, &CAudioAdvancedDistributedComponent::reloadRegisteredDevices, Qt::QueuedConnection);

        this->setButtons();
    }

    CAudioAdvancedDistributedComponent::~CAudioAdvancedDistributedComponent()
    {}

    void CAudioAdvancedDistributedComponent::toggleAudioStartStop()
    {
        if (!hasContexts()) { return; }
        const bool started = sGui->getCContextAudioBase()->isAudioStarted();
        if (started)
        {
            sGui->getCContextAudioBase()->afvClient()->disconnectFromAndStop();
        }
        else
        {
            sGui->getCContextAudioBase()->afvClient()->startAudio();
            if (sGui->getIContextNetwork()->isConnected())
            {
                const bool connected = sGui->getCContextAudioBase()->connectAudioWithNetworkCredentials();
                Q_UNUSED(connected)

                // one reason for not connecting is NOT using the VATSIM ecosystem
            }
        }

        this->setButtons(2000);
    }

    void CAudioAdvancedDistributedComponent::toggleAudioEnableDisable()
    {
        if (!hasContexts()) { return; }
        const bool enabled = sGui->getCContextAudioBase()->isAudioEnabled();
        if (enabled)
        {
            sGui->getCContextAudioBase()->disableVoiceClient();
        }
        else
        {
            sGui->getCContextAudioBase()->enableVoiceClientAndStart();
        }

        this->setButtons(2000);
    }

    void CAudioAdvancedDistributedComponent::reloadRegisteredDevices()
    {
        if (!hasContexts()) { return; }
        const CAudioDeviceInfoList registeredDevices = sGui->getIContextAudio()->getRegisteredDevices();
        ui->tvp_RegisteredDevices->updateContainerMaybeAsync(registeredDevices);
    }

    void CAudioAdvancedDistributedComponent::setButtons()
    {
        if (!hasContexts()) { return; }
        const bool started = sGui->getCContextAudioBase()->isAudioStarted();
        const bool enabled = sGui->getCContextAudioBase()->isAudioEnabled();
        ui->pb_StartStop->setText(started ? "stop" : "start");
        ui->pb_StartStop->setEnabled(enabled);
        ui->pb_EnableDisable->setText(enabled ? "disable" : "enable");
    }

    void CAudioAdvancedDistributedComponent::setButtons(int delayMs)
    {
        if (!hasContexts()) { return; }
        QPointer<CAudioAdvancedDistributedComponent> myself(this);
        QTimer::singleShot(delayMs, this, [=] {
            if (!sGui || !myself || sGui->isShuttingDown()) { return; }
            this->setButtons();
        });
    }

    void CAudioAdvancedDistributedComponent::onAudioStarted(const CAudioDeviceInfo &inputDevice, const CAudioDeviceInfo &outputDevice)
    {
        Q_UNUSED(inputDevice)
        Q_UNUSED(outputDevice)
        this->setButtons();
        this->reloadRegisteredDevices();
    }

    void CAudioAdvancedDistributedComponent::onAudioStoppend()
    {
        this->setButtons();
        this->reloadRegisteredDevices();
    }

    bool CAudioAdvancedDistributedComponent::hasContexts()
    {
        if (!sGui || sGui->isShuttingDown() || !sGui->getCContextAudioBase()) { return false; }
        if (!sGui->getIContextNetwork()) { return false; }
        return true;
    }
}
