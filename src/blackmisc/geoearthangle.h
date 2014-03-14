/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_GEOEARTHANGLE_H
#define BLACKMISC_GEOEARTHANGLE_H
#include "blackmisc/pqangle.h"

namespace BlackMisc
{
    namespace Geo
    {
        /*!
         * \brief Base class for latitude / longitude
         */
        template <class LATorLON> class CEarthAngle : public BlackMisc::PhysicalQuantities::CAngle
        {
        protected:
            //! \brief Default constructor
            CEarthAngle() : CAngle(0.0, BlackMisc::PhysicalQuantities::CAngleUnit::deg()) {}

            //! \brief Init by double value
            CEarthAngle(double value, const BlackMisc::PhysicalQuantities::CAngleUnit &unit) : CAngle(value, unit) {}

            //! \brief Init by CAngle value
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

        public:
            //! \brief Virtual destructor
            virtual ~CEarthAngle() {}

            //! \brief Equal operator ==
            bool operator==(const CEarthAngle &latOrLon) const
            {
                return this->CAngle::operator ==(latOrLon);
            }

            //! \brief Not equal operator !=
            bool operator!=(const CEarthAngle &latOrLon) const
            {
                return this->CAngle::operator !=(latOrLon);
            }

            //! \brief Plus operator +=
            CEarthAngle &operator +=(const CEarthAngle &latOrLon)
            {
                this->CAngle::operator +=(latOrLon);
                return *this;
            }

            //! \brief Minus operator-=
            CEarthAngle &operator -=(const CEarthAngle &latOrLon)
            {
                this->CAngle::operator -=(latOrLon);
                return *this;
            }

            /*!
             * \brief Multiply operator *=
             * \param multiply
             * \return
             */
            CEarthAngle operator *=(double multiply)
            {
                this->CAngle::operator *=(multiply);
                return *this;
            }

            /*!
             * \brief Greater operator >
             * \param latOrLon
             * \return
             */
            bool operator >(const CEarthAngle &latOrLon) const
            {
                return this->CAngle::operator >(latOrLon);
            }

            //! \brief Less operator <
            bool operator <(const CEarthAngle &latOrLon) const
            {
                return this->CAngle::operator >(latOrLon);
            }

            //! \brief Less equal operator <=
            bool operator <=(const CEarthAngle &latOrLon) const
            {
                return this->CAngle::operator <=(latOrLon);
            }

            //! \brief Greater equal operator >=
            bool operator >=(const CEarthAngle &latOrLon) const
            {
                return this->CAngle::operator >=(latOrLon);
            }

            //! \brief Plus operator +
            LATorLON operator +(const CEarthAngle &latOrLon) const
            {
                LATorLON l(*this);
                l += latOrLon;
                return l;
            }

            //! \brief Minus operator -
            LATorLON operator -(const CEarthAngle &latOrLon) const
            {
                LATorLON l(*this);
                l -= latOrLon;
                return l;
            }

            /*!
             * \brief Multiply operator *
             * \param multiply
             * \return
             */
            LATorLON operator *(double multiply) const
            {
                LATorLON l(*this);
                l *= multiply;
                return l;
            }

            //! \brief Register metadata
            static void registerMetadata();

            /*!
             * \brief Latitude / Longitude from a WGS string such as
             * \param wgsCoordinate 50° 2′ 0″ N / 8° 34′ 14″ E
             * \return
             */
            static LATorLON fromWgs84(const QString &wgsCoordinate);
        };

    } // namespace
} // namespace

#endif // guard
