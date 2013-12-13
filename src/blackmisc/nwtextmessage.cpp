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
                s.append(" ").append(this->m_fromCallsign.toQString(i18n));
                s.append(" ").append(this->m_toCallsign.toQString(i18n));
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
            argument << this->m_fromCallsign;
            argument << this->m_toCallsign;
            argument << this->m_message;
            argument << this->m_frequency;
        }

        /*
         * Unmarshall from DBus
         */
        void CTextMessage::unmarshallFromDbus(const QDBusArgument &argument)
        {
            argument >> this->m_fromCallsign;
            argument >> this->m_toCallsign;
            argument >> this->m_message;
            argument >> this->m_frequency;
        }

        /*
         * Private message?
         */
        bool CTextMessage::isPrivateMessage() const
        {
            return !this->m_fromCallsign.isEmpty() && !this->m_toCallsign.isEmpty();
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
        bool CTextMessage::hasValidReceiver() const
        {
            if (!this->m_toCallsign.isEmpty()) return true;
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
        QString CTextMessage::asString(bool withFrom, bool withTo, const QString separator) const
        {
            QString s;
            if (withFrom)
            {
                if (!this->m_fromCallsign.isEmpty())
                    s.append(this->m_fromCallsign.getStringAsSet());
            }

            if (withTo)
            {
                if (!this->m_toCallsign.isEmpty())
                {
                    if (!s.isEmpty()) s.append(separator);
                    s.append(this->m_toCallsign.getStringAsSet());
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
        void CTextMessage::toggleSenderReceiver()
        {
            BlackMisc::Aviation::CCallsign csOldFrom(this->getFromCallsign());
            this->setFromCallsign(this->getToCallsign());
            this->setToCallsign(csOldFrom);
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
            hashs << qHash(this->m_fromCallsign);
            hashs << qHash(this->m_toCallsign);
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
