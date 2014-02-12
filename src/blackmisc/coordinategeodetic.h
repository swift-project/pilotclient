/*  Copyright (C) 2013 VATSIM Community / contributors
 *  This Source Code Form is subject to the terms of the Mozilla Public
 *  License, v. 2.0. If a copy of the MPL was not distributed with this
 *  file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BLACKMISC_COORDINATEGEODETIC_H
#define BLACKMISC_COORDINATEGEODETIC_H

#include "blackmisc/mathvector3dbase.h"
#include "blackmisc/geolatitude.h"
#include "blackmisc/geolongitude.h"
#include "blackmisc/pqlength.h"

namespace BlackMisc
{
    namespace Geo
    {

        /*!
         * Latitude and longitude interface
         * \brief Interface for geodetic ccordinates
         */
        class ICoordinateGeodetic
        {
        public:

            /*!
             * \brief Latitude
             * \return
             */
            virtual const CLatitude &latitude() const = 0;

            /*!
             * \brief Longitude
             * \return
             */
            virtual const CLongitude &longitude() const = 0;

            /*!
             * \brief As string
             * \return
             */
            QString latitudeAsString() const
            {
                return this->latitude().toQString(true);
            }

            /*!
             * \brief As string
             * \return
             */
            QString longitudeAsString() const
            {
                return this->longitude().toQString(true);
            }

        };

        /*!
         * \brief Great circle distance between points
         * \param coordinate1
         * \param coordinate2
         * \return
         */
        BlackMisc::PhysicalQuantities::CLength greatCircleDistance(const ICoordinateGeodetic &coordinate1, const ICoordinateGeodetic &coordinate2);

        /*!
         * \brief Geodetic coordinate
         */
        class CCoordinateGeodetic : public CValueObject, public ICoordinateGeodetic
        {
        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CCoordinateGeodetic)
            BlackMisc::Geo::CLatitude m_latitude; //!< Latitude
            BlackMisc::Geo::CLongitude m_longitude; //!< Longitude
            BlackMisc::PhysicalQuantities::CLength m_height; //!< height

        protected:
            //! \copydoc CValueObject::convertToQString
            virtual QString convertToQString(bool i18n = false) const override;

            //! \copydoc CValueObject::getMetaTypeId
            virtual int getMetaTypeId() const override;

            //! \copydoc CValueObject::isA
            virtual bool isA(int metaTypeId) const override;

            //! \copydoc CValueObject::compareImpl
            virtual int compareImpl(const CValueObject &other) const override;

            //! \copydoc CValueObject::marshallToDbus
            virtual void marshallToDbus(QDBusArgument &argument) const override;

            //! \copydoc CValueObject::unmarshallFromDbus
            virtual void unmarshallFromDbus(const QDBusArgument &argument) override;

        public:
            /*!
             * \brief Default constructor
             */
            CCoordinateGeodetic() : m_latitude(), m_longitude(), m_height() {}

            /*!
             * \brief Constructor by values
             * \param latitude
             * \param longitude
             * \param height
             */
            CCoordinateGeodetic(CLatitude latitude, CLongitude longitude, BlackMisc::PhysicalQuantities::CLength height) :
                m_latitude(latitude), m_longitude(longitude), m_height(height) {}

            /*!
             * \brief Constructor by values
             * \param latitudeDegrees
             * \param longitudeDegrees
             * \param heightMeters
             */
            CCoordinateGeodetic(double latitudeDegrees, double longitudeDegrees, double heightMeters) :
                m_latitude(latitudeDegrees, BlackMisc::PhysicalQuantities::CAngleUnit::deg()), m_longitude(longitudeDegrees, BlackMisc::PhysicalQuantities::CAngleUnit::deg()), m_height(heightMeters, BlackMisc::PhysicalQuantities::CLengthUnit::m()) {}

            /*!
             * \copydoc ICoordinateGeodetic::latitude
             */
            virtual const CLatitude &latitude() const override
            {
                return this->m_latitude;
            }

            /*!
             * \copydoc ICoordinateGeodetic::longitude
             */
            virtual const CLongitude &longitude() const override
            {
                return this->m_longitude;
            }

            /*!
             * \brief Height
             */
            const BlackMisc::PhysicalQuantities::CLength &height() const
            {
                return this->m_height;
            }

            /*!
             * \copydoc CValueObject::toQVariant
             */
            virtual QVariant toQVariant() const override
            {
                return QVariant::fromValue(*this);
            }

            /*!
             * \brief Switch unit of latitude / longitude
             * \param unit
             * \return
             */
            CCoordinateGeodetic &switchUnit(const BlackMisc::PhysicalQuantities::CAngleUnit &unit)
            {
                this->m_latitude.switchUnit(unit);
                this->m_longitude.switchUnit(unit);
                return *this;
            }

            /*!
             * \brief Switch unit of height
             * \param unit
             * \return
             */
            CCoordinateGeodetic &switchUnit(const BlackMisc::PhysicalQuantities::CLengthUnit &unit)
            {
                this->m_height.switchUnit(unit);
                return *this;
            }

            /*!
             * \brief Set latitude
             * \param latitude
             */
            void setLatitude(const CLatitude &latitude)
            {
                this->m_latitude = latitude;
            }

            /*!
             * \brief Set longitude
             * \param longitude
             */
            void setLongitude(const CLongitude &longitude)
            {
                this->m_longitude = longitude;
            }

            /*!
             * \brief Set height
             * \param height
             */
            void setHeight(const BlackMisc::PhysicalQuantities::CLength &height)
            {
                this->m_height = height;
            }

            /*!
             * \brief Equal operator ==
             * \param other
             * \return
             */
            bool operator ==(const CCoordinateGeodetic &other) const;

            /*!
             * \brief Unequal operator !=
             * \param other
             * \return
             */
            bool operator !=(const CCoordinateGeodetic &other) const;

            /*!
             * \copydoc CValueObject::getValueHash
             */
            virtual uint getValueHash() const override;

            /*!
             * Register metadata
             */
            static void registerMetadata();

            /*!
             * \brief Coordinate by WGS84 position data
             * \param latitudeWgs84
             * \param longitudeWgs84
             * \param height
             * \return
             */
            static CCoordinateGeodetic fromWgs84(const QString &latitudeWgs84, const QString &longitudeWgs84, const BlackMisc::PhysicalQuantities::CLength height = BlackMisc::PhysicalQuantities::CLength());
        };

    } // namespace
} // namespace

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Geo::CCoordinateGeodetic, (o.m_latitude, o.m_longitude, o.m_height))
Q_DECLARE_METATYPE(BlackMisc::Geo::CCoordinateGeodetic)

#endif // guard
