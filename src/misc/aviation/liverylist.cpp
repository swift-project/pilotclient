// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/aviation/liverylist.h"

#include "misc/predicates.h"
#include "misc/range.h"

using namespace swift::misc::aviation;

SWIFT_DEFINE_SEQUENCE_MIXINS(swift::misc::aviation, CLivery, CLiveryList)

namespace swift::misc::aviation
{
    CLiveryList::CLiveryList() {}

    CLiveryList::CLiveryList(const CSequence<CLivery> &other) : CSequence<CLivery>(other)
    {}

    CLiveryList CLiveryList::findByAirlineIcaoDesignator(const QString &icao) const
    {
        QString icaoCode(icao.trimmed().toUpper());
        if (icaoCode.isEmpty()) { return CLiveryList(); }
        return this->findBy(&CLivery::getAirlineIcaoCodeDesignator, icaoCode);
    }

    CLivery CLiveryList::findStdLiveryByAirlineIcaoVDesignator(const CAirlineIcaoCode &icao) const
    {
        if (this->isEmpty() || !icao.hasValidDesignator()) { return CLivery(); }
        CLiveryList candidates;
        for (const CLivery &livery : *this)
        {
            if (!livery.isAirlineStandardLivery()) { continue; }
            const CAirlineIcaoCode livIcao = livery.getAirlineIcaoCode();
            if (livIcao.isDbEqual(icao)) { return livery; }
            if (livIcao.getVDesignator() != icao.getVDesignator()) { continue; }
            if (icao.getName().size() > 5 && livery.getDescription().contains(icao.getName(), Qt::CaseInsensitive)) { return livery; }
            candidates.push_back(livery);
        }

        if (candidates.size() < 2) { return candidates.frontOrDefault(); }
        const CLiveryList operatingAirlines = candidates.findBy(&CLivery::isAirlineOperating, true);
        if (!operatingAirlines.isEmpty()) { return operatingAirlines.frontOrDefault(); }
        return candidates.frontOrDefault();
    }

    CLivery CLiveryList::findStdLiveryByAirlineIcaoVDesignator(const QString &icao) const
    {
        const QString icaoDesignator(icao.trimmed().toUpper());
        if (icaoDesignator.isEmpty()) { return CLivery(); }
        return this->findFirstByOrDefault([&](const CLivery &livery) {
            if (!livery.isAirlineStandardLivery()) { return false; }
            return livery.getAirlineIcaoCode().matchesVDesignator(icaoDesignator);
        });
    }

    CLiveryList CLiveryList::findStdLiveriesBySimplifiedAirlineName(const QString &containedString) const
    {
        if (containedString.isEmpty()) { return CLiveryList(); }
        return this->findBy([&](const CLivery &livery) {
            // keep isAirlineStandardLivery first (faster)
            return livery.isAirlineStandardLivery() &&
                   livery.isContainedInSimplifiedAirlineName(containedString);
        });
    }

    CLiveryList CLiveryList::findStdLiveriesByNamesOrTelephonyDesignator(const QString &candidate) const
    {
        if (candidate.isEmpty()) { return CLiveryList(); }
        return this->findBy([&](const CLivery &livery) {
            // keep isAirlineStandardLivery first (faster)
            return livery.isAirlineStandardLivery() &&
                   livery.getAirlineIcaoCode().matchesNamesOrTelephonyDesignator(candidate);
        });
    }

    CLivery CLiveryList::findColorLiveryOrDefault(const CRgbColor &fuselage, const CRgbColor &tail) const
    {
        if (!fuselage.isValid() || !tail.isValid()) { return CLivery(); }
        return this->findFirstByOrDefault([&](const CLivery &livery) {
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
        return this->findFirstByOrDefault([&](const CLivery &livery) {
            return livery.matchesCombinedCode(combinedCode);
        });
    }

    QStringList CLiveryList::getCombinedCodes(bool sort) const
    {
        if (this->isEmpty()) { return QStringList(); }
        QStringList codes = this->transform(predicates::MemberTransform(&CLivery::getCombinedCode));
        if (sort) { codes.sort(); }
        return codes;
    }

    QStringList CLiveryList::getCombinedCodesPlusInfo(bool sort) const
    {
        if (this->isEmpty()) { return QStringList(); }
        QStringList codes = this->transform(predicates::MemberTransform(&CLivery::getCombinedCodePlusInfo));
        if (sort) { codes.sort(); }
        return codes;
    }

    QStringList CLiveryList::getCombinedCodesPlusInfoAndId(bool sort) const
    {
        if (this->isEmpty()) { return QStringList(); }
        QStringList codes = this->transform(predicates::MemberTransform(&CLivery::getCombinedCodePlusInfoAndId));
        if (sort) { codes.sort(); }
        return codes;
    }

    CAirlineIcaoCodeList CLiveryList::getAirlines() const
    {
        CAirlineIcaoCodeList icaos;
        for (const CLivery &livery : *this)
        {
            icaos.push_back(livery.getAirlineIcaoCode());
        }
        return icaos;
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
            const QString cc(liveryPattern.getCombinedCode());
            const CLivery l(this->findByCombinedCode(cc));
            if (l.hasCompleteData()) { return l; }
        }

        // by airline
        if (liveryPattern.hasValidAirlineDesignator())
        {
            const QString icao(liveryPattern.getAirlineIcaoCodeDesignator());
            const CLivery l(this->findStdLiveryByAirlineIcaoVDesignator(icao));
            if (l.hasCompleteData()) { return l; }

            // lenient search by assuming that a virtual airline is not annotated by "V"
            // VHDU not found, but HDU
            const CLiveryList liveries = this->findByAirlineIcaoDesignator(icao);
            if (liveries.size() == 1) { return liveries.front(); }
            if (liveries.size() > 1)
            {
                if (liveryPattern.hasAirlineName())
                {
                    // reduce by name
                    const CLiveryList liveriesByName = liveries.findStdLiveriesBySimplifiedAirlineName(liveryPattern.getAirlineName());
                    if (!liveriesByName.isEmpty()) { return liveriesByName.front(); }
                }
                return liveries.front();
            }
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

    CLiveryList CLiveryList::fromDatabaseJsonCaching(const QJsonArray &array, const CAirlineIcaoCodeList &relatedAirlines)
    {
        AirlineIcaoIdMap airlineIcaos = relatedAirlines.toIdMap();

        CLiveryList models;
        for (const QJsonValue &value : array)
        {
            models.push_back(CLivery::fromDatabaseJsonCaching(value.toObject(), airlineIcaos));
        }
        return models;
    }
} // namespace swift::misc::aviation
