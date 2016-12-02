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

        CAircraftIcaoCodeList CAircraftIcaoCodeList::findByDesignator(const QString &designator) const
        {
            if (!CAircraftIcaoCode::isValidDesignator(designator)) { return CAircraftIcaoCodeList(); }
            return this->findBy([&](const CAircraftIcaoCode & code)
            {
                return code.matchesDesignator(designator);
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
            QString ends = icaoEnding.trimmed().toUpper();
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

        CAircraftIcaoCodeList CAircraftIcaoCodeList::findByIataCode(const QString &iata) const
        {
            if (iata.isEmpty()) { return CAircraftIcaoCodeList(); }
            return this->findBy([&](const CAircraftIcaoCode & code)
            {
                return code.matchesIataCode(iata);
            });
        }

        CAircraftIcaoCodeList CAircraftIcaoCodeList::findByFamily(const QString &family) const
        {
            if (family.isEmpty()) { return CAircraftIcaoCodeList(); }
            return this->findBy([&](const CAircraftIcaoCode & code)
            {
                return code.matchesFamily(family);
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

        CAircraftIcaoCode CAircraftIcaoCodeList::findFirstByDesignatorAndRank(const QString &designator) const
        {
            if (!CAircraftIcaoCode::isValidDesignator(designator)) { return CAircraftIcaoCode(); }
            CAircraftIcaoCodeList codes(findByDesignator(designator));
            if (codes.isEmpty()) { return CAircraftIcaoCode(); }
            if (codes.size() < 2) { return codes.front(); }
            codes.sortBy(&CAircraftIcaoCode::getRank, &CAircraftIcaoCode::getDbKey);
            return codes.front();
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

        CAircraftIcaoCodeList CAircraftIcaoCodeList::fromDatabaseJson(const QJsonArray &array, bool ignoreIncomplete)
        {
            CAircraftIcaoCodeList codes;
            for (const QJsonValue &value : array)
            {
                CAircraftIcaoCode icao(CAircraftIcaoCode::fromDatabaseJson(value.toObject()));
                if (ignoreIncomplete && !icao.hasSpecialDesignator() && !icao.hasCompleteData())
                {
                    continue;
                }
                codes.push_back(icao);
            }
            return codes;
        }

        CAircraftIcaoCode CAircraftIcaoCodeList::smartAircraftIcaoSelector(const CAircraftIcaoCode &icaoPattern) const
        {
            if (icaoPattern.hasValidDbKey())
            {
                int k = icaoPattern.getDbKey();
                CAircraftIcaoCode c(this->findByKey(k));
                if (c.hasCompleteData()) { return c; }
            }

            // get an initial set of data we can choose from
            const QString d(icaoPattern.getDesignator());
            if (d.isEmpty()) { return CAircraftIcaoCode(); }
            CAircraftIcaoCodeList codes;
            do
            {
                codes = this->findByDesignator(d);
                if (!codes.isEmpty()) break;

                // now we search if the ICAO designator is actually an IATA code
                codes = this->findByIataCode(d);
                if (!codes.isEmpty()) break;

                if (d.length() > 4)
                {
                    codes = this->findByDesignator(d.left(4));
                    if (!codes.isEmpty()) break;
                }

                // still empty, try to find by family
                codes = this->findByFamily(d);
                if (!codes.isEmpty()) break;

                // now try to find as ending
                codes = this->findEndingWith(d);
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
            CAircraftIcaoCodeList grouped;
            QString designator;
            QString manufacturer;
            for (const CAircraftIcaoCode code : as_const(copy))
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
