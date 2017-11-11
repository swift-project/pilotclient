/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/network/textmessage.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/aviation/comsystem.h"
#include "blackmisc/aviation/selcal.h"
#include "blackmisc/pq/constants.h"
#include "blackmisc/pq/physicalquantity.h"
#include "blackmisc/stringutils.h"

#include <Qt>
#include <QtGlobal>

using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;

namespace BlackMisc
{
    namespace Network
    {

        QString CTextMessage::convertToQString(bool i18n) const
        {
            QString s(m_message);
            if (this->isPrivateMessage())
            {
                s.append(" ").append(m_senderCallsign.toQString(i18n));
                s.append(" ").append(m_recipientCallsign.toQString(i18n));
            }
            else
            {
                s.append(" ").append(m_frequency.toQString(i18n));
            }
            return s;
        }

        bool CTextMessage::isPrivateMessage() const
        {
            return !m_senderCallsign.isEmpty() && !m_recipientCallsign.isEmpty();
        }

        bool CTextMessage::isSupervisorMessage() const
        {
            return m_senderCallsign.isSupervisorCallsign();
        }

        bool CTextMessage::wasSent() const
        {
            return m_wasSent;
        }

        void CTextMessage::markAsSent()
        {
            m_wasSent = true;
            if (!this->hasValidTimestamp())
            {
                this->setCurrentUtcTime();
            }
        }

        QString CTextMessage::getRecipientCallsignOrFrequency() const
        {
            if (!m_recipientCallsign.isEmpty()) { return m_recipientCallsign.asString(); }
            if (m_frequency.isNull()) { return ""; }
            return m_frequency.valueRoundedWithUnit(CFrequencyUnit::MHz(), 3);
        }

        bool CTextMessage::isSendToFrequency(const PhysicalQuantities::CFrequency &frequency) const
        {
            if (!this->isRadioMessage()) { return false; }
            return m_frequency == frequency;
        }

        bool CTextMessage::isSendToUnicom() const
        {
            return this->isSendToFrequency(BlackMisc::PhysicalQuantities::CPhysicalQuantitiesConstants::FrequencyUnicom());
        }

        bool CTextMessage::hasValidRecipient() const
        {
            if (!m_recipientCallsign.isEmpty()) return true;
            return CComSystem::isValidCivilAviationFrequency(m_frequency);
        }

        bool CTextMessage::isRadioMessage() const
        {
            return (CComSystem::isValidCivilAviationFrequency(m_frequency));
        }

        bool CTextMessage::isServerMessage() const
        {
            if (!this->isPrivateMessage()) return false;
            const CCallsign cs = this->getSenderCallsign();
            return (cs.asString().startsWith("SERVER", Qt::CaseInsensitive));
        }

        QString CTextMessage::asString(bool withSender, bool withRecipient, const QString &separator) const
        {
            QString s(this->getFormattedUtcTimestampHms());
            if (withSender)
            {
                if (!m_senderCallsign.isEmpty())
                {
                    if (!s.isEmpty()) s.append(separator);
                    s.append(m_senderCallsign.getStringAsSet());
                }
            }

            if (withRecipient)
            {
                if (!m_recipientCallsign.isEmpty())
                {
                    if (!s.isEmpty()) s.append(separator);
                    s.append(m_recipientCallsign.getStringAsSet());
                }
                else
                {
                    if (CComSystem::isValidCivilAviationFrequency(m_frequency))
                    {
                        if (!s.isEmpty()) s.append(separator);
                        s.append(m_frequency.valueRoundedWithUnit(3, true));
                    }

                }
            } // to

            if (m_message.isEmpty()) return s;
            if (!s.isEmpty()) s.append(separator);
            s.append(m_message);
            return s;
        }

        CStatusMessage CTextMessage::asStatusMessage(bool withSender, bool withRecipient, const QString &separator) const
        {
            QString m = this->asString(withSender, withRecipient, separator);
            return { this, CStatusMessage::SeverityInfo, m };
        }

        void CTextMessage::toggleSenderRecipient()
        {
            qSwap(m_senderCallsign, m_recipientCallsign);
        }

        bool CTextMessage::isSelcalMessage() const
        {
            // some first level checks, before really parsing the message
            if (this->isEmpty()) return false;
            if (this->isPrivateMessage()) return false;
            if (m_message.length() > 15 || m_message.length() < 10) return false; // SELCAL AB-CD -> 12, I allow some more characters as I do not know wheter in real life it exactly matches
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

            if (this->isEmpty()) return {};
            if (this->isPrivateMessage()) return {};
            if (!m_message.startsWith(QLatin1String("SELCAL"), Qt::CaseInsensitive)) return {};
            if (m_message.length() > 15 || m_message.length() < 10) return {}; // SELCAL AB-CD -> 12, I allow some more characters as I do not know wheter in real life it exactly matches
            QString candidate = removeChars(m_message, [](QChar c) { return !c.isLetter(); }); // SELCALABCD
            if (candidate.length() != 10) return {};
            return std::move(candidate).right(4).toUpper();
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
            if (index.isMyself()) { return CVariant::from(*this); }
            if (ITimestampBased::canHandleIndex(index)) { return ITimestampBased::propertyByIndex(index); }

            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexSenderCallsign: return m_senderCallsign.propertyByIndex(index.copyFrontRemoved());
            case IndexRecipientCallsign: return m_recipientCallsign.propertyByIndex(index.copyFrontRemoved());
            case IndexRecipientCallsignOrFrequency: return CVariant::fromValue(this->getRecipientCallsignOrFrequency());
            case IndexMessage: return CVariant::fromValue(m_message);
            default: return CValueObject::propertyByIndex(index);
            }
        }

        void CTextMessage::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.to<CTextMessage>(); return; }
            if (ITimestampBased::canHandleIndex(index)) { ITimestampBased::setPropertyByIndex(index, variant); return; }

            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexSenderCallsign: m_senderCallsign.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
            case IndexRecipientCallsign: m_recipientCallsign.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
            case IndexMessage: m_message = variant.value<QString>(); break;
            default: CValueObject::setPropertyByIndex(index, variant); break;
            }
        }
    } // namespace
} // namespace
