/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "airlineicaocode.h"
#include "callsign.h"
#include "blackmisc/db/datastoreutility.h"
#include "blackmisc/comparefunctions.h"
#include "blackmisc/icons.h"
#include "blackmisc/logcategories.h"
#include "blackmisc/propertyindexref.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/stringutils.h"
#include "blackmisc/swiftdirectories.h"
#include "blackmisc/directoryutils.h"
#include "blackmisc/verify.h"

#include <QJsonValue>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QStringBuilder>
#include <QDir>
#include <Qt>
#include <QtGlobal>

using namespace BlackMisc;
using namespace BlackMisc::Db;

namespace BlackMisc
{
    namespace Aviation
    {
        CAirlineIcaoCode::CAirlineIcaoCode(const QString &airlineDesignator)
            : m_designator(airlineDesignator.trimmed().toUpper())
        {
            if (m_designator.length() == 4)
            {
                this->setDesignator(m_designator);
            }
        }

        CAirlineIcaoCode::CAirlineIcaoCode(const QString &airlineDesignator, const QString &airlineName, const CCountry &country, const QString &telephony, bool virtualAirline, bool operating)
            : m_designator(airlineDesignator.trimmed().toUpper()), m_name(airlineName), m_telephonyDesignator(telephony), m_country(country), m_isVa(virtualAirline), m_isOperating(operating)
        {
            if (m_designator.length() == 4)
            {
                this->setDesignator(m_designator);
            }
        }

        const QString CAirlineIcaoCode::getVDesignator() const
        {
            if (!this->isVirtualAirline()) { return m_designator; }
            return u'V' % m_designator;
        }

        QString CAirlineIcaoCode::getVDesignatorDbKey() const
        {
            return this->isLoadedFromDb() ?
                   this->getVDesignator() % this->getDbKeyAsStringInParentheses(" ") :
                   this->getVDesignator();
        }

        void CAirlineIcaoCode::setDesignator(const QString &icaoDesignator)
        {
            m_designator = icaoDesignator.trimmed().toUpper();
            if (m_designator.length() == 4 && m_designator.startsWith("V"))
            {
                // a virtual designator was provided
                this->setVirtualAirline(true);
                m_designator = m_designator.right(3);
            }
        }

        QString CAirlineIcaoCode::getDesignatorDbKey() const
        {
            return (this->isLoadedFromDb()) ?
                   this->getDesignator() % QStringLiteral(" ") % this->getDbKeyAsStringInParentheses() :
                   this->getDesignator();
        }

        QString CAirlineIcaoCode::getDesignatorNameCountry() const
        {
            return this->getDesignator() %
                   (this->hasName() ? (u' ' % this->getName()) : QString()) %
                   (this->hasValidCountry() ? (u' ' % this->getCountryIso()) : QString());
        }

        QString CAirlineIcaoCode::getSimplifiedName() const
        {
            return simplifyNameForSearch(this->getName());
        }

        bool CAirlineIcaoCode::hasValidCountry() const
        {
            return m_country.isValid();
        }

        bool CAirlineIcaoCode::hasValidDesignator() const
        {
            return isValidAirlineDesignator(m_designator);
        }

        bool CAirlineIcaoCode::hasIataCode() const
        {
            return !m_iataCode.isEmpty();
        }

        bool CAirlineIcaoCode::matchesDesignator(const QString &designator) const
        {
            if (designator.isEmpty() || m_designator.isEmpty()) { return false; }
            return caseInsensitiveStringCompare(m_designator, designator.trimmed());
        }

        bool CAirlineIcaoCode::matchesVDesignator(const QString &designator) const
        {
            if (designator.isEmpty() || m_designator.isEmpty()) { return false; }
            return caseInsensitiveStringCompare(this->getVDesignator(), designator.trimmed());
        }

        bool CAirlineIcaoCode::matchesIataCode(const QString &iata) const
        {
            if (iata.isEmpty() || m_iataCode.isEmpty()) { return false; }
            return caseInsensitiveStringCompare(m_iataCode, iata.trimmed());
        }

        bool CAirlineIcaoCode::matchesDesignatorOrIataCode(const QString &candidate) const
        {
            if (candidate.isEmpty()) { return false; }
            return this->matchesDesignator(candidate) || this->matchesIataCode(candidate);
        }

        bool CAirlineIcaoCode::matchesVDesignatorOrIataCode(const QString &candidate) const
        {
            if (candidate.isEmpty()) { return false; }
            return this->matchesVDesignator(candidate) || this->matchesIataCode(candidate);
        }

