/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVIATION_AIRCRAFTLIGHTS_H
#define BLACKMISC_AVIATION_AIRCRAFTLIGHTS_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/valueobject.h"
#include "blackmisc/propertyindex.h"

namespace BlackMisc
{
    namespace Aviation
    {
        //! Value object encapsulating information about aircraft's lights
        class BLACKMISC_EXPORT CAircraftLights : public CValueObject<CAircraftLights>
        {
        public:
            //! Properties by index
            enum ColumnIndex
            {
                IndexStrobe = BlackMisc::CPropertyIndex::GlobalIndexCAircraftLights,
                IndexLanding,
                IndexTaxi,
                IndexBeacon,
                IndexNav,
                IndexLogo
            };


            //! Default constructor
            CAircraftLights() = default;

            //! Constructor
            CAircraftLights(bool strobeOn, bool landingOn, bool taxiOn, bool beaconOn, bool navOn, bool logoOn);

            //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
            CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

            //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index);

            //! Strobes lights on?
            bool isStrobeOn() const { return m_strobeOn; }

            //! Set strobe lights
            void setStrobeOn(bool on) { m_strobeOn = on; }

            //! Landing lights on?
            bool isLandingOn() const { return m_landingOn; }

            //! Set landing lights
            void setLandingOn(bool on) { m_landingOn = on; }

            //! Taxi lights on?
            bool isTaxiOn() const { return m_taxiOn; }

            //! Set taxi lights
            void setTaxiOn(bool on) { m_taxiOn = on; }

            //! Beacon lights on?
            bool isBeaconOn() const { return m_beaconOn; }

            //! Set beacon lights
            void setBeaconOn(bool on) { m_beaconOn = on; }

            //! Nac lights on?
            bool isNavOn() const { return m_navOn; }

            //! Set nav lights
            void setNavOn(bool on) { m_navOn = on; }

            //! Logo lights on?
            bool isLogoOn() const { return m_logoOn; }

            //! Set logo lights
            void setLogoOn(bool on) { m_logoOn = on; }

            //! All on
            void setAllOn();

            //! All off
            void setAllOff();

            //! Returns object with all lights switched on
            static CAircraftLights allLightsOn();

            //! Returns object with all lights switched off
            static CAircraftLights allLightsOff();

            //! \copydoc BlackMisc::Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

        private:
            bool m_strobeOn = false;
            bool m_landingOn = false;
            bool m_taxiOn = false;
            bool m_beaconOn = false;
            bool m_navOn = false;
            bool m_logoOn = false;

            BLACK_METACLASS(
                CAircraftLights,
                BLACK_METAMEMBER_NAMED(strobeOn, "strobe_on"),
                BLACK_METAMEMBER_NAMED(landingOn, "landing_on"),
                BLACK_METAMEMBER_NAMED(taxiOn, "taxi_on"),
                BLACK_METAMEMBER_NAMED(beaconOn, "beacon_on"),
                BLACK_METAMEMBER_NAMED(navOn, "nav_on"),
                BLACK_METAMEMBER_NAMED(logoOn, "logo_on")
            );
        };
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CAircraftLights)

#endif // guard
