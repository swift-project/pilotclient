//  SPDX-FileCopyrightText: Copyright (C) swift Project Community / Contributors
//  SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#ifndef SWIFT_MISC_AVIATION_FLIGHTPLAN_AIRCRAFT_INFO_H
#define SWIFT_MISC_AVIATION_FLIGHTPLAN_AIRCRAFT_INFO_H

#include "misc/swiftmiscexport.h"
#include "misc/valueobject.h"
#include "misc/aviation/aircrafticaocode.h"
#include "misc/aviation/comnavequipment.h"
#include "misc/aviation/ssrequipment.h"
#include "misc/aviation/waketurbulencecategory.h"

#include <QString>

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::misc::aviation, CFlightPlanAircraftInfo)

namespace swift::misc::aviation
{
    //! Flightplan-related information about an aircraft (aircraft ICAO, equipment and WTC)
    class SWIFT_MISC_EXPORT CFlightPlanAircraftInfo : public CValueObject<CFlightPlanAircraftInfo>
    {
    public:
        CFlightPlanAircraftInfo() = default;

        //! Create info from given aircraft ICAO, equipment and wake turbulence category
        CFlightPlanAircraftInfo(const CAircraftIcaoCode &aircraftIcao, const CComNavEquipment &comNavEquipment,
                                const CSsrEquipment &ssrEquipment, const CWakeTurbulenceCategory &wtc);

        //! Create aircraft info from a string that contains the 4 parts of an ICAO equipment code (AIRCRAFT_ICAO/WTC-EQUIPMENT/SSR).
        //! Passing FAA equipment codes like "H/B772/F" is supported as well
        explicit CFlightPlanAircraftInfo(QString equipmentCodeAndAircraft);

        //! Full string in ICAO format: "AIRCRAFT_ICAO/WTC-EQUIPMENT/SSR"
        QString asIcaoString() const;

        //! Full string in FAA format: "H/J (if heavy/super)/AIRCRAFT_ICAO/EQUIPMENT-CODE"
        QString asFaaString() const;

        //! Get Aircraft ICAO
        CAircraftIcaoCode getAircraftIcao() const;

        //! Get COM/NAV equipment
        CComNavEquipment getComNavEquipment() const;

        //! Get SSR equipment
        CSsrEquipment getSsrEquipment() const;

        //! Get Wake Turbulence Category
        CWakeTurbulenceCategory getWtc() const;

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! Transform single character FAA equipment code to ICAO-based COM/NAV and SSR equipment
        static std::tuple<CComNavEquipment, CSsrEquipment> faaCodeToEquipment(QChar equipmentCode);

        //! Transform ICAO-based COM/NAV and SSR equipment to a single character FAA equipment code
        static QChar equipmentToFaaCode(const CComNavEquipment &equip, const CSsrEquipment &ssr);

    private:
        CAircraftIcaoCode m_aircraftIcao; //!< Aircraft ICAO code
        CComNavEquipment m_comNavEquipment; //!< COM & NAV equipment (flight plan field 10a)
        CSsrEquipment m_ssrEquipment; //!< secondary surveillance radar equipment (flight plan field 10b)
        CWakeTurbulenceCategory m_wtc; //!< wake turbulence category. This information is also part of m_aircraftIcao, but is not always filled.

        void parseIcaoEquipmentCode(const QString &equipment); //!< Initialize members from ICAO format equipment codes
        void parseFaaEquipmentCode(const QString &equipment); //!< Initialize members from FAA format equipment codes
        void parseUnknownEquipmentCode(const QString &equipment); //!< Initialize members from unknown format equipment strings (best guesses)

        SWIFT_METACLASS(
            CFlightPlanAircraftInfo,
            SWIFT_METAMEMBER(aircraftIcao),
            SWIFT_METAMEMBER(comNavEquipment),
            SWIFT_METAMEMBER(ssrEquipment),
            SWIFT_METAMEMBER(wtc));
    };

}

Q_DECLARE_METATYPE(swift::misc::aviation::CFlightPlanAircraftInfo)

#endif // SWIFT_MISC_AVIATION_FLIGHTPLAN_AIRCRAFT_INFO_H
