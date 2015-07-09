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
#include "blackmisc/datastoreutility.h"
#include <tuple>
#include <QRegularExpression>

using namespace BlackMisc;

namespace BlackMisc
{
    namespace Aviation
    {
        CAircraftIcaoCode::CAircraftIcaoCode(const QString &designator, const QString &combinedType) :
            m_designator(designator), m_combinedType(combinedType)
        {}

        CAircraftIcaoCode::CAircraftIcaoCode(const QString &icao, const QString &combinedType, const QString &manufacturer, const QString &model, const QString &wtc, bool realworld, bool legacy, bool military)
            : m_designator(icao.trimmed().toUpper()), m_combinedType(combinedType.trimmed().toUpper()), m_manufacturer(manufacturer.trimmed()),
              m_modelDescription(model.trimmed()), m_wtc(wtc.trimmed().toUpper()), m_realworld(realworld), m_legacy(legacy), m_military(military)
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

        bool CAircraftIcaoCode::hasCompleteData() const
        {
            return hasCombinedType() && hasDesignator() && hasValidWtc();
        }

        CVariant CAircraftIcaoCode::propertyByIndex(const BlackMisc::CPropertyIndex &index) const
        {
            if (index.isMyself()) { return CVariant::from(*this); }
            if (IDatastoreObjectWithIntegerKey::canHandleIndex(index)) { return IDatastoreObjectWithIntegerKey::propertyByIndex(index); }
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
            if (index.isMyself()) { (*this) = variant.to<CAircraftIcaoCode>(); return; }
            if (IDatastoreObjectWithIntegerKey::canHandleIndex(index)) { IDatastoreObjectWithIntegerKey::setPropertyByIndex(variant, index); return; }
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
            Q_ASSERT_X(!inner.isEmpty(), Q_FUNC_INFO, "Missing JSON");
            if (inner.isEmpty()) { return CAircraftIcaoCode(); }

            int i = 0;
            int dbKey(inner.at(i++).toInt(-1));
            QString designator(inner.at(i++).toString());
            QString manufacturer(inner.at(i++).toString());
            QString model(inner.at(i++).toString());
            QString type(inner.at(i++).toString());
            QString engine(inner.at(i++).toString());
            QString engineCount(inner.at(i++).toString());
            QString combined(createdCombinedString(type, engineCount, engine));
            QString wtc(inner.at(i++).toString());
            if (wtc.length() > 1 && wtc.contains("/"))
            {
                // "L/M" -> "M"
                wtc = wtc.right(1);
            }
            bool real = CDatastoreUtility::dbBoolStringToBool(inner.at(i++).toString());
            bool legacy = CDatastoreUtility::dbBoolStringToBool(inner.at(i++).toString());
            bool military = CDatastoreUtility::dbBoolStringToBool(inner.at(i++).toString());

            Q_ASSERT_X(wtc.length() < 2, Q_FUNC_INFO, "WTC too long");

            CAircraftIcaoCode code(
                designator,
                combined,
                manufacturer,
                model,
                wtc,
                real,
                legacy,
                military
            );
            code.setDbKey(dbKey);
            return code;
        }

        QString CAircraftIcaoCode::createdCombinedString(const QString &type, const QString &engineCount, const QString &engine)
        {
            Q_ASSERT_X(engineCount.length() < 2, Q_FUNC_INFO, "Wrong engine count");

            QString c(type.trimmed().toUpper().left(1));
            if (c.isEmpty()) { c.append("-"); }
            if (engineCount.isEmpty())
            {
                c.append("-");
            }
            else
            {
                c.append(engineCount);
            }
            if (engine.isEmpty())
            {
                c.append("-");
            }
            else
            {
                c.append(engine.left(1));
            }
            Q_ASSERT_X(c.length() == 3, Q_FUNC_INFO, "Wrong combined length");
            return c;
        }

    } // namespace
} // namespace
