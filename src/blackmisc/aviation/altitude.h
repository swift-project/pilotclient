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
#include "blackmisc/compare.h"
#include "blackmisc/dbus.h"
#include "blackmisc/dictionary.h"
#include "blackmisc/icon.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/pq/pqstring.h"
#include "blackmisc/pq/units.h"
#include "blackmisc/propertyindexvariantmap.h"
#include "blackmisc/stringutils.h"
#include "blackmisc/variant.h"

#include <QMetaType>
#include <QString>

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

            //! \copydoc BlackMisc::Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

            //! Default constructor: 0 Altitude true
            CAltitude() : CLength(0, BlackMisc::PhysicalQuantities::CLengthUnit::m()), m_datum(MeanSeaLevel) {}

            //! Constructor
            CAltitude(double value, ReferenceDatum datum, const BlackMisc::PhysicalQuantities::CLengthUnit &unit) : CLength(value, unit), m_datum(datum) {}

            //! Constructor, value as CAltitude::MeanSeaLevel
            CAltitude(double value, const BlackMisc::PhysicalQuantities::CLengthUnit &unit) : CLength(value, unit), m_datum(MeanSeaLevel) {}

            //! Altitude as string
            CAltitude(const QString &altitudeAsString, BlackMisc::PhysicalQuantities::CPqString::SeparatorMode mode = BlackMisc::PhysicalQuantities::CPqString::SeparatorsLocale);

            //! Constructor by CLength
            CAltitude(const BlackMisc::PhysicalQuantities::CLength &altitude, ReferenceDatum datum) : CLength(altitude), m_datum(datum) {}

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

            //! Parse value from string
            void parseFromString(const QString &value);

            //! Parse value from string, with specified separator
            void parseFromString(const QString &value, BlackMisc::PhysicalQuantities::CPqString::SeparatorMode mode);

            //! \copydoc BlackMisc::Mixin::Icon::toIcon
            BlackMisc::CIcon toIcon() const;

            //! Null altitude (MSL)
            static const CAltitude &null();

        private:
            ReferenceDatum m_datum; //!< MSL or AGL?

            BLACK_METACLASS(
                CAltitude,
                BLACK_METAMEMBER(datum)
            );
        };
    }
}

Q_DECLARE_METATYPE(BlackMisc::Aviation::CAltitude)
Q_DECLARE_METATYPE(BlackMisc::Aviation::CAltitude::ReferenceDatum)

#endif // guard
