/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/aviation/airport.h"
#include "blackmisc/pq/angle.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/variant.h"

#include <QCoreApplication>
#include <Qt>
#include <QtGlobal>

using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Geo;

namespace BlackMisc
{
    namespace Aviation
    {
        CAirport::CAirport()
        { }

        CAirport::CAirport(const QString &icao)  :
            m_icao(icao)
        { }

        CAirport::CAirport(const CAirportIcaoCode &icao, const BlackMisc::Geo::CCoordinateGeodetic &position) :
            m_icao(icao), m_position(position)
        { }

        CAirport::CAirport(const CAirportIcaoCode &icao, const BlackMisc::Geo::CCoordinateGeodetic &position, const QString &descriptiveName) :
            m_icao(icao), m_descriptiveName(descriptiveName), m_position(position)
        { }

        QString CAirport::convertToQString(bool i18n) const
        {
            QString s = i18n ? QCoreApplication::translate("Aviation", "Airport") : "Airport";
            if (!this->m_icao.isEmpty()) { s.append(' ').append(this->m_icao.toQString(i18n)); }

            // position
            s.append(' ').append(this->m_position.toQString(i18n));
            return s;

            // force strings for translation in resource files
            (void)QT_TRANSLATE_NOOP("Aviation", "Airport");
        }

        void CAirport::convertFromDatabaseJson(const QJsonObject &json)
        {
            Q_ASSERT(json.value("icao").isString());
            setIcao(json.value("icao").toString());

            if (json.value("alpha3").isString() && json.value("country").isString())
            {
                CCountry country(json.value("alpha3").toString(), json.value("country").toString());
                setCountry(country);
            }

            Q_ASSERT(json.value("name").isString());
            setDescriptiveName(json.value("name").toString());

            Q_ASSERT(json.value("altitude").isDouble());
            setElevation(CLength(json.value("altitude").toInt(), CLengthUnit::ft()));

            Q_ASSERT(json.value("latitude").isDouble());
            Q_ASSERT(json.value("longitude").isDouble());
            CCoordinateGeodetic pos(json.value("latitude").toDouble(), json.value("longitude").toDouble(), 0);
            setPosition(pos);
        }

        CAirport CAirport::fromDatabaseJson(const QJsonObject &json, const QString &prefix)
        {
            CAirport airport(json.value("icao").toString());
            airport.setDescriptiveName(json.value("name").toString());
            airport.setElevation(CLength(json.value("altitude").toInt(), CLengthUnit::ft()));
            CCoordinateGeodetic pos(json.value("latitude").toDouble(), json.value("longitude").toDouble(), 0);
            airport.setPosition(pos);

            if (json.value("alpha3").isString() && json.value("country").isString())
            {
                CCountry country(json.value("alpha3").toString(), json.value("country").toString());
                airport.setCountry(country);
            }

            airport.setKeyAndTimestampFromDatabaseJson(json, prefix);
            return airport;
        }

        CVariant CAirport::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexIcao:
                return this->m_icao.propertyByIndex(index.copyFrontRemoved());
            case IndexDescriptiveName:
                return CVariant(this->m_descriptiveName);
            case IndexPosition:
                return this->m_position.propertyByIndex(index.copyFrontRemoved());
            case IndexElevation:
                return this->getElevation().propertyByIndex(index.copyFrontRemoved());
            default:
                return (ICoordinateWithRelativePosition::canHandleIndex(index)) ?
                       ICoordinateWithRelativePosition::propertyByIndex(index) :
                       CValueObject::propertyByIndex(index);
            }
        }

        void CAirport::setPropertyByIndex(const BlackMisc::CPropertyIndex &index, const CVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.to<CAirport>(); return; }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexIcao:
                this->m_icao.setPropertyByIndex(index.copyFrontRemoved(), variant);
                break;
            case IndexDescriptiveName:
                this->setDescriptiveName(variant.toQString());
                break;
            case IndexPosition:
                this->m_position.setPropertyByIndex(index.copyFrontRemoved(), variant);
                break;
            default:
                if (ICoordinateWithRelativePosition::canHandleIndex(index))
                {
                    ICoordinateWithRelativePosition::setPropertyByIndex(index, variant);
                }
                else
                {
                    CValueObject::setPropertyByIndex(index, variant);
                }
                break;
            }
        }

        int CAirport::comparePropertyByIndex(const CPropertyIndex &index, const CAirport &compareValue) const
        {
            if (index.isMyself()) { return this->m_icao.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getIcao()); }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexIcao:
                return this->m_icao.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getIcao());
            case IndexDescriptiveName:
                return this->m_descriptiveName.compare(compareValue.getDescriptiveName(), Qt::CaseInsensitive);
            default:
                if (ICoordinateWithRelativePosition::canHandleIndex(index))
                {
                    return ICoordinateWithRelativePosition::comparePropertyByIndex(index, compareValue);
                }
                break;
            }
            Q_ASSERT_X(false, Q_FUNC_INFO, "Compare failed");
            return 0;
        }
    } // namespace
} // namespace
