/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/network/textmessage.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include "blackmisc/pq/constants.h"
#include "blackmisc/aviation/comsystem.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/aviation/selcal.h"

using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackMisc
{
    namespace Network
    {

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

        bool CTextMessage::isPrivateMessage() const
        {
            return !this->m_senderCallsign.isEmpty() && !this->m_recipientCallsign.isEmpty();
        }

        bool CTextMessage::isSupervisorMessage() const
        {
            return this->m_senderCallsign.isSupervisorCallsign();
        }

        bool CTextMessage::wasSent() const
        {
            return m_wasSent;
        }

        void CTextMessage::markAsSent()
        {
            m_wasSent = true;
        }

        QString CTextMessage::getRecipientCallsignOrFrequency() const
        {
            if (!this->m_recipientCallsign.isEmpty()) { return m_recipientCallsign.asString(); }
            if (this->m_frequency.isNull()) { return ""; }
            return this->m_frequency.valueRoundedWithUnit(CFrequencyUnit::MHz(), 3);
        }

        bool CTextMessage::isSendToFrequency(const PhysicalQuantities::CFrequency &frequency) const
        {
            if (!this->isRadioMessage()) { return false; }
            return this->m_frequency == frequency;
        }

        bool CTextMessage::isSendToUnicom() const
        {
            return this->isSendToFrequency(BlackMisc::PhysicalQuantities::CPhysicalQuantitiesConstants::FrequencyUnicom());
        }

        bool CTextMessage::hasValidRecipient() const
        {
            if (!this->m_recipientCallsign.isEmpty()) return true;
            return BlackMisc::Aviation::CComSystem::isValidCivilAviationFrequency(this->m_frequency);
        }

        bool CTextMessage::isRadioMessage() const
        {
            return (BlackMisc::Aviation::CComSystem::isValidCivilAviationFrequency(this->m_frequency));
        }

        bool CTextMessage::isServerMessage() const
        {
            if (!this->isPrivateMessage()) return false;
            CCallsign cs = this->getSenderCallsign();
            return (cs.asString().startsWith("SERVER", Qt::CaseInsensitive));
        }

        QString CTextMessage::asString(bool withSender, bool withRecipient, const QString &separator) const
        {
            QString s(this->getFormattedUtcTimestampHms());
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

        CStatusMessage CTextMessage::asStatusMessage(bool withSender, bool withRecipient, const QString &separator) const
        {
            QString m = this->asString(withSender, withRecipient, separator);
            return { this, CStatusMessage::SeverityInfo, m };
        }

        void CTextMessage::toggleSenderRecipient()
        {
            qSwap(this->m_senderCallsign, this->m_recipientCallsign);
        }

        bool CTextMessage::isSelcalMessage() const
        {
            // some first level checks, before really parsing the message
            if (this->isEmpty()) return false;
            if (this->isPrivateMessage()) return false;
            if (this->m_message.length() > 15 || this->m_message.length() < 10) return false; // SELCAL AB-CD -> 12, I allow some more characters as I do not know wheter in real life it exactly matches
            return this->getSelcalCode().length() == 4;
        }

        bool CTextMessage::isSelcalMessageFor(const QString &selcal) const
        {
            if (!CSelcal::isValidCode(selcal)) return false;
            return selcal.toUpper() ==  this->getSelcalCode();
        }

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
            return candidate.right(4).toUpper();
        }

        CIcon CTextMessage::toIcon() const
        {
            return m_senderCallsign.toIcon();
        }

        QPixmap CTextMessage::toPixmap() const
        {
            return m_senderCallsign.toPixmap();
        }

        CVariant CTextMessage::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return this->toCVariant(); }
            if (ITimestampBased::canHandleIndex(index)) { return ITimestampBased::propertyByIndex(index); }

            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexSenderCallsign:
                return this->m_senderCallsign.propertyByIndex(index.copyFrontRemoved());
            case IndexRecipientCallsign:
                return this->m_recipientCallsign.propertyByIndex(index.copyFrontRemoved());
            case IndexRecipientCallsignOrFrequency:
                return CVariant::fromValue(this->getRecipientCallsignOrFrequency());
            case IndexMessage:
                return CVariant::fromValue(this->m_message);
            default:
                return CValueObject::propertyByIndex(index);
            }
        }

        void CTextMessage::setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index)
        {
            if (index.isMyself())
            {
                this->convertFromCVariant(variant);
                return;
            }
            if (ITimestampBased::canHandleIndex(index)) { ITimestampBased::setPropertyByIndex(variant, index); return; }

            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexSenderCallsign:
                this->m_senderCallsign.setPropertyByIndex(variant, index.copyFrontRemoved());
                break;
            case IndexRecipientCallsign:
                this->m_recipientCallsign.setPropertyByIndex(variant, index.copyFrontRemoved());
                break;
            case IndexMessage:
                this->m_message = variant.value<QString>();
                break;
            default:
                CValueObject::setPropertyByIndex(variant, index);
                break;
            }
        }

    } // namespace
} // namespace
