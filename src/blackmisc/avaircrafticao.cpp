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

#include <tuple>
#include <QRegularExpression>

namespace BlackMisc
{
    namespace Aviation
    {
        /*
         * Convert to string
         */
        QString CAircraftIcao::convertToQString(bool /** i18n **/) const
        {
            QString s(this->m_aircraftDesignator);
            if (this->hasAircraftCombinedType()) s.append(" ").append(this->m_aircraftCombinedType);
            if (this->hasAirlineDesignator()) s.append(" ").append(this->m_airlineDesignator);
            if (this->hasLivery()) s.append(" ").append(this->m_livery);
            if (this->hasAircraftColor()) s.append(" ").append(this->m_aircraftColor);
            return s;
        }

        /*
         * As string
         */
        QString CAircraftIcao::asString() const
        {
            if (this->m_aircraftDesignator.isEmpty()) return "";
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

        bool CAircraftIcao::matchesWildcardIcao(const CAircraftIcao &otherIcao) const
        {
            if ((*this) == otherIcao) return true;
            if (otherIcao.hasAircraftDesignator() && otherIcao.getAircraftDesignator() != this->getAircraftDesignator()) return false;
            if (otherIcao.hasAirlineDesignator() && otherIcao.getAirlineDesignator() != this->getAirlineDesignator()) return false;
            if (otherIcao.hasAircraftCombinedType() && otherIcao.getAircraftCombinedType() != this->getAircraftCombinedType()) return false;
            if (otherIcao.hasLivery() && otherIcao.getLivery() != this->getLivery()) return false;
            if (otherIcao.hasAircraftColor() && otherIcao.getAircraftColor() != this->getAircraftColor()) return false;
            return true;
        }

        /*
         * Property by index
         */
        QVariant CAircraftIcao::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return this->toQVariant(); }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexAircraftDesignator:
                return QVariant::fromValue(this->m_aircraftDesignator);
            case IndexAirlineDesignator:
                return QVariant::fromValue(this->m_airlineDesignator);
            case IndexCombinedAircraftType:
                return QVariant::fromValue(this->m_aircraftCombinedType);
            case IndexAircraftColor:
                return QVariant::fromValue(this->m_aircraftColor);
            case IndexAsString:
                return QVariant::fromValue(this->asString());
            default:
                return CValueObject::propertyByIndex(index);
            }
        }

        /*
         * Property by index
         */
        void CAircraftIcao::setPropertyByIndex(const QVariant &variant, const BlackMisc::CPropertyIndex &index)
        {
            if (index.isMyself())
            {
                this->convertFromQVariant(variant);
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

        /*
         * Valid designator?
         */
        bool CAircraftIcao::isValidDesignator(const QString &designator)
        {
            static QRegularExpression regexp("^[A-Z]+[A-Z0-9]*$");
            if (designator.length() < 2 || designator.length() > 5) return false;
            return (regexp.match(designator).hasMatch());
        }

    } // namespace
} // namespace
