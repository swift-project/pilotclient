/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/db/datastoreutility.h"
#include "blackmisc/aviation/livery.h"
#include "blackmisc/mixin/mixincompare.h"
#include "blackmisc/comparefunctions.h"
#include "blackmisc/logcategories.h"
#include "blackmisc/propertyindexref.h"
#include "blackmisc/statusmessage.h"
#include "blackmisc/stringutils.h"
#include "blackmisc/verify.h"

#include <QCoreApplication>
#include <QJsonValue>
#include <Qt>
#include <QtGlobal>
#include <QStringBuilder>

using namespace BlackMisc;
using namespace BlackMisc::Db;

BLACK_DEFINE_VALUEOBJECT_MIXINS(BlackMisc::Aviation, CLivery)

namespace BlackMisc::Aviation
{
    CLivery::CLivery()
    { }

    CLivery::CLivery(const QString &combinedCode, const CAirlineIcaoCode &airline, const QString &description) :
        CLivery(-1, combinedCode, airline, description, "", "", false)
    { }

    CLivery::CLivery(const QString &combinedCode, const CAirlineIcaoCode &airline, const QString &description, const QString &colorFuselage, const QString &colorTail, bool isMilitary) :
        CLivery(-1, combinedCode, airline, description, colorFuselage, colorTail, isMilitary)
    { }

    CLivery::CLivery(const QString &combinedCode, const CAirlineIcaoCode &airline, const QString &description, const CRgbColor &colorFuselage, const CRgbColor &colorTail, bool isMilitary) :
        CLivery(-1, combinedCode, airline, description, colorFuselage, colorTail, isMilitary)
    { }

    CLivery::CLivery(int dbKey, const QString &combinedCode, const CAirlineIcaoCode &airline, const QString &description, const QString &colorFuselage, const QString &colorTail, bool isMilitary) :
        IDatastoreObjectWithIntegerKey(dbKey),
        m_airline(airline),
        m_combinedCode(combinedCode.trimmed().toUpper()), m_description(description.trimmed()),
        m_colorFuselage(CRgbColor(colorFuselage)), m_colorTail(CRgbColor(colorTail)),
        m_military(isMilitary)
    { }

    CLivery::CLivery(int dbKey, const QString &combinedCode, const CAirlineIcaoCode &airline, const QString &description, const CRgbColor &colorFuselage, const CRgbColor &colorTail, bool isMilitary) :
        IDatastoreObjectWithIntegerKey(dbKey),
        m_airline(airline),
        m_combinedCode(combinedCode.trimmed().toUpper()), m_description(description.trimmed()),
        m_colorFuselage(colorFuselage), m_colorTail(colorTail),
        m_military(isMilitary)
    { }

    QString CLivery::getCombinedCodePlusInfo() const
    {
        return this->getCombinedCode() % (
                    this->getDescription().isEmpty() ?
                    QString() :
                    (u' ' % this->getDescription()));
    }

    QString CLivery::getCombinedCodePlusId() const
    {
        return this->getCombinedCode() %
                this->getDbKeyAsStringInParentheses(" ");
    }

    QString CLivery::getCombinedCodePlusInfoAndId() const
    {
        return this->getCombinedCodePlusInfo() %
                this->getDbKeyAsStringInParentheses(" ");
    }

    bool CLivery::isContainedInSimplifiedAirlineName(const QString &candidate) const
    {
        return this->getAirlineIcaoCode().isContainedInSimplifiedName(candidate);
    }

    bool CLivery::setAirlineIcaoCode(const CAirlineIcaoCode &airlineIcao)
    {
        if (m_airline == airlineIcao) { return false; }
        m_airline = airlineIcao;
        return true;
    }

    bool CLivery::setAirlineIcaoCodeDesignator(const QString &airlineIcaoDesignator)
    {
        if (m_airline.getDesignator() == airlineIcaoDesignator) { return false; }
        m_airline.setDesignator(airlineIcaoDesignator);
        return true;
    }

    bool CLivery::hasColorFuselage() const
    {
        return m_colorFuselage.isValid();
    }

    bool CLivery::hasColorTail() const
    {
        return m_colorTail.isValid();
    }

    bool CLivery::hasValidColors() const
    {
        return this->hasColorFuselage() && this->hasColorTail();
    }

