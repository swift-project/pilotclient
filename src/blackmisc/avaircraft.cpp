/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "avaircraft.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include "blackmisc/pqconstants.h"
#include "blackmisc/icon.h"

using namespace BlackMisc::PhysicalQuantities;

namespace BlackMisc
{
    namespace Aviation
    {
        CAircraft::CAircraft(const CCallsign &callsign, const Network::CUser &user, const CAircraftSituation &situation)
            : m_callsign(callsign), m_pilot(user), m_situation(situation), m_distanceToPlane(0, BlackMisc::PhysicalQuantities::CLengthUnit::nullUnit())
        {
            // sync callsigns
            if (!this->m_pilot.hasValidCallsign() && !callsign.isEmpty())
                this->m_pilot.setCallsign(callsign);
        }

        /*
         * Convert to string
         */
        QString CAircraft::convertToQString(bool i18n) const
        {
            QString s(this->m_callsign.toQString(i18n));
            s.append(" ").append(this->m_pilot.toQString(i18n));
            s.append(" ").append(this->m_situation.toQString(i18n));
            s.append(" ").append(this->m_com1system.toQString(i18n));
            s.append(" ").append(this->m_com2system.toQString(i18n));
            s.append(" ").append(this->m_transponder.toQString(i18n));
            return s;
        }

        /*
         * Marshall to DBus
         */
        void CAircraft::marshallToDbus(QDBusArgument &argument) const
        {
            argument << TupleConverter<CAircraft>::toTuple(*this);
        }

        /*
         * Unmarshall from DBus
         */
        void CAircraft::unmarshallFromDbus(const QDBusArgument &argument)
        {
            argument >> TupleConverter<CAircraft>::toTuple(*this);
        }

        /*
         * Distance to plane
         */
        const PhysicalQuantities::CLength &CAircraft::setCalculcatedDistanceToPosition(const Geo::CCoordinateGeodetic &position)
        {
            this->m_distanceToPlane = Geo::greatCircleDistance(position, this->m_situation.getPosition());
            return this->m_distanceToPlane;
        }

        /*
         * Set cockpit data
         */
        void CAircraft::setCockpit(const CComSystem &com1, const CComSystem &com2, const CTransponder &transponder)
        {
            this->setCom1System(com1);
            this->setCom2System(com2);
            this->setTransponder(transponder);
        }

        /*
         * Set cockpit data
         */
        void CAircraft::setCockpit(const CComSystem &com1, const CComSystem &com2, qint32 transponderCode)
        {
            this->setCom1System(com1);
            this->setCom2System(com2);
            this->m_transponder.setTransponderCode(transponderCode);
        }


        /*
         * Changed data
         */
        bool CAircraft::hasChangedCockpitData(const CComSystem &com1, const CComSystem &com2, const CTransponder &transponder) const
        {
            return this->getCom1System() != com1 || this->getCom2System() != com2 || this->getTransponder() != transponder;
        }

        /*
         * Distance to plane
         */
        PhysicalQuantities::CLength CAircraft::calculcateDistanceToPosition(const Geo::CCoordinateGeodetic &position) const
        {
            return Geo::greatCircleDistance(position, this->m_situation.getPosition());
        }


        /*
         * Same COM system data
         */
        bool CAircraft::hasSameComData(const CComSystem &com1, const CComSystem &com2, const CTransponder &transponder)
        {
            return this->getCom1System() == com1 && this->getCom2System() == com2 && this->getTransponder() == transponder;
        }

        /*
         * Valid for login
         */
        bool CAircraft::isValidForLogin() const
        {
            if (this->m_callsign.asString().isEmpty()) return false;
            if (!this->m_pilot.isValid()) return false;
            return true;
        }

        /*
         * Meaningful values
         */
        void CAircraft::initComSystems()
        {
            CComSystem com1("COM1", CPhysicalQuantitiesConstants::FrequencyUnicom(), CPhysicalQuantitiesConstants::FrequencyUnicom());
            CComSystem com2("COM2", CPhysicalQuantitiesConstants::FrequencyUnicom(), CPhysicalQuantitiesConstants::FrequencyUnicom());
            this->setCom1System(com1);
            this->setCom2System(com2);
        }

        /*
         * Meaningful values
         */
        void CAircraft::initTransponder()
        {
            CTransponder xpdr("TRANSPONDER", 7000, CTransponder::StateStandby);
            this->setTransponder(xpdr);
        }

        /*
         * Equal?
         */
        bool CAircraft::operator ==(const CAircraft &other) const
        {
            if (this == &other) return true;
            return TupleConverter<CAircraft>::toTuple(*this) == TupleConverter<CAircraft>::toTuple(other);
        }

        /*
         * Unequal?
         */
        bool CAircraft::operator !=(const CAircraft &other) const
        {
            return !((*this) == other);
        }

        /*
         * Hash
         */
        uint CAircraft::getValueHash() const
        {
            return qHash(TupleConverter<CAircraft>::toTuple(*this));
        }

        /*
         * metaTypeId
         */
        int CAircraft::getMetaTypeId() const
        {
            return qMetaTypeId<CAircraft>();
        }

        /*
         * is a
         */
        bool CAircraft::isA(int metaTypeId) const
        {
            if (metaTypeId == qMetaTypeId<CAircraft>()) { return true; }

            return this->CValueObject::isA(metaTypeId);
        }

