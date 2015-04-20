/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_GEO_EARTHANGLE_H
#define BLACKMISC_GEO_EARTHANGLE_H

#include "blackmisc/pq/angle.h"

namespace BlackMisc
{
    namespace Geo { template <class> class CEarthAngle; }

    //! \private
    template <class LATorLON> struct CValueObjectPolicy<Geo::CEarthAngle<LATorLON>> : public CValueObjectPolicy<>
    {
        using MetaType = Policy::MetaType::None;
    };

    namespace Geo
    {
        /*!
         * Base class for latitude / longitude
         */
        template <class LATorLON> class CEarthAngle : public CValueObject<CEarthAngle<LATorLON>, PhysicalQuantities::CAngle>
        {
        public:
            //! Plus operator +=
            CEarthAngle &operator +=(const CEarthAngle &latOrLon)
            {
                this->PhysicalQuantities::CAngle::operator +=(latOrLon);
                return *this;
            }

            //! Minus operator-=
            CEarthAngle &operator -=(const CEarthAngle &latOrLon)
            {
                this->PhysicalQuantities::CAngle::operator -=(latOrLon);
                return *this;
            }

            //! Multiply operator *=
            CEarthAngle operator *=(double multiply)
            {
                this->PhysicalQuantities::CAngle::operator *=(multiply);
                return *this;
            }

            //! Greater operator >
            bool operator >(const CEarthAngle &latOrLon) const
            {
                return this->PhysicalQuantities::CAngle::operator >(latOrLon);
            }

            //! Less operator <
            bool operator <(const CEarthAngle &latOrLon) const
            {
                return this->PhysicalQuantities::CAngle::operator >(latOrLon);
            }

            //! Less equal operator <=
            bool operator <=(const CEarthAngle &latOrLon) const
            {
                return this->PhysicalQuantities::CAngle::operator <=(latOrLon);
            }

            //! Greater equal operator >=
            bool operator >=(const CEarthAngle &latOrLon) const
            {
                return this->PhysicalQuantities::CAngle::operator >=(latOrLon);
            }

            //! Plus operator +
            LATorLON operator +(const CEarthAngle &latOrLon) const
            {
                LATorLON l(*this);
                l += latOrLon;
                return l;
            }

            //! Minus operator -
            LATorLON operator -(const CEarthAngle &latOrLon) const
            {
                LATorLON l(*this);
                l -= latOrLon;
                return l;
            }

            //! Multiply operator *
            LATorLON operator *(double multiply) const
            {
                LATorLON l(*this);
                l *= multiply;
                return l;
            }

            //! \copydoc CValueObject::toIcon
            CIcon toIcon() const override;

            /*!
             * Latitude / Longitude from a WGS string such as
             * \param wgsCoordinate 50° 2′ 0″ N / 8° 34′ 14″ E
             * \return
             */
            static LATorLON fromWgs84(const QString &wgsCoordinate);

        protected:
            //! Default constructor
            CEarthAngle() : CEarthAngle::CValueObject(0.0, BlackMisc::PhysicalQuantities::CAngleUnit::deg()) {}

            //! Init by double value
            CEarthAngle(double value, const BlackMisc::PhysicalQuantities::CAngleUnit &unit) : CEarthAngle::CValueObject(value, unit) {}

            //! Init by CAngle value
            CEarthAngle(const BlackMisc::PhysicalQuantities::CAngle &angle) : CEarthAngle::CValueObject(angle) {}

            //! \copydoc CValueObject::convertToQString
            virtual QString convertToQString(bool i18n = false) const override
            {
                return this->valueRoundedWithUnit(BlackMisc::PhysicalQuantities::CAngleUnit::deg(), 6, i18n);
            }

        private:
            //! Easy access to derived class (CRTP template parameter)
            LATorLON const *derived() const { return static_cast<LATorLON const *>(this); }

            //! Easy access to derived class (CRTP template parameter)
            LATorLON *derived() { return static_cast<LATorLON *>(this); }
        };
    }
}

#endif // guard
