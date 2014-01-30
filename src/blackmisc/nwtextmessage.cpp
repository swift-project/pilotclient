#include "nwtextmessage.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include "blackmisc/pqconstants.h"
#include "blackmisc/aviocomsystem.h"
#include "blackmisc/avcallsign.h"
#include "blackmisc/avselcal.h"

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
         * metaTypeId
         */
        int CTextMessage::getMetaTypeId() const
        {
            return qMetaTypeId<CTextMessage>();
        }

        /*
         * is a
         */
        bool CTextMessage::isA(int metaTypeId) const
        {
            if (metaTypeId == qMetaTypeId<CTextMessage>()) { return true; }

            return this->CValueObject::isA(metaTypeId);
        }

        /*
         * Compare
         */
        int CTextMessage::compareImpl(const CValueObject &otherBase) const
        {
            const auto &other = static_cast<const CTextMessage &>(otherBase);

            return this->m_message.compare(other.m_message);
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
            qSwap(this->m_senderCallsign, this->m_recipientCallsign);
        }

        /*
         * Find out if this is a SELCAL message
         */
        bool CTextMessage::isSelcalMessage() const
        {
            // some first level checks, before really parsing the message
            if (this->isEmpty()) return false;
            if (this->isPrivateMessage()) return false;
            if (this->m_message.length() > 15 || this->m_message.length() < 10) return false; // SELCAL AB-CD -> 12, I allow some more characters as I do not know wheter in real life it exactly matches
            return this->getSelcalCode().length() == 4;
        }

        /*
         *  Matching given SELCAL code
         */
        bool CTextMessage::isSelcalMessageFor(const QString &selcal) const
        {
            if (!CSelcal::isValidCode(selcal)) return false;
            return selcal.toUpper() ==  this->getSelcalCode();
        }

        /*
         * SELCAL code, 4 letters
         */
        QString CTextMessage::getSelcalCode() const
        {
            // http://forums.vatsim.net/viewtopic.php?f=8&t=63467#p458062
            // When the ATC client sends a selcal, it goes out as a text message
            // on their primary frequency, formatted like so:
            // SELCAL AB-CD

            const QString invalid;

            // some first level checks, before really parsing the message
            if (this->isEmpty()) return invalid;
            if (this->isPrivateMessage()) return invalid;
            if (this->m_message.length() > 15 || this->m_message.length() < 10) return invalid; // SELCAL AB-CD -> 12, I allow some more characters as I do not know wheter in real life it exactly matches
            QString candidate = this->m_message.toUpper().remove(QRegExp("[^A-Z]")); // SELCALABCD
            if (candidate.length() != 10) return invalid;
            if (!candidate.startsWith("SELCAL")) return invalid;
            return candidate.right(4);
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
