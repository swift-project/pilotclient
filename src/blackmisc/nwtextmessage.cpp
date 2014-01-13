#include "nwtextmessage.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include "blackmisc/pqconstants.h"
#include "blackmisc/aviocomsystem.h"
#include "blackmisc/avcallsign.h"

using namespace BlackMisc::Aviation;

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
                s.append(" ").append(this->m_senderCallsign.toQString(i18n));
                s.append(" ").append(this->m_recipientCallsign.toQString(i18n));
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
            argument << this->m_senderCallsign;
            argument << this->m_recipientCallsign;
            argument << this->m_message;
            argument << this->m_frequency;
            argument << this->m_received;
        }

        /*
         * Unmarshall from DBus
         */
        void CTextMessage::unmarshallFromDbus(const QDBusArgument &argument)
        {
            argument >> this->m_senderCallsign;
            argument >> this->m_recipientCallsign;
            argument >> this->m_message;
            argument >> this->m_frequency;
            argument >> this->m_received;
        }

        /*
         * Private message?
         */
        bool CTextMessage::isPrivateMessage() const
        {
            return !this->m_senderCallsign.isEmpty() && !this->m_recipientCallsign.isEmpty();
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
            if (!this->m_recipientCallsign.isEmpty()) return true;
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
         * Initial server message?
         */
        bool CTextMessage::isServerMessage() const
        {
            if (!this->isPrivateMessage()) return false;
            CCallsign cs = this->getSenderCallsign();
            return (cs.asString().startsWith("SERVER", Qt::CaseInsensitive));
        }

        /*
         * Formatted string
         */
        QString CTextMessage::asString(bool withSender, bool withRecipient, const QString separator) const
        {
            QString s = this->receivedTime();
            if (withSender)
            {
                if (!this->m_senderCallsign.isEmpty())
                {
                    if (!s.isEmpty()) s.append(separator);
                    s.append(this->m_senderCallsign.getStringAsSet());
                }
            }

            if (withRecipient)
            {
                if (!this->m_recipientCallsign.isEmpty())
                {
                    if (!s.isEmpty()) s.append(separator);
                    s.append(this->m_recipientCallsign.getStringAsSet());
                }
                else
                {
                    if (BlackMisc::Aviation::CComSystem::isValidCivilAviationFrequency(this->m_frequency))
                    {
                        if (!s.isEmpty()) s.append(separator);
                        s.append(this->m_frequency.valueRoundedWithUnit(3, true));
                    }

                }
            } // to

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
            BlackMisc::Aviation::CCallsign csOldFrom(this->getSenderCallsign());
            this->setSenderCallsign(this->getRecipientCallsign());
            this->setRecipientCallsign(csOldFrom);
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
            hashs << qHash(this->m_senderCallsign.getValueHash());
            hashs << qHash(this->m_recipientCallsign.getValueHash());
            hashs << qHash(this->m_frequency.getValueHash());
            hashs << qHash(this->m_message);
            hashs << qHash(this->m_received);
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
