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
#include <QThreadStorage>
#include <QRegularExpression>
#include "blackmisc/logmessage.h"

using namespace BlackMisc;

namespace BlackMisc
{
    namespace Aviation
    {
        CAirlineIcaoCode::CAirlineIcaoCode(const QString &airlineDesignator)
            : m_designator(airlineDesignator.trimmed().toUpper())
        {}

        CAirlineIcaoCode::CAirlineIcaoCode(const QString &airlineDesignator, const QString &airlineName, const BlackMisc::CCountry &country, const QString &telephony, bool virtualAirline, bool operating)
            : m_designator(airlineDesignator.trimmed().toUpper()), m_name(airlineName), m_country(country), m_telephonyDesignator(telephony), m_isVa(virtualAirline), m_isOperating(operating)
        {}

        const QString CAirlineIcaoCode::getVDesignator() const
        {
            if (!isVirtualAirline()) { return this->m_designator; }
            return QString("V").append(this->m_designator);
        }

        QString CAirlineIcaoCode::getDesignatorNameCountry() const
        {
            QString s(this->getDesignator());
            if (this->hasName()) { s = s.append(" ").append(this->getName()); }
            if (this->hasValidCountry()) { s = s.append(" ").append(this->getCountryIso()); }
            return s.trimmed();
        }

        bool CAirlineIcaoCode::hasValidCountry() const
        {
            return this->m_country.isValid();
        }

        bool CAirlineIcaoCode::hasValidDesignator() const
        {
            return isValidAirlineDesignator(m_designator);
        }

        bool CAirlineIcaoCode::matchesDesignator(const QString &designator) const
        {
            if (designator.isEmpty()) { return false; }
            return designator.trimmed().toUpper() == this->m_designator;
        }

        bool CAirlineIcaoCode::matchesVDesignator(const QString &designator) const
        {
            if (designator.isEmpty()) { return false; }
            return designator.trimmed().toUpper() == this->getVDesignator();
        }

        bool CAirlineIcaoCode::hasCompleteData() const
        {
            return this->hasValidDesignator() && this->hasValidCountry() && this->hasName();
        }

        CIcon CAirlineIcaoCode::toIcon() const
        {
            if (this->m_designator.length() > 2)
            {
                return CIcon("images/airlines/" + m_designator.toLower() + ".png",
                             this->convertToQString());
            }
            else
            {
                return CIcon::iconByIndex(CIcons::StandardIconEmpty);
            }
        }

        QString CAirlineIcaoCode::convertToQString(bool i18n) const
        {
            Q_UNUSED(i18n);
            QString s(this->m_designator);
            if (this->m_name.isEmpty()) { return ""; }
            if (!this->m_name.isEmpty()) { s.append(" (").append(this->m_name).append(")"); }
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
                return CVariant::fromValue(this->getCountryIso());
            case IndexAirlineCountry:
                return this->m_country.propertyByIndex(index.copyFrontRemoved());
            case IndexAirlineName:
                return CVariant::fromValue(this->m_name);
            case IndexTelephonyDesignator:
                return CVariant::fromValue(this->m_telephonyDesignator);
            case IndexIsVirtualAirline:
                return CVariant::fromValue(this->m_isVa);
            case IndexIsOperating:
                return CVariant::fromValue(this->m_isOperating);
            case IndexDesignatorNameCountry:
                return CVariant::fromValue(this->getDesignatorNameCountry());
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
            case IndexAirlineCountry:
                this->setCountry(variant.value<CCountry>());
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
            case IndexIsOperating:
                this->setOperating(variant.toBool());
                break;
            default:
                CValueObject::setPropertyByIndex(variant, index);
                break;
            }
        }

        CStatusMessageList CAirlineIcaoCode::validate() const
        {
            static const CLogCategoryList cats(CLogCategoryList(this).join({ CLogCategory::validation() }));
            CStatusMessageList msgs;
            if (!hasValidDesignator()) { msgs.push_back(CStatusMessage(cats, CStatusMessage::SeverityError, "Airline: missing designator")); }
            if (!hasValidCountry()) { msgs.push_back(CStatusMessage(cats, CStatusMessage::SeverityError, "Airline: missing country")); }
            if (!hasName()) { msgs.push_back(CStatusMessage(cats, CStatusMessage::SeverityError, "Airline: no name")); }
            return msgs;
        }

        bool CAirlineIcaoCode::isValidAirlineDesignator(const QString &airline)
        {
            if (airline.length() < 2 || airline.length() > 5) return false;

            static QThreadStorage<QRegularExpression> tsRegex;
            if (! tsRegex.hasLocalData()) { tsRegex.setLocalData(QRegularExpression("^[A-Z]+[A-Z0-9]*$")); }
            const QRegularExpression &regexp = tsRegex.localData();
            return (regexp.match(airline).hasMatch());
        }

        QString CAirlineIcaoCode::getCombinedStringWithKey() const
        {
            QString s(getVDesignator());
            if (hasName()) { s = s.append(" ").append(getName()); }
            return s.append(" ").append(getDbKeyAsStringInParentheses());
        }

        void CAirlineIcaoCode::updateMissingParts(const CAirlineIcaoCode &otherIcaoCode)
        {
            if (!this->hasValidDesignator()) { this->setDesignator(otherIcaoCode.getDesignator()); }
            if (!this->hasValidCountry()) { this->setCountry(otherIcaoCode.getCountry()); }
            if (!this->hasName()) { this->setName(otherIcaoCode.getName()); }
            if (!this->hasTelephonyDesignator()) { this->setTelephonyDesignator(otherIcaoCode.getTelephonyDesignator()); }
            if (!this->hasValidDbKey())
            {
                this->setDbKey(otherIcaoCode.getDbKey());
                this->setUtcTimestamp(otherIcaoCode.getUtcTimestamp());
            }
        }

        CAirlineIcaoCode CAirlineIcaoCode::fromDatabaseJson(const QJsonObject &json, const QString &prefix)
        {
            if (!existsKey(json, prefix))
            {
                // when using relationship, this can be null
                return CAirlineIcaoCode();
            }

            QString designator(json.value(prefix + "designator").toString());
            QString telephony(json.value(prefix + "callsign").toString());
            QString name(json.value(prefix + "name").toString());
            QString countryIso(json.value(prefix + "country").toString());
            QString countryName(json.value(prefix + "countryname").toString());
            bool va = json.value(prefix + "va").toString().startsWith("Y", Qt::CaseInsensitive); // VA
            bool operating = json.value(prefix + "operating").toString().startsWith("Y", Qt::CaseInsensitive); // operating
            CAirlineIcaoCode code(
                designator, name,
                CCountry(countryIso, countryName),
                telephony, va, operating
            );
            code.setKeyAndTimestampFromDatabaseJson(json, prefix);
            return code;
        }

    } // namespace
} // namespace
