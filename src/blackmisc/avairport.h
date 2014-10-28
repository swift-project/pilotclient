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
        class CAirport : public BlackMisc::CValueObject, public BlackMisc::Geo::ICoordinateGeodetic
        {
        public:
            //! Properties by index
            enum ColumnIndex
            {
                IndexIcao = BlackMisc::CPropertyIndex::GlobalIndexCAirport,
                IndexDescriptiveName,
                IndexPosition,
                IndexElevation,
                IndexDistance,
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

            //! \copydoc CValueObject::toQVariant()
            virtual QVariant toQVariant() const override { return QVariant::fromValue(*this); }

            //! \copydoc CValueObject::convertFromQVariant
            virtual void convertFromQVariant(const QVariant &variant) override { BlackMisc::setFromQVariant(this, variant); }

            //! Equal operator ==
            bool operator ==(const CAirport &other) const;

            //! Unequal operator !=
            bool operator !=(const CAirport &other) const;

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
            const BlackMisc::PhysicalQuantities::CLength getElevation() const { return this->getPosition().geodeticHeight(); }

            //! Get the distance to own plane
            const BlackMisc::PhysicalQuantities::CLength &getDistanceToPlane() const { return m_distanceToPlane; }

            //! Set distance to own plane
            void setDistanceToPlane(const BlackMisc::PhysicalQuantities::CLength &distance) { this->m_distanceToPlane = distance; }

            //! Get the bearing to own plane
            const BlackMisc::PhysicalQuantities::CAngle &getBearingToPlane() const { return m_bearingToPlane; }

            //! Set bearing to own plane
            void setBearingToPlane(const BlackMisc::PhysicalQuantities::CAngle &angle) { this->m_bearingToPlane = angle; }

            //! Valid distance?
            bool hasValidDistance() const { return !this->m_distanceToPlane.isNull();}

            //! Valid bearing?
            bool hasValidBearing() const { return !this->m_bearingToPlane.isNull();}

            //! Valid ICAO code
            bool hasValidIcaoCode() const { return !this->getIcao().isEmpty(); }

            //! Calculcate distance and bearing to plane, set it, and return distance
            BlackMisc::PhysicalQuantities::CLength calculcateDistanceAndBearingToPlane(const BlackMisc::Geo::CCoordinateGeodetic &position, bool updateValues = true);

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

            //! \copydoc CValueObject::getValueHash()
            virtual uint getValueHash() const override;

            //! \copydoc CValueObject::propertyByIndex()
            virtual QVariant propertyByIndex(const BlackMisc::CPropertyIndex &index) const override;

            //! \copydoc CValueObject::setPropertyByIndex(variant, index)
            virtual void setPropertyByIndex(const QVariant &variant, const BlackMisc::CPropertyIndex &index) override;

            //! \copydoc CValueObject::toJson
            virtual QJsonObject toJson() const override;

            //! \copydoc CValueObject::convertFromJson
            virtual void convertFromJson(const QJsonObject &json) override;

            //! Register metadata
            static void registerMetadata();

            //! \copydoc TupleConverter::jsonMembers()
            static const QStringList &jsonMembers();

        protected:
            //! \copydoc CValueObject::convertToQString
            virtual QString convertToQString(bool i18n = false) const override;

            //! \copydoc CValueObject::getMetaTypeId
            virtual int getMetaTypeId() const override;

            //! \copydoc CValueObject::isA
            virtual bool isA(int metaTypeId) const override;

            //! \copydoc CValueObject::compareImpl
            virtual int compareImpl(const CValueObject &other) const override;

            //! \copydoc CValueObject::marshallToDbus()
            virtual void marshallToDbus(QDBusArgument &argument) const override;

            //! \copydoc CValueObject::unmarshallFromDbus()
            virtual void unmarshallFromDbus(const QDBusArgument &argument) override;

        private:
            BLACK_ENABLE_TUPLE_CONVERSION(CAirport)
            CAirportIcao m_icao;
            QString m_descriptiveName;
            BlackMisc::Geo::CCoordinateGeodetic m_position;
            BlackMisc::PhysicalQuantities::CAngle m_bearingToPlane;
            BlackMisc::PhysicalQuantities::CLength m_distanceToPlane; // make mutable ?
        };
    } // namespace
} // namespace

BLACK_DECLARE_TUPLE_CONVERSION(BlackMisc::Aviation::CAirport, (o.m_icao, o.m_position))
Q_DECLARE_METATYPE(BlackMisc::Aviation::CAirport)

#endif // guard
