/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/simulation/matchingutils.h"
#include "blackmisc/aviation/aircraftsituationchange.h"
#include "blackmisc/aviation/aircrafticaocode.h"
#include "blackmisc/db/datastoreutility.h"
#include "blackmisc/comparefunctions.h"
#include "blackmisc/logcategorylist.h"
#include "blackmisc/propertyindex.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/variant.h"

#include <QChar>
#include <QJsonValue>
#include <QMultiMap>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QThreadStorage>
#include <QStringBuilder>
#include <Qt>
#include <QtGlobal>

using namespace BlackMisc;
using namespace BlackMisc::Db;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Simulation;

namespace BlackMisc
{
    namespace Aviation
    {
        CAircraftIcaoCode::CAircraftIcaoCode(const QString &icao, const QString &combinedType) :
            m_designator(icao.trimmed().toUpper()), m_combinedType(combinedType.trimmed().toUpper())
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

        CAircraftIcaoCode::CAircraftIcaoCode(const QString &icao, const QString &iata, const QString &family, const QString &combinedType, const QString &manufacturer,
                                             const QString &model, const QString &modelIata, const QString &modelSwift, const QString &wtc, bool realworld, bool legacy, bool military, int rank)
            : m_designator(icao.trimmed().toUpper()),
              m_iataCode(iata.trimmed().toUpper()),
              m_family(family.trimmed().toUpper()),
              m_combinedType(combinedType.trimmed().toUpper()),
              m_manufacturer(manufacturer.trimmed()),
              m_modelDescription(model.trimmed()), m_modelIataDescription(modelIata.trimmed()), m_modelSwiftDescription(modelSwift.trimmed()),
              m_wtc(wtc.trimmed().toUpper()), m_realWorld(realworld), m_legacy(legacy), m_military(military), m_rank(rank)
        {
            if (m_rank < 0 || m_rank >= 10) { m_rank = 10; }
        }

        QString CAircraftIcaoCode::getDesignatorDbKey() const
        {
            return (this->isLoadedFromDb()) ?
                   this->getDesignator() % u' ' % this->getDbKeyAsStringInParentheses() :
                   this->getDesignator();
        }

        QString CAircraftIcaoCode::convertToQString(bool i18n) const
        {
            Q_UNUSED(i18n);
            return QStringLiteral("%1 %2 %3").arg(this->getDesignatorDbKey(), this->getCombinedType(), this->getWtc()).trimmed();
        }

        void CAircraftIcaoCode::updateMissingParts(const CAircraftIcaoCode &otherIcaoCode)
        {
            if (!this->hasValidDesignator() && otherIcaoCode.hasValidDesignator()) { this->setDesignator(otherIcaoCode.getDesignator()); }
            if (!this->hasValidWtc() && otherIcaoCode.hasValidWtc()) { this->setWtc(otherIcaoCode.getDesignator()); }
            if (!this->hasValidCombinedType() && otherIcaoCode.hasValidCombinedType()) { this->setCombinedType(otherIcaoCode.getCombinedType()); }
            if (m_manufacturer.isEmpty()) { this->setManufacturer(otherIcaoCode.getManufacturer());}
            if (m_modelDescription.isEmpty()) { this->setModelDescription(otherIcaoCode.getModelDescription()); }
            if (m_modelIataDescription.isEmpty()) { this->setModelIataDescription(otherIcaoCode.getModelIataDescription()); }
            if (m_modelSwiftDescription.isEmpty()) { this->setModelSwiftDescription(otherIcaoCode.getModelSwiftDescription()); }
            if (m_family.isEmpty()) { this->setFamily(otherIcaoCode.getFamily()); }
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
            if (this->isLoadedFromDb())
            {
                // actually we would expect all DB data to be valid, however right now
                // we only check special cases
                if (this->getDesignator() == this->getUnassignedDesignator()) { return msg; } // DB ZZZZ
            }

            if (!hasKnownDesignator()) { msg.push_back(CStatusMessage(cats, CStatusMessage::SeverityError, "Aircraft ICAO: unknown designator")); }
            if (!hasValidCombinedType()) { msg.push_back(CStatusMessage(cats, CStatusMessage::SeverityError, "Aircraft ICAO: invalid combined type")); }
            if (!hasValidWtc()) { msg.push_back(CStatusMessage(cats, CStatusMessage::SeverityError, "Aircraft ICAO: wrong WTC")); }
            if (!hasManufacturer()) { msg.push_back(CStatusMessage(cats, CStatusMessage::SeverityError, "Aircraft ICAO: missing manufacturer")); }
            if (!hasModelDescription()) { msg.push_back(CStatusMessage(cats, CStatusMessage::SeverityError, "Aircraft ICAO: no description")); }
            return msg;
        }

