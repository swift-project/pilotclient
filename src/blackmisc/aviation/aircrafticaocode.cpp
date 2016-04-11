/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/aviation/airlineicaocode.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/variant.h"
#include "blackmisc/datastoreutility.h"
#include "blackmisc/comparefunctions.h"
#include <tuple>
#include <QThreadStorage>
#include <QRegularExpression>

using namespace BlackMisc;

namespace BlackMisc
{
    namespace Aviation
    {
        CAircraftIcaoCode::CAircraftIcaoCode(const QString &designator, const QString &combinedType) :
            m_designator(designator.trimmed().toUpper()), m_combinedType(combinedType.trimmed().toUpper())
        {}

        CAircraftIcaoCode::CAircraftIcaoCode(const QString &icao, const QString &combinedType, const QString &manufacturer, const QString &model, const QString &wtc, bool realworld, bool legacy, bool military, int rank)
            : m_designator(icao.trimmed().toUpper()),
              m_combinedType(combinedType.trimmed().toUpper()),
              m_manufacturer(manufacturer.trimmed()),
              m_modelDescription(model.trimmed()), m_wtc(wtc.trimmed().toUpper()), m_realWorld(realworld), m_legacy(legacy), m_military(military), m_rank(rank)
        {
            if (m_rank < 0 || m_rank >= 10) { m_rank = 10; }
        }

        CAircraftIcaoCode::CAircraftIcaoCode(const QString &icao, const QString &iata, const QString &combinedType, const QString &manufacturer, const QString &model, const QString &wtc, bool realworld, bool legacy, bool military, int rank)
            : m_designator(icao.trimmed().toUpper()),
              m_iataCode(iata.trimmed().toUpper()),
              m_combinedType(combinedType.trimmed().toUpper()),
              m_manufacturer(manufacturer.trimmed()),
              m_modelDescription(model.trimmed()), m_wtc(wtc.trimmed().toUpper()), m_realWorld(realworld), m_legacy(legacy), m_military(military), m_rank(rank)
        {
            if (m_rank < 0 || m_rank >= 10) { m_rank = 10; }
        }

        QString CAircraftIcaoCode::convertToQString(bool i18n) const
        {
            Q_UNUSED(i18n);
            QString s(this->m_designator);
            if (this->hasValidCombinedType()) { s.append(" ").append(this->m_combinedType); }
            if (this->hasValidWtc()) { s.append(" ").append(this->m_wtc); }
            return s;
        }

        void CAircraftIcaoCode::updateMissingParts(const CAircraftIcaoCode &otherIcaoCode)
        {
            if (!this->hasValidDesignator() && otherIcaoCode.hasValidDesignator()) { this->setDesignator(otherIcaoCode.getDesignator()); }
            if (!this->hasValidWtc() && otherIcaoCode.hasValidWtc()) { this->setWtc(otherIcaoCode.getDesignator()); }
            if (!this->hasValidCombinedType() && otherIcaoCode.hasValidCombinedType()) { this->setCombinedType(otherIcaoCode.getCombinedType()); }
            if (this->m_manufacturer.isEmpty()) { this->setManufacturer(otherIcaoCode.getManufacturer());}
            if (this->m_modelDescription.isEmpty()) { this->setModelDescription(otherIcaoCode.getModelDescription()); }
            if (!this->hasValidDbKey())
            {
                // need to observe if it makes sense to copy the key but not copying the whole object
                this->setDbKey(otherIcaoCode.getDbKey());
                this->setUtcTimestamp(otherIcaoCode.getUtcTimestamp());
            }
        }

        CStatusMessageList CAircraftIcaoCode::validate() const
        {
            static const CLogCategoryList cats({ CLogCategory("swift.blackmisc.aircrafticao"), CLogCategory::validation()});
            CStatusMessageList msg;
            if (!hasKnownDesignator()) { msg.push_back(CStatusMessage(cats, CStatusMessage::SeverityError, "Aircraft ICAO: unknown designator")); }
            if (!hasValidCombinedType()) { msg.push_back(CStatusMessage(cats, CStatusMessage::SeverityError, "Aircraft ICAO: invalid combined type")); }
            if (!hasValidWtc()) { msg.push_back(CStatusMessage(cats, CStatusMessage::SeverityError, "Aircraft ICAO: wrong WTC")); }
            if (!hasManufacturer()) { msg.push_back(CStatusMessage(cats, CStatusMessage::SeverityError, "Aircraft ICAO: missing manufacturer")); }
            if (!hasModelDescription()) { msg.push_back(CStatusMessage(cats, CStatusMessage::SeverityError, "Aircraft ICAO: no description")); }
            return msg;
        }

