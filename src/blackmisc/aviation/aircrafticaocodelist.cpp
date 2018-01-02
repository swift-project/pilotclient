/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/aviation/aircrafticaocodelist.h"
#include "blackmisc/range.h"

#include <QJsonObject>
#include <QJsonValue>
#include <Qt>

namespace BlackMisc
{
    namespace Aviation
    {
        CAircraftIcaoCodeList::CAircraftIcaoCodeList()
        { }

        CAircraftIcaoCodeList::CAircraftIcaoCodeList(const CSequence<CAircraftIcaoCode> &other) :
            CSequence<CAircraftIcaoCode>(other)
        { }

        CAircraftIcaoCodeList CAircraftIcaoCodeList::findByDesignator(const QString &designator, int fuzzySearch) const
        {
            if (!fuzzySearch && !CAircraftIcaoCode::isValidDesignator(designator)) { return CAircraftIcaoCodeList(); }
            if (fuzzySearch && designator.length() < 3) { return CAircraftIcaoCodeList(); }
            return this->findBy([&](const CAircraftIcaoCode & code)
            {
                return code.matchesDesignator(designator, fuzzySearch);
            });
        }

        CAircraftIcaoCode CAircraftIcaoCodeList::findBestFuzzyMatchOrDefault(const QString &designator, int cutoff) const
        {
            if (designator.length() < 3) { return CAircraftIcaoCode(); }
            int best = 0;
            int current = 0;
            CAircraftIcaoCode found;
            const QString d(designator.trimmed().toUpper());
            for (const CAircraftIcaoCode &code : * this)
            {
                if (!code.matchesDesignator(d, cutoff, &current)) { continue; }
                if (current == 100.0) { return code; }
                if (best < current) { found = code; }
            }
            return found;
        }

        CAircraftIcaoCodeList CAircraftIcaoCodeList::findByValidDesignator() const
        {
            return this->findBy([](const CAircraftIcaoCode & code)
            {
                return code.hasValidDesignator();
            });
        }

        CAircraftIcaoCodeList CAircraftIcaoCodeList::findByInvalidDesignator() const
        {
            return this->findBy([](const CAircraftIcaoCode & code)
            {
                return !code.hasValidDesignator();
            });
        }

        CAircraftIcaoCodeList CAircraftIcaoCodeList::findByDesignatorOrIataCode(const QString &icaoOrIata) const
        {
            if (icaoOrIata.isEmpty()) { return CAircraftIcaoCodeList(); }
            return this->findBy([&](const CAircraftIcaoCode & code)
            {
                return code.matchesDesignatorOrIata(icaoOrIata);
            });
        }

        CAircraftIcaoCodeList CAircraftIcaoCodeList::findByDesignatorIataOrFamily(const QString &icaoIataOrFamily) const
        {
            if (icaoIataOrFamily.isEmpty()) { return CAircraftIcaoCodeList(); }
            return this->findBy([&](const CAircraftIcaoCode & code)
            {
                return code.matchesDesignatorIataOrFamily(icaoIataOrFamily);
            });
        }

        CAircraftIcaoCodeList CAircraftIcaoCodeList::findEndingWith(const QString &icaoEnding) const
        {
            const QString ends = icaoEnding.trimmed().toUpper();
            if (ends.isEmpty()) { return CAircraftIcaoCodeList(); }
            CAircraftIcaoCodeList icaosDesignator;
            CAircraftIcaoCodeList icaosFamily;
            for (const CAircraftIcaoCode &icao : *this)
            {
                if (icao.getDesignator().endsWith(ends))
                {
                    icaosDesignator.push_back(icao);
                }
                else if (icao.getFamily().endsWith(ends))
                {
                    icaosFamily.push_back(icao);
                }
            }
            return icaosDesignator.isEmpty() ? icaosFamily : icaosDesignator;
        }

