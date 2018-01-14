/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVIATION_ALTITUDE_H
#define BLACKMISC_AVIATION_ALTITUDE_H

#include "blackmisc/pq/length.h"
#include "blackmisc/pq/pqstring.h"
#include "blackmisc/pq/units.h"
#include "blackmisc/statusmessagelist.h"
#include "blackmisc/compare.h"
#include "blackmisc/dbus.h"
#include "blackmisc/dictionary.h"
#include "blackmisc/icon.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/pq/pressure.h"
#include "blackmisc/propertyindexvariantmap.h"
#include "blackmisc/stringutils.h"
#include "blackmisc/variant.h"
#include "blackmisc/blackmiscexport.h"

#include <QMetaType>
#include <QString>
#include <QRegularExpression>

namespace BlackMisc
{
    namespace Aviation
    {
        /*!
         * Altitude as used in aviation, can be AGL or MSL altitude
         * \remarks Intentionally allowing +/- CLength , and >= / <= CLength.
         */
        class BLACKMISC_EXPORT CAltitude :
            public PhysicalQuantities::CLength,
            public Mixin::MetaType<CAltitude>,
            public Mixin::EqualsByMetaClass<CAltitude>,
            public Mixin::CompareByMetaClass<CAltitude>,
            public Mixin::HashByMetaClass<CAltitude>,
            public Mixin::DBusByMetaClass<CAltitude>,
            public Mixin::JsonByMetaClass<CAltitude>,
            public Mixin::String<CAltitude>,
            public Mixin::Icon<CAltitude>,
            public Mixin::Index<CAltitude>
        {
        public:
            //! Base type
            using base_type = PhysicalQuantities::CLength;

            BLACKMISC_DECLARE_USING_MIXIN_METATYPE(CAltitude)
            BLACKMISC_DECLARE_USING_MIXIN_STRING(CAltitude)
            BLACKMISC_DECLARE_USING_MIXIN_DBUS(CAltitude)
            BLACKMISC_DECLARE_USING_MIXIN_JSON(CAltitude)
            BLACKMISC_DECLARE_USING_MIXIN_ICON(CAltitude)
            BLACKMISC_DECLARE_USING_MIXIN_INDEX(CAltitude)

            /*!
             * Enum type to distinguish between MSL and AGL
             */
            enum ReferenceDatum
            {
                MeanSeaLevel = 0,   //!< MSL
                AboveGround,        //!< AGL
                FlightLevel         //!< Flight level
            };

            //! Altitude type
            enum AltitudeType
            {
                PressureAltitude,   //!< Altitude above the standard datum plane
                TrueAltitude        //!< Height of the airplane above Mean Sea Level (MSL)
            };

            //! \copydoc BlackMisc::Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

            //! Default constructor: 0m Altitude MSL
            CAltitude() : CLength(0, PhysicalQuantities::CLengthUnit::m()), m_datum(MeanSeaLevel) {}

            //! Constructor
            CAltitude(double value, ReferenceDatum datum, const PhysicalQuantities::CLengthUnit &unit) : CLength(value, unit), m_datum(datum) {}

            //! Constructor
            CAltitude(double value, ReferenceDatum datum, AltitudeType type, const PhysicalQuantities::CLengthUnit &unit) : CLength(value, unit), m_datum(datum), m_altitudeType(type) {}

            //! Constructor, value as CAltitude::MeanSeaLevel
            CAltitude(double value, const PhysicalQuantities::CLengthUnit &unit) : CLength(value, unit), m_datum(MeanSeaLevel) {}

            //! Altitude as string
            CAltitude(const QString &altitudeAsString, PhysicalQuantities::CPqString::SeparatorMode mode = PhysicalQuantities::CPqString::SeparatorsLocale);

            //! Constructor by CLength
            CAltitude(const PhysicalQuantities::CLength &altitude, ReferenceDatum datum) : CLength(altitude), m_datum(datum) {}

            //! AGL Above ground level?
            bool isAboveGroundLevel() const { return AboveGround == this->m_datum; }

            //! MSL Mean sea level?
            bool isMeanSeaLevel() const { return MeanSeaLevel == this->m_datum; }

            //! Flight level?
            bool isFlightLevel() const { return FlightLevel == this->m_datum; }

            //! Get reference datum (MSL or AGL)
            ReferenceDatum getReferenceDatum() const { return m_datum; }

            //! MSL to flightlevel
            void toFlightLevel();

            //! Flightlevel to MSL
            void toMeanSeaLevel();

            //! Current altitude type
            AltitudeType getAltitudeType() const { return m_altitudeType; }

            //! Converts this to pressure altitude. Requires the current barometric pressure at MSL
            void convertToPressureAltitude(const PhysicalQuantities::CPressure &seaLevelPressure);

            //! Returns the altitude converted to pressure altitude. Requires the current barometric pressure at MSL
            CAltitude toPressureAltitude(const PhysicalQuantities::CPressure &seaLevelPressure) const;

            //! Parse value from string
            void parseFromString(const QString &value);

            //! Parse value from string, with specified separator
            void parseFromString(const QString &value, PhysicalQuantities::CPqString::SeparatorMode mode);

            //! Parse from FP altitude string
            //! \sa CFlightPlan::asFpAltitudeString
            bool parseFromFpAltitudeString(const QString &value, CStatusMessageList *msgs = nullptr);

            //! Is this a valid FP altitude
            //! \sa CFlightPlan::asFpAltitudeString
            bool isValidFpAltitude(CStatusMessageList *msgs = nullptr) const;

            //! Altitude string (official version)
            //! * flight level, expressed as "F" followed by 3 figures, example: F085 (which means flight level 085),
            //! * standard metric level in tens of meters, expressed as "S" followed by 4 figures, example: S0150 (which means 1500 metres)
            //! * altitude in hundreds of feet, expressed as "A" followed by 3 figures, example: A055 (which means 5500 feet altitude)
            //! * altitude in tens of meters expressed as "M" followed by 4 figures, example: M0610 (which means 6100 metres altitude)
            QString asFpAltitudeString() const;

            //! As simple VATSIM string, only FLxxx or altitude as ft
            QString asFpAltitudeSimpleVatsimString() const;

            //! Checking FP altitude strings like "A20", "FL100"
            //! \sa CFlightPlan::asFpAltitudeString
            static const QRegularExpression &fpAltitudeRegExp();

            //! Info for FP altitude strings
            //! \sa CFlightPlan::asFpAltitudeString
            static QString fpAltitudeInfo(const QString &separator = ", ");

            //! Examples of FP altitude strings
            static QString fpAltitudeExamples();

            //! \copydoc BlackMisc::Mixin::Icon::toIcon
            BlackMisc::CIcon toIcon() const;

            //! Null altitude (MSL)
            static const CAltitude &null();

            //! Standard pressure 1013.25mbar/hPa
            static const PhysicalQuantities::CPressure &standardISASeaLevelPressure();

        private:
            ReferenceDatum m_datum; //!< MSL or AGL?
            AltitudeType m_altitudeType = TrueAltitude;

            BLACK_METACLASS(
                CAltitude,
                BLACK_METAMEMBER(datum),
                BLACK_METAMEMBER(altitudeType)
            );
        };
    } // ns
} // ns

Q_DECLARE_METATYPE(BlackMisc::Aviation::CAltitude)
Q_DECLARE_METATYPE(BlackMisc::Aviation::CAltitude::ReferenceDatum)
Q_DECLARE_METATYPE(BlackMisc::Aviation::CAltitude::AltitudeType)

#endif // guard
