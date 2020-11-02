/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVIATION_AIRCRAFTLIGHTS_H
#define BLACKMISC_AVIATION_AIRCRAFTLIGHTS_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/propertyindexref.h"
#include "blackmisc/valueobject.h"

#include <QMetaType>
#include <QString>

namespace BlackMisc
{
    namespace Aviation
    {
        class CAircraftSituation;

        //! Value object encapsulating information about aircraft's lights
        class BLACKMISC_EXPORT CAircraftLights : public CValueObject<CAircraftLights>
        {
        public:
            //! Properties by index
            enum ColumnIndex
            {
                IndexIsNull = CPropertyIndexRef::GlobalIndexINullable,
                IndexStrobe = CPropertyIndexRef::GlobalIndexCAircraftLights,
                IndexLanding,
                IndexTaxi,
                IndexBeacon,
                IndexNav,
                IndexLogo,
                IndexRecognition,
                IndexCabin
            };

            //! Default constructor
            CAircraftLights() = default;

            //! Null constructor
            CAircraftLights(std::nullptr_t) : m_isNull(true) {}

            //! Constructor
            CAircraftLights(bool strobeOn, bool landingOn, bool taxiOn, bool beaconOn, bool navOn, bool logoOn);

            //! Constructor
            CAircraftLights(bool strobeOn, bool landingOn, bool taxiOn, bool beaconOn, bool navOn, bool logoOn, bool recognition, bool cabin);

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

            //! Recognition lights on?
            bool isRecognitionOn() const { return m_recognition; }

            //! Set recognition lights
            void setRecognitionOn(bool on) { m_recognition = on; }

            //! Cabin lights on?
            bool isCabinOn() const { return m_cabin; }

            //! Set cabin lights
            void setCabinOn(bool on) { m_cabin = on; }

            //! All on
            void setAllOn();

            //! All off
            void setAllOff();

            //! Guess the lights
            void guessLights(const CAircraftSituation &situation);

            //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
            QVariant propertyByIndex(CPropertyIndexRef index) const;

            //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

            //! Compare by index
            int comparePropertyByIndex(CPropertyIndexRef index, const CAircraftLights &compareValue) const;

            //! \copydoc BlackMisc::Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

            //! Returns object with all lights switched on
            static CAircraftLights allLightsOn();

            //! Returns object with all lights switched off
            static CAircraftLights allLightsOff();

            //! Guessed lights
            static CAircraftLights guessedLights(const CAircraftSituation &situation);

            //! Null?
            bool isNull() const { return m_isNull; }

            //! Null?
            void setNull(bool null) { m_isNull = null; }

        private:
            bool m_isNull = false; //!< null?
            bool m_strobeOn = false;
            bool m_landingOn = false;
            bool m_taxiOn = false;
            bool m_beaconOn = false;
            bool m_navOn = false;
            bool m_logoOn = false;
            bool m_recognition = false; //!< not supported by aircraft config (VATSIM)
            bool m_cabin = false;       //!< not supported by aircraft config (VATSIM)

            BLACK_METACLASS(
                CAircraftLights,
                BLACK_METAMEMBER(isNull, 0, DisabledForJson), // disable since JSON is used for network
                BLACK_METAMEMBER_NAMED(strobeOn, "strobe_on"),
                BLACK_METAMEMBER_NAMED(landingOn, "landing_on"),
                BLACK_METAMEMBER_NAMED(taxiOn, "taxi_on"),
                BLACK_METAMEMBER_NAMED(beaconOn, "beacon_on"),
                BLACK_METAMEMBER_NAMED(navOn, "nav_on"),
                BLACK_METAMEMBER_NAMED(logoOn, "logo_on"),
                BLACK_METAMEMBER(recognition, 0, DisabledForJson), // disable since JSON is used for network
                BLACK_METAMEMBER(cabin, 0, DisabledForJson) // disable since JSON is used for network
            );
        };
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CAircraftLights)

#endif // guard