        CAircraftIcaoCodeList CAircraftIcaoCodeList::findByIataCode(const QString &iata, int fuzzySearch) const
        {
            if (iata.isEmpty()) { return CAircraftIcaoCodeList(); }
            return this->findBy([&](const CAircraftIcaoCode & code)
            {
                return code.matchesIataCode(iata, fuzzySearch);
            });
        }

        CAircraftIcaoCodeList CAircraftIcaoCodeList::findByFamily(const QString &family, int fuzzySearch) const
        {
            if (family.isEmpty()) { return CAircraftIcaoCodeList(); }
            return this->findBy([&](const CAircraftIcaoCode & code)
            {
                return code.matchesFamily(family, fuzzySearch);
            });
        }

        CAircraftIcaoCodeList CAircraftIcaoCodeList::findByManufacturer(const QString &manufacturer) const
        {
            if (manufacturer.isEmpty()) { return CAircraftIcaoCodeList(); }
            return this->findBy([&](const CAircraftIcaoCode & code)
            {
                return code.getManufacturer().startsWith(manufacturer, Qt::CaseInsensitive);
            });
        }

        CAircraftIcaoCodeList CAircraftIcaoCodeList::findByDescription(const QString &description) const
        {
            if (description.isEmpty()) { return CAircraftIcaoCodeList(); }
            return this->findBy([&](const CAircraftIcaoCode & code)
            {
                return code.getModelDescription().startsWith(description, Qt::CaseInsensitive);
            });
        }

        CAircraftIcaoCodeList CAircraftIcaoCodeList::findMatchingByAnyDescription(const QString &description) const
        {
            return this->findBy([&](const CAircraftIcaoCode & code)
            {
                return code.matchesAnyDescription(description);
            });
        }

        CAircraftIcaoCodeList CAircraftIcaoCodeList::findWithIataCode(bool removeWhenSameAsDesignator) const
        {
            return this->findBy([&](const CAircraftIcaoCode & code)
            {
                if (!code.hasIataCode()) { return false; }
                return !removeWhenSameAsDesignator || !code.isIataSameAsDesignator();
            });
        }

        CAircraftIcaoCodeList CAircraftIcaoCodeList::findWithFamily(bool removeWhenSameAsDesignator) const
        {
            return this->findBy([&](const CAircraftIcaoCode & code)
            {
                if (!code.hasFamily()) { return false; }
                return !removeWhenSameAsDesignator || !code.isFamilySameAsDesignator();
            });
        }

        CAircraftIcaoCodeList CAircraftIcaoCodeList::findByMilitaryFlag(bool military) const
        {
            return this->findBy([&](const CAircraftIcaoCode & code)
            {
                return (code.isMilitary() == military);
            });
        }

        CAircraftIcaoCode CAircraftIcaoCodeList::findFirstByDesignatorAndRank(const QString &designator) const
        {
            if (!CAircraftIcaoCode::isValidDesignator(designator)) { return CAircraftIcaoCode(); }
            CAircraftIcaoCodeList codes(findByDesignator(designator));
            if (codes.isEmpty()) { return CAircraftIcaoCode(); }
            if (codes.size() < 2) { return codes.front(); }
            codes.sortBy(&CAircraftIcaoCode::getRank, &CAircraftIcaoCode::getDbKey);
            return codes.front();
        }

        bool CAircraftIcaoCodeList::containsDesignator(const QString &designator) const
        {
            if (designator.isEmpty()) { return false; }
            return this->contains(&CAircraftIcaoCode::getDesignator, designator);
        }

        void CAircraftIcaoCodeList::sortByRank()
        {
            this->sortBy(&CAircraftIcaoCode::getRank);
        }

        void CAircraftIcaoCodeList::sortByDesignatorAndRank()
        {
            this->sortBy(&CAircraftIcaoCode::getDesignator, &CAircraftIcaoCode::getRank);
        }

        void CAircraftIcaoCodeList::sortByDesignatorManufacturerAndRank()
        {
            this->sortBy(&CAircraftIcaoCode::getDesignator, &CAircraftIcaoCode::getManufacturer, &CAircraftIcaoCode::getRank);
        }

