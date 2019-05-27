/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "audiovolumecomponent.h"
#include "blackcore/context/contextaudio.h"
#include "blackgui/components/audiovolumecomponent.h"
#include "blackgui/guiapplication.h"
#include "blackmisc/audio/audioutils.h"
#include "ui_audiovolumecomponent.h"

#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>
#include <QString>
#include <QtGlobal>

using namespace BlackMisc;
using namespace BlackCore;
using namespace BlackCore::Context;

namespace BlackGui
{
    namespace Components
    {
        CAudioVolumeComponent::CAudioVolumeComponent(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CAudioVolumeComponent)
        {
            ui->setupUi(this);
            const int volume = sGui && sGui->getIContextAudio() ?
                               sGui->getIContextAudio()->getVoiceOutputVolume() :
                               100;
            ui->hs_Volume->setValue(volumeToSliderValue(volume));
            ui->sb_Volume->setValue(volume);

            bool c = connect(ui->pb_ShowWinMixer, &QPushButton::pressed, this, &CAudioVolumeComponent::onWindowsMixerRequested);
            Q_ASSERT(c);
            Q_UNUSED(c);
            c = connect(ui->hs_Volume, &QSlider::valueChanged, this, &CAudioVolumeComponent::changeOutputVolumeFromSlider);
            Q_ASSERT(c);
            Q_UNUSED(c);
            c = connect(ui->sb_Volume, qOverload<int>(&QSpinBox::valueChanged), this, &CAudioVolumeComponent::changeOutputVolumeFromSpinBox);
            Q_ASSERT(c);
            Q_UNUSED(c);
            c = connect(ui->pb_Volume100, &QPushButton::clicked, this, &CAudioVolumeComponent::setVolume100);
            Q_ASSERT(c);
            Q_UNUSED(c);

            c = connect(sGui->getIContextAudio(), &IContextAudio::changedMute, this, &CAudioVolumeComponent::onMuteChanged);
            Q_ASSERT(c);
            Q_UNUSED(c);
            connect(sGui->getIContextAudio(), &IContextAudio::changedAudioVolume, this, &CAudioVolumeComponent::onOutputVolumeChanged);
            Q_ASSERT(c);
            Q_UNUSED(c);

            // to audio audio context
            c = connect(ui->pb_Mute, &QPushButton::toggled, sGui->getIContextAudio(), &IContextAudio::setMute);
            Q_ASSERT(c);
            Q_UNUSED(c);

            if (sGui->getIContextAudio()->isUsingImplementingObject())
            {
                ui->lbl_ContextLocation->setText("local");
            }
            else
            {
                ui->lbl_ContextLocation->setText("remote");
            }

            // init volume
            this->changeOutputVolumeFromSpinBox(volume); // init volume
        }

        CAudioVolumeComponent::~CAudioVolumeComponent()
        { }

        void CAudioVolumeComponent::onMuteChanged(bool muted)
        {
            if (muted == ui->pb_Mute->isChecked()) { return; } // avoid roundtrips
            ui->pb_Mute->setChecked(muted);
        }

        void CAudioVolumeComponent::onOutputVolumeChanged(int volume)
        {
            ui->hs_Volume->setToolTip(QString::number(volume));

            // comparisons to avoid rountrips
            const QString v = QString::number(volume);
            if (volume != ui->sb_Volume->value())
            {
                ui->sb_Volume->setValue(volume);
                ui->sb_Volume->setToolTip(v);
            }

            if (volume != ui->hs_Volume->value())
            {
                ui->hs_Volume->setValue(volumeToSliderValue(volume));
                ui->hs_Volume->setToolTip(v);
            }
        }

        int CAudioVolumeComponent::volumeToSliderValue(int volume)
        {
            if (volume <= 100) { return volume; }
            const int vol = volume - 100;
            return 100 + vol / 5;
        }

        void CAudioVolumeComponent::setVolume100()
        {
            this->onOutputVolumeChanged(100);
        }

        void CAudioVolumeComponent::changeOutputVolumeFromSlider(int volume)
        {
            if (!sGui || sGui->isShuttingDown() || !sGui->getIContextAudio()) { return; }
            if (volume > 100)
            {
                // 100 -> 100, 120 -> 200, 140 -> 300
                const int v = volume - 100;
                volume = 100 + v * 5;
            }
            ui->hs_Volume->setToolTip(QString::number(volume));

            if (sGui->getIContextAudio()->getVoiceOutputVolume() != volume)
            {
                sGui->getIContextAudio()->setVoiceOutputVolume(volume);
            }
        }

        void CAudioVolumeComponent::changeOutputVolumeFromSpinBox(int volume)
        {
            if (!sGui || sGui->isShuttingDown() || !sGui->getIContextAudio()) { return; }
            ui->sb_Volume->setToolTip(QString::number(volume));
            Q_ASSERT(sGui->getIContextAudio());
            if (sGui->getIContextAudio()->getVoiceOutputVolume() != volume)
            {
                sGui->getIContextAudio()->setVoiceOutputVolume(volume);
            }
        }

        void CAudioVolumeComponent::onWindowsMixerRequested()
        {
            Audio::startWindowsMixer();
        }

    } // namespace
} // namespace