        bool CAircraftIcaoCode::hasDesignator() const
        {
            return !this->m_designator.isEmpty();
        }

        bool CAircraftIcaoCode::hasValidDesignator() const
        {
            return isValidDesignator(this->m_designator);
        }

        bool CAircraftIcaoCode::hasKnownDesignator() const
        {
            return (this->hasValidDesignator() && this->getDesignator() != getUnassignedDesignator());
        }

        bool CAircraftIcaoCode::hasSpecialDesignator() const
        {
            if (!this->hasDesignator()) { return false; }
            return getSpecialDesignators().contains(this->getDesignator());
        }

        bool CAircraftIcaoCode::isIataSameAsDesignator() const
        {
            return hasDesignator() && hasIataCode() && m_iataCode == m_designator;
        }

        bool CAircraftIcaoCode::isFamilySameAsDesignator() const
        {
            return hasFamily() && hasDesignator() && m_designator == m_family;
        }

        bool CAircraftIcaoCode::hasValidCombinedType() const
        {
            return isValidCombinedType(getCombinedType());
        }

        QString CAircraftIcaoCode::getEngineType() const
        {
            if (this->m_combinedType.length() != 3) return "";
            return this->m_combinedType.right(1);
        }

        int CAircraftIcaoCode::getEngineCount() const
        {
            if (this->m_combinedType.length() < 2) { return -1; }
            QString c(this->m_combinedType.mid(1, 1));
            if (c == "-") { return -1; }
            bool ok;
            int ec = c.toInt(&ok);
            if (ok && ec >= 0 && ec < 10) { return ec; }
            return -1;
        }

        QString CAircraftIcaoCode::getEngineCountString() const
        {
            if (this->m_combinedType.length() < 2) { return ""; }
            return this->m_combinedType.mid(1, 1);
        }

        QString CAircraftIcaoCode::getAircraftType() const
        {
            if (this->m_combinedType.length() < 1) { return ""; }
            QString c(this->m_combinedType.left(1));
            if (c == "-") { return ""; }
            return c;
        }

        bool CAircraftIcaoCode::matchesCombinedCode(const QString &combinedCode) const
        {
            const QString cc(combinedCode.toUpper().trimmed().replace(' ', '*').replace('-', '*'));
            if (combinedCode.length() != 3) { return false; }
            if (cc == this->getCombinedType()) { return true; }

            const bool wildcard = cc.contains('*');
            if (!wildcard) { return false; }
            QChar at = cc.at(0);
            QChar c = cc.at(1);
            QChar et = cc.at(2);
            if (at != '*')
            {
                const QString cat = getAircraftType();
                if (cat.isEmpty() || cat.at(0) != at) { return false; }
            }
            if (c != '*')
            {
                if (getEngineCount() != c.digitValue()) { return false; }
            }
            if (et == '*') { return true; }
            const QString cet = getEngineType();
            return cet.length() == 1 && cet.at(0) == et;
        }

        QString CAircraftIcaoCode::getDesignatorManufacturer() const
        {
            QString d(getDesignator());
            if (this->hasManufacturer()) { d = d.append(" ").append(this->getManufacturer());}
            return d.trimmed();
        }