    bool CLivery::matchesCombinedCode(const QString &candidate) const
    {
        if (candidate.isEmpty() || !this->hasCombinedCode()) { return false; }
        const QString c(candidate.trimmed().toUpper());
        return c == m_combinedCode;
    }

    bool CLivery::matchesColors(const CRgbColor &fuselage, const CRgbColor &tail) const
    {
        return this->getColorFuselage() == fuselage && this->getColorTail() == tail;
    }

    QString CLivery::convertToQString(bool i18n) const
    {
        static const QString livery("Livery");
        static const QString liveryI18n(QCoreApplication::translate("Aviation", "Livery"));

        return (i18n ? liveryI18n : livery) %
                u" cc: '" % m_combinedCode %
                u"' airline: '" % m_airline.toQString(i18n) %
                u"' desc.: '" % m_description %
                u"' F: '" % m_colorFuselage.hex() %
                u"' T: '" % m_colorTail.hex() %
                u"' Mil: " % boolToYesNo(this->isMilitary());

        // force strings for translation in resource files
        (void)QT_TRANSLATE_NOOP("Aviation", "Livery");
    }

    bool CLivery::hasCompleteData() const
    {
        return !m_description.isEmpty() && !m_combinedCode.isEmpty();
    }

    CStatusMessageList CLivery::validate() const
    {
        static const CLogCategoryList cats(CLogCategoryList(this).withValidation());
        CStatusMessageList msg;
        if (!hasCombinedCode()) { msg.push_back(CStatusMessage(cats, CStatusMessage::SeverityError, u"Livery: missing livery code")); }
        if (!hasColorFuselage()) { msg.push_back(CStatusMessage(cats, CStatusMessage::SeverityWarning, u"Livery: no fuselage color")); }
        if (!hasColorTail()) { msg.push_back(CStatusMessage(cats, CStatusMessage::SeverityWarning, u"Livery: no tail color")); }
        if (this->isColorLivery())
        {
            if (!this->getAirlineIcaoCodeDesignator().isEmpty())
            {
                // color livery, supposed to have empty airline
                msg.push_back(CStatusMessage(cats, CStatusMessage::SeverityWarning, u"Livery: color livery, but airline looks odd"));
            }
        }
        else
        {
            msg.push_back(m_airline.validate());
        }
        return msg;
    }

    bool CLivery::hasValidAirlineDesignator() const
    {
        return m_airline.hasValidDesignator();
    }

    bool CLivery::hasAirlineName() const
    {
        return m_airline.hasName();
    }

    bool CLivery::hasCombinedCode() const
    {
        Q_ASSERT_X(!m_combinedCode.startsWith("." + standardLiveryMarker()), Q_FUNC_INFO, "illegal combined code");
        return !m_combinedCode.isEmpty();
    }

    bool CLivery::isAirlineLivery() const
    {
        return (m_airline.hasValidDesignator());
    }

    bool CLivery::isAirlineOperating() const
    {
        return this->isAirlineLivery() && this->getAirlineIcaoCode().isOperating();
    }

    bool CLivery::isAirlineStandardLivery() const
    {
        if (isColorLivery()) { return false; }
        return (m_airline.hasValidDesignator() && m_combinedCode.endsWith(standardLiveryMarker()));
    }

    bool CLivery::isColorLivery() const
    {
        return m_combinedCode.startsWith(colorLiveryMarker());
    }

    double CLivery::getColorDistance(const CLivery &otherLivery) const
    {
        return this->getColorDistance(otherLivery.getColorFuselage(), otherLivery.getColorTail());
    }

    double CLivery::getColorDistance(const CRgbColor &fuselage, const CRgbColor &tail) const
    {
        if (!fuselage.isValid() || !tail.isValid()) { return 1.0; }
        if (this->getColorFuselage().isValid() && this->getColorTail().isValid())
        {
            if (this->matchesColors(fuselage, tail)) { return 0.0; } // avoid rounding
            const double xDist = this->getColorFuselage().colorDistance(fuselage);
            const double yDist = this->getColorTail().colorDistance(tail);
            const double d = xDist * xDist + yDist * yDist;
            return d / 2.0; // normalize to 0..1
        }
        else
        {
            return 1.0;
        }
    }

