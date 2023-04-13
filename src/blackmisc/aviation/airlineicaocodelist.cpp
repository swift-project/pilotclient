/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackmisc/aviation/airlineicaocodelist.h"
#include "blackmisc/logcategories.h"
#include "blackmisc/country.h"
#include "blackmisc/setbuilder.h"

#include <QJsonValue>
#include <QStringBuilder>

BLACK_DEFINE_SEQUENCE_MIXINS(BlackMisc::Aviation, CAirlineIcaoCode, CAirlineIcaoCodeList)

namespace BlackMisc::Aviation
{
    const QStringList &CAirlineIcaoCodeList::getLogCategories()
    {
        static const QStringList cats(CLogCategories::aviation());
        return cats;
    }

    CAirlineIcaoCodeList::CAirlineIcaoCodeList()
    {}

    CAirlineIcaoCodeList::CAirlineIcaoCodeList(const CSequence<CAirlineIcaoCode> &other) : CSequence<CAirlineIcaoCode>(other)
    {}

    CAirlineIcaoCodeList::CAirlineIcaoCodeList(std::initializer_list<CAirlineIcaoCode> il) : CSequence<CAirlineIcaoCode>(il)
    {}

    CAirlineIcaoCodeList CAirlineIcaoCodeList::findByDesignator(const QString &designator) const
    {
        if (!CAirlineIcaoCode::isValidAirlineDesignator(designator)) { return CAirlineIcaoCodeList(); }
        return this->findBy([&](const CAirlineIcaoCode &code) {
            return code.matchesDesignator(designator);
        });
    }

    CAirlineIcaoCodeList CAirlineIcaoCodeList::findByIataCode(const QString &iata) const
    {
        if (!CAirlineIcaoCode::isValidIataCode(iata)) { return CAirlineIcaoCodeList(); }
        return this->findBy([&](const CAirlineIcaoCode &code) {
            return code.matchesIataCode(iata);
        });
    }

    CAirlineIcaoCode CAirlineIcaoCodeList::findByUniqueIataCodeOrDefault(const QString &iata) const
    {
        const CAirlineIcaoCodeList codes = this->findByIataCode(iata);
        return codes.size() == 1 ? codes.front() : CAirlineIcaoCode();
    }

    CAirlineIcaoCodeList CAirlineIcaoCodeList::findByDesignatorOrIataCode(const QString &designatorOrIata) const
    {
        if (designatorOrIata.isEmpty()) { return CAirlineIcaoCodeList(); }
        return this->findBy([&](const CAirlineIcaoCode &code) {
            return code.matchesDesignatorOrIataCode(designatorOrIata);
        });
    }

    CAirlineIcaoCodeList CAirlineIcaoCodeList::findByVDesignator(const QString &designator) const
    {
        if (!CAirlineIcaoCode::isValidAirlineDesignator(designator)) { return CAirlineIcaoCodeList(); }
        return this->findBy([&](const CAirlineIcaoCode &code) {
            return code.matchesVDesignator(designator);
        });
    }

    CAirlineIcaoCode CAirlineIcaoCodeList::findByUniqueVDesignatorOrDefault(const QString &designator, bool preferOperatingAirlines) const
    {
        CAirlineIcaoCodeList codes = this->findByVDesignator(designator);
        if (codes.size() > 1 && preferOperatingAirlines)
        {
            codes.removeIf(&CAirlineIcaoCode::isOperating, false);
        }
        return codes.size() == 1 ? codes.front() : CAirlineIcaoCode();
    }

    CAirlineIcaoCodeList CAirlineIcaoCodeList::findByVDesignatorOrIataCode(const QString &designatorOrIata) const
    {
        if (designatorOrIata.isEmpty()) { return CAirlineIcaoCodeList(); }
        return this->findBy([&](const CAirlineIcaoCode &code) {
            return code.matchesVDesignatorOrIataCode(designatorOrIata);
        });
    }

