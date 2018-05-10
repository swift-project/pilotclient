/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of Swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/aviation/callsign.h"
#include "blackmisc/compare.h"

#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QStringList>
#include <QThreadStorage>
#include <Qt>
#include <QtGlobal>

namespace BlackMisc
{
    namespace Aviation
    {
        QString CCallsign::convertToQString(bool i18n) const
        {
            Q_UNUSED(i18n);
            return m_callsign;
        }

        QString CCallsign::unifyCallsign(const QString &callsign)
        {
            return removeChars(callsign.toUpper().trimmed(), [](QChar c) { return !c.isLetterOrNumber() && c != '_'; });
        }

        const CIcon &CCallsign::convertToIcon(const CCallsign &callsign)
        {
            if (callsign.getTypeHint() == CCallsign::Aircraft || !callsign.hasSuffix())
            {
                return CIcon::iconByIndex(CIcons::NetworkRolePilot);
            }
            else
            {
                return atcSuffixToIcon(callsign.getSuffix());
            }
        }

        const CIcon &CCallsign::atcSuffixToIcon(const QString &suffix)
        {
            if (suffix.length() < 3) { return CIcon::iconByIndex(CIcons::NetworkRoleUnknown); }
            QString sfx = suffix.toUpper();
            if ("APP" == sfx) { return CIcon::iconByIndex(CIcons::NetworkRoleApproach); }
            if ("GND" == sfx) { return CIcon::iconByIndex(CIcons::NetworkRoleGround); }
            if ("TWR" == sfx) { return CIcon::iconByIndex(CIcons::NetworkRoleTower); }
            if ("DEL" == sfx) { return CIcon::iconByIndex(CIcons::NetworkRoleDelivery); }
            if ("CTR" == sfx) { return CIcon::iconByIndex(CIcons::NetworkRoleCenter); }
            if ("SUP" == sfx) { return CIcon::iconByIndex(CIcons::NetworkRoleSup); }
            if ("OBS" == sfx) { return CIcon::iconByIndex(CIcons::NetworkRoleObs); }
            if ("INS" == sfx) { return CIcon::iconByIndex(CIcons::NetworkRoleMnt); }
            if ("FSS" == sfx) { return CIcon::iconByIndex(CIcons::NetworkRoleFss); }
            if ("ATIS" == sfx) { return CIcon::iconByIndex(CIcons::AviationAtis); }
            if ("EXAM" == sfx) { return CIcon::iconByIndex(CIcons::NetworkRoleMnt); }
            if ("VATSIM" == sfx) { return CIcon::iconByIndex(CIcons::NetworkVatsimLogoWhite); }
            return CIcon::iconByIndex(CIcons::NetworkRoleUnknown);
        }

        bool CCallsign::isAtcCallsign() const
        {
            if (this->getTypeHint() == Atc) { return true; }
            if (!this->hasSuffix()) { return false; }
            return atcCallsignSuffixes().contains(this->getSuffix(), Qt::CaseInsensitive);
        }

        bool CCallsign::isSupervisorCallsign() const
        {
            if (this->getTypeHint() == Aircraft) { return false; }
            return m_callsign.endsWith("SUP");
        }

        QString CCallsign::getIcaoCode() const
        {
            if (this->isAtcCallsign())
            {
                if (m_callsign.length() >= 4)
                {
                    return m_callsign.left(4).toUpper();
                }
            }
            return "";
        }

        bool CCallsign::isAtcAlikeCallsign() const
        {
            if (this->getTypeHint() == Aircraft) { return false; }
            if (!this->hasSuffix()) { return false; }
            return atcAlikeCallsignSuffixes().contains(this->getSuffix(), Qt::CaseInsensitive);
        }

        bool CCallsign::isObserverCallsign() const
        {
            if (this->getTypeHint() == Aircraft) { return false; }
            return m_callsignAsSet.endsWith("_OBS", Qt::CaseInsensitive);
        }

        QString CCallsign::getAsObserverCallsignString() const
        {
            if (this->isEmpty()) { return ""; }
            QString obs = this->getStringAsSet();
            if (obs.endsWith("_OBS", Qt::CaseInsensitive)) { return obs; }
            if (obs.contains('_')) { obs = obs.left(obs.lastIndexOf('_')); }
            return obs.append("_OBS").toUpper();
        }

        QString CCallsign::getSuffix() const
        {
            QString s;
            if (this->hasSuffix())
            {
                s = this->getStringAsSet().section('_', -1).toUpper();
            }
            return s;
        }

