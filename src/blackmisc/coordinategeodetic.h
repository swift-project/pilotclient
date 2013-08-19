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
};

/*!
 * \brief Geodetic coordinate
 */
class CCoordinateGeodetic : public CStreamable, public ICoordinateGeodetic
{
private:
    BlackMisc::Geo::CLatitude m_latitude; //!< Latitude
    BlackMisc::Geo::CLongitude m_longitude; //!< Longitude
    BlackMisc::PhysicalQuantities::CLength m_height; //!< height

protected:
    /*!
     * \brief String for converter
     * \param i18n
     * \return
     */
    virtual QString convertToQString(bool i18n = false) const;

    /*!
     * \brief Stream to DBus
     * \param argument
     */
    virtual void marshallToDbus(QDBusArgument &argument) const;

    /*!
     * \brief Stream from DBus
     * \param argument
     */
    virtual void unmarshallFromDbus(const QDBusArgument &argument);

public:
    /*!
     * \brief Default constructor
     */
    CCoordinateGeodetic() : m_latitude(), m_longitude(), m_height() {}

    /*!
     * \brief Copy constructor
     */
    CCoordinateGeodetic(const CCoordinateGeodetic &geoCoordinate) :
        m_latitude(geoCoordinate.m_latitude), m_longitude(geoCoordinate.m_longitude), m_height(geoCoordinate.m_height) {}

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
     * \brief Latitude
     * \return
     */
    const CLatitude &latitude() const
    {
        return this->m_latitude;
    }

    /*!
     * \brief Longitude
     * \return
     */
    const CLongitude &longitude() const
    {
        return this->m_longitude;
    }

    /*!
     * \brief Height
     * \return
     */
    const BlackMisc::PhysicalQuantities::CLength &height() const
    {
        return this->m_height;
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
    bool operator ==(const CCoordinateGeodetic &other) const
    {
        if (this == &other) return true;
        return this->m_height == other.m_height &&
               this->m_latitude == other.m_latitude &&
               this->m_longitude == other.m_longitude;
    }

    /*!
     * \brief Unequal operator !=
     * \param other
     * \return
     */
    bool operator !=(const CCoordinateGeodetic &other) const
    {
        return !((*this) == other);
    }

    /*
     * Register metadata
     */
    static void registerMetadata();
};

} // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Geo::CCoordinateGeodetic)

#endif // guard
