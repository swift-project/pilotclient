/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "voiceroomscomponent.h"
#include "ui_voiceroomscomponent.h"
#include "blackcore/context_audio.h"
#include "blackmisc/notificationsounds.h"

using namespace BlackCore;
using namespace BlackSound;
using namespace BlackMisc::Audio;


namespace BlackGui
{
    namespace Components
    {

        CVoiceRoomsComponent::CVoiceRoomsComponent(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CVoiceRoomsComponent)
        {
            ui->setupUi(this);
            this->setVoiceRoomUrlFields();
            connect(ui->cb_CockpitVoiceRoom1Override, &QCheckBox::toggled, this, &CVoiceRoomsComponent::ps_voiceRoomOverrideChanged);
            connect(ui->le_CockpitVoiceRoomCom1, &QLineEdit::returnPressed, this, &CVoiceRoomsComponent::ps_voiceRoomUrlsReturnPressed);
        }

        CVoiceRoomsComponent::~CVoiceRoomsComponent()
        { }

        void CVoiceRoomsComponent::runtimeHasBeenSet()
        {
            this->connect(this->getIContextAudio(), &IContextAudio::changedVoiceRooms, this, &CVoiceRoomsComponent::ps_updateAudioVoiceRoomsFromContext);
        }

        void CVoiceRoomsComponent::ps_voiceRoomOverrideChanged(bool checked)
        {
            Q_UNUSED(checked);
            this->setVoiceRoomUrlFields();
        }

        void CVoiceRoomsComponent::ps_voiceRoomUrlsReturnPressed()
        {

        }

        void CVoiceRoomsComponent::setVoiceRoomUrlFields()
        {
            bool c1 = ui->cb_CockpitVoiceRoom1Override->isChecked();
            bool c2 = ui->cb_CockpitVoiceRoom2Override->isChecked();
            this->ui->le_CockpitVoiceRoomCom1->setReadOnly(!c1);
            this->ui->le_CockpitVoiceRoomCom2->setReadOnly(!c2);
        }

        void CVoiceRoomsComponent::ps_updateAudioVoiceRoomsFromContext(const CVoiceRoomList &selectedVoiceRooms, bool connected)
        {
            Q_ASSERT(selectedVoiceRooms.size() == 2);
            CVoiceRoom room1 = selectedVoiceRooms[0];
            CVoiceRoom room2 = selectedVoiceRooms[1];

            // remark
            // isAudioPlaying() is not set, as this is only a temporary value when really "something is playing"

            bool changedUrl1 = (room1.getVoiceRoomUrl() == this->ui->le_CockpitVoiceRoomCom1->text());
            this->ui->le_CockpitVoiceRoomCom1->setText(room1.getVoiceRoomUrl());
            if (room1.isConnected())
            {
                this->ui->le_CockpitVoiceRoomCom1->setStyleSheet("background: green");
                if (this->getIContextAudio()) this->ui->tvp_CockpitVoiceRoom1->updateContainer(this->getIContextAudio()->getCom1RoomUsers());
            }
            else
            {
                this->ui->le_CockpitVoiceRoomCom1->setStyleSheet("");
                this->ui->tvp_CockpitVoiceRoom1->clear();
            }

            bool changedUrl2 = (room2.getVoiceRoomUrl() == this->ui->le_CockpitVoiceRoomCom2->text());
            this->ui->le_CockpitVoiceRoomCom2->setText(room2.getVoiceRoomUrl());
            if (room2.isConnected())
            {
                this->ui->le_CockpitVoiceRoomCom2->setStyleSheet("background: green");
            }
            else
            {
                this->ui->le_CockpitVoiceRoomCom2->setStyleSheet("");
                this->ui->tvp_CockpitVoiceRoom2->clear();
            }
            if (changedUrl1 || changedUrl2)
            {
                this->updateVoiceRoomMembers();

                // notify
                if (this->getIContextAudio())
                {
                    CNotificationSounds::Notification sound = connected ?
                            CNotificationSounds::NotificationVoiceRoomJoined :
                            CNotificationSounds::NotificationVoiceRoomLeft;
                    this->getIContextAudio()->playNotification(static_cast<uint>(sound), true);
                }
            }
        }

        void CVoiceRoomsComponent::updateVoiceRoomMembers()
        {
            if (!this->getIContextAudio()) return;
            if (!this->ui->le_CockpitVoiceRoomCom1->text().trimmed().isEmpty())
            {
                this->ui->tvp_CockpitVoiceRoom1->updateContainer(this->getIContextAudio()->getCom1RoomUsers());
            }
            else
            {
                this->ui->tvp_CockpitVoiceRoom1->clear();
            }

            if (!this->ui->le_CockpitVoiceRoomCom2->text().trimmed().isEmpty())
            {
                this->ui->tvp_CockpitVoiceRoom2->updateContainer(this->getIContextAudio()->getCom2RoomUsers());
            }
            else
            {
                this->ui->tvp_CockpitVoiceRoom2->clear();
            }
        }

    } // namespace
} // namespace