    CAirlineIcaoCodeList CAirlineIcaoCodeList::findByCountryIsoCode(const QString &isoCode) const
    {
        if (isoCode.length() != 2) { return CAirlineIcaoCodeList(); }
        const QString iso(isoCode.toUpper());
        return this->findBy([&](const CAirlineIcaoCode &code) {
            return code.getCountry().getIsoCode() == iso;
        });
    }

    CAirlineIcaoCodeList CAirlineIcaoCodeList::findBySimplifiedNameContaining(const QString &containedString) const
    {
        if (containedString.isEmpty()) { return CAirlineIcaoCodeList(); }
        return this->findBy([&](const CAirlineIcaoCode &code) {
            return code.isContainedInSimplifiedName(containedString);
        });
    }

    CAirlineIcaoCodeList CAirlineIcaoCodeList::findByTelephonyDesignator(const QString &candidate) const
    {
        if (candidate.isEmpty()) { return CAirlineIcaoCodeList(); }
        return this->findBy([&](const CAirlineIcaoCode &code) {
            return code.matchesTelephonyDesignator(candidate);
        });
    }

    CAirlineIcaoCodeList CAirlineIcaoCodeList::findByNamesOrTelephonyDesignator(const QString &candidate) const
    {
        if (candidate.isEmpty()) { return CAirlineIcaoCodeList(); }
        return this->findBy([&](const CAirlineIcaoCode &code) {
            return code.matchesNamesOrTelephonyDesignator(candidate);
        });
    }

    CAirlineIcaoCodeList CAirlineIcaoCodeList::findByMilitary(bool military) const
    {
        return this->findBy([&](const CAirlineIcaoCode &code) {
            return code.isMilitary() == military;
        });
    }

    CAirlineIcaoCodeList CAirlineIcaoCodeList::findByValidDesignator() const
    {
        return this->findBy([](const CAirlineIcaoCode &code) {
            return code.hasValidDesignator();
        });
    }

    CAirlineIcaoCodeList CAirlineIcaoCodeList::findByInvalidDesignator() const
    {
        return this->findBy([](const CAirlineIcaoCode &code) {
            return !code.hasValidDesignator();
        });
    }

    CAirlineIcaoCode CAirlineIcaoCodeList::smartAirlineIcaoSelector(const CAirlineIcaoCode &icaoPattern, const CCallsign &callsign) const
    {
        if (icaoPattern.hasValidDbKey()) { return icaoPattern; }
        const CAirlineIcaoCode patternUsed = icaoPattern.thisOrCallsignCode(callsign);

        // search by parts
        CAirlineIcaoCodeList codesFound;
        if (patternUsed.hasValidDesignator())
        {
            if (patternUsed.isVirtualAirline())
            {
                // we can tell for sure we search an VA
                codesFound = this->findByVDesignator(patternUsed.getVDesignator());
            }
            else
            {
                // we do not know if we are looking for an VA
                codesFound = this->findByDesignator(patternUsed.getDesignator());
            }
        }
        else
        {
            codesFound = this->findByIataCode(patternUsed.getIataCode());
        }

        if (codesFound.size() == 1) { return codesFound.front(); }
        if (codesFound.isEmpty())
        {
            // nothing found so far
            codesFound = this->findByNamesOrTelephonyDesignator(patternUsed.getName());
            codesFound = codesFound.ifPossibleReduceByTelephonyDesignator(patternUsed.getTelephonyDesignator());
            codesFound = codesFound.ifPossibleReduceByCountry(patternUsed.getCountryIso());
        }
        else
        {
            // further reduce
            bool reduced = false;
            codesFound = codesFound.ifPossibleReduceNameTelephonyCountry(callsign, patternUsed.getName(), patternUsed.getTelephonyDesignator(), patternUsed.getCountryIso(), reduced, QString(), nullptr);
        }

        return codesFound.frontOrDefault();
    }