        bool CAircraftIcaoCode::hasManufacturer() const
        {
            return !m_manufacturer.isEmpty();
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

        void CAircraftIcaoCode::setCodeFlags(bool military, bool legacy, bool realWorld)
        {
            m_military = military;
            m_legacy = legacy;
            m_realWorld = realWorld;
        }

        void CAircraftIcaoCode::setMilitary(bool military)
        {
            m_military =  military;
        }

        void CAircraftIcaoCode::setRealWorld(bool realWorld)
        {
            m_realWorld = realWorld;
        }

        void CAircraftIcaoCode::setLegacy(bool legacy)
        {
            m_legacy = legacy;
        }

        QString CAircraftIcaoCode::getRankString() const
        {
            return QString::number(getRank());
        }

        void CAircraftIcaoCode::setRank(int rank)
        {
            if (rank < 0 || rank >= 10)
            {
                m_rank = 10;
            }
            else
            {
                m_rank = rank;
            }
        }

        QString CAircraftIcaoCode::getCombinedIcaoStringWithKey() const
        {
            QString s(getDesignator());
            if (hasManufacturer()) { s = s.append(" ").append(getManufacturer()); }
            if (hasModelDescription()) { s = s.append(" ").append(getModelDescription()); }
            return s.append(" ").append(getDbKeyAsStringInParentheses());
        }

        QString CAircraftIcaoCode::getCombinedIataStringWithKey() const
        {
            if (!this->hasIataCode()) { return ""; }
            QString s(getIataCode());
            s = s.append(" [IATA]");
            if (hasDesignator()) { s.append(" ").append(getDesignator()); }
            if (hasManufacturer()) { s = s.append(" ").append(getManufacturer()); }
            if (hasModelDescription()) { s = s.append(" ").append(getModelDescription()); }
            return s.append(" ").append(getDbKeyAsStringInParentheses());
        }

        QString CAircraftIcaoCode::getCombinedFamilyStringWithKey() const
        {
            if (!this->hasFamily()) { return ""; }
            QString s(getFamily());
            s = s.append(" [family]");
            if (hasDesignator()) { s.append(" ").append(getDesignator()); }
            if (hasManufacturer()) { s.append(" ").append(getManufacturer()); }
            if (hasModelDescription()) { s.append(" ").append(getModelDescription()); }
            return s.append(" ").append(getDbKeyAsStringInParentheses());
        }

        bool CAircraftIcaoCode::hasCompleteData() const
        {
            return hasValidCombinedType() && hasDesignator() && hasValidWtc() && hasManufacturer();
        }

        bool CAircraftIcaoCode::matchesDesignator(const QString &designator) const
        {
            if (designator.isEmpty()) { return false; }
            return designator.trimmed().toUpper() == this->m_designator;
        }

        bool CAircraftIcaoCode::matchesIataCode(const QString &iata) const
        {
            if (iata.isEmpty()) { return false; }
            return iata.trimmed().toUpper() == this->m_iataCode;
        }

        bool CAircraftIcaoCode::matchesFamily(const QString &family) const
        {
            if (family.isEmpty()) { return false; }
            return family.trimmed().toUpper() == this->m_family;
        }

        bool CAircraftIcaoCode::matchesDesignatorOrIata(const QString &icaoOrIata) const
        {
            if (icaoOrIata.isEmpty()) { return false; }
            return matchesDesignator(icaoOrIata) || matchesIataCode(icaoOrIata);
        }

        bool CAircraftIcaoCode::matchesDesignatorIataOrFamily(const QString &icaoIataOrFamily) const
        {
            if (icaoIataOrFamily.isEmpty()) { return false; }
            return matchesDesignator(icaoIataOrFamily) ||
                   matchesIataCode(icaoIataOrFamily) ||
                   matchesFamily(icaoIataOrFamily);
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
            case IndexIataCode:
                return CVariant::fromValue(this->m_iataCode);
            case IndexFamily:
                return CVariant::fromValue(this->m_family);
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
                return CVariant::fromValue(this->m_realWorld);
            case IndexRank:
                return CVariant::fromValue(this->m_rank);
            case IndexDesignatorManufacturer:
                return CVariant::fromValue(this->getDesignatorManufacturer());
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
            case IndexIataCode:
                this->setIataCode(variant.value<QString>());
                break;
            case IndexFamily:
                this->setFamily(variant.value<QString>());
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
            case IndexRank:
                this->m_rank = variant.toInt();
                break;
            default:
                CValueObject::setPropertyByIndex(variant, index);
                break;
            }
        }

        int CAircraftIcaoCode::comparePropertyByIndex(const CAircraftIcaoCode &compareValue, const CPropertyIndex &index) const
        {
            if (index.isMyself()) { return m_designator.compare(compareValue.getDesignator(), Qt::CaseInsensitive); }
            if (IDatastoreObjectWithIntegerKey::canHandleIndex(index)) { return IDatastoreObjectWithIntegerKey::comparePropertyByIndex(compareValue, index);}
            ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexAircraftDesignator:
                return m_designator.compare(compareValue.getDesignator(), Qt::CaseInsensitive);
            case IndexIataCode:
                return m_iataCode.compare(compareValue.getIataCode(), Qt::CaseInsensitive);
            case IndexFamily:
                return m_family.compare(compareValue.getFamily(), Qt::CaseInsensitive);
            case IndexCombinedAircraftType:
                return m_combinedType.compare(compareValue.getCombinedType(), Qt::CaseInsensitive);
            case IndexModelDescription:
                return m_modelDescription.compare(compareValue.getModelDescription(), Qt::CaseInsensitive);
            case IndexManufacturer:
                return m_manufacturer.compare(compareValue.getManufacturer(), Qt::CaseInsensitive);
            case IndexWtc:
                return m_wtc.compare(compareValue.getWtc(), Qt::CaseInsensitive);
            case IndexIsLegacy:
                return Compare::compare(m_legacy, compareValue.isLegacyAircraft());
            case IndexIsMilitary:
                return Compare::compare(m_military, compareValue.isMilitary());
            case IndexIsVtol:
                return Compare::compare(isVtol(), compareValue.isVtol());
            case IndexIsRealworld:
                return Compare::compare(m_realWorld, compareValue.isRealWorld());
            case IndexRank:
                return Compare::compare(m_rank, compareValue.getRank());
            case IndexDesignatorManufacturer:
                return getDesignatorManufacturer().compare(compareValue.getDesignatorManufacturer(), Qt::CaseInsensitive);
            default:
                break;
            }
            Q_ASSERT_X(false, Q_FUNC_INFO, "No comparison");
            return 0;
        }

