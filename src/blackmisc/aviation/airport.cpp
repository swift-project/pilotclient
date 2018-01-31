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
#include "blackmisc/stringutils.h"

#include <QCoreApplication>
#include <Qt>
#include <QtGlobal>

using namespace BlackMisc;
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
            m_descriptiveName(descriptiveName), m_icao(icao), m_position(position)
        { }

        bool CAirport::matchesLocation(const QString &location) const
        {
            if (location.isEmpty()) { return false; }
            return caseInsensitiveStringCompare(location, this->getLocation());
        }

        bool CAirport::matchesDescriptiveName(const QString &name) const
        {
            if (name.isEmpty()) { return false; }
            return caseInsensitiveStringCompare(name, this->getDescriptiveName());
        }

        void CAirport::updateMissingParts(const CAirport &airport)
        {
            if (!this->m_country.hasIsoCode() && airport.getCountry().hasIsoCode()) { this->m_country = airport.getCountry(); }
            if (this->m_descriptiveName.isEmpty()) { this->m_descriptiveName = airport.getDescriptiveName(); }
        }

        bool CAirport::isNull() const
        {
            return (m_icao.isEmpty() && m_descriptiveName.isEmpty());
        }

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

        CAirport CAirport::fromDatabaseJson(const QJsonObject &json, const QString &prefix)
        {
            CAirport airport(json.value(prefix + "icao").toString());
            airport.setDescriptiveName(json.value(prefix + "name").toString());
            airport.setLocation(json.value(prefix + "location").toString());
            const CCoordinateGeodetic pos(
                json.value(prefix + "latitude").toDouble(),
                json.value(prefix + "longitude").toDouble(),
                json.value(prefix + "altitude").toDouble()
            );
            airport.setPosition(pos);
            airport.setOperating(json.value(prefix + "operating").toString() == QStringLiteral("Y"));

            const CCountry country = CCountry::fromDatabaseJson(json, "ctry_");
            airport.setCountry(country);

            airport.setKeyAndTimestampFromDatabaseJson(json, prefix);
            return airport;
        }

        CVariant CAirport::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexIcao:
                return this->m_icao.propertyByIndex(index.copyFrontRemoved());
            case IndexLocation:
                return CVariant(this->m_location);
            case IndexDescriptiveName:
                return CVariant(this->m_descriptiveName);
            case IndexPosition:
                return this->m_position.propertyByIndex(index.copyFrontRemoved());
            case IndexElevation:
                return this->getElevation().propertyByIndex(index.copyFrontRemoved());
            case IndexOperating:
                return CVariant::from(this->isOperating());
            default:
                return (ICoordinateWithRelativePosition::canHandleIndex(index)) ?
                       ICoordinateWithRelativePosition::propertyByIndex(index) :
                       CValueObject::propertyByIndex(index);
            }
        }

        void CAirport::setPropertyByIndex(const BlackMisc::CPropertyIndex &index, const CVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.to<CAirport>(); return; }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexIcao:
                this->m_icao.setPropertyByIndex(index.copyFrontRemoved(), variant);
                break;
            case IndexLocation:
                this->setLocation(variant.toQString());
                break;
            case IndexDescriptiveName:
                this->setDescriptiveName(variant.toQString());
                break;
            case IndexPosition:
                this->m_position.setPropertyByIndex(index.copyFrontRemoved(), variant);
                break;
            case IndexOperating:
                this->setOperating(variant.toBool());
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
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexIcao:
                return this->m_icao.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getIcao());
            case IndexLocation:
                return this->m_location.compare(compareValue.getLocation(), Qt::CaseInsensitive);
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