        bool CAirlineIcaoCode::matchesTelephonyDesignator(const QString &candidate) const
        {
            if (candidate.isEmpty() || m_telephonyDesignator.isEmpty()) { return false; }
            return caseInsensitiveStringCompare(m_telephonyDesignator, candidate.trimmed());
        }

        bool CAirlineIcaoCode::matchesNamesOrTelephonyDesignator(const QString &candidate) const
        {
            const QString cand(candidate.toUpper().trimmed());
            if (this->getName().contains(cand, Qt::CaseInsensitive) || this->getTelephonyDesignator().contains(cand, Qt::CaseInsensitive)) { return true; }
            return this->isContainedInSimplifiedName(candidate);
        }

        bool CAirlineIcaoCode::isContainedInSimplifiedName(const QString &candidate) const
        {
            if (candidate.isEmpty() || !this->hasName()) { return false; }
            auto simplifiedName = makeRange(getName().begin(), getName().end()).findBy([](QChar c) { return c.isLetter(); });
            auto it = std::search(simplifiedName.begin(), simplifiedName.end(), candidate.begin(), candidate.end(), [](QChar a, QChar b) { return a.toUpper() == b.toUpper(); });
            return it != simplifiedName.end();
        }

        bool CAirlineIcaoCode::hasSimplifiedName() const
        {
            return this->hasName() && !this->getSimplifiedName().isEmpty();
        }

        bool CAirlineIcaoCode::hasCompleteData() const
        {
            return this->hasValidDesignator() && this->hasValidCountry() && this->hasName();
        }

        CIcons::IconIndex CAirlineIcaoCode::toIcon() const
        {
            //if (this->hasValidDbKey() && CAirlineIcaoCode::iconIds().contains(this->getDbKey()))
            //{
            //    static const QString p("airlines/%1_%2.png");
            //    const QString n(p.arg(this->getDbKey(), 5, 10, QChar('0')).arg(this->getDesignator()));
            //    return CIcon(n, this->convertToQString());
            //}
            return CIcons::StandardIconEmpty;
        }

        QString CAirlineIcaoCode::getIconResourcePath() const
        {
            if (this->hasValidDbKey() && CAirlineIcaoCode::iconIds().contains(this->getDbKey()))
            {
                static const QString p("airlines/%1_%2.png");
                const QString n(p.arg(this->getDbKey(), 5, 10, QChar('0')).arg(this->getDesignator()));
                return CFileUtils::appendFilePaths(CSwiftDirectories::imagesDirectory(), n);
            }
            return {};
        }

        QString CAirlineIcaoCode::getIconAsHTMLImage() const
        {
            if (this->hasValidDbKey() && CAirlineIcaoCode::iconIds().contains(this->getDbKey()))
            {
                return u"<img src=\"" % this->getIconResourcePath() % u"\">";
            }
            return {};
        }

        QString CAirlineIcaoCode::convertToQString(bool i18n) const
        {
            Q_UNUSED(i18n);
            const QString s = this->getDesignatorDbKey() %
                              (this->hasName() ? u' ' % m_name : QString()) %
                              u" Op: " % boolToYesNo(this->isOperating()) %
                              u" VA: " % boolToYesNo(this->isVirtualAirline()) %
                              u" Mil: " % boolToYesNo(this->isMilitary());
            return s.trimmed();
        }

        QVariant CAirlineIcaoCode::propertyByIndex(CPropertyIndexRef index) const
        {
            if (index.isMyself()) { return QVariant::fromValue(*this); }
            if (IDatastoreObjectWithIntegerKey::canHandleIndex(index)) { return IDatastoreObjectWithIntegerKey::propertyByIndex(index); }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexAirlineDesignator: return QVariant::fromValue(m_designator);
            case IndexIataCode: return QVariant::fromValue(m_iataCode);
            case IndexAirlineCountryIso: return QVariant::fromValue(this->getCountryIso());
            case IndexAirlineCountry: return m_country.propertyByIndex(index.copyFrontRemoved());
            case IndexAirlineName: return QVariant::fromValue(m_name);
            case IndexAirlineIconHTML: return QVariant::fromValue(this->getIconAsHTMLImage());
            case IndexTelephonyDesignator: return QVariant::fromValue(m_telephonyDesignator);
            case IndexIsVirtualAirline: return QVariant::fromValue(m_isVa);
            case IndexIsOperating: return QVariant::fromValue(m_isOperating);
            case IndexIsMilitary: return QVariant::fromValue(m_isMilitary);
            case IndexDesignatorNameCountry: return QVariant::fromValue(this->getDesignatorNameCountry());
            case IndexGroupDesignator: return QVariant::fromValue(this->getGroupDesignator());
            case IndexGroupName: return QVariant::fromValue(this->getGroupName());
            case IndexGroupId: return QVariant::fromValue(this->getGroupId());
            default: return CValueObject::propertyByIndex(index);
            }
        }

