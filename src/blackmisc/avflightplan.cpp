/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "avflightplan.h"
#include "iconlist.h"

namespace BlackMisc
{
    namespace Aviation
    {

        bool CFlightPlan::operator ==(const CFlightPlan &other) const
        {
            return TupleConverter<CFlightPlan>::toTuple(*this) == TupleConverter<CFlightPlan>::toTuple(other);
        }

        bool CFlightPlan::operator !=(const CFlightPlan &other) const
        {
            return !(*this == other);
        }

        uint CFlightPlan::getValueHash() const
        {
            return qHash(TupleConverter<CFlightPlan>::toTuple(*this));
        }

        QString CFlightPlan::convertToQString(bool i18n) const
        {
            QString s;
            s.append(m_equipmentIcao);
            s.append(" ").append(m_originAirportIcao.toQString(i18n));
            s.append(" ").append(m_destinationAirportIcao.toQString(i18n));
            s.append(" ").append(m_alternateAirportIcao.toQString(i18n));
            s.append(" ").append(m_takeoffTimePlanned.toString("ddhhmm"));
            s.append(" ").append(m_takeoffTimeActual.toString("ddhhmm"));
            s.append(" ").append(m_enrouteTime.toQString(i18n));
            s.append(" ").append(m_fuelTime.toQString(i18n));
            s.append(" ").append(m_cruiseAltitude.toQString(i18n));
            s.append(" ").append(m_cruiseTrueAirspeed.toQString(i18n));
            switch (m_flightRules)
            {
            case VFR:   s.append(" VFR"); break;
            case IFR:   s.append(" IFR"); break;
            case SVFR:  s.append(" SVFR"); break;
            default:    s.append(" ???"); break;
            }
            s.append(" ").append(m_route);
            s.append(" / ").append(m_remarks);
            return s;
        }

        int CFlightPlan::getMetaTypeId() const
        {
            return qMetaTypeId<CFlightPlan>();
        }

        bool CFlightPlan::isA(int metaTypeId) const
        {
            if (metaTypeId == qMetaTypeId<CFlightPlan>()) { return true; }

            return CValueObject::isA(metaTypeId);
        }

        int CFlightPlan::compareImpl(const CValueObject &otherBase) const
        {
            const auto &other = static_cast<const CFlightPlan &>(otherBase);

            return compare(TupleConverter<CFlightPlan>::toTuple(*this), TupleConverter<CFlightPlan>::toTuple(other));
        }

        void CFlightPlan::marshallToDbus(QDBusArgument &argument) const
        {
            argument << TupleConverter<CFlightPlan>::toTuple(*this);
        }

        void CFlightPlan::unmarshallFromDbus(const QDBusArgument &argument)
        {
            argument >> TupleConverter<CFlightPlan>::toTuple(*this);
        }

        const QStringList &CFlightPlan::jsonMembers()
        {
            return TupleConverter<CFlightPlan>::jsonMembers();
        }

        QJsonObject CFlightPlan::toJson() const
        {
            return BlackMisc::serializeJson(CFlightPlan::jsonMembers(), TupleConverter<CFlightPlan>::toTuple(*this));
        }

        void CFlightPlan::fromJson(const QJsonObject &json)
        {
            BlackMisc::deserializeJson(json, CFlightPlan::jsonMembers(), TupleConverter<CFlightPlan>::toTuple(*this));
        }

        BlackMisc::CIcon CFlightPlan::toIcon() const
        {
            return BlackMisc::CIconList::iconForIndex(CIcons::StandardIconAppFlightPlan16);
        }

        void CFlightPlan::registerMetadata()
        {
            qRegisterMetaType<CFlightPlan>();
            qDBusRegisterMetaType<CFlightPlan>();
        }

    } // namespace
} // namespace
