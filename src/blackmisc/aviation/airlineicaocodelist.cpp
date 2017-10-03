/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/aviation/airlineicaocodelist.h"
#include "blackmisc/country.h"
#include "blackmisc/range.h"

#include <QJsonObject>
#include <QJsonValue>

namespace BlackMisc
{
    namespace Aviation
    {
        CAirlineIcaoCodeList::CAirlineIcaoCodeList()
        { }

        CAirlineIcaoCodeList::CAirlineIcaoCodeList(const CSequence<CAirlineIcaoCode> &other) :
            CSequence<CAirlineIcaoCode>(other)
        { }

        CAirlineIcaoCodeList CAirlineIcaoCodeList::findByDesignator(const QString &designator) const
        {
            if (!CAirlineIcaoCode::isValidAirlineDesignator(designator)) { return CAirlineIcaoCodeList(); }
            return this->findBy([&](const CAirlineIcaoCode & code)
            {
                return code.matchesDesignator(designator);
            });
        }

        CAirlineIcaoCodeList CAirlineIcaoCodeList::findByIataCode(const QString &iata) const
        {
            if (iata.isEmpty()) { return CAirlineIcaoCodeList(); }
            return this->findBy([&](const CAirlineIcaoCode & code)
            {
                return code.matchesIataCode(iata);
            });
        }

        CAirlineIcaoCodeList CAirlineIcaoCodeList::findByDesignatorOrIataCode(const QString &designatorOrIata) const
        {
            if (designatorOrIata.isEmpty()) { return CAirlineIcaoCodeList(); }
            return this->findBy([&](const CAirlineIcaoCode & code)
            {
                return code.matchesDesignatorOrIataCode(designatorOrIata);
            });
        }

        CAirlineIcaoCodeList CAirlineIcaoCodeList::findByVDesignator(const QString &designator) const
        {
            if (!CAirlineIcaoCode::isValidAirlineDesignator(designator)) { return CAirlineIcaoCodeList(); }
            return this->findBy([&](const CAirlineIcaoCode & code)
            {
                return code.matchesVDesignator(designator);
            });
        }

        CAirlineIcaoCodeList CAirlineIcaoCodeList::findByVDesignatorOrIataCode(const QString &designatorOrIata) const
        {
            if (designatorOrIata.isEmpty()) { return CAirlineIcaoCodeList(); }
            return this->findBy([&](const CAirlineIcaoCode & code)
            {
                return code.matchesVDesignatorOrIataCode(designatorOrIata);
            });
        }

        CAirlineIcaoCodeList CAirlineIcaoCodeList::findByCountryIsoCode(const QString &isoCode) const
        {
            if (isoCode.length() != 2) { return CAirlineIcaoCodeList(); }
            const QString iso(isoCode.toUpper());
            return this->findBy([&](const CAirlineIcaoCode & code)
            {
                return code.getCountry().getIsoCode() == iso;
            });
        }

        CAirlineIcaoCodeList CAirlineIcaoCodeList::findBySimplifiedNameContaining(const QString &containedString) const
        {
            if (containedString.isEmpty()) { return CAirlineIcaoCodeList(); }
            return this->findBy([&](const CAirlineIcaoCode & code)
            {
                return code.isContainedInSimplifiedName(containedString);
            });
        }

        CAirlineIcaoCodeList CAirlineIcaoCodeList::findByNamesOrTelephonyDesignator(const QString &candidate) const
        {
            if (candidate.isEmpty()) { return CAirlineIcaoCodeList(); }
            return this->findBy([&](const CAirlineIcaoCode & code)
            {
                return code.matchesNamesOrTelephonyDesignator(candidate);
            });
        }

        CAirlineIcaoCodeList CAirlineIcaoCodeList::findByMilitary(bool military) const
        {
            return this->findBy([&](const CAirlineIcaoCode & code)
            {
                return code.isMilitary() == military;
            });
        }

        CAirlineIcaoCodeList CAirlineIcaoCodeList::findByValidDesignator() const
        {
            return this->findBy([](const CAirlineIcaoCode & code)
            {
                return code.hasValidDesignator();
            });
        }

        CAirlineIcaoCodeList CAirlineIcaoCodeList::findByInvalidDesignator() const
        {
            return this->findBy([](const CAirlineIcaoCode & code)
            {
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
                codesFound = this->findByNamesOrTelephonyDesignator(patternUsed.getName());
            }
            else
            {
                // further reduce
                if (patternUsed.hasName())
                {
                    const CAirlineIcaoCodeList backup(codesFound);
                    codesFound = this->findByNamesOrTelephonyDesignator(patternUsed.getName());
                    if (codesFound.isEmpty()) { codesFound = backup; }
                }
            }

            // further reduce
            if (patternUsed.hasValidCountry())
            {
                CAirlineIcaoCodeList countryCodes = codesFound.findByCountryIsoCode(patternUsed.getCountry().getIsoCode());
                if (!countryCodes.isEmpty()) { return countryCodes.front(); }
            }

            if (!codesFound.isEmpty()) { return codesFound.front(); }
            return patternUsed;
        }

        CAirlineIcaoCode CAirlineIcaoCodeList::findBestMatchByCallsign(const CCallsign &callsign) const
        {
            if (this->isEmpty() || callsign.isEmpty()) { return CAirlineIcaoCode(); }
            const QString airline = callsign.getAirlineSuffix().toUpper();
            if (airline.isEmpty()) { return CAirlineIcaoCode(); }
            const CAirlineIcaoCode airlineCode = (airline.length() == 3) ?
                                                 this->findFirstByOrDefault(&CAirlineIcaoCode::getDesignator, airline) :
                                                 this->findFirstByOrDefault(&CAirlineIcaoCode::getVDesignator, airline);
            return airlineCode;
        }

        CAirlineIcaoCodeList CAirlineIcaoCodeList::fromDatabaseJson(const QJsonArray &array,  bool ignoreIncomplete, CAirlineIcaoCodeList *inconsistent)
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

        bool CAirlineIcaoCodeList::containsDesignator(const QString &designator) const
        {
            if (designator.isEmpty()) { return false; }
            return this->contains(&CAirlineIcaoCode::getDesignator, designator.toUpper());
        }
    } // ns
} // ns
