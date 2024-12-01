// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_MISC_AVIATION_AIRPORT_H
#define SWIFT_MISC_AVIATION_AIRPORT_H

#include <array>

#include <QMetaType>
#include <QString>
#include <QVector3D>

#include "misc/aviation/airporticaocode.h"
#include "misc/country.h"
#include "misc/db/datastore.h"
#include "misc/geo/coordinategeodetic.h"
#include "misc/geo/latitude.h"
#include "misc/geo/longitude.h"
#include "misc/metaclass.h"
#include "misc/pq/length.h"
#include "misc/propertyindexref.h"
#include "misc/swiftmiscexport.h"
#include "misc/valueobject.h"

SWIFT_DECLARE_VALUEOBJECT_MIXINS(swift::misc::aviation, CAirport)

namespace swift::misc::aviation
{
    //! Value object encapsulating information about an airpot.
    class SWIFT_MISC_EXPORT CAirport :
        public CValueObject<CAirport>,
        public geo::ICoordinateWithRelativePosition,
        public swift::misc::db::IDatastoreObjectWithIntegerKey
    {
    public:
        //! Properties by index
        enum ColumnIndex
        {
            IndexIcao = swift::misc::CPropertyIndexRef::GlobalIndexCAirport,
            IndexLocation,
            IndexDescriptiveName,
            IndexPosition,
            IndexCountry,
            IndexElevation,
            IndexOperating,
        };

        //! Default constructor.
        CAirport() = default;

        //! Simplified constructor
        CAirport(const QString &icao);

        //! ATC station constructor
        CAirport(const CAirportIcaoCode &icao, const swift::misc::geo::CCoordinateGeodetic &position);

        //! ATC station constructor
        CAirport(const CAirportIcaoCode &icao, const swift::misc::geo::CCoordinateGeodetic &position,
                 const QString &descriptiveName);

        //! Get ICAO code.
        const CAirportIcaoCode &getIcao() const { return m_icao; }

        //! Get ICAO code as string.
        const QString &getIcaoAsString() const { return m_icao.asString(); }

        //! Set ICAO code.
        void setIcao(const CAirportIcaoCode &icao) { m_icao = icao; }

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
        const swift::misc::geo::CCoordinateGeodetic &getPosition() const { return m_position; }

        //! Set position
        void setPosition(const swift::misc::geo::CCoordinateGeodetic &position) { this->m_position = position; }

        //! Get the country
        const CCountry &getCountry() const { return m_country; }

        //! Set the country
        void setCountry(const CCountry &country) { this->m_country = country; }

        //! Elevation
        //! \sa geodeticHeight
        const swift::misc::aviation::CAltitude &getElevation() const { return this->geodeticHeight(); }

        //! Elevation
        //! \sa setGeodeticHeight
        void setElevation(const swift::misc::aviation::CAltitude &elevation)
        {
            this->m_position.setGeodeticHeight(elevation);
        }

        //! Is the airport still active?
        bool isOperating() const { return m_operating; }

        //! Sets the value of \sa isOperating().
        void setOperating(bool operating) { m_operating = operating; }

        //! Update the missing parts in airport
        void updateMissingParts(const CAirport &airport);

        //! \copydoc geo::ICoordinateGeodetic::geodeticHeight
        const swift::misc::aviation::CAltitude &geodeticHeight() const override
        {
            return this->m_position.geodeticHeight();
        }

        //! Valid ICAO code
        bool hasValidIcaoCode() const { return !this->getIcao().isEmpty(); }

        //! NULL airport?
        virtual bool isNull() const override;

        //! \copydoc geo::ICoordinateGeodetic::latitude
        virtual swift::misc::geo::CLatitude latitude() const override { return this->getPosition().latitude(); }

        //! \copydoc geo::ICoordinateGeodetic::longitude
        virtual swift::misc::geo::CLongitude longitude() const override { return this->getPosition().longitude(); }

        //! \copydoc geo::ICoordinateGeodetic::normalVector
        virtual QVector3D normalVector() const override { return this->getPosition().normalVector(); }

        //! \copydoc geo::ICoordinateGeodetic::normalVectorDouble
        virtual std::array<double, 3> normalVectorDouble() const override
        {
            return this->getPosition().normalVectorDouble();
        }

        //! \copydoc swift::misc::mixin::Index::propertyByIndex
        QVariant propertyByIndex(swift::misc::CPropertyIndexRef index) const;

        //! \copydoc swift::misc::mixin::Index::setPropertyByIndex
        void setPropertyByIndex(swift::misc::CPropertyIndexRef index, const QVariant &variant);

        //! Compare by index
        int comparePropertyByIndex(CPropertyIndexRef index, const CAirport &compareValue) const;

        //! \copydoc swift::misc::mixin::String::toQString
        QString convertToQString(bool i18n = false) const;

        //! Object from JSON
        static CAirport fromDatabaseJson(const QJsonObject &json, const QString &prefix = QString());

    private:
        QString m_location;
        QString m_descriptiveName;
        bool m_operating = true;
        CAirportIcaoCode m_icao;
        swift::misc::CCountry m_country;
        swift::misc::geo::CCoordinateGeodetic m_position;

        SWIFT_METACLASS(
            CAirport,
            SWIFT_METAMEMBER(icao),
            SWIFT_METAMEMBER(location),
            SWIFT_METAMEMBER(descriptiveName),
            SWIFT_METAMEMBER(position),
            SWIFT_METAMEMBER(country),
            SWIFT_METAMEMBER(operating),
            // ICoordinateWithRelativePosition
            SWIFT_METAMEMBER(relativeDistance),
            SWIFT_METAMEMBER(relativeBearing),
            // IDatastoreObjectWithIntegerKey
            SWIFT_METAMEMBER(dbKey),
            SWIFT_METAMEMBER(timestampMSecsSinceEpoch));
    };
} // namespace swift::misc::aviation

Q_DECLARE_METATYPE(swift::misc::aviation::CAirport)

#endif // SWIFT_MISC_AVIATION_AIRPORT_H