    CAirlineIcaoCodeList CAirlineIcaoCodeList::ifPossibleReduceNameTelephonyCountry(const CCallsign &cs, const QString &airlineName, const QString &telephony, const QString &countryIso, bool &reduced, const QString &loginfo, CStatusMessageList *log) const
    {
        reduced = false;
        if (this->isEmpty())
        {
            if (log) { CCallsign::addLogDetailsToList(log, cs, loginfo % u" Empty input list, cannot reduce", getLogCategories()); }
            return *this;
        }

        if (telephony.isEmpty() && airlineName.isEmpty() && countryIso.isEmpty())
        {
            if (log) { CCallsign::addLogDetailsToList(log, cs, loginfo % u" No name/telephony/country, cannot reduce " % QString::number(this->size()) % u" entries", getLogCategories()); }
            return *this;
        }

        CAirlineIcaoCodeList step1Data = airlineName.isEmpty() ? *this : this->findByNamesOrTelephonyDesignator(airlineName);
        if (step1Data.isEmpty() || step1Data.size() == this->size())
        {
            if (log) { CCallsign::addLogDetailsToList(log, cs, loginfo % QStringLiteral(" cannot reduce by '%1'").arg(airlineName), getLogCategories()); }
            step1Data = *this;
        }
        else
        {
            reduced = true;
            if (log) { CCallsign::addLogDetailsToList(log, cs, loginfo % QStringLiteral(" reduced by '%1'").arg(airlineName), getLogCategories()); }
        }
        if (step1Data.size() == 1) { return step1Data; }

        CAirlineIcaoCodeList step2Data = telephony.isEmpty() ? step1Data : step1Data.findByNamesOrTelephonyDesignator(telephony);
        if (step2Data.isEmpty() || step2Data.size() == this->size())
        {
            if (log) { CCallsign::addLogDetailsToList(log, cs, loginfo % QStringLiteral(" cannot reduce by name '%1'").arg(telephony), getLogCategories()); }
            step2Data = step1Data;
        }
        else
        {
            reduced = true;
            if (log) { CCallsign::addLogDetailsToList(log, cs, loginfo % QStringLiteral(" reduced by telephony '%1'").arg(telephony), getLogCategories()); }
        }
        if (step2Data.size() == 1) { return step2Data; }

        CAirlineIcaoCodeList step3Data = countryIso.isEmpty() ? step2Data : step2Data.findByCountryIsoCode(countryIso);
        if (step3Data.isEmpty() || step3Data.size() == this->size())
        {
            if (log) { CCallsign::addLogDetailsToList(log, cs, loginfo % QStringLiteral(" cannot reduce by country '%1'").arg(countryIso), getLogCategories()); }
            step3Data = step2Data;
        }
        else
        {
            reduced = true;
            if (log) { CCallsign::addLogDetailsToList(log, cs, loginfo % QStringLiteral(" reduced by '%1'").arg(countryIso), getLogCategories()); }
        }
        return step3Data;
    }

    CAirlineIcaoCodeList CAirlineIcaoCodeList::ifPossibleReduceByCountry(const QString &countryIso) const
    {
        if (countryIso.isEmpty()) { return *this; }
        if (this->isEmpty()) { return *this; }
        const CAirlineIcaoCodeList found = this->findByCountryIsoCode(countryIso);
        if (found.size() == this->size() || found.isEmpty()) { return *this; }
        return found;
    }

    CAirlineIcaoCodeList CAirlineIcaoCodeList::ifPossibleReduceByTelephonyDesignator(const QString &telephonyDesignator) const
    {
        if (telephonyDesignator.isEmpty()) { return *this; }
        if (this->isEmpty()) { return *this; }
        const CAirlineIcaoCodeList found = this->findByTelephonyDesignator(telephonyDesignator);
        if (found.size() == this->size() || found.isEmpty()) { return *this; }
        return found;
    }

    CAirlineIcaoCode CAirlineIcaoCodeList::findBestMatchByCallsign(const CCallsign &callsign) const
    {
        if (this->isEmpty() || callsign.isEmpty()) { return CAirlineIcaoCode(); }
        const QString airline = callsign.getAirlinePrefix().toUpper();
        if (airline.isEmpty()) { return CAirlineIcaoCode(); }
        const CAirlineIcaoCode airlineCode = (airline.length() == 3) ?
                                                 this->findFirstByOrDefault(&CAirlineIcaoCode::getDesignator, airline) :
                                                 this->findFirstByOrDefault(&CAirlineIcaoCode::getVDesignator, airline);
        return airlineCode;
    }

