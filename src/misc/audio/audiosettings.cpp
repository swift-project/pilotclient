// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/audio/audiosettings.h"

#include <QDir>
#include <QtGlobal>

#include "misc/fileutils.h"
#include "misc/swiftdirectories.h"

using namespace swift::misc::audio;

SWIFT_DEFINE_VALUEOBJECT_MIXINS(swift::misc::audio, CSettings)

namespace swift::misc::audio
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
        m_notificationVolume = std::clamp(volume, 0, 100);
    }

    void CSettings::setOutVolume(int volume)
    {
        m_outVolume = fixOutVolume(volume);
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
} // namespace swift::misc::audio
