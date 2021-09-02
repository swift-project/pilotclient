/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/audio/audiosettings.h"
#include "blackmisc/swiftdirectories.h"
#include "blackmisc/fileutils.h"
#include <QChar>
#include <QtGlobal>
#include <QStringBuilder>
#include <QDir>

using namespace BlackMisc::Audio;

namespace BlackMisc
{
    namespace Audio
    {
        int CSettings::fixOutVolume(int v)
        {
            if (v > OutMax) { return OutMax; }
            if (v < OutMin) { return OutMin; }
            return v;
        }

        int CSettings::fixInVolume(int v)
        {
            if (v > InMax) { return InMax; }
            if (v < InMin) { return InMin; }
            return v;
        }

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

        const QString &CSettings::getNotificationSoundDirectoryOrDefault() const
        {
            if (!m_notificationSoundDir.isEmpty())
            {
                const QDir d(m_notificationSoundDir);
                if (d.exists()) { return m_notificationSoundDir; }
            }
            return CSwiftDirectories::soundFilesDirectory();
        }

        QString CSettings::getNotificationFilePath(const QString &fileName) const
        {
            if (fileName.isEmpty()) { return {}; }
            return CSwiftDirectories::soundFilePathOrDefaultPath(m_notificationSoundDir, fileName);
        }

        void CSettings::setNotificationVolume(int volume)
        {
            m_notificationVolume = volume;
            if (m_notificationVolume < 0) { m_notificationVolume = 0; }
            else if (m_notificationVolume > 100) { m_notificationVolume = 100; }
        }

        void CSettings::setOutVolumeCom1(int volume)
        {
            m_outVolumeCom1 = fixOutVolume(volume);
        }

        void CSettings::setOutVolumeCom2(int volume)
        {
            m_outVolumeCom2 = fixOutVolume(volume);
        }

        void CSettings::setInVolume(int volume)
        {
            m_inVolume = fixInVolume(volume);
        }

        QString CSettings::convertToQString(bool i18n) const
        {
            Q_UNUSED(i18n)
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
