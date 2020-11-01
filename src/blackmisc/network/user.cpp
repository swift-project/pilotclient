/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/network/user.h"
#include "blackmisc/aviation/airporticaocode.h"
#include "blackmisc/propertyindexref.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/stringutils.h"
#include "blackmisc/variant.h"

#include <QChar>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <Qt>
#include <QtGlobal>
#include <QStringBuilder>

using namespace BlackMisc::Aviation;

namespace BlackMisc
{
    namespace Network
    {
        CUser::CUser(const CCallsign &callsign)
            : m_callsign(callsign)
        {
            this->deriveHomeBaseFromCallsign();
        }

        CUser::CUser(const QString &id, const QString &realname, const CCallsign &callsign)
            : m_id(decode(id)), m_realname(decode(realname)), m_callsign(callsign)
        {
            this->deriveHomeBaseFromCallsign();
            this->setRealName(m_realname); // extracts homebase if this is included in real name
        }

        CUser::CUser(const QString &id, const QString &realname, const QString &email, const QString &password, const CCallsign &callsign)
            : m_id(decode(id)), m_realname(decode(realname)), m_email(decode(email)), m_password(decode(password)), m_callsign(callsign)
        {
            this->deriveHomeBaseFromCallsign();
            this->setRealName(m_realname); // extracts homebase
        }

        QString CUser::getRealNameAndHomeBase(const QString &separator) const
        {
            if (!this->hasHomeBase()) { return this->getRealName(); }
            if (!this->hasRealName()) { return this->getHomeBase().asString(); }
            return this->getRealName() % separator % this->getHomeBase().asString();
        }

        QString CUser::getRealNameAndId() const
        {
            if (this->hasRealName())
            {
                if (this->hasId()) { return this->getRealName() % u" (" % this->getId()  % QStringLiteral(")"); }
                return this->getRealName();
            }
            return this->getId();
        }

        bool CUser::setCallsign(const CCallsign &callsign)
        {
            if (m_callsign == callsign) { return false; }
            m_callsign = callsign;
            this->deriveHomeBaseFromCallsign();
            return true;
        }

        QString CUser::convertToQString(bool i18n) const
        {
            Q_UNUSED(i18n)
            if (m_realname.isEmpty()) return QStringLiteral("<no realname>");
            QString s = m_realname;
            if (this->hasId())
            {
                s += u" (" % m_id % u')';
            }
            if (this->hasCallsign())
            {
                s += u' ' % this->getCallsign().getStringAsSet();
            }
            return s;
        }

        void CUser::deriveHomeBaseFromCallsign()
        {
            if (m_callsign.isEmpty()) { return; }
            if (m_homebase.isEmpty())
            {
                if (m_callsign.isAtcCallsign())
                {
                    // option 1: real ATC station
                    // option 2: a copilot cmoning in Observer
                    if (m_callsign.isObserverCallsign())
                    {
                        // copilot
                    }
                    else
                    {
                        m_homebase = m_callsign.getIcaoCode();
                    }
                }
            }
        }

        void CUser::setRealName(const QString &realname)
        {
            QString rn(simplifyAccents(decode(realname).simplified()));
            if (rn.isEmpty())
            {
                m_realname.clear();
                return;
            }

            if (!this->hasHomeBase())
            {
                // only apply stripping if home base is not explicitly given
                // try to strip homebase: I understand the limitations, but we will have more correct hits as failures I assume
                thread_local QRegularExpression tsRegex("(-\\s*|\\s)([A-Z]{4})$");
                const QRegularExpressionMatch match = tsRegex.match(rn);
                if (match.hasMatch())
                {
                    const int pos = match.capturedStart(0);
                    const QString icao = match.captured(0).trimmed().right(4);
                    rn = rn.leftRef(pos).trimmed().toString();
                    this->setHomeBase(CAirportIcaoCode(icao));
                }
            }

            // do not beautify before stripping home base
            m_realname = beautifyRealName(rn);
        }

        bool CUser::hasValidHomeBase() const
        {
            return m_homebase.hasValidIcaoCode(false);
        }

        bool CUser::hasValidOrEmptyHomeBase() const
        {
            return m_homebase.isEmpty() || this->hasValidHomeBase();
        }

        CStatusMessageList CUser::validate() const
        {
            CStatusMessageList msgs;
            // callsign optional
            if (!this->hasId()) { msgs.push_back(CStatusMessage(CStatusMessage::SeverityWarning, u"Invalid id"));}
            if (!this->hasRealName()) { msgs.push_back(CStatusMessage(CStatusMessage::SeverityWarning, u"Invalid real name"));}
            if (!this->hasCredentials()) { msgs.push_back(CStatusMessage(CStatusMessage::SeverityWarning, u"Invalid credentials"));}
            return msgs;
        }

        QString CUser::get7DigitId() const
        {
            if (!this->hasNumericId()) { return m_id; }
            if (m_id.length() > 6) { return m_id; }

            static const QString zeros("0000000");
            return zeros.left(7 - m_id.length()) % m_id;
        }

