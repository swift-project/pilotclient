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

        class CLatitude;
        class CLongitude;

        /*!
         * Base class for latitude / longitude
         */
        template <class LATorLON> class CEarthAngle : public CValueObject<CEarthAngle<LATorLON>, PhysicalQuantities::CAngle>
        {
        public:
            //! Plus operator +=
            CEarthAngle &operator +=(const CEarthAngle &latOrLon);

            //! Minus operator-=
            CEarthAngle &operator -=(const CEarthAngle &latOrLon);

            //! Multiply operator *=
            CEarthAngle &operator *=(double multiply);

            //! Plus operator +
            LATorLON operator +(const CEarthAngle &latOrLon) const;

            //! Minus operator -
            LATorLON operator -(const CEarthAngle &latOrLon) const;

            //! Multiply operator *
            LATorLON operator *(double multiply) const;

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
            CEarthAngle();

            //! Init by double value
            CEarthAngle(double value, const BlackMisc::PhysicalQuantities::CAngleUnit &unit);

            //! Init by CAngle value
            CEarthAngle(const BlackMisc::PhysicalQuantities::CAngle &angle);

        public:
            //! \copydoc CValueObject::convertToQString
            virtual QString convertToQString(bool i18n = false) const override;

        private:
            //! Easy access to derived class (CRTP template parameter)
            LATorLON const *derived() const;

            //! Easy access to derived class (CRTP template parameter)
            LATorLON *derived();
        };

        //! \cond PRIVATE
        extern template class BLACKMISC_EXPORT_TEMPLATE CEarthAngle<CLatitude>;
        extern template class BLACKMISC_EXPORT_TEMPLATE CEarthAngle<CLongitude>;
        //! \endcond

    }
}

#endif // guard
