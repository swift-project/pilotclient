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
#include "blackcore/context_ownaircraft.h"
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
            this->setVoiceRoomUrlFieldsReadOnlyState();
            connect(ui->cb_CockpitVoiceRoom1Override, &QCheckBox::toggled, this, &CVoiceRoomsComponent::ps_onVoiceRoomOverrideChanged);
            connect(ui->cb_CockpitVoiceRoom2Override, &QCheckBox::toggled, this, &CVoiceRoomsComponent::ps_onVoiceRoomOverrideChanged);
            connect(ui->le_CockpitVoiceRoomCom1, &QLineEdit::returnPressed, this, &CVoiceRoomsComponent::ps_onVoiceRoomUrlsReturnPressed);
            connect(ui->le_CockpitVoiceRoomCom2, &QLineEdit::returnPressed, this, &CVoiceRoomsComponent::ps_onVoiceRoomUrlsReturnPressed);
        }

        CVoiceRoomsComponent::~CVoiceRoomsComponent()
        { }

        void CVoiceRoomsComponent::runtimeHasBeenSet()
        {
            this->connect(this->getIContextAudio(), &IContextAudio::changedVoiceRooms, this, &CVoiceRoomsComponent::ps_updateAudioVoiceRoomsFromContext);
            this->connect(this->getIContextAudio(), &IContextAudio::changedVoiceRoomMembers, this, &CVoiceRoomsComponent::ps_updateVoiceRoomMembers);
        }

        void CVoiceRoomsComponent::ps_onVoiceRoomOverrideChanged(bool checked)
        {
            Q_UNUSED(checked);
            this->setVoiceRoomUrlFieldsReadOnlyState();
            this->ps_onVoiceRoomUrlsReturnPressed(); // use this function to update voicerooms
        }

        void CVoiceRoomsComponent::ps_onVoiceRoomUrlsReturnPressed()
        {
            Q_ASSERT(this->getIContextOwnAircraft()); // voice room resolution is part of own aircraft
            QString url1;
            QString url2;
            if (this->ui->cb_CockpitVoiceRoom1Override->isChecked()) { url1 = this->ui->le_CockpitVoiceRoomCom1->text().trimmed(); }
            if (this->ui->cb_CockpitVoiceRoom2Override->isChecked()) { url2 = this->ui->le_CockpitVoiceRoomCom2->text().trimmed(); }
            this->getIContextOwnAircraft()->setAudioVoiceRoomOverrideUrls(url1, url2);
        }

        void CVoiceRoomsComponent::setVoiceRoomUrlFieldsReadOnlyState()
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
                if (this->getIContextAudio()) this->ui->tvp_CockpitVoiceRoom1->updateContainer(this->getIContextAudio()->getRoomUsers(BlackMisc::Aviation::CComSystem::Com1));
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
                this->ps_updateVoiceRoomMembers();

                // notify
                if (this->getIContextAudio())
                {
                    CNotificationSounds::Notification sound = connected ?
                            CNotificationSounds::NotificationVoiceRoomJoined :
                            CNotificationSounds::NotificationVoiceRoomLeft;
                    this->getIContextAudio()->playNotification(sound, true);
                }
            }
        }

        void CVoiceRoomsComponent::ps_updateVoiceRoomMembers()
        {
            if (!this->getIContextAudio()) { return; }
            if (!this->ui->le_CockpitVoiceRoomCom1->text().trimmed().isEmpty())
            {
                this->ui->tvp_CockpitVoiceRoom1->updateContainer(this->getIContextAudio()->getRoomUsers(BlackMisc::Aviation::CComSystem::Com1));
            }
            else
            {
                this->ui->tvp_CockpitVoiceRoom1->clear();
            }

            if (!this->ui->le_CockpitVoiceRoomCom2->text().trimmed().isEmpty())
            {
                this->ui->tvp_CockpitVoiceRoom2->updateContainer(this->getIContextAudio()->getRoomUsers(BlackMisc::Aviation::CComSystem::Com2));
            }
            else
            {
                this->ui->tvp_CockpitVoiceRoom2->clear();
            }
        }

    } // namespace
} // namespace

