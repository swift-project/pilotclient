/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/aviation/aircraftlights.h"
#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/stringutils.h"
#include "blackmisc/comparefunctions.h"
#include "blackmisc/variant.h"
#include <QStringBuilder>

using namespace BlackMisc::PhysicalQuantities;

namespace BlackMisc
{
    namespace Aviation
    {
        CAircraftLights::CAircraftLights(bool strobeOn, bool landingOn, bool taxiOn, bool beaconOn, bool navOn, bool logoOn)
            : m_strobeOn(strobeOn), m_landingOn(landingOn), m_taxiOn(taxiOn), m_beaconOn(beaconOn), m_navOn(navOn), m_logoOn(logoOn)
        { }

        CAircraftLights::CAircraftLights(bool strobeOn, bool landingOn, bool taxiOn, bool beaconOn, bool navOn, bool logoOn, bool recognition, bool cabin)
            : m_strobeOn(strobeOn), m_landingOn(landingOn), m_taxiOn(taxiOn), m_beaconOn(beaconOn), m_navOn(navOn), m_logoOn(logoOn), m_recognition(recognition), m_cabin(cabin)
        { }

        CAircraftLights CAircraftLights::allLightsOn()
        {
            return CAircraftLights {true, true, true, true, true, true, true, true};
        }

        CAircraftLights CAircraftLights::allLightsOff()
        {
            return CAircraftLights {false, false, false, false, false, false, false, false};
        }

        CAircraftLights CAircraftLights::guessedLights(const CAircraftSituation &situation)
        {
            const bool isOnGround = situation.getOnGround() == CAircraftSituation::OnGround;
            const double gsKts = situation.getGroundSpeed().value(CSpeedUnit::kts());
            CAircraftLights lights;
            lights.setCabinOn(true);
            lights.setRecognitionOn(true);

            // when first detected moving, lights on
            if (isOnGround)
            {
                lights.setTaxiOn(true);
                lights.setBeaconOn(true);
                lights.setNavOn(true);

                if (gsKts > 5)
                {
                    // mode taxi
                    lights.setTaxiOn(true);
                    lights.setLandingOn(false);
                }
                else if (gsKts > 30)
                {
                    // mode accelaration for takeoff
                    lights.setTaxiOn(false);
                    lights.setLandingOn(true);
                }
                else
                {
                    // slow movements or parking
                    lights.setTaxiOn(false);
                    lights.setLandingOn(false);
                }
            }
            else
            {
                // not on ground
                lights.setTaxiOn(false);
                lights.setBeaconOn(true);
                lights.setNavOn(true);
                // landing lights for < 10000ft (normally MSL, here ignored)
                lights.setLandingOn(situation.getAltitude().value(CLengthUnit::ft()) < 10000);
            }
            return lights;
        }

        QString CAircraftLights::convertToQString(bool i18n) const
        {
            Q_UNUSED(i18n);
            const QString s = u"strobe: " % boolToYesNo(m_strobeOn) %
                              u" landing: " % boolToYesNo(m_landingOn) %
                              u" taxi: " % boolToYesNo(m_taxiOn) %
                              u" beacon: " % boolToYesNo(m_beaconOn) %
                              u" nav: " % boolToYesNo(m_navOn) %
                              u" logo: " % boolToYesNo(m_logoOn) %
                              u" recognition: " % boolToYesNo(m_recognition) %
                              u" cabin: " % boolToYesNo(m_cabin);
            return s;
        }

        CVariant CAircraftLights::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }

            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexIsNull: return CVariant::from(this->isNull());
            case IndexBeacon: return CVariant::from(m_beaconOn);
            case IndexLanding: return CVariant::from(m_landingOn);
            case IndexLogo: return CVariant::from(m_logoOn);
            case IndexNav: return CVariant::from(m_navOn);
            case IndexStrobe: return CVariant::from(m_strobeOn);
            case IndexTaxi: return CVariant::from(m_taxiOn);
            case IndexRecognition: return CVariant::from(m_recognition);
            case IndexCabin: return CVariant::from(m_cabin);
            default: return CValueObject::propertyByIndex(index);
            }
        }

        void CAircraftLights::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.to<CAircraftLights>(); return; }

            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexIsNull: m_isNull = variant.toBool(); break;
            case IndexBeacon: m_beaconOn = variant.toBool(); break;
            case IndexLanding: m_landingOn = variant.toBool(); break;
            case IndexLogo: m_logoOn = variant.toBool(); break;
            case IndexNav: m_navOn = variant.toBool(); break;
            case IndexStrobe: m_strobeOn = variant.toBool(); break;
            case IndexTaxi: m_taxiOn = variant.toBool(); break;
            case IndexCabin: m_cabin = variant.toBool(); break;
            case IndexRecognition: m_recognition = variant.toBool(); break;
            default: CValueObject::setPropertyByIndex(index, variant); break;
            }
        }

        int CAircraftLights::comparePropertyByIndex(const CPropertyIndex &index, const CAircraftLights &compareValue) const
        {
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexIsNull: return Compare::compare(m_isNull, compareValue.isNull());
            case IndexBeacon: return Compare::compare(m_beaconOn, compareValue.isBeaconOn());
            case IndexLanding: return Compare::compare(m_landingOn, compareValue.isLandingOn());
            case IndexLogo: return Compare::compare(m_logoOn, compareValue.isLogoOn());
            case IndexNav: return Compare::compare(m_navOn, compareValue.isNavOn());
            case IndexStrobe: return Compare::compare(m_strobeOn, compareValue.isStrobeOn());
            case IndexTaxi: return Compare::compare(m_taxiOn, compareValue.isTaxiOn());
            case IndexCabin: return Compare::compare(m_cabin, compareValue.isCabinOn());
            case IndexRecognition: return Compare::compare(m_recognition, compareValue.isRecognitionOn());
            default: break;
            }
            return 0;
        }

        void CAircraftLights::setAllOn()
        {
            m_beaconOn = true;
            m_landingOn = true;
            m_logoOn = true;
            m_navOn = true;
            m_strobeOn = true;
            m_taxiOn = true;
            m_cabin = true;
            m_recognition = true;
        }

        void CAircraftLights::setAllOff()
        {
            m_beaconOn = false;
            m_landingOn = false;
            m_logoOn = false;
            m_navOn = false;
            m_strobeOn = false;
            m_taxiOn = false;
            m_recognition = false;
            m_cabin = false;
        }

        void CAircraftLights::guessLights(const CAircraftSituation &situation)
        {
            *this = guessedLights(situation);
        }
    } // namespace
} // namespace