    CAirlineIcaoCodeList CAirlineIcaoCodeList::fromDatabaseJson(const QJsonArray &array, bool ignoreIncomplete, CAirlineIcaoCodeList *inconsistent)
    {
        CAirlineIcaoCodeList codes;
        for (const QJsonValue &value : array)
        {
            const CAirlineIcaoCode icao(CAirlineIcaoCode::fromDatabaseJson(value.toObject()));
            const bool incomplete = !icao.hasCompleteData();
            if (incomplete)
            {
                if (ignoreIncomplete) { continue; }
                if (inconsistent)
                {
                    inconsistent->push_back(icao);
                    continue;
                }
            }
            codes.push_back(icao);
        }
        return codes;
    }

    QStringList CAirlineIcaoCodeList::toIcaoDesignatorCompleterStrings(bool combinedString, bool sort) const
    {
        QStringList c;
        for (const CAirlineIcaoCode &icao : *this)
        {
            if (combinedString)
            {
                if (!icao.hasValidDbKey()) { continue; }
                const QString cs(icao.getCombinedStringWithKey());
                if (cs.isEmpty()) { continue; }
                c.append(cs);
            }
            else
            {
                const QString d(icao.getDesignator());
                if (c.contains(d) || d.isEmpty()) { continue; }
                c.append(d);
            }
        }
        if (sort) { c.sort(); }
        return c;
    }

    QStringList CAirlineIcaoCodeList::toIcaoDesignatorNameCountryCompleterStrings(bool sort) const
    {
        QStringList c;
        for (const CAirlineIcaoCode &icao : *this)
        {
            if (!icao.hasValidDesignator()) { continue; }
            const QString cs(icao.getDesignatorNameCountry());
            if (cs.isEmpty()) { continue; }
            c.append(cs);
        }
        if (sort) { c.sort(); }
        return c;
    }

    QStringList CAirlineIcaoCodeList::toNameCompleterStrings(bool sort) const
    {
        QStringList c;
        for (const CAirlineIcaoCode &icao : *this)
        {
            if (!icao.hasValidDbKey()) { continue; }
            const QString cs(icao.getNameWithKey());
            if (cs.isEmpty()) { continue; }
            c.append(cs);
        }
        if (sort) { c.sort(); }
        return c;
    }

    QSet<QString> CAirlineIcaoCodeList::allDesignators() const
    {
        CSetBuilder<QString> designators;
        for (const CAirlineIcaoCode &icao : *this)
        {
            if (!icao.hasValidDesignator()) { continue; }
            designators.insert(icao.getDesignator());
        }
        return designators;
    }

    QSet<QString> CAirlineIcaoCodeList::allVDesignators() const
    {
        CSetBuilder<QString> designators;
        for (const CAirlineIcaoCode &icao : *this)
        {
            if (!icao.hasValidDesignator()) { continue; }
            designators.insert(icao.getVDesignator());
        }
        return designators;
    }

    bool CAirlineIcaoCodeList::containsDesignator(const QString &designator) const
    {
        if (designator.isEmpty()) { return false; }
        return this->contains(&CAirlineIcaoCode::getDesignator, designator.toUpper());
    }

    bool CAirlineIcaoCodeList::containsVDesignator(const QString &vDesignator) const
    {
        if (vDesignator.isEmpty()) { return false; }
        if (vDesignator.length() < 4) { return this->containsDesignator(vDesignator); }
        return this->contains(&CAirlineIcaoCode::getVDesignator, vDesignator.toUpper());
    }

    AirlineIcaoIdMap CAirlineIcaoCodeList::toIdMap() const
    {
        AirlineIcaoIdMap map;
        for (const CAirlineIcaoCode &code : *this)
        {
            if (!code.hasValidDbKey()) { continue; }
            map.insert(code.getDbKey(), code);
        }
        return map;
    }
} // ns
