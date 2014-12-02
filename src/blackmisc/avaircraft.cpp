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
#include "blackmisc/propertyindex.h"

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
        void CAircraft::setCockpit(const CComSystem &com1, const CComSystem &com2, int transponderCode, CTransponder::TransponderMode transponderMode)
        {
            this->setCom1System(com1);
            this->setCom2System(com2);
            this->m_transponder.setTransponderCode(transponderCode);
            this->m_transponder.setTransponderMode(transponderMode);
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
         * Property by index
         */
        CVariant CAircraft::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return this->toCVariant(); }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexCallsign:
                return this->m_callsign.propertyByIndex(index.copyFrontRemoved());
            case IndexPilot:
                return this->m_pilot.propertyByIndex(index.copyFrontRemoved());
            case IndexDistance:
                return this->m_distanceToPlane.propertyByIndex(index.copyFrontRemoved());
            case IndexCom1System:
                return this->m_com1system.propertyByIndex(index.copyFrontRemoved());
            case IndexCom2System:
                return this->m_com2system.propertyByIndex(index.copyFrontRemoved());
            case IndexTransponder:
                return this->m_transponder.propertyByIndex(index.copyFrontRemoved());
            case IndexSituation:
                return this->m_situation.propertyByIndex(index.copyFrontRemoved());
            case IndexIcao:
                return this->m_icao.propertyByIndex(index.copyFrontRemoved());
            default:
                return (ICoordinateGeodetic::canHandleIndex(index)) ?
                       ICoordinateGeodetic::propertyByIndex(index) :
                       CValueObject::propertyByIndex(index);
            }
        }

        /*
         * Property by index (setter)
         */
        void CAircraft::setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index)
        {
            if (index.isMyself())
            {
                this->convertFromCVariant(variant);
                return;
            }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexCallsign:
                this->m_callsign.setPropertyByIndex(variant, index.copyFrontRemoved());
                break;
            case IndexPilot:
                this->m_pilot.setPropertyByIndex(variant, index.copyFrontRemoved());
                break;
            case IndexDistance:
                this->m_distanceToPlane.setPropertyByIndex(variant, index.copyFrontRemoved());
                break;
            case IndexCom1System:
                this->m_com1system.setPropertyByIndex(variant, index.copyFrontRemoved());
                break;
            case IndexCom2System:
                this->m_com2system.setPropertyByIndex(variant, index.copyFrontRemoved());
                break;
            case IndexTransponder:
                this->m_transponder.setPropertyByIndex(variant, index.copyFrontRemoved());
                break;
            case IndexIcao:
                this->m_icao.setPropertyByIndex(variant, index.copyFrontRemoved());
                break;
            case IndexSituation:
                this->m_situation.setPropertyByIndex(variant, index.copyFrontRemoved());
                break;
            default:
                CValueObject::setPropertyByIndex(variant, index);
                break;
            }
        }


    } // namespace
} // namespace
