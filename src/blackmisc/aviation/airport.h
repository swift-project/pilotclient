/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AVIATION_AIRPORT_H
#define BLACKMISC_AVIATION_AIRPORT_H

#include "blackmisc/blackmiscexport.h"
#include "blackmisc/aviation/airporticaocode.h"
#include "blackmisc/geo/coordinategeodetic.h"
#include "blackmisc/propertyindex.h"

namespace BlackMisc
{
    namespace Aviation
    {
        //! Value object encapsulating information about an airpot.
        class BLACKMISC_EXPORT CAirport : public CValueObject<CAirport>, public Geo::ICoordinateWithRelativePosition
        {
        public:
            //! Properties by index
            enum ColumnIndex
            {
                IndexIcao = BlackMisc::CPropertyIndex::GlobalIndexCAirport,
                IndexDescriptiveName,
                IndexPosition,
                IndexElevation,
                IndexDistanceToOwnAircraft,
                IndexBearing
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
            QString getIcaoAsString() const { return m_icao.asString(); }

            //! Set ICAO code.
            void setIcao(const CAirportIcaoCode &icao) {  m_icao = icao; }

            //! Get descriptive name
            QString getDescriptiveName() const { return m_descriptiveName; }

            //! Set descriptive name
            void setDescriptiveName(const QString &name) { this->m_descriptiveName = name; }

            //! Get the position
            const BlackMisc::Geo::CCoordinateGeodetic &getPosition() const { return m_position; }

            //! Set position
            void setPosition(const BlackMisc::Geo::CCoordinateGeodetic &position) { this->m_position = position; }

            //! Elevation
            //! \sa geodeticHeight
            const BlackMisc::PhysicalQuantities::CLength getElevation() const { return this->geodeticHeight(); }

            //! Elevation
            //! \sa setGeodeticHeight
            void setElevation(const BlackMisc::PhysicalQuantities::CLength &elevation) { return this->m_position.setGeodeticHeight(elevation); }

            //! \copydoc Geo::ICoordinateGeodetic::geodeticHeight
            //! \remarks this should be used for elevation as depicted here: http://en.wikipedia.org/wiki/Altitude#mediaviewer/File:Vertical_distances.svg
            const BlackMisc::PhysicalQuantities::CLength &geodeticHeight() const override { return this->m_position.geodeticHeight(); }

            //! Valid ICAO code
            bool hasValidIcaoCode() const { return !this->getIcao().isEmpty(); }

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
            CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const;

            //! \copydoc BlackMisc::Mixin::Index::setPropertyByIndex
            void setPropertyByIndex(const BlackMisc::CPropertyIndex &index, const CVariant &variant);

            //! Compare for index
            int comparePropertyByIndex(const CPropertyIndex &index, const CAirport &compareValue) const;

            //! \copydoc BlackMisc::Mixin::String::toQString
            QString convertToQString(bool i18n = false) const;

        private:
            CAirportIcaoCode                    m_icao;
            QString                             m_descriptiveName;
            BlackMisc::Geo::CCoordinateGeodetic m_position;

            BLACK_METACLASS(
                CAirport,
                BLACK_METAMEMBER(icao),
                BLACK_METAMEMBER(descriptiveName),
                BLACK_METAMEMBER(position),
                BLACK_METAMEMBER(distanceToOwnAircraft),
                BLACK_METAMEMBER(bearingToOwnAircraft)
            );
        };
    } // namespace
} // namespace

Q_DECLARE_METATYPE(BlackMisc::Aviation::CAirport)

#endif // guard