        bool CAircraftIcaoCode::isValidDesignator(const QString &designator)
        {
            if (designator.length() < 2 || designator.length() > 5) { return false; }

            static QThreadStorage<QRegularExpression> tsRegex;
            if (! tsRegex.hasLocalData()) { tsRegex.setLocalData(QRegularExpression("^[A-Z]+[A-Z0-9]*$")); }
            const QRegularExpression &regexp = tsRegex.localData();
            return (regexp.match(designator).hasMatch());
        }

        bool CAircraftIcaoCode::isValidCombinedType(const QString &combinedType)
        {
            if (combinedType.length() != 3) { return false; }

            // Amphibian, Glider, Helicopter, Seaplane, Landplane, Tilt wing
            // Electric, Jet, Piston, Turpoprop
            static QThreadStorage<QRegularExpression> tsRegex;
            if (! tsRegex.hasLocalData()) { tsRegex.setLocalData(QRegularExpression("^[AGHSLT][0-9][EJPT]$")); }
            const QRegularExpression &regexp = tsRegex.localData();
            return (regexp.match(combinedType).hasMatch());
        }

        bool CAircraftIcaoCode::isValidWtc(const QString &candidate)
        {
            if (candidate.isEmpty()) { return true; } // we accept unspecified
            if (candidate.length() == 1)
            {
                return candidate == "L" || candidate == "M" || candidate == "H";
            }
            return false;
        }

        const QString &CAircraftIcaoCode::getUnassignedDesignator()
        {
            static const QString i("ZZZZ");
            return i;
        }

        const QStringList &CAircraftIcaoCode::getSpecialDesignators()
        {
            static const QStringList s({ "ZZZZ", "SHIP", "BALL", "GLID", "ULAC", "GYRO", "UHEL" });
            return s;
        }

        const QString CAircraftIcaoCode::normalizeDesignator(const QString candidate)
        {
            QString n(candidate.trimmed().toUpper());
            if (n.isEmpty()) { return n; }

            static QThreadStorage<QRegularExpression> tsRegex;
            if (! tsRegex.hasLocalData()) { tsRegex.setLocalData(QRegularExpression("[^a-zA-Z\\d\\s]")); }
            const QRegularExpression &regexp = tsRegex.localData();
            return n.remove(regexp);
        }

        CAircraftIcaoCode CAircraftIcaoCode::fromDatabaseJson(const QJsonObject &json, const QString &prefix)
        {
            if (!existsKey(json,  prefix))
            {
                // when using relationship, this can be null
                return CAircraftIcaoCode();
            }

            QString designator(json.value(prefix + "designator").toString());
            QString iata(json.value(prefix + "iata").toString());
            QString family(json.value(prefix + "family").toString());
            QString manufacturer(json.value(prefix + "manufacturer").toString());
            QString model(json.value(prefix + "model").toString());
            QString type(json.value(prefix + "type").toString());
            QString engine(json.value(prefix + "engine").toString());
            int engineCount(json.value(prefix + "enginecount").toInt(-1));
            QString combined(createdCombinedString(type, engineCount, engine));
            QString wtc(json.value(prefix + "wtc").toString());
            if (wtc.length() > 1 && wtc.contains("/"))
            {
                // "L/M" -> "M"
                wtc = wtc.right(1);
            }
            Q_ASSERT_X(wtc.length() < 2, Q_FUNC_INFO, "WTC too long");

            bool real = CDatastoreUtility::dbBoolStringToBool(json.value(prefix + "realworld").toString());
            bool legacy = CDatastoreUtility::dbBoolStringToBool(json.value(prefix + "legacy").toString());
            bool military = CDatastoreUtility::dbBoolStringToBool(json.value(prefix + "military").toString());
            int rank(json.value(prefix + "rank").toInt(10));

            CAircraftIcaoCode code(
                designator, iata, combined,
                manufacturer, model, wtc,
                real, legacy, military, rank
            );
            code.setFamily(family);
            code.setKeyAndTimestampFromDatabaseJson(json, prefix);
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

        QString CAircraftIcaoCode::createdCombinedString(const QString &type, int engineCount, const QString &engine)
        {
            if (engineCount >= 0 && engineCount < 10)
            {
                return createdCombinedString(type, QString::number(engineCount), engine);
            }
            else
            {
                return createdCombinedString(type, "", engine);
            }
        }
    } // namespace
} // namespace