    CLivery CLivery::fromDatabaseJson(const QJsonObject &json, const QString &prefix)
    {
        if (!existsKey(json,  prefix))
        {
            // when using relationship, this can be null
            return CLivery();
        }

        const QString combinedCode(json.value(prefix % u"combinedcode").toString());
        if (combinedCode.isEmpty())
        {
            CLivery liveryStub; // only consists of id, maybe key and timestamp
            liveryStub.setKeyVersionTimestampFromDatabaseJson(json, prefix);
            return liveryStub;
        }

        const bool isColorLivery = combinedCode.startsWith(colorLiveryMarker());
        const QString description(json.value(prefix % u"description").toString());
        const CRgbColor colorFuselage(json.value(prefix % u"colorfuselage").toString());
        const CRgbColor colorTail(json.value(prefix % u"colortail").toString());
        const bool military = CDatastoreUtility::dbBoolStringToBool(json.value(prefix % u"military").toString());
        CAirlineIcaoCode airline;
        if (!isColorLivery) { airline = CAirlineIcaoCode::fromDatabaseJson(json, "al_"); }
        CLivery livery(combinedCode, airline, description, colorFuselage, colorTail, military);
        livery.setKeyVersionTimestampFromDatabaseJson(json, prefix);

        // color liveries must have default ICAO, but airline liveries must have DB airline
        BLACK_VERIFY_X((livery.isColorLivery() && !livery.getAirlineIcaoCode().hasValidDbKey()) || (livery.isAirlineLivery() && livery.getAirlineIcaoCode().hasValidDbKey()), Q_FUNC_INFO, "inconsistent data");

        return livery;
    }

    CLivery CLivery::fromDatabaseJsonCaching(const QJsonObject &json, AirlineIcaoIdMap &airlineIcaos, const QString &prefix)
    {
        if (!existsKey(json,  prefix))
        {
            // when using relationship, this can be null
            return CLivery();
        }

        const QString combinedCode(json.value(prefix % u"combinedcode").toString());
        if (combinedCode.isEmpty())
        {
            CLivery liveryStub; // only consists of id, maybe key and timestamp
            liveryStub.setKeyVersionTimestampFromDatabaseJson(json, prefix);
            return liveryStub;
        }

        const bool isColorLivery = combinedCode.startsWith(colorLiveryMarker());
        const QString description(json.value(prefix % u"description").toString());
        const CRgbColor colorFuselage(json.value(prefix % u"colorfuselage").toString());
        const CRgbColor colorTail(json.value(prefix % u"colortail").toString());
        const bool military = CDatastoreUtility::dbBoolStringToBool(json.value(prefix % u"military").toString());

        CAirlineIcaoCode airline;
        if (!isColorLivery)
        {
            static const QString prefixAirline("al_");
            const int idAirlineIcao = json.value(prefixAirline % u"id").toInt(-1);
            const bool cachedAirlineIcao = idAirlineIcao >= 0 && airlineIcaos.contains(idAirlineIcao);

            airline = cachedAirlineIcao ?
                        airlineIcaos[idAirlineIcao] :
                        CAirlineIcaoCode::fromDatabaseJson(json, prefixAirline);

            if (!cachedAirlineIcao && airline.isLoadedFromDb())
            {
                airlineIcaos[idAirlineIcao] = airline;
            }
        }

        CLivery livery(combinedCode, airline, description, colorFuselage, colorTail, military);
        livery.setKeyVersionTimestampFromDatabaseJson(json, prefix);

        // color liveries must have default ICAO, but airline liveries must have DB airline
        BLACK_VERIFY_X((livery.isColorLivery() && !livery.getAirlineIcaoCode().hasValidDbKey()) || (livery.isAirlineLivery() && livery.getAirlineIcaoCode().hasValidDbKey()), Q_FUNC_INFO, "inconsistent data");

        return livery;
    }

    bool CLivery::isValidCombinedCode(const QString &candidate)
    {
        if (candidate.isEmpty()) { return false; }
        if (candidate.startsWith(colorLiveryMarker()))
        {
            return candidate.length() > colorLiveryMarker().length() + 1;
        }
        else
        {
            if (candidate.count('.') != 1) { return false; }
            return candidate.length() > 2;
        }
    }

    const QString &CLivery::standardLiveryMarker()
    {
        static const QString s("_STD");
        return s;
    }

