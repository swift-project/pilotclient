/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of Swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/aviation/callsign.h"
#include "blackmisc/iconlist.h"

#include <QThreadStorage>
#include <QRegularExpression>

namespace BlackMisc
{
    namespace Aviation
    {
        QString CCallsign::convertToQString(bool i18n) const
        {
            Q_UNUSED(i18n);
            return this->m_callsign;
        }

        QString CCallsign::unifyCallsign(const QString &callsign)
        {
            QString unified = callsign.toUpper();
            // allow A-Z, 0-9, _, but no spaces
            unified = unified.remove(QRegExp("[^A-Z\\d_]"));
            return unified;
        }

        const CIcon &CCallsign::convertToIcon(const CCallsign &callsign)
        {
            if (callsign.getTypeHint() == CCallsign::Aircraft || !callsign.hasSuffix())
            {
                return CIconList::iconByIndex(CIcons::NetworkRolePilot);
            }
            else
            {
                return atcSuffixToIcon(callsign.getSuffix());
            }
        }

        const CIcon &CCallsign::atcSuffixToIcon(const QString &suffix)
        {
            if (suffix.length() < 3) { return CIconList::iconByIndex(CIcons::NetworkRoleUnknown); }
            QString sfx = suffix.toUpper();
            if ("APP" == sfx) { return CIconList::iconByIndex(CIcons::NetworkRoleApproach); }
            if ("GND" == sfx) { return CIconList::iconByIndex(CIcons::NetworkRoleGround); }
            if ("TWR" == sfx) { return CIconList::iconByIndex(CIcons::NetworkRoleTower); }
            if ("DEL" == sfx) { return CIconList::iconByIndex(CIcons::NetworkRoleDelivery); }
            if ("CTR" == sfx) { return CIconList::iconByIndex(CIcons::NetworkRoleCenter); }
            if ("SUP" == sfx) { return CIconList::iconByIndex(CIcons::NetworkRoleSup); }
            if ("OBS" == sfx) { return CIconList::iconByIndex(CIcons::NetworkRoleObs); }
            if ("FSS" == sfx) { return CIconList::iconByIndex(CIcons::NetworkRoleFss); }
            if ("ATIS" == sfx) { return CIconList::iconByIndex(CIcons::AviationAtis); }
            if ("EXAM" == sfx) { return CIconList::iconByIndex(CIcons::NetworkRoleMnt); }
            if ("VATSIM" == sfx) { return CIconList::iconByIndex(CIcons::NetworkVatsimLogoWhite); }
            return CIconList::iconByIndex(CIcons::NetworkRoleUnknown);
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
            return this->m_callsign.endsWith("SUP");
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
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexCallsignString:
                return CVariant(this->asString());
            case IndexCallsignStringAsSet:
                return CVariant(this->getStringAsSet());
            case IndexTelephonyDesignator:
                return CVariant(this->getTelephonyDesignator());
            case IndexSuffix:
                return CVariant(this->getSuffix());
            default:
                return CValueObject::propertyByIndex(index);
            }
        }

        void CCallsign::setPropertyByIndex(const CVariant &variant, const CPropertyIndex &index)
        {
            if (index.isMyself()) { (*this) = variant.to<CCallsign>(); return; }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexCallsignString:
                this->m_callsign = variant.toQString();
                break;
            case IndexCallsignStringAsSet:
                this->m_callsignAsSet = variant.toQString();
                break;
            case IndexTelephonyDesignator:
                this->m_telephonyDesignator = variant.toQString();
                break;
            default:
                return CValueObject::setPropertyByIndex(variant, index);
            }
        }

        int CCallsign::comparePropertyByIndex(const CCallsign &compareValue, const CPropertyIndex &index) const
        {
            if (index.isMyself()) { return this->m_callsign.compare(compareValue.m_callsign, Qt::CaseInsensitive); }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexCallsignString:
                return this->m_callsign.compare(compareValue.m_callsign, Qt::CaseInsensitive);
            case IndexCallsignStringAsSet:
                return this->m_callsignAsSet.compare(compareValue.m_callsignAsSet, Qt::CaseInsensitive);
            case IndexTelephonyDesignator:
                return this->m_telephonyDesignator.compare(compareValue.m_telephonyDesignator, Qt::CaseInsensitive);
            default:
                break;
            }
            Q_ASSERT_X(false, Q_FUNC_INFO, "Compare failed");
            return 0;
        }

        bool CCallsign::isValid() const
        {
            switch (m_typeHint)
            {
            case Atc:
                return isValidAtcCallsign(*this);
            case Aircraft:
                return isValidAircraftCallsign(*this);
            default:
                return !this->isEmpty();
            }
        }

        bool CCallsign::isValidAircraftCallsign(const QString &callsign)
        {
            if (callsign.length() < 2 || callsign.length() > 10) { return false; }

            // We allow all number callsigns
            static QThreadStorage<QRegularExpression> tsRegex;
            if (! tsRegex.hasLocalData()) { tsRegex.setLocalData(QRegularExpression("^[A-Z0-9]*$")); }
            const QRegularExpression &regexp = tsRegex.localData();
            return (regexp.match(callsign).hasMatch());
        }

        bool CCallsign::isValidAircraftCallsign(const CCallsign &callsign)
        {
            return isValidAircraftCallsign(callsign.asString());
        }

        bool CCallsign::isValidAtcCallsign(const QString &callsign)
        {
            if (callsign.length() < 2 || callsign.length() > 10) { return false; }

            // We allow all number callsigns
            static QThreadStorage<QRegularExpression> tsRegex;
            if (! tsRegex.hasLocalData()) { tsRegex.setLocalData(QRegularExpression("^[A-Z0-9_]*$")); }
            const QRegularExpression &regexp = tsRegex.localData();
            return (regexp.match(callsign).hasMatch());
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
            static const QStringList a({ "ATIS", "APP", "GND", "OBS", "TWR", "DEL", "CTR", "SUP", "FSS" });
            return a;
        }

    } // namespace
} // namespace
