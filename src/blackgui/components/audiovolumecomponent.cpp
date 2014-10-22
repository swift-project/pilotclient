/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/context_audio.h"
#include "audiovolumecomponent.h"
#include "ui_audiovolumecomponent.h"

using namespace BlackCore;

namespace BlackGui
{
    namespace Components
    {

        CAudioVolumeComponent::CAudioVolumeComponent(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CAudioVolumeComponent)
        {
            ui->setupUi(this);
            bool c = connect(this->ui->pb_ShowWinMixer, &QPushButton::pressed, this, &CAudioVolumeComponent::ps_onWindowsMixer);
            Q_ASSERT(c);
        }

        CAudioVolumeComponent::~CAudioVolumeComponent()
        { }

        void CAudioVolumeComponent::runtimeHasBeenSet()
        {
            bool c = connect(this->getIContextAudio(), &IContextAudio::changedMute, this, &CAudioVolumeComponent::ps_onMuteChanged);
            Q_ASSERT(c);
            c = connect(this->ui->pb_Mute, &QPushButton::toggled, this->getIContextAudio(), &IContextAudio::setMute);
            Q_ASSERT(c);
        }

        void CAudioVolumeComponent::ps_onMuteChanged(bool muted)
        {
            if (muted == this->ui->pb_Mute->isChecked()) { return; } // avoid roundtrips
            this->ui->pb_Mute->setChecked(muted);
        }

        void CAudioVolumeComponent::ps_onVolumeChanged(QList<qint32> volumes)
        {
            Q_ASSERT(volumes.length() == 2);
            if (volumes.length() != 2) return;
            this->ui->hs_VolumeCom1->setValue(volumes.at(0));
            this->ui->hs_VolumeCom2->setValue(volumes.at(1));
        }

        void CAudioVolumeComponent::ps_onWindowsMixer()
        {
            BlackMisc::Audio::startWindowsMixer();
        }

    } // namespace
} // namespace
