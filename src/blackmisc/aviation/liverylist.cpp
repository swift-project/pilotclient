/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/aviation/liverylist.h"
#include "blackmisc/metaclassprivate.h"
#include "blackmisc/predicates.h"
#include "blackmisc/range.h"

#include <QtGlobal>
#include <tuple>

using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Aviation;


namespace BlackMisc
{
    namespace Aviation
    {
        CLiveryList::CLiveryList() { }

        CLiveryList::CLiveryList(const CSequence<CLivery> &other) :
            CSequence<CLivery>(other)
        { }

        CLiveryList CLiveryList::findByAirlineIcaoDesignator(const QString &icao) const
        {
            QString i(icao.trimmed().toUpper());
            if (i.isEmpty()) { return CLiveryList(); }
            return this->findBy(&CLivery::getAirlineIcaoCodeDesignator, i);
        }

        CLivery CLiveryList::findStdLiveryByAirlineIcaoVDesignator(const QString &icao) const
        {
            QString i(icao.trimmed().toUpper());
            if (i.isEmpty()) { return CLivery(); }
            return this->findFirstByOrDefault([&](const CLivery & livery)
            {
                if (!livery.isAirlineStandardLivery()) { return false; }
                return livery.getAirlineIcaoCode().matchesVDesignator(i);
            });
        }

        CLiveryList CLiveryList::findStdLiveriesBySimplifiedAirlineName(const QString &containedString) const
        {
            if (containedString.isEmpty()) { return CLiveryList(); }
            return this->findBy([&](const CLivery & livery)
            {
                // keep isAirlineStandardLivery first (faster)
                return livery.isAirlineStandardLivery() &&
                       livery.isContainedInSimplifiedAirlineName(containedString);
            });
        }

        CLiveryList CLiveryList::findStdLiveriesByNamesOrTelephonyDesignator(const QString &candidate) const
        {
            if (candidate.isEmpty()) { return CLiveryList(); }
            return this->findBy([&](const CLivery & livery)
            {
                // keep isAirlineStandardLivery first (faster)
                return livery.isAirlineStandardLivery() &&
                       livery.getAirlineIcaoCode().matchesNamesOrTelephonyDesignator(candidate);
            });
        }

        CLivery CLiveryList::findStdLiveryByAirlineIcaoVDesignator(const CAirlineIcaoCode &icao) const
        {
            return this->findStdLiveryByAirlineIcaoVDesignator(icao.getVDesignator());
        }

        CLivery CLiveryList::findColorLiveryOrDefault(const CRgbColor &fuselage, const CRgbColor &tail) const
        {
            if (!fuselage.isValid() || !tail.isValid()) { return CLivery(); }
            return this->findFirstByOrDefault([&](const CLivery & livery)
            {
                if (!livery.isColorLivery()) { return false; }
                return livery.matchesColors(fuselage, tail);
            });
        }

        CLivery CLiveryList::findClosestColorLiveryOrDefault(const CRgbColor &fuselage, const CRgbColor &tail) const
        {
            if (!fuselage.isValid() || !tail.isValid()) { return CLivery(); }
            CLivery bestMatch;
            double bestDistance = 1.0;
            for (const CLivery &livery : *this)
            {
                if (!livery.isColorLivery()) { continue; }
                const double d = livery.getColorDistance(fuselage, tail);
                if (d == 0.0) { return livery; } // exact match
                if (d < bestDistance)
                {
                    bestMatch = livery;
                    bestDistance = d;
                }
            }
            return bestMatch;
        }

        CLivery CLiveryList::findByCombinedCode(const QString &combinedCode) const
        {
            if (!CLivery::isValidCombinedCode(combinedCode)) { return CLivery(); }
            return this->findFirstByOrDefault([&](const CLivery & livery)
            {
                return livery.matchesCombinedCode(combinedCode);
            });
        }

        QStringList CLiveryList::getCombinedCodes(bool sort) const
        {
            if (this->isEmpty()) { return QStringList(); }
            QStringList codes = this->transform(Predicates::MemberTransform(&CLivery::getCombinedCode));
            if (sort) { codes.sort(); }
            return codes;
        }

        QStringList CLiveryList::getCombinedCodesPlusInfo(bool sort) const
        {
            if (this->isEmpty()) { return QStringList(); }
            QStringList codes = this->transform(Predicates::MemberTransform(&CLivery::getCombinedCodePlusInfo));
            if (sort) { codes.sort(); }
            return codes;
        }

        CLivery CLiveryList::smartLiverySelector(const CLivery &liveryPattern) const
        {
            // multiple searches are slow, maybe we can performance optimize this
            // in the future

            // first try on id, that would be perfect
            if (liveryPattern.hasValidDbKey()) { return liveryPattern; }

            // by combined code
            if (liveryPattern.hasCombinedCode())
            {
                QString cc(liveryPattern.getCombinedCode());
                const CLivery l(this->findByCombinedCode(cc));
                if (l.hasCompleteData()) { return l; }
            }

            // by airline
            if (liveryPattern.hasValidAirlineDesignator())
            {
                const QString icao(liveryPattern.getAirlineIcaoCodeDesignator());
                const CLivery l(this->findStdLiveryByAirlineIcaoVDesignator(icao));
                if (l.hasCompleteData()) { return l; }
            }

            // lenient search by name contained (slow)
            if (liveryPattern.getAirlineIcaoCode().hasName())
            {
                const QString search(liveryPattern.getAirlineIcaoCode().getSimplifiedName());
                const CLiveryList liveries(this->findStdLiveriesByNamesOrTelephonyDesignator(search));
                if (!liveries.isEmpty())
                {
                    return liveries.front();
                }
            }
            return CLivery();
        }
    } // namespace
} // namespace
