// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "audioadvanceddistributedcomponent.h"
#include "ui_audioadvanceddistributedcomponent.h"

#include "blackgui/guiapplication.h"
#include "core/context/contextaudio.h"
#include "core/context/contextnetwork.h"
#include "core/afv/clients/afvclient.h"

using namespace swift::misc::audio;
using namespace swift::core::context;

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