        QString CCallsign::getAirlineSuffix() const
        {
            if (m_callsign.length() < 3) { return ""; }
            if (this->isAtcCallsign()) { return ""; }

            thread_local const QRegularExpression regExp("^[A-Z]{3,}");
            QRegularExpressionMatch match = regExp.match(m_callsign);
            if (!match.hasMatch()) { return QString(); }
            const QString airline = match.captured(0);

            // hard facts
            if (airline.length() == 3) { return airline; } // we allow 3 letters
            if (airline.length() == 4 && airline.startsWith('V')) { return airline; } // we allow virtual 4 letter codes, e.g. VDLD

            // some people use callsigns like UPSE123
            const QString number = match.captured(1);
            if (number.length() >= 3 && airline.length() == 4) { return airline.left(3); }

            return ""; // invalid
        }

        bool CCallsign::hasSuffix() const
        {
            return this->getStringAsSet().contains('_');
        }

        bool CCallsign::equalsString(const QString &callsignString) const
        {
            CCallsign other(callsignString);
            return other == (*this);
        }

        CVariant CCallsign::propertyByIndex(const CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexCallsignString: return CVariant(this->asString());
            case IndexCallsignStringAsSet: return CVariant(this->getStringAsSet());
            case IndexTelephonyDesignator: return CVariant(this->getTelephonyDesignator());
            case IndexSuffix: return CVariant(this->getSuffix());
            default: return CValueObject::propertyByIndex(index);
            }
        }

        void CCallsign::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.to<CCallsign>(); return; }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexCallsignString: m_callsign = m_callsign = unifyCallsign(variant.toQString()); break;
            case IndexCallsignStringAsSet: m_callsignAsSet = variant.toQString(); break;
            case IndexTelephonyDesignator: m_telephonyDesignator = variant.toQString(); break;
            default:
                CValueObject::setPropertyByIndex(index, variant);
                break;
            }
        }

        int CCallsign::comparePropertyByIndex(const CPropertyIndex &index, const CCallsign &compareValue) const
        {
            if (index.isMyself()) { return m_callsign.compare(compareValue.m_callsign, Qt::CaseInsensitive); }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexCallsignString: return m_callsign.compare(compareValue.m_callsign, Qt::CaseInsensitive);
            case IndexCallsignStringAsSet: return m_callsignAsSet.compare(compareValue.m_callsignAsSet, Qt::CaseInsensitive);
            case IndexTelephonyDesignator: return m_telephonyDesignator.compare(compareValue.m_telephonyDesignator, Qt::CaseInsensitive);
            case IndexSuffix: return this->getSuffix().compare(compareValue.getSuffix(), Qt::CaseInsensitive);
            default: break;
            }
            Q_ASSERT_X(false, Q_FUNC_INFO, "Compare failed");
            return 0;
        }

        bool CCallsign::isValid() const
        {
            switch (m_typeHint)
            {
            case Atc: return isValidAtcCallsign(*this);
            case Aircraft: return isValidAircraftCallsign(*this);
            default: return !this->isEmpty();
            }
        }

        bool CCallsign::isValidAircraftCallsign(const QString &callsign)
        {
            if (callsign.length() < 2 || callsign.length() > 10) { return false; }
            return !containsChar(callsign, [](QChar c) { return !c.isUpper() && !c.isDigit(); });
        }

        bool CCallsign::isValidAircraftCallsign(const CCallsign &callsign)
        {
            return isValidAircraftCallsign(callsign.asString());
        }

        bool CCallsign::isValidAtcCallsign(const QString &callsign)
        {
            if (callsign.length() < 2 || callsign.length() > 10) { return false; }
            return !containsChar(callsign, [](QChar c) { return c != '_' && !c.isUpper() && !c.isDigit(); });
        }

        bool CCallsign::isValidAtcCallsign(const CCallsign &callsign)
        {
            return isValidAtcCallsign(callsign.asString());
        }

        const QStringList &CCallsign::atcCallsignSuffixes()
        {
            static const QStringList a({ "APP", "GND", "TWR", "DEL", "CTR" });
            return a;
        }

        const QStringList &CCallsign::atcAlikeCallsignSuffixes()
        {
            static const QStringList a({ "ATIS", "APP", "GND", "OBS", "TWR", "DEL", "CTR", "SUP", "FSS", "INS" });
            return a;
        }
    } // namespace
} // namespace
