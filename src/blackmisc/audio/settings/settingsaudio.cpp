/* Copyright (C) 2013
 * swift Project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "settingsaudio.h"
#include "logmessage.h"

using namespace BlackMisc::Audio;

namespace BlackMisc
{
    namespace Audio
    {
        namespace Settings
        {
            CSettingsAudio::CSettingsAudio()
            {
                this->initDefaultValues();
            }

            bool CSettingsAudio::getNotificationFlag(CNotificationSounds::Notification notification) const
            {
                const int i = static_cast<int>(notification);
                if (i >= m_notificationFlags.length()) return true; // default
                QChar f = m_notificationFlags.at(i);
                return '1' == f;
            }

            QString CSettingsAudio::convertToQString(bool i18n) const
            {
                Q_UNUSED(i18n);
                QString s("Notification flags:");
                s.append(" ").append(m_notificationFlags);
                return s;
            }

            void CSettingsAudio::initDefaultValues()
            {
                this->initNotificationFlags();
            }

            void CSettingsAudio::initNotificationFlags()
            {
                // if we add flags in the future, we automatically extend ...
                static const int l = 1 + static_cast<int>(CNotificationSounds::Notification::NotificationsLoadSounds);
                if (this->m_notificationFlags.length() < l)
                {
                    int cl = m_notificationFlags.length();
                    this->m_notificationFlags.append(QString(l - cl, '1'));
                }
            }

        } // namespace
    } // namespace
} // namespace