        QString CAircraftIcaoCode::asHtmlSummary() const
        {
            return this->getCombinedIcaoStringWithKey();
        }

        int CAircraftIcaoCode::calculateScore(const CAircraftIcaoCode &otherCode, CStatusMessageList *log) const
        {
            if (this->isDbEqual(otherCode))
            {
                CMatchingUtils::addLogDetailsToList(log, *this, QString("Equal DB code: 100"));
                return 100;
            }

            int score = 0;
            if (this->hasValidDesignator() && this->getDesignator() == otherCode.getDesignator())
            {
                // 0..65
                score += 50; // same designator
                if (log)
                {
                    CMatchingUtils::addLogDetailsToList(log, *this, QStringLiteral("Same designator: %1").arg(score));
                }

                int scoreOld = score;
                if (this->getRank() == 0) { score += 15; }
                else if (this->getRank() == 1) { score += 12; }
                else if (this->getRank() < 10) { score += (10 - this->getRank()); }
                if (score > scoreOld)
                {
                    CMatchingUtils::addLogDetailsToList(log, *this, QStringLiteral("Added rank: %1").arg(score));
                }
            }
            else
            {
                if (this->hasFamily() && this->getFamily() == otherCode.getFamily())
                {
                    score += 30;
                    CMatchingUtils::addLogDetailsToList(log, *this, QStringLiteral("Added family: %1").arg(score));
                }
                else if (this->hasValidCombinedType() && otherCode.getCombinedType() == this->getCombinedType())
                {
                    score += 20;
                    CMatchingUtils::addLogDetailsToList(log, *this, QStringLiteral("Added combined code: %1").arg(score));
                }
                else if (this->hasValidCombinedType())
                {
                    if (this->getEnginesCount() == otherCode.getEnginesCount()) { score += 2; }
                    if (this->getEngineType() == otherCode.getEngineType()) { score += 2; }
                    if (this->getAircraftType() == otherCode.getAircraftType()) { score += 2; }
                    CMatchingUtils::addLogDetailsToList(log, *this, QStringLiteral("Added combined code parts: %1").arg(score));
                }
            }

            // 0..65 so far
            if (this->hasManufacturer() && otherCode.hasManufacturer())
            {
                if (this->matchesManufacturer(otherCode.getManufacturer()))
                {
                    score += 10;
                    CMatchingUtils::addLogDetailsToList(log, *this, QStringLiteral("Matches manufacturer '%1': %2").arg(this->getManufacturer()).arg(score));
                }
                else if (this->getManufacturer().contains(otherCode.getManufacturer(), Qt::CaseInsensitive))
                {
                    CMatchingUtils::addLogDetailsToList(log, *this, QStringLiteral("Contains manufacturer '%1': %2").arg(this->getManufacturer()).arg(score));
                    score += 5;
                }
            }

            // 0..75 so far
            if (this->isMilitary() == otherCode.isMilitary())
            {
                score += 10;
                CMatchingUtils::addLogDetailsToList(log, *this, QStringLiteral("Matches military flag '%1': %2").arg(boolToYesNo(this->isMilitary())).arg(score));
            }
            // 0..85
            return score;
        }