        int CUser::getIntegerId() const
        {
            if (m_id.isEmpty()) { return -1; }
            if (is09OnlyString(m_id)) { return m_id.toInt(); }
            return -1;
        }

        bool CUser::hasNumericId() const
        {
            if (m_id.isEmpty()) { return false; }
            return is09OnlyString(m_id);
        }

        void CUser::updateMissingParts(const CUser &otherUser)
        {
            if (this == &otherUser) { return; }
            if (!this->hasRealName()) { this->setRealName(otherUser.getRealName()); }
            if (!this->hasId()) { this->setId(otherUser.getId()); }
            if (!this->hasValidEmail()) { this->setEmail(otherUser.getEmail()); }
            if (!this->hasCallsign()) { this->setCallsign(otherUser.getCallsign()); }
        }

        void CUser::synchronizeData(CUser &otherUser)
        {
            if (this == &otherUser) { return; }
            this->updateMissingParts(otherUser);
            otherUser.updateMissingParts(*this);
        }

        bool CUser::isValidVatsimId(const QString &id)
        {
            if (id.isEmpty()) { return false; }
            if (!is09OnlyString(id)) { return false; }
            const int i = id.toInt();
            return i >= 100000 && i <= 9999999;
        }

        QString CUser::beautifyRealName(const QString &realName)
        {
            QString newRealName(realName.simplified().trimmed());
            if (newRealName.isEmpty())       { return newRealName; }
            if (is09OnlyString(newRealName)) { return newRealName; } // new VATSIM COD, allowing id as name, see https://discordapp.com/channels/539048679160676382/539846348275449887/599969308247851018

            int uc = 0;
            int lc = 0;
            for (const QChar &ch : realName)
            {
                // Joe Doe -> valid
                // jOE dOE -> invalid
                // Joe McArthur -> valid
                if (uc > 1 && lc > 2 && lc > uc) { return newRealName; } // mixed case name, no need to beautify
                if (ch.isLower()) { lc++; continue; }
                if (ch.isUpper()) { uc++; continue; }
            }

            // simple title case beautifying
            newRealName = newRealName.toLower();
            QString::Iterator i = newRealName.begin();
            bool upperNextChar = true;
            while (i != newRealName.end())
            {
                if (i->isSpace() || *i == '-')
                {
                    upperNextChar = true;
                }
                else if (upperNextChar)
                {
                    const QChar u(i->toUpper());
                    *i = u;
                    upperNextChar = false;
                }
                ++i;
            }
            return newRealName;
        }

        QVariant CUser::propertyByIndex(CPropertyIndexRef index) const
        {
            if (index.isMyself()) { return QVariant::fromValue(*this); }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexEmail: return QVariant(m_email);
            case IndexId: return QVariant(m_id);
            case IndexId7Digit:  return QVariant(this->get7DigitId());
            case IndexIdInteger: return QVariant::fromValue(this->getIntegerId());
            case IndexPassword:  return QVariant(m_password);
            case IndexRealName:  return QVariant(m_realname);
            case IndexHomebase:  return m_homebase.propertyByIndex(index.copyFrontRemoved());
            case IndexCallsign:  return m_callsign.propertyByIndex(index.copyFrontRemoved());
            default: return CValueObject::propertyByIndex(index);
            }
        }

        void CUser::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.value<CUser>(); return; }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexEmail: this->setEmail(variant.value<QString>()); break;
            case IndexIdInteger: this->setId(QString::number(variant.toInt())); break;
            case IndexId7Digit: // fallthru
            case IndexId: this->setId(variant.value<QString>()); break;
            case IndexPassword: this->setPassword(variant.value<QString>()); break;
            case IndexRealName: this->setRealName(variant.value<QString>()); break;
            case IndexHomebase: m_homebase.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
            case IndexCallsign: m_callsign.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
            default: CValueObject::setPropertyByIndex(index, variant); break;
            }
        }

        int CUser::comparePropertyByIndex(CPropertyIndexRef index, const CUser &compareValue) const
        {
            if (index.isMyself()) { return this->getRealName().compare(compareValue.getRealName(), Qt::CaseInsensitive); }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexEmail: return m_email.compare(compareValue.getEmail(), Qt::CaseInsensitive);
            case IndexId: return m_id.compare(compareValue.getId(), Qt::CaseInsensitive);
            case IndexId7Digit: return this->get7DigitId().compare(compareValue.get7DigitId(), Qt::CaseInsensitive);
            case IndexRealName: return m_realname.compare(compareValue.getRealName(), Qt::CaseInsensitive);
            case IndexHomebase: return m_homebase.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getHomeBase());
            case IndexCallsign: return m_callsign.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getCallsign());
            case IndexPassword: break;
            default: break;
            }
            Q_ASSERT_X(false, Q_FUNC_INFO, "compare failed");
            return 0;
        }
    } // namespace
} // namespace
