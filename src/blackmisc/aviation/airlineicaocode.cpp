/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/aviation/airlineicaocode.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include "blackmisc/variant.h"

#include <tuple>
#include <QRegularExpression>

namespace BlackMisc
{
    namespace Aviation
    {

        CAirlineIcaoCode::CAirlineIcaoCode(const QString &airlineDesignator)
            : m_designator(airlineDesignator.trimmed().toUpper())
        {}

        CAirlineIcaoCode::CAirlineIcaoCode(const QString &airlineDesignator, const QString &airlineName, const QString &country, const QString &telephony, bool virtualAirline)
            : m_designator(airlineDesignator), m_name(airlineName), m_country(country), m_telephonyDesignator(telephony), m_isVa(virtualAirline)
        {}

        QString CAirlineIcaoCode::convertToQString(bool i18n) const
        {
            Q_UNUSED(i18n);
            QString s(this->m_designator);
            return s;
        }

        CVariant CAirlineIcaoCode::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexAirlineDesignator:
                return CVariant::fromValue(this->m_designator);
            case IndexAirlineCountry:
                return CVariant::fromValue(this->m_country);
            case IndexAirlineName:
                return CVariant::fromValue(this->m_name);
            case IndexTelephonyDesignator:
                return CVariant::fromValue(this->m_telephonyDesignator);
            case IndexIsVirtualAirline:
                return CVariant::fromValue(this->m_isVa);
            default:
                return CValueObject::propertyByIndex(index);
            }
        }

        void CAirlineIcaoCode::setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index)
        {
            if (index.isMyself())
            {
                this->convertFromCVariant(variant);
                return;
            }

            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexAirlineDesignator:
                this->setDesignator(variant.value<QString>());
                break;
            case IndexAirlineCountry:
                this->setCountry(variant.value<QString>());
                break;
            case IndexAirlineName:
                this->setName(variant.value<QString>());
                break;
            case IndexTelephonyDesignator:
                this->setTelephonyDesignator(variant.value<QString>());
                break;
            case IndexIsVirtualAirline:
                this->setVirtualAirline(variant.toBool());
                break;
            default:
                CValueObject::setPropertyByIndex(variant, index);
                break;
            }
        }

        bool CAirlineIcaoCode::isValidAirlineDesignator(const QString &airline)
        {
            static QRegularExpression regexp("^[A-Z]+[A-Z0-9]*$");
            if (airline.length() < 2 || airline.length() > 5) return false;
            return (regexp.match(airline).hasMatch());
        }

        CAirlineIcaoCode CAirlineIcaoCode::fromDatabaseJson(const QJsonObject &json)
        {
            QJsonArray inner = json["cell"].toArray();
            if (inner.isEmpty()) { return CAirlineIcaoCode(); }
            CAirlineIcaoCode code(
                inner.at(1).toString(),
                inner.at(3).toString(), // name
                inner.at(4).toString(), // country
                inner.at(2).toString(), // telephony
                inner.at(5).toString().startsWith("Y") // VA
            );
            return code;
        }

    } // namespace
} // namespace