        void CAircraftIcaoCodeList::removeInvalidCombinedCodes()
        {
            this->removeIf([](const CAircraftIcaoCode & icao) { return !icao.hasValidCombinedType(); });
        }

        void CAircraftIcaoCodeList::removeDuplicates()
        {
            this->removeIf(&CAircraftIcaoCode::isDbDuplicate, true);
        }

        QStringList CAircraftIcaoCodeList::toCompleterStrings(bool withIataCodes, bool withFamily, bool sort) const
        {
            QStringList c;
            CAircraftIcaoCodeList icaos(*this);
            if (sort) { icaos.sortByDesignatorAndRank(); }

            // 3 steps to get a proper sort order
            for (const CAircraftIcaoCode &icao : as_const(icaos))
            {
                c.append(icao.getCombinedIcaoStringWithKey());
            }

            if (withFamily)
            {
                icaos = this->findWithFamily(true);
                for (const CAircraftIcaoCode &icao : as_const(icaos))
                {
                    c.append(icao.getCombinedFamilyStringWithKey());
                }
            }

            if (withIataCodes)
            {
                icaos = icaos.findWithIataCode(true);
                if (sort) { icaos.sortBy(&CAircraftIcaoCode::getIataCode, &CAircraftIcaoCode::getRank); }
                for (const CAircraftIcaoCode &icao : as_const(icaos))
                {
                    c.append(icao.getCombinedIataStringWithKey());
                }
            }

            return c;
        }

        QSet<QString> CAircraftIcaoCodeList::allIcaoCodes(bool onlyKnownDesignators) const
        {
            QSet<QString> c;
            for (const CAircraftIcaoCode &icao : *this)
            {
                if (onlyKnownDesignators && !icao.hasKnownDesignator()) { continue; }
                const QString d(icao.getDesignator());
                c.insert(d);
            }
            return c;
        }

        QSet<QString> CAircraftIcaoCodeList::allFamilies() const
        {
            QSet<QString> c;
            for (const CAircraftIcaoCode &icao : *this)
            {
                if (!icao.hasFamily()) { continue; }
                const QString d(icao.getFamily());
                c.insert(d);
            }
            return c;
        }

        QSet<QString> CAircraftIcaoCodeList::allManufacturers(bool onlyKnownDesignators) const
        {
            QSet<QString> c;
            for (const CAircraftIcaoCode &icao : *this)
            {
                if (onlyKnownDesignators && !icao.hasKnownDesignator()) { continue; }
                const QString m(icao.getManufacturer());
                if (m.isEmpty()) { continue; }
                c.insert(m); // checks if already contains m
            }
            return c;
        }

        QMap<QString, int> CAircraftIcaoCodeList::countManufacturers() const
        {
            QMap<QString, int> count;
            for (const CAircraftIcaoCode &icao : *this)
            {
                if (!icao.hasManufacturer()) continue;
                const QString m(icao.getManufacturer());
                if (count.contains(m))
                {
                    count[m]++;
                }
                else
                {
                    count[m] = 1;
                }
            }
            return count;
        }

        QPair<QString, int> CAircraftIcaoCodeList::maxCountManufacturer() const
        {
            if (this->isEmpty()) return QPair<QString, int>("", 0);
            const QMap<QString, int> counts(countManufacturers());
            QPair<QString, int> max;
            for (const QString &m : counts.keys())
            {
                const int mv = counts[m];
                if (mv > max.second)
                {
                    max.first = m;
                    max.second = mv;
                }
            }
            return max;
        }

