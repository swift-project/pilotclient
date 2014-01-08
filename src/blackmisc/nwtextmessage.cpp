#include "nwtextmessage.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include "blackmisc/pqconstants.h"
#include "blackmisc/aviocomsystem.h"

namespace BlackMisc
{
    namespace Network
    {
        /*
         * Convert to string
         */
        QString CTextMessage::convertToQString(bool i18n) const
        {
            QString s(this->m_message);
            if (this->isPrivateMessage())
            {
                s.append(" ").append(this->m_sender.toQString(i18n));
                s.append(" ").append(this->m_recipient.toQString(i18n));
            }
            else
            {
                s.append(" ").append(this->m_frequency.toQString(i18n));
            }
            return s;
        }

        /*
         * Marshall to DBus
         */
        void CTextMessage::marshallToDbus(QDBusArgument &argument) const
        {
            argument << this->m_sender;
            argument << this->m_recipient;
            argument << this->m_message;
            argument << this->m_frequency;
        }

        /*
         * Unmarshall from DBus
         */
        void CTextMessage::unmarshallFromDbus(const QDBusArgument &argument)
        {
            argument >> this->m_sender;
            argument >> this->m_recipient;
            argument >> this->m_message;
            argument >> this->m_frequency;
        }

        /*
         * Private message?
         */
        bool CTextMessage::isPrivateMessage() const
        {
            return !this->m_sender.isEmpty() && !this->m_recipient.isEmpty();
        }

        /*
         * Sent to frequency?
         */
        bool CTextMessage::isSendToFrequency(const PhysicalQuantities::CFrequency &frequency) const
        {
            if (!this->isRadioMessage()) return false;
            return this->m_frequency == frequency;
        }

        /*
         * Sent to UNICOM?
         */
        bool CTextMessage::isSendToUnicom() const
        {
            return this->isSendToFrequency(BlackMisc::PhysicalQuantities::CPhysicalQuantitiesConstants::FrequencyUnicom());
        }

        /*
         * Valid receiver?
         */
        bool CTextMessage::hasValidRecipient() const
        {
            if (!this->m_recipient.isEmpty()) return true;
            return BlackMisc::Aviation::CComSystem::isValidCivilAviationFrequency(this->m_frequency);
        }

        /*
         * Radio message?
         */
        bool CTextMessage::isRadioMessage() const
        {
            return (BlackMisc::Aviation::CComSystem::isValidCivilAviationFrequency(this->m_frequency));
        }

        /*
         * Formatted string
         */
        QString CTextMessage::asString(bool withSender, bool withRecipient, const QString separator) const
        {
            QString s;
            if (withSender)
            {
                if (!this->m_sender.isEmpty())
                {
                    s.append(this->m_sender.getStringAsSet());
                }
            }

            if (withRecipient)
            {
                if (!this->m_recipient.isEmpty())
                {
                    if (!s.isEmpty()) s.append(separator);
                    s.append(this->m_recipient.getStringAsSet());
                }
                else
                {
                    if (BlackMisc::Aviation::CComSystem::isValidCivilAviationFrequency(this->m_frequency))
                    {
                        if (!s.isEmpty()) s.append(separator);
                        s.append(this->m_frequency.valueRoundedWithUnit(3, true));
                    }
                }
            }

            if (this->m_message.isEmpty()) return s;
            if (!s.isEmpty()) s.append(separator);
            s.append(this->m_message);
            return s;
        }

        /*
         * Toggle sender / receiver
         */
        void CTextMessage::toggleSenderRecipient()
        {
            qSwap(this->m_sender, this->m_recipient);
        }

        /*
         * Equal?
         */
        bool CTextMessage::operator ==(const CTextMessage &other) const
        {
            if (this == &other) return true;
            return this->getValueHash() == other.getValueHash();
        }

        /*
         * Unequal?
         */
        bool CTextMessage::operator !=(const CTextMessage &other) const
        {
            return !((*this) == other);
        }

        /*
         * Hash
         */
        uint CTextMessage::getValueHash() const
        {
            QList<uint> hashs;
            hashs << qHash(this->m_sender);
            hashs << qHash(this->m_recipient);
            hashs << qHash(this->m_frequency);
            hashs << qHash(this->m_message);
            return BlackMisc::calculateHash(hashs, "CTextMessage");
        }

        /*
         * Register metadata
         */
        void CTextMessage::registerMetadata()
        {
            qRegisterMetaType<CTextMessage>();
            qDBusRegisterMetaType<CTextMessage>();
        }

    } // namespace
} // namespace
