/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_AUDIONOTIFICATION_COMPONENT_H
#define BLACKGUI_AUDIONOTIFICATION_COMPONENT_H

#include "blackgui/blackguiexport.h"
#include "blackmisc/audio/audiosettings.h"
#include "blackmisc/audio/audiodeviceinfolist.h"
#include "blackmisc/settingscache.h"

#include <QFrame>
#include <QCheckBox>
#include <QObject>
#include <QScopedPointer>

namespace Ui
{
    class CAudioNotificationComponent;
}
namespace BlackGui::Components
{
    //! Audio component, volume, ...
    class BLACKGUI_EXPORT CAudioNotificationComponent : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CAudioNotificationComponent(QWidget *parent = nullptr);

        //! Destructor
        virtual ~CAudioNotificationComponent() override;

        //! Play any sounds?
        bool playNotificationSounds() const;

    private:
        //! Init
        void init();

        //! Reload settings
        void reloadSettings();

        //! Notification flags toggled
        void onNotificationsToggled(bool checked);

        //! Notification sounds dir
        void selectNotificationSoundsDir();

        //! Notification sounds dir
        void resetNotificationSoundsDir();

        //! Volume has been changed
        void onNotificationVolumeChanged(int volume);

        //! CheckBox to flag
        BlackMisc::Audio::CNotificationSounds::NotificationFlag checkBoxToFlag(const QCheckBox *cb) const;

        QScopedPointer<Ui::CAudioNotificationComponent> ui;
        BlackMisc::CSetting<BlackMisc::Audio::TSettings> m_audioSettings { this, &CAudioNotificationComponent::reloadSettings };
    };
} // namespace

#endif // guard