        CAircraftIcaoCodeList CAircraftIcaoCodeList::fromDatabaseJson(const QJsonArray &array, bool ignoreIncompleteAndDuplicates, CAircraftIcaoCodeList *inconsistent)
        {
            CAircraftIcaoCodeList codes;
            for (const QJsonValue &value : array)
            {
                const CAircraftIcaoCode icao(CAircraftIcaoCode::fromDatabaseJson(value.toObject()));
                if (!icao.hasSpecialDesignator() && !icao.hasCompleteData())
                {
                    if (ignoreIncompleteAndDuplicates) { continue; }
                    if (inconsistent)
                    {
                        inconsistent->push_back(icao);
                        continue;
                    }
                }
                if (icao.isDbDuplicate())
                {
                    if (ignoreIncompleteAndDuplicates) { continue; }
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

        CAircraftIcaoCode CAircraftIcaoCodeList::smartAircraftIcaoSelector(const CAircraftIcaoCode &icaoPattern) const
        {
            if (icaoPattern.hasValidDbKey())
            {
                const int k = icaoPattern.getDbKey();
                const CAircraftIcaoCode c(this->findByKey(k));
                if (c.hasCompleteData()) { return c; }
            }

            // get an initial set of data we can choose from
            const QString designator(icaoPattern.getDesignator());
            if (designator.isEmpty()) { return CAircraftIcaoCode(); }
            CAircraftIcaoCodeList codes;
            do
            {
                codes = this->findByDesignator(designator);
                if (!codes.isEmpty()) break;

                // now we search if the ICAO designator is actually an IATA code
                codes = this->findByIataCode(designator);
                if (!codes.isEmpty()) break;

                // search fuzzy and restrict length
                const CAircraftIcaoCode bestMatch = this->findBestFuzzyMatchOrDefault(designator.length() < 5 ? designator : designator.left(5), 70);
                if (bestMatch.hasValidDesignator()) { return bestMatch; }

                // still empty, try to find by family
                codes = this->findByFamily(designator);
                if (!codes.isEmpty()) break;

                // by any description
                codes = this->findMatchingByAnyDescription(designator);
            }
            while (false);

            if (codes.isEmpty()) { return icaoPattern; }
            if (codes.size() == 1) { return codes.front(); }

            // further reduce by manufacturer
            codes.sortByRank();
            if (icaoPattern.hasManufacturer() && codes.contains(&CAircraftIcaoCode::getManufacturer, icaoPattern.getManufacturer()))
            {
                const QString m(icaoPattern.getManufacturer());
                codes = codes.findByManufacturer(m);
                if (codes.size() == 1) { return codes.front(); }

                // intentionally continue here
            }

            // further reduce by IATA
            if (icaoPattern.hasIataCode() && codes.contains(&CAircraftIcaoCode::getIataCode, icaoPattern.getIataCode()))
            {
                const QString i(icaoPattern.getIataCode());
                codes = codes.findByIataCode(i);
                if (codes.size() == 1) { return codes.front(); }

                // intentionally continue here
            }

            // lucky punch on description?
            if (icaoPattern.hasModelDescription() && codes.contains(&CAircraftIcaoCode::getModelDescription, icaoPattern.getModelDescription()))
            {
                // do not affect codes here, it might return no results
                const QString d(icaoPattern.getModelDescription());
                CAircraftIcaoCodeList cm(codes.findByDescription(d));
                if (cm.size() == 1) { return cm.front(); }
            }
            return codes.frontOrDefault(); // sorted by rank
        }

        CAircraftIcaoCodeList CAircraftIcaoCodeList::groupByDesignatorAndManufacturer() const
        {
            CAircraftIcaoCodeList copy(*this);
            copy.sortByDesignatorManufacturerAndRank();
            CAircraftIcaoCodeList grouped; // will contain the entries with the best rank
            QString designator;
            QString manufacturer;
            for (const CAircraftIcaoCode &code : as_const(copy))
            {
                if (code.getDesignator() != designator ||  code.getManufacturer() != manufacturer)
                {
                    designator = code.getDesignator();
                    manufacturer = code.getManufacturer();
                    grouped.push_back(code);
                }
            }
            return grouped;
        }
    } // namespace
} // namespace