    QString CLivery::getStandardCode(const CAirlineIcaoCode &airline)
    {
        QString code(airline.getVDesignator());
        return code.isEmpty() ? "" : code.append('.').append(standardLiveryMarker());
    }

    const QString &CLivery::colorLiveryMarker()
    {
        static const QString s("_CC");
        return s;
    }

    const QString &CLivery::tempLiveryCode()
    {
        static const QString temp("_CC_NOCOLOR");
        return temp;
    }

    CStatusMessage CLivery::logMessage(const CLivery &livery, const QString &message, const QStringList &extraCategories, CStatusMessage::StatusSeverity s)
    {
        static const CLogCategoryList cats({ CLogCategories::aviation() });
        const CStatusMessage m(cats.with(CLogCategoryList::fromQStringList(extraCategories)), s, livery.hasCombinedCode() ? livery.getCombinedCodePlusInfoAndId() + ": " + message.trimmed() : message.trimmed());
        return m;
    }

    void CLivery::addLogDetailsToList(CStatusMessageList *log, const CLivery &livery, const QString &message, const QStringList &extraCategories, CStatusMessage::StatusSeverity s)
    {
        if (!log) { return; }
        if (message.isEmpty()) { return; }
        log->push_back(logMessage(livery, message, extraCategories, s));
    }

