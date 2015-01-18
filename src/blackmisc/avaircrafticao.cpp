/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "avaircrafticao.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include "blackmisc/variant.h"

#include <tuple>
#include <QRegularExpression>

namespace BlackMisc
{
    namespace Aviation
    {
        QString CAircraftIcao::convertToQString(bool i18n) const
        {
            Q_UNUSED(i18n);
            QString s(this->m_aircraftDesignator);
            if (this->hasAircraftCombinedType()) s.append(" ").append(this->m_aircraftCombinedType);
            if (this->hasAirlineDesignator()) s.append(" ").append(this->m_airlineDesignator);
            if (this->hasLivery()) s.append(" ").append(this->m_livery);
            if (this->hasAircraftColor()) s.append(" ").append(this->m_aircraftColor);
            return s;
        }

        bool CAircraftIcao::hasAircraftDesignator() const
        {
            return !this->m_aircraftDesignator.isEmpty();
        }

        bool CAircraftIcao::hasKnownAircraftDesignator() const
        {
            return (this->hasAircraftDesignator() && this->getAircraftDesignator() != "ZZZZ");
        }

        QString CAircraftIcao::asString() const
        {
            if (this->m_aircraftDesignator.isEmpty()) { return ""; }
            QString s(this->m_aircraftDesignator);
            if (!this->m_airlineDesignator.isEmpty())
            {
                s.append(" (").append(this->m_airlineDesignator).append(")");
                return s;
            }
            if (!this->m_aircraftColor.isEmpty())
            {
                s.append(" (").append(this->m_aircraftColor).append(")");
                return s;
            }
            return s;
        }

        void CAircraftIcao::updateMissingParts(const CAircraftIcao &icao)
        {
            if (this->m_aircraftDesignator.isEmpty()) { this->setAircraftDesignator(icao.getAircraftDesignator()); }
            if (this->m_airlineDesignator.isEmpty()) { this->setAirlineDesignator(icao.getAirlineDesignator()); }
            if (this->m_aircraftCombinedType.isEmpty()) { this->setAircraftCombinedType(icao.getAircraftCombinedType()); }
            if (this->m_aircraftColor.isEmpty()) { this->setAircraftColor(icao.getAircraftColor()); }
            if (this->m_livery.isEmpty()) { this->setLivery(icao.getLivery()); }
        }

        bool CAircraftIcao::matchesWildcardIcao(const CAircraftIcao &otherIcao) const
        {
            if ((*this) == otherIcao) return true;
            if (otherIcao.hasAircraftDesignator() && otherIcao.getAircraftDesignator() != this->getAircraftDesignator()) { return false; }
            if (otherIcao.hasAirlineDesignator() && otherIcao.getAirlineDesignator() != this->getAirlineDesignator()) { return false; }
            if (otherIcao.hasAircraftCombinedType() && otherIcao.getAircraftCombinedType() != this->getAircraftCombinedType()) { return false; }
            if (otherIcao.hasLivery() && otherIcao.getLivery() != this->getLivery()) { return false; }
            if (otherIcao.hasAircraftColor() && otherIcao.getAircraftColor() != this->getAircraftColor()) { return false; }
            return true;
        }

        CVariant CAircraftIcao::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return this->toCVariant(); }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexAircraftDesignator:
                return CVariant::fromValue(this->m_aircraftDesignator);
            case IndexAirlineDesignator:
                return CVariant::fromValue(this->m_airlineDesignator);
            case IndexCombinedAircraftType:
                return CVariant::fromValue(this->m_aircraftCombinedType);
            case IndexAircraftColor:
                return CVariant::fromValue(this->m_aircraftColor);
            case IndexAsString:
                return CVariant::fromValue(this->asString());
            default:
                return CValueObject::propertyByIndex(index);
            }
        }

        void CAircraftIcao::setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index)
        {
            if (index.isMyself())
            {
                this->convertFromCVariant(variant);
                return;
            }

            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexAircraftDesignator:
                this->setAircraftDesignator(variant.value<QString>());
                break;
            case IndexAirlineDesignator:
                this->setAirlineDesignator(variant.value<QString>());
                break;
            case IndexCombinedAircraftType:
                this->setAircraftCombinedType(variant.value<QString>());
                break;
            case IndexAircraftColor:
                this->setAircraftColor(variant.value<QString>());
                break;
            default:
                CValueObject::setPropertyByIndex(variant, index);
                break;
            }
        }

        bool CAircraftIcao::isValidDesignator(const QString &designator)
        {
            static QRegularExpression regexp("^[A-Z]+[A-Z0-9]*$");
            if (designator.length() < 2 || designator.length() > 5) { return false; }
            return (regexp.match(designator).hasMatch());
        }

        bool CAircraftIcao::isValidCombinedType(const QString &combinedType)
        {
            static QRegularExpression regexp("^[A-Z][0-9][A-Z]$");
            if (combinedType.length() != 3) return false;
            return (regexp.match(combinedType).hasMatch());
        }

        bool CAircraftIcao::isValidAirlineDesignator(const QString &airline)
        {
            static QRegularExpression regexp("^[A-Z]+[A-Z0-9]*$");
            if (airline.length() < 2 || airline.length() > 5) return false;
            return (regexp.match(airline).hasMatch());
        }

    } // namespace
} // namespace
