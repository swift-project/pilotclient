/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "aircrafticaocodelist.h"

namespace BlackMisc
{
    namespace Aviation
    {

        CAircraftIcaoCodeList::CAircraftIcaoCodeList(const CSequence<CAircraftIcaoCode> &other) :
            CSequence<CAircraftIcaoCode>(other)
        { }

        CAircraftIcaoCodeList CAircraftIcaoCodeList::findByDesignator(const QString &designator)
        {
            if (!CAircraftIcaoCode::isValidDesignator(designator)) { return CAircraftIcaoCodeList(); }
            return this->findBy([&](const CAircraftIcaoCode & code)
            {
                return code.matchesDesignator(designator);
            });
        }

        CAircraftIcaoCodeList CAircraftIcaoCodeList::findByManufacturer(const QString &manufacturer)
        {
            if (manufacturer.isEmpty()) { return CAircraftIcaoCodeList(); }
            return this->findBy([&](const CAircraftIcaoCode & code)
            {
                return code.getManufacturer().startsWith(manufacturer, Qt::CaseInsensitive);
            });
        }

        CAircraftIcaoCodeList CAircraftIcaoCodeList::findByDescription(const QString &description)
        {
            if (description.isEmpty()) { return CAircraftIcaoCodeList(); }
            return this->findBy([&](const CAircraftIcaoCode & code)
            {
                return code.getModelDescription().startsWith(description, Qt::CaseInsensitive);
            });
        }

        CAircraftIcaoCode CAircraftIcaoCodeList::findFirstByDesignatorAndRank(const QString &designator)
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

        QStringList CAircraftIcaoCodeList::toCompleterStrings() const
        {
            QStringList c;
            for (const CAircraftIcaoCode &icao : *this)
            {
                c.append(icao.getCombinedStringWithKey());
            }
            return c;
        }

        CAircraftIcaoCodeList CAircraftIcaoCodeList::fromDatabaseJson(const QJsonArray &array, bool ignoreIncomplete)
        {
            CAircraftIcaoCodeList codes;
            for (const QJsonValue &value : array)
            {
                CAircraftIcaoCode icao(CAircraftIcaoCode::fromDatabaseJson(value.toObject()));
                if (ignoreIncomplete && !icao.hasCompleteData()) { continue; }
                codes.push_back(icao);
            }
            return codes;
        }

    } // namespace
} // namespace