    QVariant CLivery::propertyByIndex(BlackMisc::CPropertyIndexRef index) const
    {
        if (index.isMyself()) { return QVariant::fromValue(*this); }
        if (IDatastoreObjectWithIntegerKey::canHandleIndex(index)) { return IDatastoreObjectWithIntegerKey::propertyByIndex(index); }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexAirlineIcaoCode: return m_airline.propertyByIndex(index.copyFrontRemoved());
        case IndexColorFuselage:   return m_colorFuselage.propertyByIndex(index.copyFrontRemoved());;
        case IndexColorTail:       return m_colorTail.propertyByIndex(index.copyFrontRemoved());
        case IndexDescription:     return QVariant::fromValue(m_description);
        case IndexCombinedCode:    return QVariant::fromValue(m_combinedCode);
        case IndexIsMilitary:      return QVariant::fromValue(m_military);
        default: return CValueObject::propertyByIndex(index);
        }
    }

    void CLivery::setPropertyByIndex(CPropertyIndexRef index, const QVariant &variant)
    {
        if (index.isMyself()) { (*this) = variant.value<CLivery>(); return; }
        if (IDatastoreObjectWithIntegerKey::canHandleIndex(index)) { IDatastoreObjectWithIntegerKey::setPropertyByIndex(index, variant); return; }
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexDescription:     m_description = variant.toString(); break;
        case IndexAirlineIcaoCode: m_airline.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
        case IndexColorFuselage:   m_colorFuselage.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
        case IndexColorTail:       m_colorTail.setPropertyByIndex(index.copyFrontRemoved(), variant); break;
        case IndexCombinedCode:    this->setCombinedCode(variant.toString()); break;
        case IndexIsMilitary:      this->setMilitary(variant.toBool()); break;
        default: CValueObject::setPropertyByIndex(index, variant); break;
        }
    }

    int CLivery::comparePropertyByIndex(CPropertyIndexRef index, const CLivery &compareValue) const
    {
        if (index.isMyself()) { return this->getCombinedCode().compare(compareValue.getCombinedCode()); }
        if (IDatastoreObjectWithIntegerKey::canHandleIndex(index)) { return IDatastoreObjectWithIntegerKey::comparePropertyByIndex(index, compareValue);}
        const ColumnIndex i = index.frontCasted<ColumnIndex>();
        switch (i)
        {
        case IndexDescription: return m_description.compare(compareValue.getDescription(), Qt::CaseInsensitive);
        case IndexAirlineIcaoCode: return m_airline.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getAirlineIcaoCode());
        case IndexColorFuselage: return m_colorFuselage.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getColorFuselage());
        case IndexColorTail: return m_colorTail.comparePropertyByIndex(index.copyFrontRemoved(), compareValue.getColorTail());
        case IndexCombinedCode: return this->getCombinedCode().compare(compareValue.getCombinedCode());
        case IndexIsMilitary: return Compare::compare(this->isMilitary(), compareValue.isMilitary());
        default: break;
        }
        Q_ASSERT_X(false, Q_FUNC_INFO, "No compare function");
        return 0;
    }

    void CLivery::updateMissingParts(const CLivery &otherLivery)
    {
        if (!this->hasValidDbKey() && otherLivery.hasValidDbKey())
        {
            // we have no DB data, but the other one has
            // so we change roles. We take the DB object as base, and update our parts
            CLivery copy(otherLivery);
            copy.updateMissingParts(*this);
            *this = copy;
            return;
        }

        if (!m_colorFuselage.isValid()) { this->setColorFuselage(otherLivery.getColorFuselage()); }
        if (!m_colorTail.isValid())     { this->setColorTail(otherLivery.getColorTail()); }
        if (m_combinedCode.isEmpty())   { this->setCombinedCode(otherLivery.getCombinedCode());}
        if (m_description.isEmpty())    { this->setDescription(otherLivery.getDescription());}

        m_airline.updateMissingParts(otherLivery.getAirlineIcaoCode());
        if (!this->hasValidDbKey())
        {
            this->setDbKey(otherLivery.getDbKey());
            this->setUtcTimestamp(otherLivery.getUtcTimestamp());
        }
    }

    QString CLivery::asHtmlSummary(const QString &separator) const
    {
        return QStringLiteral("%1%2Airline: %3").arg(
                    this->getCombinedCodePlusInfoAndId(), separator,
                    this->getAirlineIcaoCode().getDesignator().isEmpty() ? "No airline" : this->getAirlineIcaoCode().getCombinedStringWithKey()
                ).replace(" ", "&nbsp;");
    }

    int CLivery::calculateScore(const CLivery &otherLivery, bool preferColorLiveries, CStatusMessageList *log) const
    {
        if (this->isDbEqual(otherLivery))
        {
            addLogDetailsToList(log, *this, QStringLiteral("Equal DB code: 100"));
            return 100;
        }

        // get a level
        static const int sameAirlineIcaoLevel = CAirlineIcaoCode("DLH").calculateScore(CAirlineIcaoCode("DLH"));
        Q_ASSERT_X(sameAirlineIcaoLevel == 60, Q_FUNC_INFO, "airline scoring changed");

        int score = 0;
        const double colorMultiplier = 1.0 - this->getColorDistance(otherLivery);

        if (this->isColorLivery() && otherLivery.isColorLivery())
        {
            // 2 color liveries 25..85
            score = 25;
            score += 60 * colorMultiplier;
            addLogDetailsToList(log, *this, QStringLiteral("2 color liveries, color multiplier %1: %2").arg(colorMultiplier).arg(score));
        }
        else if (this->isAirlineLivery() && otherLivery.isAirlineLivery())
        {
            // 2 airline liveries 0..85
            // 0..50 based on ICAO
            // 0..25 based on color distance
            // 0..10 based on mil.flag
            // same ICAO at least means 30, max 50
            score = qRound(0.5 * this->getAirlineIcaoCode().calculateScore(otherLivery.getAirlineIcaoCode(), log));
            score += 25 * colorMultiplier;
            addLogDetailsToList(log, *this, QStringLiteral("2 airline liveries, color multiplier %1: %2").arg(colorMultiplier).arg(score));
            if (this->isMilitary() == otherLivery.isMilitary())
            {
                addLogDetailsToList(log, *this, QStringLiteral("Mil.flag '%1' matches: %2").arg(boolToYesNo(this->isMilitary())).arg(score));
                score += 10;
            }
        }
        else if ((this->isColorLivery() && otherLivery.isAirlineLivery()) || (otherLivery.isColorLivery() && this->isAirlineLivery()))
        {
            // 1 airline, 1 color livery
            // 0 .. 50
            // 25 is weaker as same ICAO code / 2 from above
            score = preferColorLiveries ? 25 : 0;
            score += 25 * colorMultiplier; // needs to be the same as in 2 airlines
            addLogDetailsToList(log, *this, QStringLiteral("Color/airline mixed, color multiplier %1: %2").arg(colorMultiplier).arg(score));
        }
        return score;
    }

    bool CLivery::isNull() const
    {
        return m_airline.isNull() && m_combinedCode.isEmpty() && m_description.isEmpty();
    }

    const CLivery &CLivery::null()
    {
        static const CLivery null;
        return null;
    }
} // namespace
