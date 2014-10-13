/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

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
        QString CTextMessage::asString(bool withSender, bool withRecipient, const QString &separator) const
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
         * As status message
         */
        CStatusMessage CTextMessage::asStatusMessage(bool withSender, bool withRecipient, const QString &separator) const
        {
            QString m = this->asString(withSender, withRecipient, separator);
            return { this, CStatusMessage::SeverityInfo, m };
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
         * Compare
         */
        int CTextMessage::compareImpl(const CValueObject &otherBase) const
        {
            const auto &other = static_cast<const CTextMessage &>(otherBase);
            return compare(TupleConverter<CTextMessage>::toTuple(*this), TupleConverter<CTextMessage>::toTuple(other));
        }

        /*
         * Marshall to DBus
         */
        void CTextMessage::marshallToDbus(QDBusArgument &argument) const
        {
            argument << TupleConverter<CTextMessage>::toTuple(*this);
        }

        /*
         * Unmarshall from DBus
         */
        void CTextMessage::unmarshallFromDbus(const QDBusArgument &argument)
        {
            argument >> TupleConverter<CTextMessage>::toTuple(*this);
        }

        /*
         * Hash
         */
        uint CTextMessage::getValueHash() const
        {
            return qHash(TupleConverter<CTextMessage>::toTuple(*this));
        }

        /*
         * Equal?
         */
        bool CTextMessage::operator ==(const CTextMessage &other) const
        {
            if (this == &other) return true;
            return TupleConverter<CTextMessage>::toTuple(*this) == TupleConverter<CTextMessage>::toTuple(other);
        }

        /*
         * Unequal?
         */
        bool CTextMessage::operator !=(const CTextMessage &other) const
        {
            return !((*this) == other);
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