        void CAircraftIcaoCode::guessModelParameters(CLength &guessedCGOut, CSpeed &guessedVRotateOut) const
        {
            // we do not override values
            if (!guessedCGOut.isNull() && !guessedVRotateOut.isNull()) { return; }

            // init to defaults
            CLength guessedCG = CLength(1.5, CLengthUnit::m());
            CSpeed guessedVRotate = this->isVtol() ? CSpeed::null() : CSpeed(70, CSpeedUnit::km_h());

            const int engines = this->getEnginesCount();
            const QChar engineType = this->getEngineType().at(0).toUpper();
            do
            {
                if (engines == 1)
                {
                    if (engineType == 'T') { guessedCG = CLength(2.0, CLengthUnit::m()); break; }
                }
                else if (engines == 2)
                {
                    guessedCG = CLength(2.0, CLengthUnit::m());
                    guessedVRotate = CSpeed(100, CSpeedUnit::kts());
                    if (engineType == 'T') { guessedCG = CLength(2.0, CLengthUnit::m()); break; }
                    if (engineType == 'J')
                    {
                        // a B737 has VR 105-160kts
                        guessedVRotate = CSpeed(120, CSpeedUnit::kts());
                        guessedCG = CLength(2.5, CLengthUnit::m());
                        break;
                    }
                }
                else if (engines > 2)
                {
                    guessedCG = CLength(4.0, CLengthUnit::m());
                    guessedVRotate = CSpeed(70, CSpeedUnit::kts());
                    if (engineType == 'J')
                    {
                        // A typical B747 has VR around 160kts
                        guessedCG = CLength(6.0, CLengthUnit::m());
                        guessedVRotate = CSpeed(140, CSpeedUnit::kts());
                        break;
                    }
                }
            }
            while (false);

            if (guessedCGOut.isNull()) { guessedCGOut = guessedCG; }
            if (guessedVRotateOut.isNull()) { guessedVRotateOut = guessedVRotate; }
        }

        bool CAircraftIcaoCode::isNull() const
        {
            return m_designator.isEmpty() && m_manufacturer.isEmpty() && m_modelDescription.isEmpty();
        }

        const CAircraftIcaoCode &CAircraftIcaoCode::null()
        {
            static const CAircraftIcaoCode null;
            return null;
        }

        bool CAircraftIcaoCode::hasDesignator() const
        {
            return !m_designator.isEmpty();
        }

        bool CAircraftIcaoCode::hasValidDesignator() const
        {
            return isValidDesignator(m_designator);
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
            if (m_combinedType.length() != 3) return {};
            return m_combinedType.right(1);
        }

        QChar CAircraftIcaoCode::getEngineTypeChar() const
        {
            const QString et = this->getEngineType();
            if (et.length() == 1) { return et[0]; }
            return QChar();
        }

        int CAircraftIcaoCode::getEnginesCount() const
        {
            if (m_combinedType.length() < 2) { return -1; }
            const QString c(m_combinedType.mid(1, 1));
            if (c == "-") { return -1; }
            bool ok;
            int ec = c.toInt(&ok);
            if (ok && ec >= 0 && ec < 10) { return ec; }
            return -1;
        }

        QString CAircraftIcaoCode::getEngineCountString() const
        {
            if (m_combinedType.length() < 2) { return {}; }
            return m_combinedType.mid(1, 1);
        }

        QString CAircraftIcaoCode::getAircraftType() const
        {
            if (m_combinedType.length() < 1) { return {}; }
            QString c(m_combinedType.at(0));
            if (c == "-") { return {}; }
            return c;
        }

        QString CAircraftIcaoCode::getCombinedModelDescription() const
        {
            // Shortcut for most cases
            if (!this->hasModelIataDescription() && !this->hasModelSwiftDescription()) { return this->getModelDescription(); }

            QStringList combined({ this->getModelDescription() });
            if (this->hasModelIataDescription()) { combined.append(this->getModelIataDescription()); }
            if (this->hasModelSwiftDescription()) { combined.append(this->getModelSwiftDescription()); }
            combined.removeDuplicates();
            return combined.join(", ");
        }

