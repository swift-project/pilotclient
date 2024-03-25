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

        connect(ui->pb_ReloadRegistered, &QPushButton::pressed, this, &CAudioAdvancedDistributedComponent::reloadRegisteredDevices, Qt::QueuedConnection);
    }

    CAudioAdvancedDistributedComponent::~CAudioAdvancedDistributedComponent()
    {}

    void CAudioAdvancedDistributedComponent::reloadRegisteredDevices()
    {
        if (!hasContexts()) { return; }
        const CAudioDeviceInfoList registeredDevices = sGui->getIContextAudio()->getRegisteredDevices();
        ui->tvp_RegisteredDevices->updateContainerMaybeAsync(registeredDevices);
    }

    bool CAudioAdvancedDistributedComponent::hasContexts()
    {
        if (!sGui || sGui->isShuttingDown() || !sGui->getCContextAudioBase()) { return false; }
        if (!sGui->getIContextNetwork()) { return false; }
        return true;
    }
}
