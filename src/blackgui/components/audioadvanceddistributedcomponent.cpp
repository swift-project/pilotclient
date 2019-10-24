/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "audioadvanceddistributedcomponent.h"
#include "ui_audioadvanceddistributedcomponent.h"

#include "blackgui/guiapplication.h"
#include "blackcore/context/contextaudio.h"
#include "blackcore/afv/clients/afvclient.h"

using namespace BlackMisc::Audio;
using namespace BlackCore::Context;

namespace BlackGui
{
    namespace Components
    {
        CAudioAdvancedDistributedComponent::CAudioAdvancedDistributedComponent(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CAudioAdvancedDistributedComponent)
        {
            ui->setupUi(this);

            connect(sGui->getCContextAudioBase(), &CContextAudioBase::startedAudio, this, &CAudioAdvancedDistributedComponent::onAudioStarted,  Qt::QueuedConnection);
            connect(sGui->getCContextAudioBase(), &CContextAudioBase::stoppedAudio, this, &CAudioAdvancedDistributedComponent::onAudioStoppend, Qt::QueuedConnection);
            connect(ui->pb_StartStop,             &QPushButton::released,           this, &CAudioAdvancedDistributedComponent::toggleAudioStartStop, Qt::QueuedConnection);

            this->setStartButton();
        }

        CAudioAdvancedDistributedComponent::~CAudioAdvancedDistributedComponent()
        { }

        void CAudioAdvancedDistributedComponent::toggleAudioStartStop()
        {
            if (!hasContexts()) { return; }
            const bool started = sGui->getCContextAudioBase()->isAudioStarted();
            if (started)
            {
                sGui->getCContextAudioBase()->afvClient()->stopAudio();
            }
            else
            {
                sGui->getCContextAudioBase()->afvClient()->startAudio();
            }
        }

        void CAudioAdvancedDistributedComponent::setStartButton()
        {
            if (!hasContexts()) { return; }
            const bool started = sGui->getCContextAudioBase()->isAudioStarted();
            ui->pb_StartStop->setText(started ? "stop" : "start");
        }

        void CAudioAdvancedDistributedComponent::onAudioStarted(const CAudioDeviceInfo &inputDevice, const CAudioDeviceInfo &outputDevice)
        {
            Q_UNUSED(inputDevice)
            Q_UNUSED(outputDevice)
            this->setStartButton();
        }

        void CAudioAdvancedDistributedComponent::onAudioStoppend()
        {
            this->setStartButton();
        }

        bool CAudioAdvancedDistributedComponent::hasContexts()
        {
            if (!sGui || sGui->isShuttingDown() || !sGui->getCContextAudioBase()) { return false; }
            if (!sGui->getCContextAudioBase()->afvClient()) { return false; }
            return true;
        }
    }
}
