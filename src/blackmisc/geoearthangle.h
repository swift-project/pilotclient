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
            /*!
             * \brief Default constructor
             */
            CEarthAngle() : CAngle(0.0, BlackMisc::PhysicalQuantities::CAngleUnit::deg()) {}

            /*!
             * \brief Init by double value
             * \param value
             * \param unit
             */
            CEarthAngle(double value, const BlackMisc::PhysicalQuantities::CAngleUnit &unit) : CAngle(value, unit) {}

            /*!
             * \brief Init by double value
             * \param angle
             */
            CEarthAngle(const BlackMisc::PhysicalQuantities::CAngle &angle) : CAngle(angle) {}

            /*!
             * \brief String representation
             * \param i18n
             * \return
             */
            virtual QString convertToQString(bool i18n = false) const
            {
                return this->valueRoundedWithUnit(BlackMisc::PhysicalQuantities::CAngleUnit::deg(), 6, i18n);
            }

            /*!
             * \copydoc CValueObject::getMetaTypeId
             */
            virtual int getMetaTypeId() const;

            /*!
             * \copydoc CValueObject::isA
             */
            virtual bool isA(int metaTypeId) const;

            /*!
             * \copydoc CValueObject::compareImpl
             */
            virtual int compareImpl(const CValueObject &other) const;

            /*!
             * \brief Stream to DBus <<
             * \param argument
             */
            virtual void marshallToDbus(QDBusArgument &argument) const
            {
                this->CAngle::marshallToDbus(argument);
            }

            /*!
             * \brief Stream from DBus >>
             * \param argument
             */
            virtual void unmarshallFromDbus(const QDBusArgument &argument)
            {
                this->CAngle::unmarshallFromDbus(argument);
            }

        public:
            /*!
             * \brief Virtual destructor
             */
            virtual ~CEarthAngle() {}

            /*!
             * \brief Equal operator ==
             * \param latOrLon
             * \return
             */
            bool operator==(const LATorLON &latOrLon) const
            {
                return this->CAngle::operator ==(latOrLon);
            }

            /*!
             * \brief Not equal operator !=
             * \param latOrLon
             * \return
             */
            bool operator!=(const LATorLON &latOrLon) const
            {
                return this->CAngle::operator !=(latOrLon);
            }

            /*!
             * \brief Plus operator +=
             * \param latOrLon
             * \return
             */
            CEarthAngle &operator +=(const CEarthAngle &latOrLon)
            {
                this->CAngle::operator +=(latOrLon);
                return *this;
            }

            /*!
             * \brief Minus operator-=
             * \param latOrLon
             * \return
             */
            CEarthAngle &operator -=(const CEarthAngle &latOrLon)
            {
                this->CAngle::operator -=(latOrLon);
                return *this;
            }

            /*!
             * \brief Greater operator >
             * \param latOrLon
             * \return
             */
            bool operator >(const LATorLON &latOrLon) const
            {
                return this->CAngle::operator >(latOrLon);
            }

            /*!
             * \brief Less operator <
             * \param latOrLon
             * \return
             */
            bool operator <(const LATorLON &latOrLon) const
            {
                return this->CAngle::operator >(latOrLon);
            }

            /*!
             * \brief Less equal operator <=
             * \param latOrLon
             * \return
             */
            bool operator <=(const LATorLON &latOrLon) const
            {
                return this->CAngle::operator <=(latOrLon);
            }

            /*!
             * \brief Greater equal operator >=
             * \param latOrLon
             * \return
             */
            bool operator >=(const LATorLON &latOrLon) const
            {
                return this->CAngle::operator >=(latOrLon);
            }

            /*!
             * \brief Plus operator +
             * \param latOrLon
             * \return
             */
            LATorLON operator +(const LATorLON &latOrLon) const
            {
                LATorLON l(*this);
                l += latOrLon;
                return l;
            }

            /*!
             * \brief Minus operator -
             * \param latOrLon
             * \return
             */
            LATorLON operator -(const LATorLON &latOrLon) const
            {
                LATorLON l(*this);
                l -= latOrLon;
                return l;
            }

            /*!
             * Register metadata
             */
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
