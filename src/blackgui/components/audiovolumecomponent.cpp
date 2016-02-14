/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackgui/guiapplication.h"
#include "blackcore/contextaudio.h"
#include "blackmisc/audio/audioutils.h"
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
            Q_UNUSED(c);
            c = connect(this->ui->hs_Volume, &QSlider::valueChanged, this, &CAudioVolumeComponent::ps_changeOutputVolumeFromSlider);
            Q_ASSERT(c);
            Q_UNUSED(c);
            c = connect(this->ui->sb_Volume, static_cast<void (QSpinBox::*)(int)> (&QSpinBox::valueChanged), this, &CAudioVolumeComponent::ps_changeOutputVolumeFromSpinBox);
            Q_ASSERT(c);
            Q_UNUSED(c);
            c = connect(this->ui->pb_Volume100, &QPushButton::clicked, this, &CAudioVolumeComponent::ps_setVolume100);
            Q_ASSERT(c);
            Q_UNUSED(c);

            c = connect(sGui->getIContextAudio(), &IContextAudio::changedMute, this, &CAudioVolumeComponent::ps_onMuteChanged);
            Q_ASSERT(c);
            Q_UNUSED(c);
            connect(sGui->getIContextAudio(), &IContextAudio::changedAudioVolume, this, &CAudioVolumeComponent::ps_onOutputVolumeChanged);
            Q_ASSERT(c);
            Q_UNUSED(c);

            // to audio audio context
            c = connect(this->ui->pb_Mute, &QPushButton::toggled, sGui->getIContextAudio(), &IContextAudio::setMute);
            Q_ASSERT(c);
            Q_UNUSED(c);

            if (sGui->getIContextAudio()->isUsingImplementingObject())
            {
                this->ui->lbl_ContextLocation->setText("local");
            }
            else
            {
                this->ui->lbl_ContextLocation->setText("remote");
            }

            // init volume
            this->ps_changeOutputVolumeFromSlider(sGui->getIContextAudio()->getVoiceOutputVolume()); // init volume
        }

        CAudioVolumeComponent::~CAudioVolumeComponent()
        { }

        void CAudioVolumeComponent::ps_onMuteChanged(bool muted)
        {
            if (muted == this->ui->pb_Mute->isChecked()) { return; } // avoid roundtrips
            this->ui->pb_Mute->setChecked(muted);
        }

        void CAudioVolumeComponent::ps_onOutputVolumeChanged(int volume)
        {
            this->ui->hs_Volume->setToolTip(QString::number(volume));

            // comparisons to avoid rountrips
            QString v = QString::number(volume);
            if (volume != this->ui->sb_Volume->value())
            {
                this->ui->sb_Volume->setValue(volume);
                this->ui->sb_Volume->setToolTip(v);
            }

            if (volume > 100)
            {
                int v = volume - 100;
                volume = 100 + v / 5;
            }

            if (volume != this->ui->hs_Volume->value())
            {
                this->ui->hs_Volume->setValue(volume);
                this->ui->hs_Volume->setToolTip(v);
            }
        }

        void CAudioVolumeComponent::ps_setVolume100()
        {
            this->ps_onOutputVolumeChanged(100);
        }

        void CAudioVolumeComponent::ps_changeOutputVolumeFromSlider(int volume)
        {
            if (volume > 100)
            {
                // 100 -> 100, 120 -> 200, 140 -> 300
                int v = volume - 100;
                volume = 100 + v * 5;
            }
            this->ui->hs_Volume->setToolTip(QString::number(volume));

            Q_ASSERT(sGui->getIContextAudio());
            if (sGui->getIContextAudio()->getVoiceOutputVolume() != volume)
            {
                sGui->getIContextAudio()->setVoiceOutputVolume(volume);
            }
        }

        void CAudioVolumeComponent::ps_changeOutputVolumeFromSpinBox(int volume)
        {
            this->ui->sb_Volume->setToolTip(QString::number(volume));
            Q_ASSERT(sGui->getIContextAudio());
            if (sGui->getIContextAudio()->getVoiceOutputVolume() != volume)
            {
                sGui->getIContextAudio()->setVoiceOutputVolume(volume);
            }
        }

        void CAudioVolumeComponent::ps_onWindowsMixerRequested()
        {
            BlackMisc::Audio::startWindowsMixer();
        }

    } // namespace
} // namespace
