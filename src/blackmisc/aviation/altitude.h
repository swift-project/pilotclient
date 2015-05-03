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

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/pq/length.h"

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
            public Mixin::MetaTypeAndQList<CAltitude>,
            public Mixin::EqualsByTuple<CAltitude>,
            public Mixin::CompareByTuple<CAltitude>,
            public Mixin::HashByTuple<CAltitude>,
            public Mixin::DBusByTuple<CAltitude>,
            public Mixin::JsonByTuple<CAltitude>,
            public Mixin::String<CAltitude>,
            public Mixin::Icon<CAltitude>
        {
        public:
            //! Base type
            using base_type = PhysicalQuantities::CLength;

            BLACKMISC_DECLARE_USING_MIXIN_METATYPE_AND_QLIST(CAltitude)
            BLACKMISC_DECLARE_USING_MIXIN_STRING(CAltitude)
            BLACKMISC_DECLARE_USING_MIXIN_DBUS(CAltitude)
            BLACKMISC_DECLARE_USING_MIXIN_JSON(CAltitude)
            BLACKMISC_DECLARE_USING_MIXIN_ICON(CAltitude)

            /*!
             * Enum type to distinguish between MSL and AGL
             */
            enum ReferenceDatum : uint
            {
                MeanSeaLevel = 0,   //!< MSL
                AboveGround,        //!< AGL
                FlightLevel         //!< Flight level
            };

            //! \copydoc CValueObject::convertToQString
            QString convertToQString(bool i18n = false) const;

            //! Default constructor: 0 Altitude true
            CAltitude() : CLength(0, BlackMisc::PhysicalQuantities::CLengthUnit::m()), m_datum(MeanSeaLevel) {}

            //! Constructor
            CAltitude(double value, ReferenceDatum datum, const BlackMisc::PhysicalQuantities::CLengthUnit &unit) : CLength(value, unit), m_datum(datum) {}

            //! Altitude as string
            CAltitude(const QString &altitudeAsString, BlackMisc::PhysicalQuantities::CPqString::SeparatorMode mode = BlackMisc::PhysicalQuantities::CPqString::SeparatorsLocale);

            //! Constructor by CLength
            CAltitude(BlackMisc::PhysicalQuantities::CLength altitude, ReferenceDatum datum) : CLength(altitude), m_datum(datum) {}

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

            //! \copydoc CValueObject::parseFromString(const QString &value)
            void parseFromString(const QString &value);

            //! \copydoc CValueObject::parseFromString(const QString &value, BlackMisc::PhysicalQuantities::CPqString::SeparatorMode mode)
            void parseFromString(const QString &value, BlackMisc::PhysicalQuantities::CPqString::SeparatorMode mode);

            //! \copydoc CValueObject::toIcon
            BlackMisc::CIcon toIcon() const;

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CAltitude)
            ReferenceDatum m_datum; //!< MSL or AGL?
        };
    }
}

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Aviation::CAltitude, (o.m_datum))
Q_DECLARE_METATYPE(BlackMisc::Aviation::CAltitude)

#endif // guard