        bool CAircraftIcaoCode::matchesAnyDescription(const QString &candidate) const
        {
            if (this->hasModelDescription())
            {
                if (this->getModelDescription().contains(candidate, Qt::CaseInsensitive)) { return true; }
            }
            if (this->hasModelIataDescription())
            {
                if (this->getModelIataDescription().contains(candidate, Qt::CaseInsensitive)) { return true; }
            }
            if (this->hasModelSwiftDescription())
            {
                if (this->getModelSwiftDescription().contains(candidate, Qt::CaseInsensitive)) { return true; }
            }
            return false;
        }

        bool CAircraftIcaoCode::matchesCombinedType(const QString &combinedType) const
        {
            const QString cc(combinedType.toUpper().trimmed().replace(' ', '*').replace('-', '*'));
            if (combinedType.length() != 3) { return false; }
            if (cc == this->getCombinedType()) { return true; }

            const bool wildcard = cc.contains('*');
            if (!wildcard) { return false; }
            const QChar at = cc.at(0);
            const QChar c = cc.at(1);
            const QChar et = cc.at(2);
            if (at != '*')
            {
                const QString cat = getAircraftType();
                if (cat.isEmpty() || cat.at(0) != at) { return false; }
            }
            if (c != '*')
            {
                if (getEnginesCount() != c.digitValue()) { return false; }
            }
            if (et == '*') { return true; }
            const QString cet = this->getEngineType();
            return cet.length() == 1 && cet.at(0) == et;
        }

        QString CAircraftIcaoCode::getDesignatorManufacturer() const
        {
            return (this->hasDesignator() ? this->getDesignator() : QStringLiteral("????")) %
                   (this->hasManufacturer() ? (u' ' % this->getManufacturer()) : QString());
        }

        bool CAircraftIcaoCode::hasManufacturer() const
        {
            return !m_manufacturer.isEmpty();
        }

        bool CAircraftIcaoCode::matchesManufacturer(const QString &manufacturer) const
        {
            if (manufacturer.isEmpty()) { return false; }
            return (manufacturer.length() == m_manufacturer.length() && m_manufacturer.startsWith(manufacturer, Qt::CaseInsensitive));
        }

        bool CAircraftIcaoCode::isVtol() const
        {
            // special designators
            if (m_designator.length() == 4)
            {
                if (
                    m_designator == "BALL" ||
                    m_designator == "SHIP" ||
                    m_designator == "GYRO" ||
                    m_designator == "UHEL"
                ) { return true; }
            }

            if (!m_combinedType.isEmpty())
            {
                if (
                    m_combinedType.startsWith('G') || // gyrocopter
                    m_combinedType.startsWith('H') || // helicopter
                    m_combinedType.startsWith('T')    // tilt wing
                ) { return true; }
            }
            return false;
        }

        bool CAircraftIcaoCode::isDbDuplicate() const
        {
            return m_modelIataDescription.startsWith("duplicate", Qt::CaseInsensitive) ||
                   m_modelSwiftDescription.startsWith("do not", Qt::CaseInsensitive);
        }

        void CAircraftIcaoCode::setCodeFlags(bool military, bool legacy, bool realWorld)
        {
            m_military = military;
            m_legacy = legacy;
            m_realWorld = realWorld;
        }

        void CAircraftIcaoCode::setMilitary(bool military)
        {
            m_military = military;
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
            m_rank = (rank < 0 || rank >= 10) ? 10 : rank;
        }

