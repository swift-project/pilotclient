// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/aviation/aircraftlights.h"

#include <QStringBuilder>

#include "misc/comparefunctions.h"
#include "misc/stringutils.h"

SWIFT_DEFINE_VALUEOBJECT_MIXINS(swift::misc::aviation, CAircraftLights)

namespace swift::misc::aviation
{
    CAircraftLights::CAircraftLights(bool strobeOn, bool landingOn, bool taxiOn, bool beaconOn, bool navOn, bool logoOn)
        : m_strobeOn(strobeOn), m_landingOn(landingOn), m_taxiOn(taxiOn), m_beaconOn(beaconOn), m_navOn(navOn),
          m_logoOn(logoOn)
    {}

    CAircraftLights::CAircraftLights(bool strobeOn, bool landingOn, bool taxiOn, bool beaconOn, bool navOn, bool logoOn,
                                     bool recognition, bool cabin)
        : m_strobeOn(strobeOn), m_landingOn(landingOn), m_taxiOn(taxiOn), m_beaconOn(beaconOn), m_navOn(navOn),
          m_logoOn(logoOn), m_recognitionOn(recognition), m_cabinOn(cabin)
    {}

    CAircraftLights::CAircraftLights(bool strobeOn, bool landingOn, bool taxiOn, bool beaconOn, bool navOn, bool logoOn,
                                     bool recognition, bool cabin, bool wing)
        : m_strobeOn(strobeOn), m_landingOn(landingOn), m_taxiOn(taxiOn), m_beaconOn(beaconOn), m_navOn(navOn),
          m_logoOn(logoOn), m_recognitionOn(recognition), m_cabinOn(cabin), m_wingOn(wing)
    {}

    CAircraftLights CAircraftLights::allLightsOn()
    {
        return CAircraftLights { true, true, true, true, true, true, true, true, true };
    }

    CAircraftLights CAircraftLights::allLightsOff()
    {
        return CAircraftLights { false, false, false, false, false, false, false, false, false };
    }

    QString CAircraftLights::convertToQString(bool i18n) const
    {
        Q_UNUSED(i18n);
        const QString s = u"strobe: " % boolToYesNo(m_strobeOn) % u" landing: " % boolToYesNo(m_landingOn) %
                          u" taxi: " % boolToYesNo(m_taxiOn) % u" beacon: " % boolToYesNo(m_beaconOn) % u" nav: " %
                          boolToYesNo(m_navOn) % u" logo: " % boolToYesNo(m_logoOn) % u" recognition: " %
                          boolToYesNo(m_recognitionOn) % u" cabin: " % boolToYesNo(m_cabinOn) % u" cabin: " %
                          boolToYesNo(m_wingOn);
        return s;
    }

    QVariant CAircraftLights::propertyByIndex(swift::misc::CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }

        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexIsNull: return QVariant::fromValue(this->isNull());
        case IndexBeacon: return QVariant::fromValue(m_beaconOn);
        case IndexLanding: return QVariant::fromValue(m_landingOn);
        case IndexLogo: return QVariant::fromValue(m_logoOn);
        case IndexNav: return QVariant::fromValue(m_navOn);
        case IndexStrobe: return QVariant::fromValue(m_strobeOn);
        case IndexTaxi: return QVariant::fromValue(m_taxiOn);
        case IndexRecognition: return QVariant::fromValue(m_recognitionOn);
        case IndexCabin: return QVariant::fromValue(m_cabinOn);
        case IndexWing: return QVariant::fromValue(m_wingOn);
        default: return CValueObject::propertyByIndex(index);
        }
    }

    void CAircraftLights::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself())
        {
            (*this) = variant.value<CAircraftLights>();
            return;
        }

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
        case IndexCabin: m_cabinOn = variant.toBool(); break;
        case IndexRecognition: m_recognitionOn = variant.toBool(); break;
        case IndexWing: m_wingOn = variant.toBool(); break;
        default: CValueObject::setPropertyByIndex(index, variant); break;
        }
    }

    int CAircraftLights::comparePropertyByIndex(CPropertyIndexRef index, const CAircraftLights &compareValue) const
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
        case IndexCabin: return Compare::compare(m_cabinOn, compareValue.isCabinOn());
        case IndexRecognition: return Compare::compare(m_recognitionOn, compareValue.isRecognitionOn());
        case IndexWing: return Compare::compare(m_wingOn, compareValue.isWingOn());
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
        m_cabinOn = true;
        m_recognitionOn = true;
        m_wingOn = true;
    }

    void CAircraftLights::setAllOff()
    {
        m_beaconOn = false;
        m_landingOn = false;
        m_logoOn = false;
        m_navOn = false;
        m_strobeOn = false;
        m_taxiOn = false;
        m_recognitionOn = false;
        m_cabinOn = false;
        m_wingOn = false;
    }
} // namespace swift::misc::aviation