        void CAirlineIcaoCode::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
        {
            if (index.isMyself()) { (*this) = variant.value<CAirlineIcaoCode>(); return; }
            if (IDatastoreObjectWithIntegerKey::canHandleIndex(index)) { IDatastoreObjectWithIntegerKey::setPropertyByIndex(index, variant); return; }
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexAirlineDesignator: this->setDesignator(variant.value<QString>()); break;
            case IndexIataCode: this->setIataCode(variant.value<QString>()); break;
            case IndexAirlineCountry: this->setCountry(variant.value<CCountry>()); break;
            case IndexAirlineName: this->setName(variant.value<QString>()); break;
            case IndexTelephonyDesignator: this->setTelephonyDesignator(variant.value<QString>()); break;
            case IndexIsVirtualAirline: this->setVirtualAirline(variant.toBool()); break;
            case IndexIsOperating: this->setOperating(variant.toBool()); break;
            case IndexIsMilitary: this->setMilitary(variant.toBool()); break;
            case IndexGroupDesignator: this->setGroupDesignator(variant.toString()); break;
            case IndexGroupName: this->setGroupName(variant.toString()); break;
            case IndexGroupId: this->setGroupId(variant.toInt()); break;
            default: CValueObject::setPropertyByIndex(index, variant); break;
            }
        }

        int CAirlineIcaoCode::comparePropertyByIndex(CPropertyIndexRef index, const CAirlineIcaoCode &compareValue) const
        {
            if (index.isMyself()) { return m_designator.compare(compareValue.getDesignator(), Qt::CaseInsensitive); }
            if (IDatastoreObjectWithIntegerKey::canHandleIndex(index)) { return IDatastoreObjectWithIntegerKey::comparePropertyByIndex(index, compareValue);}
            const ColumnIndex i = index.frontCasted<ColumnIndex>();
            switch (i)
            {
            case IndexAirlineIconHTML:
            case IndexAirlineDesignator: return m_designator.compare(compareValue.getDesignator());
            case IndexIataCode: return m_iataCode.compare(compareValue.getIataCode());
            case IndexAirlineCountry: return m_country.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getCountry());
            case IndexDesignatorNameCountry: return m_country.getName().compare(compareValue.getCountry().getName(), Qt::CaseInsensitive);
            case IndexAirlineName: return m_name.compare(compareValue.getName(), Qt::CaseInsensitive);
            case IndexTelephonyDesignator: return m_telephonyDesignator.compare(compareValue.getTelephonyDesignator(), Qt::CaseInsensitive);
            case IndexIsVirtualAirline: return Compare::compare(this->isVirtualAirline(), compareValue.isVirtualAirline());
            case IndexIsOperating: return Compare::compare(this->isOperating(), compareValue.isOperating());
            case IndexIsMilitary: return Compare::compare(this->isMilitary(), compareValue.isMilitary());
            case IndexGroupDesignator: return m_groupDesignator.compare(compareValue.getGroupDesignator(), Qt::CaseInsensitive);
            case IndexGroupName: return m_groupName.compare(compareValue.getGroupName(), Qt::CaseInsensitive);
            case IndexGroupId: return Compare::compare(m_groupId, compareValue.getGroupId());
            default: break;
            }
            Q_ASSERT_X(false, Q_FUNC_INFO, "No compare function");
            return 0;
        }

        CStatusMessageList CAirlineIcaoCode::validate() const
        {
            static const CLogCategoryList cats(CLogCategoryList(this).withValidation());
            CStatusMessageList msgs;
            if (!hasValidDesignator()) { msgs.push_back(CStatusMessage(cats, CStatusMessage::SeverityError, u"Airline: missing designator")); }
            if (!hasValidCountry()) { msgs.push_back(CStatusMessage(cats, CStatusMessage::SeverityError, u"Airline: missing country")); }
            if (!hasName()) { msgs.push_back(CStatusMessage(cats, CStatusMessage::SeverityError, u"Airline: no name")); }
            return msgs;
        }

        bool CAirlineIcaoCode::isValidAirlineDesignator(const QString &airline)
        {
            // allow 2 chars for special codes like "VV"
            if (airline.length() < 2 || airline.length() > 5) { return false; }
            const auto chars = makeRange(airline.begin(), airline.end());
            if (chars.containsBy([](QChar c) { return !c.isUpper() && !c.isDigit(); })) { return false; }
            return true;
        }

        bool CAirlineIcaoCode::isValidIataCode(const QString &iataCode)
        {
            if (iataCode.length() != 2) { return false; }
            return isValidAirlineDesignator(iataCode); // allow some chars as in IACO
        }

        QSet<QString> CAirlineIcaoCode::specialValidDesignators()
        {
            static const QSet<QString> valid({ "VV", "VM"});
            return valid;
        }

        QString CAirlineIcaoCode::normalizeDesignator(const QString &candidate)
        {
            QString n(candidate.trimmed().toUpper());
            n = n.left(indexOfChar(n, [](QChar c) { return c.isSpace(); }));
            return removeChars(n, [](QChar c) { return !c.isLetterOrNumber(); });
        }

        CStatusMessage CAirlineIcaoCode::logMessage(const CAirlineIcaoCode &icaoCode, const QString &message, const QStringList &extraCategories, CStatusMessage::StatusSeverity s)
        {
            static const CLogCategoryList cats({ CLogCategories::aviation() });
            const CStatusMessage m(cats.with(CLogCategoryList::fromQStringList(extraCategories)), s, icaoCode.hasValidDesignator() ? icaoCode.getVDesignatorDbKey() + ": " + message.trimmed() : message.trimmed());
            return m;
        }

        void CAirlineIcaoCode::addLogDetailsToList(CStatusMessageList *log, const CAirlineIcaoCode &icao, const QString &message, const QStringList &extraCategories, CStatusMessage::StatusSeverity s)
        {
            if (!log) { return; }
            if (message.isEmpty()) { return; }
            log->push_back(logMessage(icao, message, extraCategories, s));
        }

        QString CAirlineIcaoCode::getCombinedStringWithKey() const
        {
            return (this->hasValidDesignator() ? this->getVDesignator() : QString()) %
                   (this->hasName() ? u' ' % m_name : QString()) %
                   this->getDbKeyAsStringInParentheses(" ");
        }

        CAirlineIcaoCode CAirlineIcaoCode::thisOrCallsignCode(const CCallsign &callsign) const
        {
            if (this->hasValidDbKey()) { return *this; }
            if (callsign.isEmpty())    { return *this; }
            const QString callsignAirline = callsign.getAirlinePrefix();
            if (callsignAirline.isEmpty()) { return *this; }
            if (callsignAirline == m_designator) { return *this; }

            const CAirlineIcaoCode callsignIcao(callsignAirline);
            if (m_designator.isEmpty()) { return callsignIcao; }

            // here we have 2 possible codes
            if (callsignIcao.isVirtualAirline())
            {

                if (callsignIcao.getDesignator().endsWith(m_designator))
                {
                    // callsign ICAO is virtual airline of myself, this is more accurate
                    return callsignIcao;
                }
            }
            return *this;
        }

        QString CAirlineIcaoCode::getNameWithKey() const
        {
            if (!this->hasValidDbKey()) { return this->getName(); }
            return this->hasName() ?
                   QString(this->getName()).append(" ").append(this->getDbKeyAsStringInParentheses()) :
                   this->getDbKeyAsStringInParentheses();
        }

        void CAirlineIcaoCode::updateMissingParts(const CAirlineIcaoCode &otherIcaoCode)
        {
            if (!this->hasValidDbKey() && otherIcaoCode.hasValidDbKey())
            {
                // we have no DB data, but the other one has
                // so we change roles. We take the DB object as base, and update our parts
                CAirlineIcaoCode copy(otherIcaoCode);
                copy.updateMissingParts(*this);
                *this = copy;
                return;
            }

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

        QString CAirlineIcaoCode::asHtmlSummary() const
        {
            return this->getCombinedStringWithKey();
        }

        int CAirlineIcaoCode::calculateScore(const CAirlineIcaoCode &otherCode, CStatusMessageList *log) const
        {
            if (this->isDbEqual(otherCode))
            {
                addLogDetailsToList(log, *this, QStringLiteral("DB equal score: 100"));
                return 100;
            }
            const bool bothFromDb = this->isLoadedFromDb() && otherCode.isLoadedFromDb();
            int score = 0;
            if (otherCode.hasValidDesignator() && this->getDesignator() == otherCode.getDesignator())
            {
                score += 60;
                addLogDetailsToList(log, *this, QStringLiteral("Same designator: %1").arg(score));
            }

            // only for DB values we check VA
            if (bothFromDb && this->isVirtualAirline() == otherCode.isVirtualAirline())
            {
                score += 20;
                addLogDetailsToList(log, *this, QStringLiteral("VA equality: %1").arg(score));
            }

            // consider the various names
            if (this->hasName() && this->getName() == otherCode.getName())
            {
                score += 20;
                addLogDetailsToList(log, *this, QStringLiteral("Same name '%1': %2").arg(this->getName()).arg(score));
            }
            else if (this->hasTelephonyDesignator() && this->getTelephonyDesignator() == otherCode.getTelephonyDesignator())
            {
                score += 15;
                addLogDetailsToList(log, *this, QStringLiteral("Same telephony '%1': %2").arg(this->getTelephonyDesignator()).arg(score));
            }
            else if (this->hasSimplifiedName() && this->getSimplifiedName() == otherCode.getSimplifiedName())
            {
                score += 10;
                addLogDetailsToList(log, *this, QStringLiteral("Same simplified name '%1': %2").arg(this->getSimplifiedName()).arg(score));
            }
            return score;
        }

        bool CAirlineIcaoCode::isNull() const
        {
            return m_designator.isNull() && m_iataCode.isNull() && m_telephonyDesignator.isNull();
        }

        const CAirlineIcaoCode &CAirlineIcaoCode::null()
        {
            static const CAirlineIcaoCode null;
            return null;
        }

        CAirlineIcaoCode CAirlineIcaoCode::fromDatabaseJson(const QJsonObject &json, const QString &prefix)
        {
            if (!existsKey(json, prefix))
            {
                // when using relationship, this can be null (e.g. for color liveries)
                return CAirlineIcaoCode();
            }

            QString designator(json.value(prefix % u"designator").toString());
            if (!CAirlineIcaoCode::isValidAirlineDesignator(designator))
            {
                designator = CAirlineIcaoCode::normalizeDesignator(designator);
            }

            const QString iata(json.value(prefix % u"iata").toString());
            const QString telephony(json.value(prefix % u"callsign").toString());
            const QString name(json.value(prefix % u"name").toString());
            const QString countryIso(json.value(prefix % u"country").toString());
            const QString countryName(json.value(prefix % u"countryname").toString());
            const QString groupName(json.value(prefix % u"groupname").toString());
            const QString groupDesignator(json.value(prefix % u"groupdesignator").toString());
            const int groupId(json.value(prefix % u"groupid").toInt(-1));
            const bool va = CDatastoreUtility::dbBoolStringToBool(json.value(prefix % u"va").toString());
            const bool operating = CDatastoreUtility::dbBoolStringToBool(json.value(prefix % u"operating").toString());
            const bool military = CDatastoreUtility::dbBoolStringToBool(json.value(prefix % u"military").toString());

            CAirlineIcaoCode code(
                designator, name,
                CCountry(countryIso, countryName),
                telephony, va, operating
            );
            code.setIataCode(iata);
            code.setMilitary(military);
            code.setGroupDesignator(groupDesignator);
            code.setGroupId(groupId);
            code.setGroupName(groupName);
            code.setKeyVersionTimestampFromDatabaseJson(json, prefix);
            return code;
        }

        //! \private
        QSet<int> iconIdsImpl()
        {
            QDir dir(CSwiftDirectories::imagesAirlinesDirectory());
            Q_ASSERT_X(dir.exists(), Q_FUNC_INFO, "image directory missing");

            QSet<int> ids;
            dir.setFilter(QDir::Files | QDir::NoSymLinks);
            dir.setSorting(QDir::Name);
            for (const QFileInfo &fileInfo : dir.entryInfoList())
            {
                const QString fn(fileInfo.fileName());
                bool ok = fn.size() > 5;
                if (!ok) { continue; }
                BLACK_VERIFY_X(ok, Q_FUNC_INFO, "wrong file name");
                const int id = fn.leftRef(5).toInt(&ok);
                BLACK_VERIFY_X(ok, Q_FUNC_INFO, "wrong id format");
                if (!ok) { continue; }
                ids.insert(id);
            }
            return ids;
        }

        const QSet<int> &CAirlineIcaoCode::iconIds()
        {
            static const QSet<int> ids = iconIdsImpl();
            return ids;
        }
    } // namespace
} // namespace
