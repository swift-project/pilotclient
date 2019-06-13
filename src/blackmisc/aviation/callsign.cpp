/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of Swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/aviation/callsign.h"
#include "blackmisc/compare.h"

#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QStringList>
#include <Qt>
#include <QtGlobal>

namespace BlackMisc
{
    namespace Aviation
    {
        CCallsign::CCallsign(const QString &callsign, CCallsign::TypeHint hint)
            : m_callsignAsSet(callsign.trimmed()), m_callsign(CCallsign::unifyCallsign(callsign, hint)), m_typeHint(hint)
        {}

        CCallsign::CCallsign(const QString &callsign, const QString &telephonyDesignator, CCallsign::TypeHint hint)
            : m_callsignAsSet(callsign.trimmed()), m_callsign(CCallsign::unifyCallsign(callsign, hint)), m_telephonyDesignator(telephonyDesignator.trimmed()), m_typeHint(hint)
        {}

        CCallsign::CCallsign(const char *callsign, CCallsign::TypeHint hint)
            : m_callsignAsSet(callsign), m_callsign(CCallsign::unifyCallsign(callsign, hint)), m_typeHint(hint)
        {}

        void CCallsign::registerMetadata()
        {
            CValueObject<CCallsign>::registerMetadata();
            qRegisterMetaType<CCallsign::TypeHint>();
        }

        QString CCallsign::convertToQString(bool i18n) const
        {
            Q_UNUSED(i18n);
            return m_callsign;
        }

        void CCallsign::clear()
        {
            *this = CCallsign();
        }

        int CCallsign::suffixToSortOrder(const QString &suffix)
        {
            if (QStringView(u"FSS")  == suffix) { return 1; }
            if (QStringView(u"CTR")  == suffix) { return 2; }
            if (QStringView(u"APP")  == suffix) { return 3; }
            if (QStringView(u"DEP")  == suffix) { return 4; }
            if (QStringView(u"TWR")  == suffix) { return 5; }
            if (QStringView(u"GND")  == suffix) { return 6; }
            if (QStringView(u"DEL")  == suffix) { return 7; }
            if (QStringView(u"ATIS") == suffix) { return 8; }
            if (QStringView(u"SUP")  == suffix) { return 9; }
            if (QStringView(u"OBS")  == suffix) { return 10; }
            if (QStringView(u"INS")  == suffix) { return 11; } // instructor/mentor
            if (QStringView(u"ADM")  == suffix) { return 12; } // admin
            if (QStringView(u"VATGOV") == suffix) { return 13; } // VATSIM governors
            if (QStringView(u"VATSIM") == suffix) { return 14; } // VATSIM founder
            if (QStringView(u"EXAM")   == suffix) { return 15; }
            return std::numeric_limits<int>::max();
        }

        QString CCallsign::unifyCallsign(const QString &callsign, TypeHint hint)
        {
            // Ref T664, allow ATC with hyphen, such as Ml-SNO_CTR
            switch (hint)
            {
            case Atc: return removeChars(callsign.toUpper().trimmed(), [](QChar c) { return !c.isLetterOrNumber() && c != '_' && c != '-'; });
            default: break;
            }
            return removeChars(callsign.toUpper().trimmed(), [](QChar c) { return !c.isLetterOrNumber() && c != '_'; });
        }

