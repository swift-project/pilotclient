/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/aviation/airport.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/variant.h"

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
            case IndexBearing:
                return this->m_bearingToOwnAircraft.propertyByIndex(index.copyFrontRemoved());
            case IndexDistanceToOwnAircraft:
                return this->m_distanceToOwnAircraft.propertyByIndex(index.copyFrontRemoved());
            default:
                return (ICoordinateGeodetic::canHandleIndex(index)) ?
                       ICoordinateGeodetic::propertyByIndex(index) :
                       CValueObject::propertyByIndex(index);
            }
        }

        void CAirport::setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index)
        {
            if (index.isMyself()) { (*this) = variant.to<CAirport>(); return; }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexIcao:
                this->m_icao.setPropertyByIndex(variant, index.copyFrontRemoved());
                break;
            case IndexDescriptiveName:
                this->setDescriptiveName(variant.toQString());
                break;
            case IndexPosition:
                this->m_position.setPropertyByIndex(variant, index.copyFrontRemoved());
                break;
            case IndexBearing:
                this->m_bearingToOwnAircraft.setPropertyByIndex(variant, index.copyFrontRemoved());
                break;
            case IndexDistanceToOwnAircraft:
                this->m_distanceToOwnAircraft.setPropertyByIndex(variant, index.copyFrontRemoved());
                break;
            default:
                CValueObject::setPropertyByIndex(variant, index);
                break;
            }
        }

    } // namespace
} // namespace
