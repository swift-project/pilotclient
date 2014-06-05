#include "setaudio.h"
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
            QChar f = m_notificationFlags.at(static_cast<int>(notification));
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
         * metaTypeId
         */
        int CSettingsAudio::getMetaTypeId() const
        {
            return qMetaTypeId<CSettingsAudio>();
        }

        /*
         * is a
         */
        bool CSettingsAudio::isA(int metaTypeId) const
        {
            if (metaTypeId == qMetaTypeId<CSettingsAudio>()) { return true; }
            return this->CValueObject::isA(metaTypeId);
        }

        /*
         * Compare
         */
        int CSettingsAudio::compareImpl(const CValueObject &otherBase) const
        {
            const auto &other = static_cast<const CSettingsAudio &>(otherBase);
            return compare(TupleConverter<CSettingsAudio>::toTuple(*this), TupleConverter<CSettingsAudio>::toTuple(other));
        }

        /*
         * Marshall
         */
        void CSettingsAudio::marshallToDbus(QDBusArgument &argument) const
        {
            argument << TupleConverter<CSettingsAudio>::toTuple(*this);
        }

        /*
         * Unmarshall
         */
        void CSettingsAudio::unmarshallFromDbus(const QDBusArgument &argument)
        {
            argument >> TupleConverter<CSettingsAudio>::toTuple(*this);
        }

        /*
         * Equal?
         */
        bool CSettingsAudio::operator ==(const CSettingsAudio &other) const
        {
            if (this == &other) return true;
            return compare(*this, other) == 0;
        }

        /*
         * Unequal?
         */
        bool CSettingsAudio::operator !=(const CSettingsAudio &other) const
        {
            return !((*this) == other);
        }

        /*
         * Hash
         */
        uint CSettingsAudio::getValueHash() const
        {
            return qHash(TupleConverter<CSettingsAudio>::toTuple(*this));
        }

        /*
         * To JSON
         */
        QJsonObject CSettingsAudio::toJson() const
        {
            return BlackMisc::serializeJson(CSettingsAudio::jsonMembers(), TupleConverter<CSettingsAudio>::toTuple(*this));
        }

        /*
         * From JSON
         */
        void CSettingsAudio::fromJson(const QJsonObject &json)
        {
            BlackMisc::deserializeJson(json, CSettingsAudio::jsonMembers(), TupleConverter<CSettingsAudio>::toTuple(*this));
        }

        /*
         * Members
         */
        const QStringList &CSettingsAudio::jsonMembers()
        {
            return TupleConverter<CSettingsAudio>::jsonMembers();
        }

        /*
         * Default values
         */
        void CSettingsAudio::initDefaultValues()
        {
            this->m_notificationFlags = QString(1 + static_cast<int>(CNotificationSounds::Notification::NotificationsLoadSounds), '1');
        }

        /*
         * Register metadata
         */
        void CSettingsAudio::registerMetadata()
        {
            qRegisterMetaType<CSettingsAudio>();
            qDBusRegisterMetaType<CSettingsAudio>();
        }

        /*
         * Value
         */
        BlackMisc::CStatusMessageList CSettingsAudio::value(const QString &path, const QString &command, const QVariant &value, bool &changedFlag)
        {
            // TODO: This needs to be refactored to a smarter way to delegate commands
            changedFlag = false;
            CStatusMessageList msgs;
            if (path == CSettingsAudio::ValueNotificationFlag())
            {
                if (command == CSettingUtilities::CmdSetTrue() || command == CSettingUtilities::CmdSetFalse())
                {
                    CNotificationSounds::Notification index = static_cast<CNotificationSounds::Notification>(value.toInt());
                    char value = (command == CSettingUtilities::CmdSetTrue()) ? '1' : '0' ;
                    this->m_notificationFlags.replace(index, 1, value);
                    return msgs;
                }
            }
            return CSettingUtilities::wrongPathMessages(path);
        }
    } // namespace
} // namespace