        const CIcon &CCallsign::convertToIcon(const CCallsign &callsign)
        {
            if (callsign.m_callsign.startsWith(QStringView(u"VATGOV"))) { return CIcon::iconByIndex(CIcons::NetworkRolePilot); }
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
            const QString sfx = suffix.toUpper();
            if (QStringView(u"APP")  == sfx) { return CIcon::iconByIndex(CIcons::NetworkRoleApproach); }
            if (QStringView(u"DEP")  == sfx) { return CIcon::iconByIndex(CIcons::NetworkRoleDeparture); }
            if (QStringView(u"GND")  == sfx) { return CIcon::iconByIndex(CIcons::NetworkRoleGround); }
            if (QStringView(u"TWR")  == sfx) { return CIcon::iconByIndex(CIcons::NetworkRoleTower); }
            if (QStringView(u"DEL")  == sfx) { return CIcon::iconByIndex(CIcons::NetworkRoleDelivery); }
            if (QStringView(u"CTR")  == sfx) { return CIcon::iconByIndex(CIcons::NetworkRoleCenter); }
            if (QStringView(u"SUP")  == sfx) { return CIcon::iconByIndex(CIcons::NetworkRoleSup); }
            if (QStringView(u"OBS")  == sfx) { return CIcon::iconByIndex(CIcons::NetworkRoleObs); }
            if (QStringView(u"INS")  == sfx) { return CIcon::iconByIndex(CIcons::NetworkRoleMnt); }
            if (QStringView(u"FSS")  == sfx) { return CIcon::iconByIndex(CIcons::NetworkRoleFss); }
            if (QStringView(u"ATIS") == sfx) { return CIcon::iconByIndex(CIcons::AviationAtis); }
            if (QStringView(u"EXAM") == sfx) { return CIcon::iconByIndex(CIcons::NetworkRoleMnt); }
            if (QStringView(u"VATSIM") == sfx) { return CIcon::iconByIndex(CIcons::NetworkVatsimLogoWhite); }
            if (QStringView(u"VATGOV") == sfx) { return CIcon::iconByIndex(CIcons::NetworkVatsimLogoWhite); }
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

        bool CCallsign::isMaybeCopilotCallsign(const CCallsign &pilotCallsign) const
        {
            return  m_callsign.startsWith(pilotCallsign.asString()) &&
                    m_callsign.size() == pilotCallsign.asString().size() + 1 &&
                    m_callsign.at(m_callsign.size() - 1) >= 'A' && m_callsign.at(m_callsign.size() - 1) <= 'Z';
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
            return {};
        }

        bool CCallsign::isAtcAlikeCallsign() const
        {
            if (this->getTypeHint() == Aircraft) { return false; }
            if (!this->hasSuffix()) { return false; }
            return atcAlikeCallsignSuffixes().contains(this->getSuffix(), Qt::CaseInsensitive);
        }

        bool CCallsign::isObserverCallsign() const
        {
            return m_callsignAsSet.endsWith("_OBS", Qt::CaseInsensitive);
        }

        QString CCallsign::getAsObserverCallsignString() const
        {
            if (this->isEmpty()) { return {}; }
            QString obs = this->getStringAsSet();
            if (obs.endsWith("_OBS", Qt::CaseInsensitive)) { return obs; } // already OBS
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
            QString flightNumber;
            return this->getAirlineSuffix(flightNumber);
        }

        QString CCallsign::getAirlineSuffix(QString &flightNumber) const
        {
            flightNumber.clear();
            if (m_callsign.length() < 3) { return {}; }
            if (this->isAtcCallsign())   { return {}; }

            thread_local const QRegularExpression regExp("^[A-Z]{3,}");
            QRegularExpressionMatch match = regExp.match(m_callsign);
            if (!match.hasMatch()) { return {}; }
            const QString airline = match.captured(0);
            flightNumber = match.captured(1);

            // hard facts
            if (airline.length() == 3) { return airline; } // we allow 3 letters
            if (airline.length() == 4 && airline.startsWith('V')) { return airline; } // we allow virtual 4 letter codes, e.g. VDLD

            // some people use callsigns like UPSE123
            flightNumber = match.captured(1);
            if (flightNumber.length() >= 3 && airline.length() == 4) { return airline.left(3); }

            return {}; // invalid
        }

        QString CCallsign::getFlightNumber() const
        {
            if (this->isAtcCallsign()) { return {}; }
            QString flightNumber;
            const QString airline = this->getAirlineSuffix(flightNumber);
            return airline.isEmpty() ? QString() : flightNumber;
        }

        int CCallsign::getFlightNumberInt() const
        {
            if (this->isAtcCallsign()) { return -1; }
            bool ok;
            const int fn = this->getFlightNumber().toInt(&ok);
            return ok ? fn : -1;
        }

        bool CCallsign::hasSuffix() const
        {
            return this->getStringAsSet().contains('_');
        }

        bool CCallsign::hasAtcSuffix() const
        {
            const QString s = this->getSuffix();
            return !s.isEmpty() && atcCallsignSuffixes().contains(s);
        }

        int CCallsign::getSuffixSortOrder() const
        {
            return suffixToSortOrder(this->getSuffix());
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
            case IndexCallsignString:      return CVariant(this->asString());
            case IndexCallsignStringAsSet: return CVariant(this->getStringAsSet());
            case IndexTelephonyDesignator: return CVariant(this->getTelephonyDesignator());
            case IndexSuffix:              return CVariant(this->getSuffix());
            default: return CValueObject::propertyByIndex(index);
            }
        }

        void CCallsign::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.to<CCallsign>(); return; }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexCallsignString:      m_callsign = unifyCallsign(variant.toQString()); break;
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
            case IndexCallsignString:      return m_callsign.compare(compareValue.m_callsign, Qt::CaseInsensitive);
            case IndexCallsignStringAsSet: return m_callsignAsSet.compare(compareValue.m_callsignAsSet, Qt::CaseInsensitive);
            case IndexTelephonyDesignator: return m_telephonyDesignator.compare(compareValue.m_telephonyDesignator, Qt::CaseInsensitive);
            case IndexSuffix:              return this->getSuffix().compare(compareValue.getSuffix(), Qt::CaseInsensitive);
            default:
                return CValueObject::comparePropertyByIndex(index, compareValue);
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
            // Ref T664, allow ATC with hyphen, such as Ml-SNO_CTR
            if (callsign.length() < 2 || callsign.length() > 10) { return false; }
            return !containsChar(callsign, [](QChar c) { return c != '-' && c != '_' && !c.isUpper() && !c.isDigit(); });
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
