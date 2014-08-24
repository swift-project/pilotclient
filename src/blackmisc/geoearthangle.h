/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_GEOEARTHANGLE_H
#define BLACKMISC_GEOEARTHANGLE_H
#include "blackmisc/pqangle.h"

namespace BlackMisc
{
    namespace Geo
    {
        /*!
         * Base class for latitude / longitude
         */
        template <class LATorLON> class CEarthAngle : public BlackMisc::PhysicalQuantities::CAngle
        {

        public:
            //! Virtual destructor
            virtual ~CEarthAngle() {}

            //! Equal operator ==
            bool operator==(const CEarthAngle &latOrLon) const
            {
                return this->CAngle::operator ==(latOrLon);
            }

            //! Not equal operator !=
            bool operator!=(const CEarthAngle &latOrLon) const
            {
                return this->CAngle::operator !=(latOrLon);
            }

            //! Plus operator +=
            CEarthAngle &operator +=(const CEarthAngle &latOrLon)
            {
                this->CAngle::operator +=(latOrLon);
                return *this;
            }

            //! Minus operator-=
            CEarthAngle &operator -=(const CEarthAngle &latOrLon)
            {
                this->CAngle::operator -=(latOrLon);
                return *this;
            }

            //! Multiply operator *=
            CEarthAngle operator *=(double multiply)
            {
                this->CAngle::operator *=(multiply);
                return *this;
            }

            //! Greater operator >
            bool operator >(const CEarthAngle &latOrLon) const
            {
                return this->CAngle::operator >(latOrLon);
            }

            //! Less operator <
            bool operator <(const CEarthAngle &latOrLon) const
            {
                return this->CAngle::operator >(latOrLon);
            }

            //! Less equal operator <=
            bool operator <=(const CEarthAngle &latOrLon) const
            {
                return this->CAngle::operator <=(latOrLon);
            }

            //! Greater equal operator >=
            bool operator >=(const CEarthAngle &latOrLon) const
            {
                return this->CAngle::operator >=(latOrLon);
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

            //! \copydoc CValueObject::toQVariant
            virtual QVariant toQVariant() const override { return QVariant::fromValue(*derived()); }

            //! \copydoc CValueObject::convertFromQVariant
            virtual void convertFromQVariant(const QVariant &variant) override { BlackMisc::setFromQVariant(derived(), variant); }

            //! Register metadata
            static void registerMetadata();

            /*!
             * Latitude / Longitude from a WGS string such as
             * \param wgsCoordinate 50° 2′ 0″ N / 8° 34′ 14″ E
             * \return
             */
            static LATorLON fromWgs84(const QString &wgsCoordinate);

        protected:
            //! Default constructor
            CEarthAngle() : CAngle(0.0, BlackMisc::PhysicalQuantities::CAngleUnit::deg()) {}

            //! Init by double value
            CEarthAngle(double value, const BlackMisc::PhysicalQuantities::CAngleUnit &unit) : CAngle(value, unit) {}

            //! Init by CAngle value
            CEarthAngle(const BlackMisc::PhysicalQuantities::CAngle &angle) : CAngle(angle) {}

            //! \copydoc CValueObject::convertToQString
            virtual QString convertToQString(bool i18n = false) const override
            {
                return this->valueRoundedWithUnit(BlackMisc::PhysicalQuantities::CAngleUnit::deg(), 6, i18n);
            }

            //! \copydoc CValueObject::getMetaTypeId
            virtual int getMetaTypeId() const override;

            //! \copydoc CValueObject::isA
            virtual bool isA(int metaTypeId) const override;

            //! \copydoc CValueObject::compareImpl
            virtual int compareImpl(const CValueObject &other) const override;

            //! \copydoc CValueObject::marshallToDbus
            virtual void marshallToDbus(QDBusArgument &argument) const override
            {
                this->CAngle::marshallToDbus(argument);
            }

            //! \copydoc CValueObject::unmarshallFromDbus
            virtual void unmarshallFromDbus(const QDBusArgument &argument) override
            {
                this->CAngle::unmarshallFromDbus(argument);
            }

        private:
            //! Easy access to derived class (CRTP template parameter)
            LATorLON const *derived() const { return static_cast<LATorLON const *>(this); }

            //! Easy access to derived class (CRTP template parameter)
            LATorLON *derived() { return static_cast<LATorLON *>(this); }
        };

    } // namespace
} // namespace

#endif // guard
