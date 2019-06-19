/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/audio/audiosettings.h"

#include <QChar>
#include <QtGlobal>
#include <QStringBuilder>

#include "blackmisc/fileutils.h"

using namespace BlackMisc::Audio;

namespace BlackMisc
{
    namespace Audio
    {
        constexpr int CSettings::MaxAudioVolume;

        CSettings::CSettings()
        {
            this->initDefaultValues();
        }

        bool CSettings::isNotificationFlagSet(CNotificationSounds::NotificationFlag notification) const
        {
            return this->getNotification().testFlag(notification);
        }

        void CSettings::setNotificationFlag(CNotificationSounds::NotificationFlag notification, bool value)
        {
            if (value)
            {
                m_notification |= notification;
            }
            else
            {
                m_notification &= ~notification;
            }
        }

        void CSettings::setNotificationSoundDirectory(const QString &dir)
        {
            QString d = dir.trimmed();
            if (d.isEmpty())
            {
                // reset
                m_notificationSoundDir.clear();
                return;
            }
            d = CFileUtils::fixWindowsUncPath(d);
            m_notificationSoundDir = d;
        }

        void CSettings::setNotificationVolume(int volume)
        {
            m_notificationVolume = volume;
            if (m_notificationVolume < 0) { m_notificationVolume = 0; }
            else if (m_notificationVolume > 100) { m_notificationVolume = 100; }
        }

        void CSettings::setAudioVolume(int volume)
        {
            m_audioVolume = volume;
            if (m_audioVolume < 0) { m_audioVolume = 0; }
            else if (m_audioVolume > MaxAudioVolume) { m_audioVolume = MaxAudioVolume; }
        }

        QString CSettings::convertToQString(bool i18n) const
        {
            Q_UNUSED(i18n);
            return u"Notification flags: " % CNotificationSounds::toString(this->getNotification()) %
                   u" volume: " % QString::number(m_notificationVolume);
        }

        void CSettings::initDefaultValues()
        {
            this->setNotification(CNotificationSounds::AllNotifications);
            m_notificationVolume = 90;
        }
    } // namespace
} // namespace
