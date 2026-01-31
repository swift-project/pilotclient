// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_AVIATION_AIRCRAFTLIGHTS_H
#define SWIFT_MISC_AVIATION_AIRCRAFTLIGHTS_H

#include <QMetaType>
#include <QString>

#include "misc/metaclass.h"
#include "misc/propertyindexref.h"
#include "misc/swiftmiscexport.h"
#include "misc/valueobject.h"

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::misc::aviation, CAircraftLights)

namespace swift::misc::aviation
{
    //! Value object encapsulating information about aircraft's lights
    class SWIFT_MISC_EXPORT CAircraftLights : public CValueObject<CAircraftLights>
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
        CAircraftLights(bool strobeOn, bool landingOn, bool taxiOn, bool beaconOn, bool navOn, bool logoOn,
                        bool recognition, bool cabin);

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
        bool isRecognitionOn() const { return m_recognitionOn; }

        //! Set recognition lights
        void setRecognitionOn(bool on) { m_recognitionOn = on; }

        //! Cabin lights on?
        bool isCabinOn() const { return m_cabinOn; }

        //! Set cabin lights
        void setCabinOn(bool on) { m_cabinOn = on; }

        //! All on
        void setAllOn();

        //! All off
        void setAllOff();

        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        QVariant propertyByIndex(CPropertyIndexRef index) const;

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
        void setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant);

        //! Compare by index
        int comparePropertyByIndex(CPropertyIndexRef index, const CAircraftLights &compareValue) const;

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! Returns object with all lights switched on
        static CAircraftLights allLightsOn();

        //! Returns object with all lights switched off
        static CAircraftLights allLightsOff();

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
        bool m_recognitionOn = false; //!< not supported by aircraft config (VATSIM)
        bool m_cabinOn = false; //!< not supported by aircraft config (VATSIM)

        SWIFT_METACLASS(
            CAircraftLights,
            SWIFT_METAMEMBER(isNull, 0, DisabledForJson), // disable since JSON is used for network
            SWIFT_METAMEMBER_NAMED(strobeOn, "strobe_on"),
            SWIFT_METAMEMBER_NAMED(landingOn, "landing_on"),
            SWIFT_METAMEMBER_NAMED(taxiOn, "taxi_on"),
            SWIFT_METAMEMBER_NAMED(beaconOn, "beacon_on"),
            SWIFT_METAMEMBER_NAMED(navOn, "nav_on"),
            SWIFT_METAMEMBER_NAMED(logoOn, "logo_on"),
            SWIFT_METAMEMBER(recognitionOn, 0, DisabledForJson), // disable since JSON is used for network
            SWIFT_METAMEMBER(cabinOn, 0, DisabledForJson) // disable since JSON is used for network
        );
    };
} // namespace swift::misc::aviation

Q_DECLARE_METATYPE(swift::misc::aviation::CAircraftLights)

#endif // SWIFT_MISC_AVIATION_AIRCRAFTLIGHTS_H