        QString CAircraftIcaoCode::getCombinedIcaoStringWithKey() const
        {
            return (this->hasDesignator() ? this->getDesignator() : QStringLiteral("????")) %
                   (this->hasManufacturer() ? (u' ' % this->getManufacturer()) : QString()) %
                   (this->hasModelDescription() ? (u' ' % this->getModelDescription()) : QString()) %
                   this->getDbKeyAsStringInParentheses(" ");
        }

        QString CAircraftIcaoCode::getCombinedIataStringWithKey() const
        {
            if (!this->hasIataCode()) { return {}; }
            return this->getIataCode() % u" [IATA" %
                   (this->hasDesignator() ? (u' ' % this->getDesignator()) : QString()) %
                   (this->hasManufacturer() ? (u' ' % this->getManufacturer()) : QString()) %
                   (this->hasModelDescription() ? (u' ' % this->getModelDescription()) : QString()) %
                   this->getDbKeyAsStringInParentheses(" ");
        }

        QString CAircraftIcaoCode::getCombinedFamilyStringWithKey() const
        {
            if (!this->hasFamily()) { return {}; }
            return this->getFamily() % u" [family" %
                   (this->hasDesignator() ? (u' ' % this->getDesignator()) : QString()) %
                   (this->hasManufacturer() ? (u' ' % this->getManufacturer()) : QString()) %
                   (this->hasModelDescription() ? (u' ' % this->getModelDescription()) : QString()) %
                   this->getDbKeyAsStringInParentheses(" ");
        }

        bool CAircraftIcaoCode::hasCompleteData() const
        {
            return hasValidCombinedType() && hasDesignator() && hasValidWtc() && hasManufacturer();
        }

        bool CAircraftIcaoCode::matchesDesignator(const QString &designator, int fuzzyMatch, int *result) const
        {
            Q_ASSERT_X(fuzzyMatch >= -1 && fuzzyMatch <= 100, Q_FUNC_INFO, "fuzzyMatch range 0..100 or -1");
            if (designator.isEmpty()) { return false; }
            const QString d = designator.trimmed().toUpper();
            if (fuzzyMatch >= 0)
            {
                const int r = fuzzyShortStringComparision(this->getDesignator(), d) >= fuzzyMatch;
                if (result) { *result = r; }
                return r >= fuzzyMatch;
            }
            else
            {
                const bool e = this->getDesignator() == d;
                if (result) { *result = e ? 100 : 0; }
                return e;
            }
        }

        bool CAircraftIcaoCode::matchesIataCode(const QString &iata, int fuzzyMatch, int *result) const
        {
            Q_ASSERT_X(fuzzyMatch >= -1 && fuzzyMatch <= 100, Q_FUNC_INFO, "fuzzyMatch range 0..100 or -1");
            if (iata.isEmpty()) { return false; }
            const QString i = iata.trimmed().toUpper();
            if (fuzzyMatch >= 0)
            {
                const int r = fuzzyShortStringComparision(this->getIataCode(), i) >= fuzzyMatch;
                if (result) { *result = r; }
                return r >= fuzzyMatch;
            }
            else
            {
                const bool e = this->getIataCode() == i;
                if (result) { *result = e ? 100 : 0; }
                return e;
            }
        }

