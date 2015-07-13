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


using namespace BlackMisc;

namespace BlackMisc
{
    namespace Aviation
    {

        CAirlineIcaoCode::CAirlineIcaoCode(const QString &airlineDesignator)
            : m_designator(airlineDesignator.trimmed().toUpper())
        {}

        CAirlineIcaoCode::CAirlineIcaoCode(const QString &airlineDesignator, const QString &airlineName, const QString &countryIso, const QString &country, const QString &telephony, bool virtualAirline)
            : m_designator(airlineDesignator.trimmed().toUpper()), m_name(airlineName), m_countryIso(countryIso.trimmed().toUpper()), m_country(country), m_telephonyDesignator(telephony), m_isVa(virtualAirline)
        {}

        const QString CAirlineIcaoCode::getVDesignator() const
        {
            if (!isVirtualAirline()) { return this->m_designator; }
            return QString("V").append(this->m_designator);
        }

        bool CAirlineIcaoCode::hasCompleteData() const
        {
            return this->hasDesignator() && this->hasCountryIso() && this->hasName();
        }

        QString CAirlineIcaoCode::convertToQString(bool i18n) const
        {
            Q_UNUSED(i18n);
            QString s(this->m_designator);
            return s;
        }

        CVariant CAirlineIcaoCode::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            if (IDatastoreObjectWithIntegerKey::canHandleIndex(index)) { return IDatastoreObjectWithIntegerKey::propertyByIndex(index); }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexAirlineDesignator:
                return CVariant::fromValue(this->m_designator);
            case IndexAirlineCountryIso:
                return CVariant::fromValue(this->m_countryIso);
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
            if (index.isMyself()) { (*this) = variant.to<CAirlineIcaoCode>(); return; }
            if (IDatastoreObjectWithIntegerKey::canHandleIndex(index)) { IDatastoreObjectWithIntegerKey::setPropertyByIndex(variant, index); return; }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexAirlineDesignator:
                this->setDesignator(variant.value<QString>());
                break;
            case IndexAirlineCountryIso:
                this->setCountryIso(variant.value<QString>());
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
            if (airline.length() < 2 || airline.length() > 5) return false;

            static QThreadStorage<QRegularExpression> tsRegex;
            if (! tsRegex.hasLocalData()) { tsRegex.setLocalData(QRegularExpression("^[A-Z]+[A-Z0-9]*$")); }
            const QRegularExpression &regexp = tsRegex.localData();
            return (regexp.match(airline).hasMatch());
        }

        CAirlineIcaoCode CAirlineIcaoCode::fromDatabaseJson(const QJsonObject &json)
        {
            // https://ubuntu12/vatrep/public/service/allairlineicao.php?rows=10
            QJsonArray inner = json["cell"].toArray();
            Q_ASSERT_X(!inner.isEmpty(), Q_FUNC_INFO, "Missing JSON");
            if (inner.isEmpty()) { return CAirlineIcaoCode(); }

            int i = 0;
            int dbKey = inner.at(i++).toInt(-1);
            QString designator(inner.at(i++).toString());
            QString vDesignator(inner.at(i++).toString());
            Q_UNUSED(vDesignator);
            QString telephony(inner.at(i++).toString());
            QString name(inner.at(i++).toString());
            QString countryIso(inner.at(i++).toString());
            QString country(inner.at(i++).toString());
            bool va = inner.at(i++).toString().startsWith("Y", Qt::CaseInsensitive); // VA
            CAirlineIcaoCode code(
                designator, name, countryIso, country, telephony, va
            );
            code.setDbKey(dbKey);
            return code;
        }

    } // namespace
} // namespace
