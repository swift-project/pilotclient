/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_AUDIOSETUPCOMPONENT_H
#define BLACKGUI_AUDIOSETUPCOMPONENT_H

#include "blackgui/blackguiexport.h"
#include "blackmisc/audio/audiosettings.h"
#include "blackmisc/audio/audiodeviceinfolist.h"
#include "blackmisc/settingscache.h"

#include <QFrame>
#include <QCheckBox>
#include <QObject>
#include <QScopedPointer>

namespace Ui { class CAudioSetupComponent; }
namespace BlackGui
{
    namespace Components
    {
        //! Audio setup such as input / output devices
        class BLACKGUI_EXPORT CAudioSetupComponent : public QFrame
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CAudioSetupComponent(QWidget *parent = nullptr);

            //! Destructor
            virtual ~CAudioSetupComponent() override;

            //! Play any sounds?
            bool playNotificationSounds() const;

        private:
            //! Init
            void init();

            //! Reload settings
            void reloadSettings();

            //! Audio device selected
            //! \param index audio device index (COM1, COM2)
            void onAudioDeviceSelected(int index);

            //! Current audio devices changed
            void onCurrentAudioDevicesChanged(const BlackMisc::Audio::CAudioDeviceInfoList &devices);

            //! Audio devices changed
            void onAudioDevicesChanged(const BlackMisc::Audio::CAudioDeviceInfoList &devices);

            //! Loopback toggled
            void onLoopbackToggled(bool loopback);

            //! Notification flags toggled
            void onNotificationsToggled(bool checked);

            //! Notification sounds dir
            void selectNotificationSoundsDir();

            //! Notification sounds dir
            void resetNotificationSoundsDir();

            //! Audio device lists from settings
            void initAudioDeviceLists();

            //! Audio is optional, check if available
            bool hasAudio() const;

            //! Volume has been changed
            void onNotificationVolumeChanged(int volume);

            //! CheckBox to flag
            BlackMisc::Audio::CNotificationSounds::NotificationFlag checkBoxToFlag(const QCheckBox *cb) const;

            QScopedPointer<Ui::CAudioSetupComponent> ui;
            BlackMisc::CSetting<BlackMisc::Audio::TSettings> m_audioSettings { this, &CAudioSetupComponent::reloadSettings };
        };
    } // namespace
} // namespace

#endif // guard
