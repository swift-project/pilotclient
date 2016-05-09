/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "aircraftlights.h"
#include "blackmisc/variant.h"
#include "blackmisc/stringutils.h"

using namespace BlackMisc;

namespace BlackMisc
{
    namespace Aviation
    {

        CAircraftLights::CAircraftLights(bool strobeOn, bool landingOn, bool taxiOn, bool beaconOn, bool navOn, bool logoOn)
            : m_strobeOn(strobeOn), m_landingOn(landingOn), m_taxiOn(taxiOn), m_beaconOn(beaconOn), m_navOn(navOn), m_logoOn(logoOn)
        { }

        CAircraftLights CAircraftLights::allLightsOn()
        {
            return CAircraftLights {true, true, true, true, true, true};
        }

        CAircraftLights CAircraftLights::allLightsOff()
        {
            return CAircraftLights {false, false, false, false, false, false};
        }

        QString CAircraftLights::convertToQString(bool /** i18n */) const
        {
            QString s;
            s += " strobe: ";
            s += boolToYesNo(m_strobeOn);
            s += " landing: ";
            s += boolToYesNo(m_landingOn);
            s += " taxi: ";
            s += boolToYesNo(m_taxiOn);
            s += " beacon: ";
            s += boolToYesNo(m_beaconOn);
            s += " nav: ";
            s += boolToYesNo(m_navOn);
            s += " logo: ";
            s += boolToYesNo(m_logoOn);
            return s;
        }

        CVariant CAircraftLights::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }

            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexBeacon:
                return CVariant::from(m_beaconOn);
            case IndexLanding:
                return CVariant::from(m_landingOn);
            case IndexLogo:
                return CVariant::from(m_logoOn);
            case IndexNav:
                return CVariant::from(m_navOn);
            case IndexStrobe:
                return CVariant::from(m_strobeOn);
            case IndexTaxi:
                return CVariant::from(m_taxiOn);
            default:
                return CValueObject::propertyByIndex(index);
            }
        }

        void CAircraftLights::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.to<CAircraftLights>(); return; }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexBeacon:
                this->m_beaconOn = variant.toBool();
                break;
            case IndexLanding:
                this->m_landingOn = variant.toBool();
                break;
            case IndexLogo:
                this->m_logoOn = variant.toBool();
                break;
            case IndexNav:
                this->m_navOn = variant.toBool();
                break;
            case IndexStrobe:
                this->m_strobeOn = variant.toBool();
                break;
            case IndexTaxi:
                this->m_taxiOn = variant.toBool();
                break;
            default:
                CValueObject::setPropertyByIndex(index, variant);
                break;
            }
        }

        void CAircraftLights::setAllOn()
        {
            m_beaconOn = true;
            m_landingOn = true;
            m_logoOn = true;
            m_navOn = true;
            m_strobeOn = true;
            m_taxiOn = true;
        }

        void CAircraftLights::setAllOff()
        {
            m_beaconOn = false;
            m_landingOn = false;
            m_logoOn = false;
            m_navOn = false;
            m_strobeOn = false;
            m_taxiOn = false;
        }

    } // namespace
} // namespace
