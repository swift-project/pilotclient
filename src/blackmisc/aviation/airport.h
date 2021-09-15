/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVIATION_AIRPORT_H
#define BLACKMISC_AVIATION_AIRPORT_H

#include "blackmisc/aviation/airporticaocode.h"
#include "blackmisc/blackmiscexport.h"
#include "blackmisc/db/datastore.h"
#include "blackmisc/geo/coordinategeodetic.h"
#include "blackmisc/geo/latitude.h"
#include "blackmisc/geo/longitude.h"
#include "blackmisc/metaclass.h"
#include "blackmisc/pq/length.h"
#include "blackmisc/country.h"
#include "blackmisc/propertyindexref.h"
#include "blackmisc/valueobject.h"

#include <QMetaType>
#include <QString>
#include <QVector3D>
#include <array>

namespace BlackMisc::Aviation
{
    //! Value object encapsulating information about an airpot.
    class BLACKMISC_EXPORT CAirport :
        public CValueObject<CAirport>,
        public Geo::ICoordinateWithRelativePosition,
        public BlackMisc::Db::IDatastoreObjectWithIntegerKey
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexIcao = BlackMisc::CPropertyIndexRef::GlobalIndexCAirport,
            IndexLocation,
            IndexDescriptiveName,
            IndexPosition,
            IndexCountry,
            IndexElevation,
            IndexOperating,
        };

        //! Default constructor.
        CAirport();

        //! Simplified constructor
        CAirport(const QString &icao);

        //! ATC station constructor
        CAirport(const CAirportIcaoCode &icao, const BlackMisc::Geo::CCoordinateGeodetic &position);

        //! ATC station constructor
        CAirport(const CAirportIcaoCode &icao, const BlackMisc::Geo::CCoordinateGeodetic &position, const QString &descriptiveName);

        //! Get ICAO code.
        const CAirportIcaoCode &getIcao() const { return m_icao; }

        //! Get ICAO code as string.
        const QString &getIcaoAsString() const { return m_icao.asString(); }

        //! Set ICAO code.
        void setIcao(const CAirportIcaoCode &icao) {  m_icao = icao; }

        //! Get location (e.g. "London")
        const QString &getLocation() const { return m_location; }

        //! Location plus optional name (if available and different from location)
        QString getLocationPlusOptionalName() const;

        //! Set location
        void setLocation(const QString &location) { this->m_location = location; }

        //! Matches location?
        bool matchesLocation(const QString &location) const;

        //! Get descriptive name
        const QString &getDescriptiveName() const { return m_descriptiveName; }

        //! Set descriptive name
        void setDescriptiveName(const QString &name) { this->m_descriptiveName = name; }

        //! Matches name?
        bool matchesDescriptiveName(const QString &name) const;

        //! Get the position
        const BlackMisc::Geo::CCoordinateGeodetic &getPosition() const { return m_position; }

        //! Set position
        void setPosition(const BlackMisc::Geo::CCoordinateGeodetic &position) { this->m_position = position; }

        //! Get the country
        const CCountry &getCountry() const { return m_country; }

        //! Set the country
        void setCountry(const CCountry &country) { this->m_country = country; }

        //! Elevation
        //! \sa geodeticHeight
        const BlackMisc::Aviation::CAltitude &getElevation() const { return this->geodeticHeight(); }

        //! Elevation
        //! \sa setGeodeticHeight
        void setElevation(const BlackMisc::Aviation::CAltitude &elevation) { this->m_position.setGeodeticHeight(elevation); }

        //! Is the airport still active?
        bool isOperating() const { return m_operating; }

        //! Sets the value of \sa isOperating().
        void setOperating(bool operating) { m_operating = operating; }

        //! Update the missing parts in airport
        void updateMissingParts(const CAirport &airport);

        //! \copydoc Geo::ICoordinateGeodetic::geodeticHeight
        const BlackMisc::Aviation::CAltitude &geodeticHeight() const override { return this->m_position.geodeticHeight(); }

        //! Valid ICAO code
        bool hasValidIcaoCode() const { return !this->getIcao().isEmpty(); }

        //! NULL airport?
        virtual bool isNull() const override;

        //! \copydoc Geo::ICoordinateGeodetic::latitude
        virtual BlackMisc::Geo::CLatitude latitude() const override
        {
            return this->getPosition().latitude();
        }

        //! \copydoc Geo::ICoordinateGeodetic::longitude
        virtual BlackMisc::Geo::CLongitude longitude() const override
        {
            return this->getPosition().longitude();
        }

        //! \copydoc Geo::ICoordinateGeodetic::normalVector
        virtual QVector3D normalVector() const override { return this->getPosition().normalVector(); }

        //! \copydoc Geo::ICoordinateGeodetic::normalVectorDouble
        virtual std::array<double, 3> normalVectorDouble() const override { return this->getPosition().normalVectorDouble(); }

        //! \copydoc BlackMisc::Mixin::Index::propertyByIndex
        QVariant propertyByIndex(BlackMisc::CPropertyIndexRef index) const;

        //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
        void setPropertyByIndex(BlackMisc::CPropertyIndexRef index, const QVariant &variant);

        //! Compare by index
        int comparePropertyByIndex(CPropertyIndexRef index, const CAirport &compareValue) const;

        //! \copydoc BlackMisc::Mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! Object from JSON
        static CAirport fromDatabaseJson(const QJsonObject &json, const QString &prefix = QString());

    private:
        QString                             m_location;
        QString                             m_descriptiveName;
        bool                                m_operating = true;
        CAirportIcaoCode                    m_icao;
        BlackMisc::CCountry                 m_country;
        BlackMisc::Geo::CCoordinateGeodetic m_position;

        BLACK_METACLASS(
            CAirport,
            BLACK_METAMEMBER(icao),
            BLACK_METAMEMBER(location),
            BLACK_METAMEMBER(descriptiveName),
            BLACK_METAMEMBER(position),
            BLACK_METAMEMBER(country),
            BLACK_METAMEMBER(operating),
            // ICoordinateWithRelativePosition
            BLACK_METAMEMBER(relativeDistance),
            BLACK_METAMEMBER(relativeBearing),
            // IDatastoreObjectWithIntegerKey
            BLACK_METAMEMBER(dbKey),
            BLACK_METAMEMBER(timestampMSecsSinceEpoch)
        );
    };
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CAirport)

#endif // guard