        bool CAircraftIcaoCode::matchesFamily(const QString &family, int fuzzyMatch, int *result) const
        {
            Q_ASSERT_X(fuzzyMatch >= -1 && fuzzyMatch <= 100, Q_FUNC_INFO, "fuzzyMatch range 0..100 or -1");
            if (family.isEmpty()) { return false; }
            const QString f = family.trimmed().toUpper();
            if (fuzzyMatch >= 0)
            {
                const int r = fuzzyShortStringComparision(this->getFamily(), f) >= fuzzyMatch;
                if (result) { *result = r; }
                return r >= fuzzyMatch;
            }
            else
            {
                const bool e = this->getFamily() == f;
                if (result) { *result = e ? 100 : 0; }
                return e;
            }
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
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexAircraftDesignator: return CVariant::fromValue(m_designator);
            case IndexIataCode: return CVariant::fromValue(m_iataCode);
            case IndexFamily: return CVariant::fromValue(m_family);
            case IndexCombinedAircraftType: return CVariant::fromValue(m_combinedType);
            case IndexModelDescription: return CVariant::fromValue(m_modelDescription);
            case IndexModelIataDescription: return CVariant::fromValue(m_modelIataDescription);
            case IndexModelSwiftDescription: return CVariant::fromValue(m_modelSwiftDescription);
            case IndexCombinedDescription: return CVariant::fromValue(this->getCombinedModelDescription());
            case IndexManufacturer: return CVariant::fromValue(m_manufacturer);
            case IndexWtc: return CVariant::fromValue(m_wtc);
            case IndexIsVtol: return CVariant::fromValue(this->isVtol());
            case IndexIsLegacy: return CVariant::fromValue(m_legacy);
            case IndexIsMilitary: return CVariant::fromValue(m_military);
            case IndexIsRealworld: return CVariant::fromValue(m_realWorld);
            case IndexRank: return CVariant::fromValue(m_rank);
            case IndexDesignatorManufacturer: return CVariant::fromValue(this->getDesignatorManufacturer());
            default: return CValueObject::propertyByIndex(index);
            }
        }