        /*
         * Compare
         */
        int CAircraft::compareImpl(const CValueObject &otherBase) const
        {
            const auto &other = static_cast<const CAircraft &>(otherBase);
            return compare(TupleConverter<CAircraft>::toTuple(*this), TupleConverter<CAircraft>::toTuple(other));
        }

        /*
         * Property by index
         */
        QVariant CAircraft::propertyByIndex(int index) const
        {
            if (index >= static_cast<int>(CAircraftIcao::IndexAircraftDesignator))
                return this->m_icao.propertyByIndex(index);

            if (index >= static_cast<int>(CAircraftSituation::IndexPosition))
                return this->m_situation.propertyByIndex(index);

            switch (index)
            {
            case IndexCallsign:
                return QVariant::fromValue(this->m_callsign);
            case IndexPixmap:
                return QVariant(this->m_callsign.toPixmap());
            case IndexCallsignAsString:
                return QVariant::fromValue(this->m_callsign.toQString(true));
            case IndexCallsignAsStringAsSet:
                return QVariant(this->m_callsign.getStringAsSet());
            case IndexPilotId:
                return QVariant::fromValue(this->m_pilot.getId());
            case IndexPilotRealName:
                return QVariant::fromValue(this->m_pilot.getRealName());
            case IndexDistance:
                return this->m_distanceToPlane.toQVariant();
            case IndexCom1System:
                return this->m_com1system.toQVariant();
            case IndexFrequencyCom1:
                return this->m_com1system.getFrequencyActive().toQVariant();
            case IndexTransponder:
                return this->m_transponder.toQVariant();
            case IndexTansponderFormatted:
                return QVariant::fromValue(this->m_transponder.getTransponderCodeAndModeFormatted());
            case IndexSituation:
                return this->m_situation.toQVariant();
            case IndexIcao:
                return this->m_icao.toQVariant();
            default:
                break;
            }

            Q_ASSERT_X(false, "CAircraft", "index unknown");
            QString m = QString("no property, index ").append(QString::number(index));
            return QVariant::fromValue(m);
        }

        /*
         * Property as string by index
         */
        QString CAircraft::propertyByIndexAsString(int index, bool i18n) const
        {
            QVariant qv = this->propertyByIndex(index);
            // special treatment
            // this is required, as it is possible that an aircraft is not
            // containing all properties
            switch (index)
            {
            case IndexFrequencyCom1:
                if (!CComSystem::isValidCivilAviationFrequency(qv.value<CFrequency>()))
                    return "";
                else
                    return qv.value<CFrequency>().valueRoundedWithUnit(3, i18n);
                break;
            case IndexDistance:
                {
                    CLength distance = qv.value<CLength>();
                    if (distance.isNegativeWithEpsilonConsidered()) return "";
                    return distance.toQString(i18n);
                }
            default:
                break;
            }
            return BlackMisc::qVariantToString(qv, i18n);
        }

        /*
         * Property by index (setter)
         */
        void CAircraft::setPropertyByIndex(const QVariant &variant, int index)
        {
            if (index >= static_cast<int>(CAircraftIcao::IndexAircraftDesignator))
                return this->m_icao.setPropertyByIndex(variant, index);

            if (index >= static_cast<int>(CAircraftSituation::IndexPosition))
                return this->m_situation.setPropertyByIndex(variant, index);

            switch (index)
            {
            case IndexCallsign:
                this->setCallsign(variant.value<CCallsign>());
                break;
            case IndexCallsignAsString:
                this->m_callsign = CCallsign(variant.value<QString>());
                break;
            case IndexPilotRealName:
                this->m_pilot.setRealName(variant.value<QString>());
                break;
            case IndexDistance:
                this->m_distanceToPlane = variant.value<CLength>();
                break;
            case IndexCom1System:
                this->setCom1System(variant.value<CComSystem>());
                break;
            case IndexFrequencyCom1:
                this->m_com1system.setFrequencyActive(variant.value<CFrequency>());
                break;
            case IndexTransponder:
                this->setTransponder(variant.value<CTransponder>());
                break;
            case IndexIcao:
                this->setIcaoInfo(variant.value<CAircraftIcao>());
                break;
            case IndexSituation:
                this->setSituation(variant.value<CAircraftSituation>());
                break;
            default:
                Q_ASSERT_X(false, "CAircraft", "index unknown");
                break;
            }
        }

        /*
         * Register metadata
         */
        void CAircraft::registerMetadata()
        {
            qRegisterMetaType<CAircraft>();
            qDBusRegisterMetaType<CAircraft>();
        }

        /*
         * To JSON
         */
        QJsonObject CAircraft::toJson() const
        {
            return BlackMisc::serializeJson(TupleConverter<CAircraft>::toMetaTuple(*this));
        }

        /*
         * From JSON
         */
        void CAircraft::fromJson(const QJsonObject &json)
        {
            BlackMisc::deserializeJson(json, TupleConverter<CAircraft>::toMetaTuple(*this));
        }

        /*
         * Members
         */
        const QStringList &CAircraft::jsonMembers()
        {
            return TupleConverter<CAircraft>::jsonMembers();
        }

    } // namespace
} // namespace
