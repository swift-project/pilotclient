/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "ui_voiceroomscomponent.h"
#include "blackgui/components/voiceroomscomponent.h"
#include "blackgui/views/userview.h"
#include "blackgui/guiapplication.h"
#include "blackgui/guiutility.h"
#include "blackcore/context/contextaudio.h"
#include "blackcore/context/contextownaircraft.h"
#include "blackmisc/audio/notificationsounds.h"
#include "blackmisc/audio/voiceroom.h"
#include "blackmisc/aviation/comsystem.h"
#include "blackmisc/sequence.h"


#include <QCheckBox>
#include <QLineEdit>
#include <QString>
#include <QtGlobal>

using namespace BlackCore;
using namespace BlackCore::Context;
using namespace BlackMisc::Audio;

namespace BlackGui
{
    namespace Components
    {
        CVoiceRoomsComponent::CVoiceRoomsComponent(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CVoiceRoomsComponent)
        {
            Q_ASSERT_X(sGui, Q_FUNC_INFO, "Need sGui");
            ui->setupUi(this);
            this->setVoiceRoomUrlFieldsReadOnlyState();
            connect(ui->cb_CockpitVoiceRoom1Override, &QCheckBox::toggled, this, &CVoiceRoomsComponent::onVoiceRoomOverrideChanged);
            connect(ui->cb_CockpitVoiceRoom2Override, &QCheckBox::toggled, this, &CVoiceRoomsComponent::onVoiceRoomOverrideChanged);
            connect(ui->le_CockpitVoiceRoomCom1, &QLineEdit::returnPressed, this, &CVoiceRoomsComponent::onVoiceRoomUrlsReturnPressed);
            connect(ui->le_CockpitVoiceRoomCom2, &QLineEdit::returnPressed, this, &CVoiceRoomsComponent::onVoiceRoomUrlsReturnPressed);
            connect(sGui->getIContextAudio(), &IContextAudio::changedVoiceRooms, this, &CVoiceRoomsComponent::updateAudioVoiceRoomsFromContext);
            connect(sGui->getIContextAudio(), &IContextAudio::changedVoiceRoomMembers, this, &CVoiceRoomsComponent::updateVoiceRoomMembers);
        }

        CVoiceRoomsComponent::~CVoiceRoomsComponent()
        { }

        void CVoiceRoomsComponent::onVoiceRoomOverrideChanged(bool checked)
        {
            Q_UNUSED(checked);
            this->setVoiceRoomUrlFieldsReadOnlyState();
            this->onVoiceRoomUrlsReturnPressed(); // use this function to update voicerooms
        }

        void CVoiceRoomsComponent::onVoiceRoomUrlsReturnPressed()
        {
            Q_ASSERT(sGui->getIContextOwnAircraft()); // voice room resolution is part of own aircraft
            QString url1;
            QString url2;
            if (ui->cb_CockpitVoiceRoom1Override->isChecked()) { url1 = ui->le_CockpitVoiceRoomCom1->text().trimmed(); }
            if (ui->cb_CockpitVoiceRoom2Override->isChecked()) { url2 = ui->le_CockpitVoiceRoomCom2->text().trimmed(); }
            sGui->getIContextOwnAircraft()->setAudioVoiceRoomOverrideUrls(url1, url2);
        }

        void CVoiceRoomsComponent::setVoiceRoomUrlFieldsReadOnlyState()
        {
            bool c1 = ui->cb_CockpitVoiceRoom1Override->isChecked();
            bool c2 = ui->cb_CockpitVoiceRoom2Override->isChecked();
            ui->le_CockpitVoiceRoomCom1->setReadOnly(!c1);
            ui->le_CockpitVoiceRoomCom2->setReadOnly(!c2);
            CGuiUtility::forceStyleSheetUpdate(this);
        }

        void CVoiceRoomsComponent::updateAudioVoiceRoomsFromContext(const CVoiceRoomList &selectedVoiceRooms, bool connected)
        {
            Q_ASSERT(selectedVoiceRooms.size() == 2);
            CVoiceRoom room1 = selectedVoiceRooms[0];
            CVoiceRoom room2 = selectedVoiceRooms[1];

            // remark
            // isAudioPlaying() is not set, as this is only a temporary value when really "something is playing"

            bool changedUrl1 = (room1.getVoiceRoomUrl() == ui->le_CockpitVoiceRoomCom1->text());
            ui->le_CockpitVoiceRoomCom1->setText(room1.getVoiceRoomUrl());
            if (room1.isConnected())
            {
                ui->le_CockpitVoiceRoomCom1->setStyleSheet("background: green");
                if (sGui->getIContextAudio()) ui->tvp_CockpitVoiceRoom1->updateContainer(sGui->getIContextAudio()->getRoomUsers(BlackMisc::Aviation::CComSystem::Com1));
            }
            else
            {
                ui->le_CockpitVoiceRoomCom1->setStyleSheet("");
                ui->tvp_CockpitVoiceRoom1->clear();
            }

            bool changedUrl2 = (room2.getVoiceRoomUrl() == ui->le_CockpitVoiceRoomCom2->text());
            ui->le_CockpitVoiceRoomCom2->setText(room2.getVoiceRoomUrl());
            if (room2.isConnected())
            {
                ui->le_CockpitVoiceRoomCom2->setStyleSheet("background: green");
            }
            else
            {
                ui->le_CockpitVoiceRoomCom2->setStyleSheet("");
                ui->tvp_CockpitVoiceRoom2->clear();
            }
            if (changedUrl1 || changedUrl2)
            {
                this->updateVoiceRoomMembers();

                // notify
                if (sGui->getIContextAudio())
                {
                    CNotificationSounds::Notification sound = connected ?
                            CNotificationSounds::NotificationVoiceRoomJoined :
                            CNotificationSounds::NotificationVoiceRoomLeft;
                    sGui->getIContextAudio()->playNotification(sound, true);
                }
            }
        }

        void CVoiceRoomsComponent::updateVoiceRoomMembers()
        {
            if (!sGui->getIContextAudio()) { return; }
            if (!ui->le_CockpitVoiceRoomCom1->text().trimmed().isEmpty())
            {
                ui->tvp_CockpitVoiceRoom1->updateContainer(sGui->getIContextAudio()->getRoomUsers(BlackMisc::Aviation::CComSystem::Com1));
            }
            else
            {
                ui->tvp_CockpitVoiceRoom1->clear();
            }

            if (!ui->le_CockpitVoiceRoomCom2->text().trimmed().isEmpty())
            {
                ui->tvp_CockpitVoiceRoom2->updateContainer(sGui->getIContextAudio()->getRoomUsers(BlackMisc::Aviation::CComSystem::Com2));
            }
            else
            {
                ui->tvp_CockpitVoiceRoom2->clear();
            }
        }
    } // namespace
} // namespace
