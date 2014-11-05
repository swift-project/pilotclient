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
            bool c = connect(this->ui->pb_ShowWinMixer, &QPushButton::pressed, this, &CAudioVolumeComponent::ps_onWindowsMixerRequested);
            Q_ASSERT(c);

            c = connect(this->ui->hs_VolumeCom1, &QSlider::sliderReleased, this, &CAudioVolumeComponent::ps_changeVolume);
            Q_ASSERT(c);

            c = connect(this->ui->hs_VolumeCom2, &QSlider::sliderReleased, this, &CAudioVolumeComponent::ps_changeVolume);
            Q_ASSERT(c);

            Q_UNUSED(c);
        }

        CAudioVolumeComponent::~CAudioVolumeComponent()
        { }

        void CAudioVolumeComponent::runtimeHasBeenSet()
        {
            // from audio context
            bool c = connect(this->getIContextAudio(), &IContextAudio::changedMute, this, &CAudioVolumeComponent::ps_onMuteChanged);
            Q_ASSERT(c);
            connect(this->getIContextAudio(), &IContextAudio::changedAudioVolumes, this, &CAudioVolumeComponent::ps_onVolumesChanged);
            Q_ASSERT(c);

            // to audio audio context
            c = connect(this->ui->pb_Mute, &QPushButton::toggled, this->getIContextAudio(), &IContextAudio::setMute);
            Q_ASSERT(c);
        }

        void CAudioVolumeComponent::ps_onMuteChanged(bool muted)
        {
            if (muted == this->ui->pb_Mute->isChecked()) { return; } // avoid roundtrips
            this->ui->pb_Mute->setChecked(muted);
        }

        void CAudioVolumeComponent::ps_onVolumesChanged(qint32 com1Volume, qint32 com2Volume)
        {
            this->ui->hs_VolumeCom1->setValue(com1Volume);
            this->ui->hs_VolumeCom2->setValue(com2Volume);
            this->ui->hs_VolumeCom1->setToolTip(QString::number(com1Volume));
            this->ui->hs_VolumeCom2->setToolTip(QString::number(com2Volume));
        }

        void CAudioVolumeComponent::ps_changeVolume()
        {
            qint32 v1 = this->ui->hs_VolumeCom1->value();
            qint32 v2 = this->ui->hs_VolumeCom2->value();
            this->ui->hs_VolumeCom1->setToolTip(QString::number(v1));
            this->ui->hs_VolumeCom2->setToolTip(QString::number(v2));
            this->getIContextAudio()->setVolumes(v1, v2);
        }

        void CAudioVolumeComponent::ps_onWindowsMixerRequested()
        {
            BlackMisc::Audio::startWindowsMixer();
        }

    } // namespace
} // namespace