        void CAircraftIcaoCode::setPropertyByIndex(const CPropertyIndex &index, const CVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.to<CAircraftIcaoCode>(); return; }
            if (IDatastoreObjectWithIntegerKey::canHandleIndex(index)) { IDatastoreObjectWithIntegerKey::setPropertyByIndex(index, variant); return; }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexAircraftDesignator: this->setDesignator(variant.value<QString>()); break;
            case IndexIataCode: this->setIataCode(variant.value<QString>()); break;
            case IndexFamily: this->setFamily(variant.value<QString>()); break;
            case IndexCombinedAircraftType: this->setCombinedType(variant.value<QString>()); break;
            case IndexModelDescription: this->setModelDescription(variant.value<QString>()); break;
            case IndexModelIataDescription: this->setModelIataDescription(variant.value<QString>()); break;
            case IndexModelSwiftDescription: this->setModelSwiftDescription(variant.value<QString>()); break;
            case IndexManufacturer: this->setManufacturer(variant.value<QString>()); break;
            case IndexWtc: this->setWtc(variant.value<QString>()); break;
            case IndexIsLegacy: m_legacy = variant.toBool(); break;
            case IndexIsMilitary: m_military = variant.toBool(); break;
            case IndexRank: m_rank = variant.toInt(); break;
            default: CValueObject::setPropertyByIndex(index, variant); break;
            }
        }

        int CAircraftIcaoCode::comparePropertyByIndex(const CPropertyIndex &index, const CAircraftIcaoCode &compareValue) const
        {
            if (index.isMyself()) { return m_designator.compare(compareValue.getDesignator(), Qt::CaseInsensitive); }
            if (IDatastoreObjectWithIntegerKey::canHandleIndex(index)) { return IDatastoreObjectWithIntegerKey::comparePropertyByIndex(index, compareValue);}
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexAircraftDesignator: return m_designator.compare(compareValue.getDesignator(), Qt::CaseInsensitive);
            case IndexIataCode: return m_iataCode.compare(compareValue.getIataCode(), Qt::CaseInsensitive);
            case IndexFamily: return m_family.compare(compareValue.getFamily(), Qt::CaseInsensitive);
            case IndexCombinedAircraftType: return m_combinedType.compare(compareValue.getCombinedType(), Qt::CaseInsensitive);
            case IndexModelDescription: return m_modelDescription.compare(compareValue.getModelDescription(), Qt::CaseInsensitive);
            case IndexModelIataDescription: return m_modelIataDescription.compare(compareValue.getModelIataDescription(), Qt::CaseInsensitive);
            case IndexModelSwiftDescription: return m_modelSwiftDescription.compare(compareValue.getModelSwiftDescription(), Qt::CaseInsensitive);
            case IndexCombinedDescription:
                {
                    // compare without generating new strings
                    int c = m_modelDescription.compare(compareValue.getModelDescription(), Qt::CaseInsensitive);
                    if (c == 0)
                    {
                        c = m_modelIataDescription.compare(compareValue.getModelIataDescription(), Qt::CaseInsensitive);
                        if (c == 0)
                        {
                            c = m_modelSwiftDescription.compare(compareValue.getModelSwiftDescription(), Qt::CaseInsensitive);
                        }
                    }
                    return c;
                }
            case IndexManufacturer: return m_manufacturer.compare(compareValue.getManufacturer(), Qt::CaseInsensitive);
            case IndexWtc: return m_wtc.compare(compareValue.getWtc(), Qt::CaseInsensitive);
            case IndexIsLegacy: return Compare::compare(m_legacy, compareValue.isLegacyAircraft());
            case IndexIsMilitary: return Compare::compare(m_military, compareValue.isMilitary());
            case IndexIsVtol: return Compare::compare(isVtol(), compareValue.isVtol());
            case IndexIsRealworld: return Compare::compare(m_realWorld, compareValue.isRealWorld());
            case IndexRank: return Compare::compare(m_rank, compareValue.getRank());
            case IndexDesignatorManufacturer: return getDesignatorManufacturer().compare(compareValue.getDesignatorManufacturer(), Qt::CaseInsensitive);
            default: return CValueObject::comparePropertyByIndex(index, *this);
            }
            Q_ASSERT_X(false, Q_FUNC_INFO, "No comparison");
            return 0;
        }

        bool CAircraftIcaoCode::isValidDesignator(const QString &designator)
        {
            if (designator.length() < 2 || designator.length() > 5) { return false; }
            if (!designator[0].isUpper()) { return false; }
            return !containsChar(designator, [](QChar c) { return !c.isUpper() && !c.isDigit(); });
        }

        bool CAircraftIcaoCode::isValidCombinedType(const QString &combinedType)
        {
            if (combinedType.length() != 3) { return false; }

            // Amphibian, Glider, Helicopter, Seaplane, Landplane, Tilt wing
            static const QString validDescriptions = "AGHSLT";
            // Electric, Jet, Piston, Turpoprop, and - for no engine
            static const QString validEngines = "EJPT-";

            if (!validDescriptions.contains(combinedType[0])) { return false; }
            if (!combinedType[1].isDigit()) { return false; }
            if (!validEngines.contains(combinedType[2])) { return false; }
            return true;
        }

        bool CAircraftIcaoCode::isValidWtc(const QString &candidate)
        {
            if (candidate.isEmpty()) { return true; } // we accept unspecified
            if (candidate.length() == 1)
            {
                return candidate == "L" || candidate == "M" || candidate == "H" || candidate == "J";
            }
            return false;
        }

        const QString &CAircraftIcaoCode::getUnassignedDesignator()
        {
            static const QString i("ZZZZ");
            return i;
        }

        const CAircraftIcaoCode &CAircraftIcaoCode::unassignedIcao()
        {
            static const CAircraftIcaoCode z(getUnassignedDesignator());
            return z;
        }

        const QStringList &CAircraftIcaoCode::getSpecialDesignators()
        {
            static const QStringList s({ "ZZZZ", "SHIP", "BALL", "GLID", "ULAC", "GYRO", "UHEL" });
            return s;
        }

        QString CAircraftIcaoCode::normalizeDesignator(const QString &candidate)
        {
            QString n(candidate.trimmed().toUpper());
            n = n.left(indexOfChar(n, [](QChar c) { return c.isSpace(); }));
            return removeChars(n, [](QChar c) { return !c.isLetterOrNumber(); });
        }

        QStringList CAircraftIcaoCode::alternativeCombinedCodes(const QString &combinedCode)
        {
            // manually add some replacements for frequently used types
            static const QMultiMap<QString, QString> knownCodes
            {
                { "L1P", "L2P" }, { "L1P", "S1P" },
                { "L2J", "L3J" }, { "L2J", "L4J" },
                { "L3J", "L4J" }
            };

            if (isValidCombinedType(combinedCode)) { return QStringList(); }
            if (knownCodes.contains(combinedCode))
            {
                return knownCodes.values(combinedCode);
            }

            // turn E to P engine
            if (combinedCode.endsWith("E")) { return QStringList({ combinedCode.leftRef(2) % u'P' }); }

            // turn T to H plane (tilt wing to helicopter
            if (combinedCode.startsWith("T")) { return QStringList({ u'H' % combinedCode.rightRef(2)}); }

            // based on engine count
            QStringList codes;
            int engineCount = combinedCode[1].digitValue();
            if (engineCount > 1)
            {
                for (int c = 2; c < 5; c++)
                {
                    if (c == engineCount) { continue; }
                    const QString code(combinedCode.at(0) + QString::number(c) + combinedCode.at(2));
                    codes.push_back(code);
                }
            }
            return codes;
        }

        CAircraftIcaoCode CAircraftIcaoCode::fromDatabaseJson(const QJsonObject &json, const QString &prefix)
        {
            if (!existsKey(json,  prefix))
            {
                // when using relationship, this can be null
                return CAircraftIcaoCode();
            }

            const QString designator(json.value(prefix % u"designator").toString());
            const QString iata(json.value(prefix % u"iata").toString());
            const QString family(json.value(prefix % u"family").toString());
            const QString manufacturer(json.value(prefix % u"manufacturer").toString());
            const QString model(json.value(prefix % u"model").toString());
            const QString modelIata(json.value(prefix % u"modeliata").toString());
            const QString modelSwift(json.value(prefix % u"modelswift").toString());
            const QString type(json.value(prefix % u"type").toString());
            const QString engine(json.value(prefix % u"engine").toString());
            const int engineCount(json.value(prefix % u"enginecount").toInt(-1));
            const QString combined(createdCombinedString(type, engineCount, engine));
            QString wtc(json.value(prefix % u"wtc").toString());
            if (wtc.length() > 1 && wtc.contains("/"))
            {
                // "L/M" -> "M"
                wtc = wtc.right(1);
            }
            Q_ASSERT_X(wtc.length() < 2, Q_FUNC_INFO, "WTC too long");

            const bool real = CDatastoreUtility::dbBoolStringToBool(json.value(prefix % u"realworld").toString());
            const bool legacy = CDatastoreUtility::dbBoolStringToBool(json.value(prefix % u"legacy").toString());
            const bool military = CDatastoreUtility::dbBoolStringToBool(json.value(prefix % u"military").toString());
            const int rank(json.value(prefix % u"rank").toInt(10));

            CAircraftIcaoCode code(
                designator, iata, family, combined, manufacturer,
                model, modelIata, modelSwift, wtc,
                real, legacy, military, rank
            );
            code.setKeyAndTimestampFromDatabaseJson(json, prefix);
            return code;
        }

        QString CAircraftIcaoCode::createdCombinedString(const QString &type, const QString &engineCount, const QString &engine)
        {
            Q_ASSERT_X(engineCount.length() < 2, Q_FUNC_INFO, "Wrong engine count");
            return (type.isEmpty() ? QStringLiteral("-") : type.trimmed().left(1).toUpper()) %
                   (engineCount.isEmpty() ? QStringLiteral("-") : engineCount.trimmed()) %
                   (engine.isEmpty() ? QStringLiteral("-") : engine.trimmed().left(1).toUpper());
        }

        QString CAircraftIcaoCode::createdCombinedString(const QString &type, int engineCount, const QString &engine)
        {
            const bool valid = engineCount >= 0 && engineCount < 10;
            return createdCombinedString(type, valid ? QString::number(engineCount) : "", engine);
        }
    } // namespace
} // namespace
