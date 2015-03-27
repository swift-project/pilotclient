/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKMISC_AIRPORT_H
#define BLACKMISC_AIRPORT_H

#include "avairporticao.h"
#include "coordinategeodetic.h"
#include "propertyindex.h"

namespace BlackMisc
{
    namespace Aviation
    {
        /*!
         * Value object encapsulating information about an airpot.
         */
        class CAirport : public CValueObject<CAirport>, public Geo::ICoordinateWithRelativePosition
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
            CAirport(const CAirportIcao &icao, const BlackMisc::Geo::CCoordinateGeodetic &position);

            //! ATC station constructor
            CAirport(const CAirportIcao &icao, const BlackMisc::Geo::CCoordinateGeodetic &position, const QString &descriptiveName);

            //! Get ICAO code.
            const CAirportIcao &getIcao() const { return m_icao; }

            //! Get ICAO code as string.
            QString getIcaoAsString() const { return m_icao.asString(); }

            //! Set ICAO code.
            void setIcao(const CAirportIcao &icao) {  m_icao = icao; }

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

            //! \copydoc ICoordinateGeodetic::geodeticHeight
            //! \remarks this should be used for elevation as depicted here: http://en.wikipedia.org/wiki/Altitude#mediaviewer/File:Vertical_distances.svg
            const BlackMisc::PhysicalQuantities::CLength &geodeticHeight() const override { return this->m_position.geodeticHeight(); }

            //! Valid ICAO code
            bool hasValidIcaoCode() const { return !this->getIcao().isEmpty(); }

            //! \copydoc ICoordinateGeodetic::latitude
            virtual const BlackMisc::Geo::CLatitude &latitude() const override
            {
                return this->getPosition().latitude();
            }

            //! \copydoc ICoordinateGeodetic::longitude
            virtual const BlackMisc::Geo::CLongitude &longitude() const override
            {
                return this->getPosition().longitude();
            }

            //! \copydoc CValueObject::propertyByIndex
            virtual CVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const override;

            //! \copydoc CValueObject::setPropertyByIndex
            virtual void setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index) override;

        protected:
            //! \copydoc CValueObject::convertToQString
            virtual QString convertToQString(bool i18n = false) const override;

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CAirport)
            CAirportIcao                        m_icao;
            QString                             m_descriptiveName;
            BlackMisc::Geo::CCoordinateGeodetic m_position;
        };
    } // namespace
} // namespace

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Aviation::CAirport, (o.m_icao, o.m_descriptiveName, o.m_position, o.m_distanceToOwnAircraft, o.m_bearingToOwnAircraft))
Q_DECLARE_METATYPE(BlackMisc::Aviation::CAirport)

#endif // guard
