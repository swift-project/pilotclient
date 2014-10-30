#include "setaudio.h"
#include "logmessage.h"
using namespace BlackSound;

namespace BlackMisc
{
    namespace Settings
    {
        /*
         * Constructor
         */
        CSettingsAudio::CSettingsAudio()
        {
            this->initDefaultValues();
        }

        /*
         * Flag
         */
        bool CSettingsAudio::getNotificationFlag(CNotificationSounds::Notification notification) const
        {
            const int i = static_cast<int>(notification);
            if (i >= m_notificationFlags.length()) return true; // default
            QChar f = m_notificationFlags.at(i);
            return '1' == f;
        }

        /*
         * Convert to string
         */
        QString CSettingsAudio::convertToQString(bool i18n) const
        {
            Q_UNUSED(i18n);
            QString s("Notification flags:");
            s.append(" ").append(m_notificationFlags);
            return s;
        }

        /*
         * Default values
         */
        void CSettingsAudio::initDefaultValues()
        {
            this->initNotificationFlags();
        }

        /*
         * Flags
         */
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

        /*
         * Value
         */
        BlackMisc::CStatusMessage CSettingsAudio::value(const QString &path, const QString &command, const CVariant &value, bool &changedFlag)
        {
            // TODO: This needs to be refactored to a smarter way to delegate commands
            changedFlag = false;
            if (path == CSettingsAudio::ValueNotificationFlag())
            {
                if (command == CSettingUtilities::CmdSetTrue() || command == CSettingUtilities::CmdSetFalse())
                {
                    CNotificationSounds::Notification index = static_cast<CNotificationSounds::Notification>(value.toInt());
                    char value = (command == CSettingUtilities::CmdSetTrue()) ? '1' : '0' ;
                    this->initNotificationFlags();
                    this->m_notificationFlags.replace(index, 1, value);
                    return {};
                }
            }
            return CLogMessage(CLogCategory::validation()).error("wrong path: %1") << path;
        }
    } // namespace
} // namespace
