/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/network/user.h"
#include "blackmisc/aviation/airporticaocode.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/stringutils.h"
#include "blackmisc/variant.h"

#include <QChar>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QThreadStorage>
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

        void CUser::setCallsign(const CCallsign &callsign)
        {
            m_callsign = callsign;
            this->deriveHomeBaseFromCallsign();
        }

        QString CUser::convertToQString(bool i18n) const
        {
            Q_UNUSED(i18n);
            if (m_realname.isEmpty()) return QStringLiteral("<no realname>");
            QString s = m_realname;
            if (this->hasId())
            {
                s += QStringLiteral(" (") % m_id % QStringLiteral(")");
            }
            if (this->hasCallsign())
            {
                s += QStringLiteral(" ") % this->getCallsign().getStringAsSet();
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
                    m_homebase = m_callsign.getIcaoCode();
                }
            }
        }

        void CUser::setRealName(const QString &realname)
        {
            QString rn(removeAccents(decode(realname).simplified()));
            if (rn.isEmpty())
            {
                m_realname.clear();
                return;
            }

            if (!this->hasHomeBase())
            {
                // only apply stripping if home base is not explicitly given
                // try to strip homebase: I understand the limitations, but we will have more correct hits as failures I assume
                static QThreadStorage<QRegularExpression> tsRegex;
                if (! tsRegex.hasLocalData()) { tsRegex.setLocalData(QRegularExpression("(-\\s*|\\s)([A-Z]{4})$")); }
                const auto &regex = tsRegex.localData();
                const QRegularExpressionMatch match = regex.match(rn);
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
            return m_homebase.hasValidIcaoCode();
        }

        CStatusMessageList CUser::validate() const
        {
            CStatusMessageList msgs;
            // callsign optional
            if (!this->hasId()) { msgs.push_back(CStatusMessage(CStatusMessage::SeverityWarning, "Invalid id"));}
            if (!this->hasRealName()) { msgs.push_back(CStatusMessage(CStatusMessage::SeverityWarning, "Invalid real name"));}
            if (!this->hasCredentials()) { msgs.push_back(CStatusMessage(CStatusMessage::SeverityWarning, "Invalid credentials"));}
            return msgs;
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
            bool ok;
            const int i = id.toInt(&ok);
            if (!ok) { return false; }
            return i >= 100000 && i <= 9999999;
        }

        QString CUser::beautifyRealName(const QString &realName)
        {
            QString newRealName(realName.simplified().trimmed());
            if (newRealName.isEmpty()) { return newRealName; }

            int uc = 0;
            int lc = 0;
            for (const QChar &ch : realName)
            {
                // Joe Doe -> valid
                // jOE dOE -> invalid
                // Joe McArthur -> valid
                if (uc > 1 && lc > 2 && lc > uc) { return newRealName; } // mixed case name, no need to beautify
                if (ch.isLower()) { lc++; continue; }
                if (ch.isUpper()) { uc++; continue;}
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

        CVariant CUser::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexEmail: return CVariant(m_email);
            case IndexId: return CVariant(m_id);
            case IndexPassword: return CVariant(m_password);
            case IndexRealName: return CVariant(m_realname);
            case IndexHomebase: return m_homebase.propertyByIndex(index.copyFrontRemoved());
            case IndexCallsign: return m_callsign.propertyByIndex(index.copyFrontRemoved());
            default: return CValueObject::propertyByIndex(index);
            }
        }

        void CUser::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.to<CUser>(); return; }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexEmail: this->setEmail(variant.value<QString>()); break;
            case IndexId: this->setId(variant.value<QString>()); break;
            case IndexPassword: this->setPassword(variant.value<QString>()); break;
            case IndexRealName: this->setRealName(variant.value<QString>()); break;
            case IndexHomebase: m_homebase.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
            case IndexCallsign: m_callsign.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
            default: CValueObject::setPropertyByIndex(index, variant); break;
            }
        }

        int CUser::comparePropertyByIndex(const CPropertyIndex &index, const CUser &compareValue) const
        {
            if (index.isMyself()) { return this->getRealName().compare(compareValue.getRealName(), Qt::CaseInsensitive); }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexEmail: return m_email.compare(compareValue.getEmail(), Qt::CaseInsensitive);
            case IndexId: return m_id.compare(compareValue.getId(), Qt::CaseInsensitive);
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
