/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "avairport.h"
#include "blackmiscfreefunctions.h"

using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Geo;

namespace BlackMisc
{
    namespace Aviation
    {

        /*
         * Constructor
         */
        CAirport::CAirport()  :
            m_distanceToPlane(0, CLengthUnit::nullUnit())
        { }

        /*
         * Constructor
         */
        CAirport::CAirport(const QString &icao)  :
            m_icao(icao), m_distanceToPlane(0, CLengthUnit::nullUnit())
        { }

        /*
         * Constructor
         */
        CAirport::CAirport(const CAirportIcao &icao, const BlackMisc::Geo::CCoordinateGeodetic &position) :
            m_icao(icao), m_position(position), m_distanceToPlane(0, CLengthUnit::nullUnit())
        { }

        /*
         * Constructor
         */
        CAirport::CAirport(const CAirportIcao &icao, const BlackMisc::Geo::CCoordinateGeodetic &position, const QString &descriptiveName) :
            m_icao(icao), m_descriptiveName(descriptiveName), m_position(position), m_distanceToPlane(0, CLengthUnit::nullUnit())
        { }

        /*
         * Convert to string
         */
        QString CAirport::convertToQString(bool i18n) const
        {
            QString s = i18n ?
                        QCoreApplication::translate("Aviation", "Airport") :
                        "Airport";
            if (!this->m_icao.isEmpty())
                s.append(' ').append(this->m_icao.toQString(i18n));

            // position
            s.append(' ').append(this->m_position.toQString(i18n));
            return s;

            // force strings for translation in resource files
            (void)QT_TRANSLATE_NOOP("Aviation", "ATC station");
        }

        /*
         * Register metadata
         */
        void CAirport::registerMetadata()
        {
            qRegisterMetaType<CAirport>();
            qDBusRegisterMetaType<CAirport>();
        }

        /*
         * Members
         */
        const QStringList &CAirport::jsonMembers()
        {
            return TupleConverter<CAirport>::jsonMembers();
        }

        /*
         * To JSON
         */
        QJsonObject CAirport::toJson() const
        {
            return BlackMisc::serializeJson(TupleConverter<CAirport>::toMetaTuple(*this));
        }

        /*
         * From Json
         */
        void CAirport::fromJson(const QJsonObject &json)
        {
            BlackMisc::deserializeJson(json, TupleConverter<CAirport>::toMetaTuple(*this));
        }

        /*
         * Compare
         */
        int CAirport::compareImpl(const CValueObject &otherBase) const
        {
            const auto &other = static_cast<const CAirport &>(otherBase);
            return compare(TupleConverter<CAirport>::toTuple(*this), TupleConverter<CAirport>::toTuple(other));
        }

        /*
         * Marshall to DBus
         */
        void CAirport::marshallToDbus(QDBusArgument &argument) const
        {
            argument << TupleConverter<CAirport>::toTuple(*this);
        }

        /*
         * Unmarshall from DBus
         */
        void CAirport::unmarshallFromDbus(const QDBusArgument &argument)
        {
            argument >> TupleConverter<CAirport>::toTuple(*this);
        }

        /*
         * Equal?
         */
        bool CAirport::operator ==(const CAirport &other) const
        {
            if (this == &other) return true;
            return TupleConverter<CAirport>::toTuple(*this) == TupleConverter<CAirport>::toTuple(other);
        }

        /*
         * Hash
         */
        uint CAirport::getValueHash() const
        {
            return qHash(TupleConverter<CAirport>::toTuple(*this));
        }

        /*
         * Unequal?
         */
        bool CAirport::operator !=(const CAirport &other) const
        {
            return !((*this) == other);
        }

        /*
         * Distance to planne
         */
        CLength CAirport::calculcateDistanceAndBearingToPlane(const CCoordinateGeodetic &position, bool updateValues)
        {
            if (!updateValues) return Geo::greatCircleDistance(this->m_position, position);
            this->m_distanceToPlane = Geo::greatCircleDistance(this->m_position, position);
            this->m_bearingToPlane = Geo::initialBearing(this->m_position, position);
            return this->m_distanceToPlane;
        }

        /*
         * Property by index
         */
        QVariant CAirport::propertyByIndex(int index) const
        {
            if (index > static_cast<int>(CAirport::IndexBearing))
            {
                return this->m_position.propertyByIndex(index);
            }

            switch (index)
            {
            case IndexIcao:
                return this->m_icao.toQVariant();
            case IndexIcaoAsString:
                return QVariant(this->m_icao.asString());
            case IndexDescriptiveName:
                return QVariant(this->m_descriptiveName);
            case IndexPosition:
                return this->m_position.toQVariant();
            case IndexElevation:
                return this->getElevation().toQVariant();
            case IndexBearing:
                return this->m_bearingToPlane.toQVariant();
            case IndexDistance:
                return this->m_distanceToPlane.toQVariant();
            default:
                break;
            }

            Q_ASSERT_X(false, "CAirport", "index unknown");
            QString m = QString("no property, index ").append(QString::number(index));
            return QVariant::fromValue(m);
        }

        /*
         * Set property as index
         */
        void CAirport::setPropertyByIndex(const QVariant &variant, int index)
        {
            if (index >= static_cast<int>(CAirport::IndexBearing))
            {
                this->m_position.setPropertyByIndex(variant, index);
            }

            switch (index)
            {
            case IndexIcao:
                this->setIcao(variant.value<CAirportIcao>());
                break;
            case IndexIcaoAsString:
                this->setIcao(CAirportIcao(variant.toString()));
                break;
            case IndexDescriptiveName:
                this->setDescriptiveName(variant.toString());
                break;
            case IndexPosition:
                this->setPosition(variant.value<CCoordinateGeodetic>());
                break;
            case IndexBearing:
                this->setBearingToPlane(variant.value<CAngle>());
                break;
            case IndexDistance:
                this->setDistanceToPlane(variant.value<CLength>());
                break;
            default:
                Q_ASSERT_X(false, "CAirport", "index unknown (setter)");
                break;
            }
        }

        /*
         * metaTypeId
         */
        int CAirport::getMetaTypeId() const
        {
            return qMetaTypeId<CAirport>();
        }

        /*
         * is a
         */
        bool CAirport::isA(int metaTypeId) const
        {
            if (metaTypeId == qMetaTypeId<CAirport>()) { return true; }
            return this->CValueObject::isA(metaTypeId);
        }

    } // namespace
} // namespace
