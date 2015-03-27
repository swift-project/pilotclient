/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVALTITUDE_H
#define BLACKMISC_AVALTITUDE_H

#include "blackmisc/pqlength.h"

namespace BlackMisc
{
    namespace Aviation { class CAltitude; }

    //! \private
    template <> struct CValueObjectPolicy<Aviation::CAltitude> : public CValueObjectPolicy<>
    {
        using Equals = Policy::Equals::MetaTuple;
        using Compare = Policy::Compare::MetaTuple;
        using Hash = Policy::Hash::MetaTuple;
        using DBus = Policy::DBus::MetaTuple;
        using Json = Policy::Json::MetaTuple;
    };

    namespace Aviation
    {
        /*!
         * Altitude as used in aviation, can be AGL or MSL altitude
         * \remarks Intentionally allowing +/- CLength , and >= / <= CLength.
         */
        class CAltitude : public CValueObject<CAltitude, PhysicalQuantities::CLength>
        {
        public:
            /*!
             * Enum type to distinguish between MSL and AGL
             */
            enum ReferenceDatum : uint
            {
                MeanSeaLevel = 0,   //!< MSL
                AboveGround,        //!< AGL
                FlightLevel         //!< Flight level
            };

        protected:
            //! \copydoc CValueObject::convertToQString
            virtual QString convertToQString(bool i18n) const override;

        public:
            //! Default constructor: 0 Altitude true
            CAltitude() : CValueObject(0, BlackMisc::PhysicalQuantities::CLengthUnit::m()), m_datum(MeanSeaLevel) {}

            //! Constructor
            CAltitude(double value, ReferenceDatum datum, const BlackMisc::PhysicalQuantities::CLengthUnit &unit) : CValueObject(value, unit), m_datum(datum) {}

            //! Altitude as string
            CAltitude(const QString &altitudeAsString, BlackMisc::PhysicalQuantities::CPqString::SeparatorMode mode = BlackMisc::PhysicalQuantities::CPqString::SeparatorsLocale);

            //! Constructor by CLength
            CAltitude(BlackMisc::PhysicalQuantities::CLength altitude, ReferenceDatum datum) : CValueObject(altitude), m_datum(datum) {}

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
            virtual void parseFromString(const QString &value) override;

            //! \copydoc CValueObject::parseFromString(const QString &value, BlackMisc::PhysicalQuantities::CPqString::SeparatorMode mode)
            virtual void parseFromString(const QString &value, BlackMisc::PhysicalQuantities::CPqString::SeparatorMode mode) override;

            //! \copydoc CValueObject::toIcon
            BlackMisc::CIcon toIcon() const override;

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CAltitude)
            ReferenceDatum m_datum; //!< MSL or AGL?
        };
    }
}

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Aviation::CAltitude, (o.m_datum))
Q_DECLARE_METATYPE(BlackMisc::Aviation::CAltitude)

#endif // guard
