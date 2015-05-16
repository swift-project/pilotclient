/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/blackmiscfreefunctions.h"
#include "blackmisc/variant.h"

#include <tuple>
#include <QRegularExpression>

namespace BlackMisc
{
    namespace Aviation
    {
        CAircraftIcaoCode::CAircraftIcaoCode(const QString &designator, const QString &combinedType) :
            m_designator(designator), m_combinedType(combinedType)
        {}

        CAircraftIcaoCode::CAircraftIcaoCode(const QString &icao, const QString &combinedType, const QString &manufacturer, const QString &model, const QString &wtc, bool military, bool realworld, bool legacy)
            : m_designator(icao.trimmed().toUpper()), m_combinedType(combinedType.trimmed().toUpper()), m_manufacturer(manufacturer.trimmed()),
              m_modelDescription(model.trimmed()), m_wtc(wtc.trimmed().toUpper()), m_military(military), m_realworld(realworld), m_legacy(legacy)
        {}

        QString CAircraftIcaoCode::convertToQString(bool i18n) const
        {
            Q_UNUSED(i18n);
            QString s(this->m_designator);
            if (this->hasCombinedType()) { s.append(" ").append(this->m_combinedType); }
            if (this->hasValidWtc()) { s.append(" ").append(this->m_wtc); }
            return s;
        }

        bool CAircraftIcaoCode::hasDesignator() const
        {
            return !this->m_designator.isEmpty();
        }

        bool CAircraftIcaoCode::hasKnownDesignator() const
        {
            return (this->hasDesignator() && this->getDesignator() != "ZZZZ");
        }

        QString CAircraftIcaoCode::getEngineType() const
        {
            if (this->m_combinedType.length() != 3) return "";
            return this->m_combinedType.right(1);
        }

        bool CAircraftIcaoCode::isVtol() const
        {
            // special designators
            if (this->m_designator.length() == 4)
            {
                if (
                    this->m_designator == "BALL" ||
                    this->m_designator == "SHIP" ||
                    this->m_designator == "GYRO" ||
                    this->m_designator == "UHEL"
                ) { return true; }
            }

            if (!m_combinedType.isEmpty())
            {
                if (
                    this->m_combinedType.startsWith('G') || // gyrocopter
                    this->m_combinedType.startsWith('H') || // helicopter
                    this->m_combinedType.startsWith('T')    // tilt wing
                ) { return true; }
            }
            return false;
        }

        CVariant CAircraftIcaoCode::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return this->toCVariant(); }
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexAircraftDesignator:
                return CVariant::fromValue(this->m_designator);
            case IndexCombinedAircraftType:
                return CVariant::fromValue(this->m_combinedType);
            case IndexModelDescription:
                return CVariant::fromValue(this->m_modelDescription);
            case IndexManufacturer:
                return CVariant::fromValue(this->m_manufacturer);
            case IndexWtc:
                return CVariant::fromValue(this->m_wtc);
            case IndexIsVtol:
                return CVariant::fromValue(this->isVtol());
            case IndexIsLegacy:
                return CVariant::fromValue(this->m_legacy);
            case IndexIsMilitary:
                return CVariant::fromValue(this->m_military);
            case IndexIsRealworld:
                return CVariant::fromValue(this->m_realworld);
            default:
                return CValueObject::propertyByIndex(index);
            }
        }

        void CAircraftIcaoCode::setPropertyByIndex(const CVariant &variant, const BlackMisc::CPropertyIndex &index)
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
                this->setDesignator(variant.value<QString>());
                break;
            case IndexCombinedAircraftType:
                this->setCombinedType(variant.value<QString>());
                break;
            case IndexModelDescription:
                this->setModelDescription(variant.value<QString>());
                break;
            case IndexManufacturer:
                this->setManufacturer(variant.value<QString>());
                break;
            case IndexWtc:
                this->setWtc(variant.value<QString>());
                break;
            case IndexIsLegacy:
                this->m_legacy = variant.toBool();
                break;
            case IndexIsMilitary:
                this->m_military = variant.toBool();
                break;
            default:
                CValueObject::setPropertyByIndex(variant, index);
                break;
            }
        }

        bool CAircraftIcaoCode::isValidDesignator(const QString &designator)
        {
            static QRegularExpression regexp("^[A-Z]+[A-Z0-9]*$");
            if (designator.length() < 2 || designator.length() > 5) { return false; }
            return (regexp.match(designator).hasMatch());
        }

        bool CAircraftIcaoCode::isValidCombinedType(const QString &combinedType)
        {
            static QRegularExpression regexp("^[A-Z][0-9][A-Z]$");
            if (combinedType.length() != 3) return false;
            return (regexp.match(combinedType).hasMatch());
        }

        CAircraftIcaoCode CAircraftIcaoCode::fromDatabaseJson(const QJsonObject &json)
        {
            QJsonArray inner = json["cell"].toArray();
            if (inner.isEmpty()) { return CAircraftIcaoCode(); }
            QString combined(
                inner.at(4).toString() +
                inner.at(6).toString() +
                inner.at(5).toString()
            );
            CAircraftIcaoCode code(
                inner.at(1).toString(),
                combined,
                inner.at(2).toString(), // manufacturer
                inner.at(3).toString(), // model
                inner.at(7).toString(), // WTC
                false, // mil
                inner.at(8).toString().startsWith("Y"), // real
                inner.at(9).toString().startsWith("Y") // legacy
            );
            return code;
        }

    } // namespace
} // namespace
