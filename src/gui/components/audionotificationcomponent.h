// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_AUDIONOTIFICATION_COMPONENT_H
#define SWIFT_GUI_AUDIONOTIFICATION_COMPONENT_H

#include "gui/swiftguiexport.h"
#include "misc/audio/audiosettings.h"
#include "misc/audio/audiodeviceinfolist.h"
#include "misc/settingscache.h"

#include <QFrame>
#include <QCheckBox>
#include <QObject>
#include <QScopedPointer>

namespace Ui
{
    class CAudioNotificationComponent;
}
namespace swift::gui::components
{
    //! Audio component, volume, ...
    class SWIFT_GUI_EXPORT CAudioNotificationComponent : public QFrame
    {
        Q_OBJECT

    public:
        //! Constructor
        explicit CAudioNotificationComponent(QWidget *parent = nullptr);

        //! Destructor
        ~CAudioNotificationComponent() override;

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
        swift::misc::audio::CNotificationSounds::NotificationFlag checkBoxToFlag(const QCheckBox *cb) const;

        QScopedPointer<Ui::CAudioNotificationComponent> ui;
        swift::misc::CSetting<swift::misc::audio::TSettings> m_audioSettings { this, &CAudioNotificationComponent::reloadSettings };
    };
} // namespace

#endif // guard
