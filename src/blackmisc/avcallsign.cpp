/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of Swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "avcallsign.h"
#include "iconlist.h"

namespace BlackMisc
{
    namespace Aviation
    {
        /*
         * Convert to string
         */
        QString CCallsign::convertToQString(bool /** i18n **/) const
        {
            return this->m_callsign;
        }

        /*
         * Unify the callsign
         */
        QString CCallsign::unifyCallsign(const QString &callsign)
        {
            QString unified = callsign.toUpper();
            unified = unified.remove(QRegExp("[^a-zA-Z\\d\\s]"));
            return unified;
        }

        /*
         * Iconify
         */
        const CIcon &CCallsign::convertToIcon(const CCallsign &callsign)
        {
            QString t = callsign.asString().toUpper();
            if (t.length() < 3) return CIconList::iconForIndex(CIcons::NetworkRoleUnknown);
            t = t.right(3);

            if (callsign.getStringAsSet().contains("_"))
            {
                if ("APP" == t) return CIconList::iconForIndex(CIcons::NetworkRoleApproach);
                if ("GND" == t) return CIconList::iconForIndex(CIcons::NetworkRoleGround);
                if ("TWR" == t) return CIconList::iconForIndex(CIcons::NetworkRoleTower);
                if ("DEL" == t) return CIconList::iconForIndex(CIcons::NetworkRoleDelivery);
                if ("CTR" == t) return CIconList::iconForIndex(CIcons::NetworkRoleCenter);
                if ("SUP" == t) return CIconList::iconForIndex(CIcons::NetworkRoleSup);
                if ("OBS" == t) return CIconList::iconForIndex(CIcons::NetworkRoleApproach);
                return CIconList::iconForIndex(CIcons::NetworkRoleUnknown);
            }
            else
            {
                return CIconList::iconForIndex(CIcons::NetworkRolePilot);
            }
        }

        /*
         * Callsign as Observer
         */
        QString CCallsign::getAsObserverCallsignString() const
        {
            if (this->isEmpty()) return "";
            QString obs = this->getStringAsSet();
            if (obs.endsWith("_OBS", Qt::CaseInsensitive)) return obs;
            if (obs.contains('_')) obs = obs.left(obs.lastIndexOf('_'));
            return obs.append("_OBS").toUpper();
        }

        /*
         * Equals callsign?
         */
        bool CCallsign::equalsString(const QString &callsignString) const
        {
            CCallsign other(callsignString);
            return other == (*this);
        }

        /*
         * Compare
         */
        int CCallsign::compareImpl(const CValueObject &otherBase) const
        {
            // intentionally compare on string only!
            const auto &other = static_cast<const CCallsign &>(otherBase);
            return this->m_callsign.compare(other.m_callsign, Qt::CaseInsensitive);
        }

        /*
         * Marshall to DBus
         */
        void CCallsign::marshallToDbus(QDBusArgument &argument) const
        {
            argument << TupleConverter<CCallsign>::toTuple(*this);
        }

        /*
         * Unmarshall from DBus
         */
        void CCallsign::unmarshallFromDbus(const QDBusArgument &argument)
        {
            argument >> TupleConverter<CCallsign>::toTuple(*this);
        }

        /*
         * To JSON
         */
        QJsonObject CCallsign::toJson() const
        {
            return BlackMisc::serializeJson(CCallsign::jsonMembers(), TupleConverter<CCallsign>::toTuple(*this));
        }

        /*
         * From JSON
         */
        void CCallsign::convertFromJson(const QJsonObject &json)
        {
            BlackMisc::deserializeJson(json, CCallsign::jsonMembers(), TupleConverter<CCallsign>::toTuple(*this));
        }

        /*
         * Index
         */
        QVariant CCallsign::propertyByIndex(const CPropertyIndex &index) const
        {
            if (index.isMyself()) { return this->toQVariant(); }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexCallsignString:
                return QVariant(this->asString());
            case IndexCallsignStringAsSet:
                return QVariant(this->getStringAsSet());
            case IndexTelephonyDesignator:
                return QVariant(this->getTelephonyDesignator());
            default:
                return CValueObject::propertyByIndex(index);
            }
        }

        /*
         * Index
         */
        void CCallsign::setPropertyByIndex(const QVariant &variant, const CPropertyIndex &index)
        {
            if (index.isMyself())
            {
                this->convertFromQVariant(variant);
                return;
            }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexCallsignString:
                this->m_callsign = variant.toString();
                break;
            case IndexCallsignStringAsSet:
                this->m_callsignAsSet = variant.toString();
                break;
            case IndexTelephonyDesignator:
                this->m_telephonyDesignator = variant.toString();
                break;
            default:
                return CValueObject::setPropertyByIndex(variant, index);
            }
        }

        /*
         * Members
         */
        const QStringList &CCallsign::jsonMembers()
        {
            return TupleConverter<CCallsign>::jsonMembers();
        }

        /*
         * Equal?
         */
        bool CCallsign::operator ==(const CCallsign &other) const
        {
            if (this == &other) return true;
            // intentionally not via Tupel converter, compare on string only
            return this->asString().compare(other.asString(), Qt::CaseInsensitive) == 0;
        }

        /*
         * Unequal?
         */
        bool CCallsign::operator !=(const CCallsign &other) const
        {
            return !((*this) == other);
        }

        /*
         * Hash
         */
        uint CCallsign::getValueHash() const
        {
            return qHash(TupleConverter<CCallsign>::toTuple(*this));
        }

        /*
         * Less than?
         */
        bool CCallsign::operator <(const CCallsign &other) const
        {
            return this->m_callsign < other.m_callsign;
        }

        /*
         * metaTypeId
         */
        int CCallsign::getMetaTypeId() const
        {
            return qMetaTypeId<CCallsign>();
        }

        /*
         * is a
         */
        bool CCallsign::isA(int metaTypeId) const
        {
            if (metaTypeId == qMetaTypeId<CCallsign>()) { return true; }

            return this->CValueObject::isA(metaTypeId);
        }

        /*
         * Register metadata
         */
        void CCallsign::registerMetadata()
        {
            qRegisterMetaType<CCallsign>();
            qDBusRegisterMetaType<CCallsign>();
        }

    } // namespace
} // namespace
