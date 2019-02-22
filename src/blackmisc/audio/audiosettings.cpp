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

using namespace BlackMisc::Audio;

namespace BlackMisc
{
    namespace Audio
    {
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

        QString CSettings::convertToQString(bool i18n) const
        {
            Q_UNUSED(i18n);
            return u"Notification flags: " % CNotificationSounds::toString(this->getNotification());
        }

        void CSettings::initDefaultValues()
        {
            this->setNotification(CNotificationSounds::AllNotifications);
        }
    } // namespace
} // namespace
