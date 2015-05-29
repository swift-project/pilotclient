/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/aviation/aircrafticaodata.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include "blackmisc/variant.h"

#include <tuple>
#include <QRegularExpression>

namespace BlackMisc
{
    namespace Aviation
    {

        CAircraftIcaoData::CAircraftIcaoData(const QString &icao, const QString &airline)
            : m_aircraftIcao(icao), m_airlineIcao(airline)
        {}

        CAircraftIcaoData::CAircraftIcaoData(const CAircraftIcaoCode &icaoAircraft, const CAirlineIcaoCode &icaoAirline, const QString &color)
            : m_aircraftIcao(icaoAircraft), m_airlineIcao(icaoAirline), m_aircraftColor(color)
        {}

        QString CAircraftIcaoData::convertToQString(bool i18n) const
        {
            Q_UNUSED(i18n);
            QString s(this->m_aircraftIcao.toQString(i18n));
            s.append(" ").append(this->m_airlineIcao.toQString(i18n));
            if (this->hasLivery()) { s.append(" ").append(this->m_livery); }
            if (this->hasAircraftColor()) { s.append(" ").append(this->m_aircraftColor); }
            return s;
        }

        QString CAircraftIcaoData::asString() const
        {
            if (!this->hasAircraftDesignator()) { return ""; }
            QString s(this->getAircraftDesignator());
            if (this->hasAirlineDesignator())
            {
                s.append(" (").append(this->getAirlineDesignator()).append(")");
                return s;
            }
            if (!this->m_aircraftColor.isEmpty())
            {
                s.append(" (").append(this->m_aircraftColor).append(")");
                return s;
            }
            return s;
        }

        void CAircraftIcaoData::updateMissingParts(const CAircraftIcaoData &icao)
        {
            if (!this->hasAircraftDesignator()) { this->setAircraftDesignator(icao.getAircraftDesignator()); }
            if (!this->hasAirlineDesignator()) { this->setAirlineDesignator(icao.getAirlineDesignator()); }
            if (!this->hasAircraftCombinedType()) { this->setAircraftCombinedType(icao.getAircraftCombinedType()); }
            if (this->m_aircraftColor.isEmpty()) { this->setAircraftColor(icao.getAircraftColor()); }
            if (this->m_livery.isEmpty()) { this->setLivery(icao.getLivery()); }
        }

        bool CAircraftIcaoData::matchesWildcardIcao(const CAircraftIcaoData &otherIcao) const
        {
            if ((*this) == otherIcao) return true;
            if (otherIcao.hasAircraftDesignator() && otherIcao.getAircraftDesignator() != this->getAircraftDesignator()) { return false; }
            if (otherIcao.hasAirlineDesignator() && otherIcao.getAirlineDesignator() != this->getAirlineDesignator()) { return false; }
            if (otherIcao.hasAircraftCombinedType() && otherIcao.getAircraftCombinedType() != this->getAircraftCombinedType()) { return false; }
            if (otherIcao.hasLivery() && otherIcao.getLivery() != this->getLivery()) { return false; }
            if (otherIcao.hasAircraftColor() && otherIcao.getAircraftColor() != this->getAircraftColor()) { return false; }
            return true;
        }

        CVariant CAircraftIcaoData::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexAircraftIcao:
                return CVariant::fromValue(this->m_aircraftIcao);
            case IndexAirlineIcao:
                return CVariant::fromValue(this->m_airlineIcao);
            case IndexAircraftColor:
                return CVariant::fromValue(this->m_aircraftColor);
            case IndexAsString:
                return CVariant::fromValue(this->asString());
            default:
                return CValueObject::propertyByIndex(index);
            }
        }

        void CAircraftIcaoData::setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index)
        {
            if (index.isMyself())
            {
                this->convertFromCVariant(variant);
                return;
            }

            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexAircraftIcao:
                this->m_aircraftIcao.setPropertyByIndex(variant, index.copyFrontRemoved());
                break;
            case IndexAirlineIcao:
                this->m_airlineIcao.setPropertyByIndex(variant, index.copyFrontRemoved());
                break;
            case IndexAircraftColor:
                this->setAircraftColor(variant.value<QString>());
                break;
            default:
                CValueObject::setPropertyByIndex(variant, index);
                break;
            }
        }

    } // namespace
} // namespace
