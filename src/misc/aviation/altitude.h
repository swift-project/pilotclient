// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_AVIATION_ALTITUDE_H
#define SWIFT_MISC_AVIATION_ALTITUDE_H

#include "misc/pq/length.h"
#include "misc/pq/pqstring.h"
#include "misc/pq/units.h"
#include "misc/statusmessagelist.h"
#include "misc/mixin/mixincompare.h"
#include "misc/mixin/mixindbus.h"
#include "misc/mixin/mixindatastream.h"
#include "misc/mixin/mixinhash.h"
#include "misc/mixin/mixinicon.h"
#include "misc/metaclass.h"
#include "misc/pq/pressure.h"
#include "misc/mixin/mixinindex.h"
#include "misc/mixin/mixinstring.h"
#include "misc/mixin/mixinmetatype.h"
#include "misc/swiftmiscexport.h"

#include <QPair>
#include <QMetaType>
#include <QString>
#include <QRegularExpression>
#include <QVector>
#include <tuple>

namespace swift::misc::aviation
{
    /*!
     * Altitude as used in aviation, can be AGL or MSL altitude
     * \remarks Intentionally allowing +/- CLength , and >= / <= CLength.
     */
    class SWIFT_MISC_EXPORT CAltitude :
        public physical_quantities::CLength,
        public mixin::MetaType<CAltitude>,
        public mixin::EqualsByMetaClass<CAltitude>,
        // public mixin::CompareByMetaClass<CAltitude>,
        public mixin::HashByMetaClass<CAltitude>,
        public mixin::DBusByMetaClass<CAltitude>,
        public mixin::DBusByMetaClass<CAltitude, LosslessTag>,
        public mixin::DataStreamByMetaClass<CAltitude>,
        public mixin::JsonByMetaClass<CAltitude>,
        public mixin::String<CAltitude>,
        public mixin::Icon<CAltitude>,
        public mixin::Index<CAltitude>
    {
    public:
        //! Base type
        using base_type = physical_quantities::CLength;

        //! Metric tuple
        using MetricTuple = std::tuple<int, int>;

        SWIFT_MISC_DECLARE_USING_MIXIN_METATYPE(CAltitude)
        SWIFT_MISC_DECLARE_USING_MIXIN_STRING(CAltitude)
        SWIFT_MISC_DECLARE_USING_MIXIN_DBUS(CAltitude)
        SWIFT_MISC_DECLARE_USING_MIXIN_DBUS(CAltitude, LosslessTag)
        SWIFT_MISC_DECLARE_USING_MIXIN_DATASTREAM(CAltitude)
        SWIFT_MISC_DECLARE_USING_MIXIN_JSON(CAltitude)
        SWIFT_MISC_DECLARE_USING_MIXIN_ICON(CAltitude)
        SWIFT_MISC_DECLARE_USING_MIXIN_INDEX(CAltitude)

        /*!
         * Enum type to distinguish between MSL and AGL
         */
        enum ReferenceDatum
        {
            MeanSeaLevel = 0, //!< MSL
            AboveGround, //!< AGL
            FlightLevel //!< Flight level
        };

        //! Altitude type
        enum AltitudeType
        {
            PressureAltitude, //!< Altitude above the standard datum plane
            TrueAltitude //!< Height of the airplane above Mean Sea Level (MSL)
        };

        //! Default constructor: 0m Altitude MSL
        CAltitude() : CLength(0, physical_quantities::CLengthUnit::m()), m_datum(MeanSeaLevel) {}

        //! Constructor
        CAltitude(double value, ReferenceDatum datum, const physical_quantities::CLengthUnit &unit) : CLength(value, unit), m_datum(datum) {}

        //! Constructor
        CAltitude(double value, ReferenceDatum datum, AltitudeType type, const physical_quantities::CLengthUnit &unit) : CLength(value, unit), m_datum(datum), m_altitudeType(type) {}

        //! Constructor, value as CAltitude::MeanSeaLevel
        CAltitude(double value, const physical_quantities::CLengthUnit &unit) : CLength(value, unit), m_datum(MeanSeaLevel) {}

        //! Constructor, value as CAltitude::MeanSeaLevel
        CAltitude(double value, const physical_quantities::CLengthUnit &unit, const physical_quantities::CLengthUnit &switchUnit) : CLength(value, unit), m_datum(MeanSeaLevel)
        {
            this->switchUnit(switchUnit);
        }

        //! Altitude as string
        CAltitude(const QString &altitudeAsString, physical_quantities::CPqString::SeparatorMode mode = physical_quantities::CPqString::SeparatorBestGuess);

        //! Constructor by CLength
        CAltitude(const physical_quantities::CLength &altitude, ReferenceDatum datum) : CLength(altitude), m_datum(datum) {}

        //! Altitude with offset
        //! \remark null offset adds nothing
        //! \remark epsilon 0 (zero) values ignored
        CAltitude withOffset(const CLength &offset) const;

        //! Add offset value
        void addOffset(const CLength &offset);

        //! Value in switched unit
        CAltitude &switchUnit(const physical_quantities::CLengthUnit &newUnit);

        //! Value in switched unit
        CAltitude switchedUnit(const physical_quantities::CLengthUnit &newUnit) const;

        //! AGL Above ground level?
        bool isAboveGroundLevel() const { return AboveGround == m_datum; }

        //! MSL Mean sea level?
        bool isMeanSeaLevel() const { return MeanSeaLevel == m_datum; }

        //! Flight level?
        bool isFlightLevel() const { return FlightLevel == m_datum; }

        //! Get reference datum (MSL or AGL)
        ReferenceDatum getReferenceDatum() const { return m_datum; }

        //! MSL to flightlevel
        bool toFlightLevel();

        //! Flightlevel to MSL
        bool toMeanSeaLevel();

        //! Non-NULL MSL value?
        bool hasMeanSeaLevelValue() const { return this->isMeanSeaLevel() && !this->isNull(); }

        //! Current altitude type
        AltitudeType getAltitudeType() const { return m_altitudeType; }

        //! Converts this to pressure altitude. Requires the current barometric pressure at MSL
        void convertToPressureAltitude(const physical_quantities::CPressure &seaLevelPressure);

        //! Returns the altitude converted to pressure altitude. Requires the current barometric pressure at MSL
        CAltitude toPressureAltitude(const physical_quantities::CPressure &seaLevelPressure) const;

        //! Parse value from string
        void parseFromString(const QString &value);

        //! Parse value from string, with specified separator
        void parseFromString(const QString &value, physical_quantities::CPqString::SeparatorMode mode);

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
        QString asFpICAOAltitudeString() const;

        //! As simple VATSIM string, only FLxxx or altitude as ft
        QString asFpVatsimAltitudeString() const;

        //! Checking FP altitude strings like "A20", "FL100"
        //! \sa CFlightPlan::asFpAltitudeString
        static const QRegularExpression &fpAltitudeRegExp();

        //! Info for FP altitude strings
        //! \sa CFlightPlan::asFpAltitudeString
        static QString fpAltitudeInfo(const QString &separator = ", ");

        //! Examples of FP altitude strings
        static QString fpAltitudeExamples();

        //! \copydoc swift::misc::mixin::Icon::toIcon
        swift::misc::CIcons::IconIndex toIcon() const;

        //! \copydoc physical_quantities::CPhysicalQuantity::compare
        int compare(const CAltitude &otherAltitude) const;

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! \copydoc mixin::Index::setPropertyByIndex
        int comparePropertyByIndex(CPropertyIndexRef index, const CAltitude &compareValue) const;

        //! Round to the nearest 100ft, like needed for China and Russia
        //! \remark https://en.wikipedia.org/wiki/Flight_level
        CAltitude roundedToNearest100ft(bool roundDown) const;

        //! @{
        //! Search the corresponding feet <-> metric / metric <-> feet
        static int findMetricAltitude(int feet);
        static int findAltitudeForMetricAltitude(int metric);
        //! @}

        //! Null altitude (MSL)
        static const CAltitude &null();

        //! Default unit for calculations
        //! \remark using this is optional and will simplify debugging and calculations
        static physical_quantities::CLengthUnit defaultUnit();

        //! Standard pressure 1013.25mbar/hPa
        static const physical_quantities::CPressure &standardISASeaLevelPressure();

        //! Register metadata
        static void registerMetadata();

    private:
        ReferenceDatum m_datum; //!< MSL or AGL?
        AltitudeType m_altitudeType = TrueAltitude; //!< type

        //! The metric tuples m/ft/FL
        static const QVector<MetricTuple> &metricTuples();

        BLACK_METACLASS(
            CAltitude,
            BLACK_METAMEMBER(datum),
            BLACK_METAMEMBER(altitudeType)
        );
    };

    using CAltitudePair = QPair<CAltitude, CAltitude>; //!< Pair of altitude
} // ns

Q_DECLARE_METATYPE(swift::misc::aviation::CAltitude)
Q_DECLARE_METATYPE(swift::misc::aviation::CAltitude::ReferenceDatum)
Q_DECLARE_METATYPE(swift::misc::aviation::CAltitude::AltitudeType)
Q_DECLARE_METATYPE(swift::misc::aviation::CAltitudePair)

#endif // guard
