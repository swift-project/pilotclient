/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackmisc/aviation/airportlist.h"
#include "blackmisc/compare.h"
#include "blackmisc/metaclassprivate.h"
#include "blackmisc/range.h"

#include <QString>
#include <tuple>

using namespace BlackMisc::PhysicalQuantities;

namespace BlackMisc
{
    namespace Aviation
    {
        CAirportList::CAirportList() { }

        CAirportList::CAirportList(const CSequence<CAirport> &other) :
            CSequence<CAirport>(other)
        { }

        CAirportList CAirportList::findByIcao(const CAirportIcaoCode &icao) const
        {
            return this->findBy(&CAirport::getIcao, icao);
        }

        void CAirportList::replaceOrAddByIcao(const CAirport &addedOrReplacedAirport)
        {
            if (!addedOrReplacedAirport.hasValidIcaoCode()) return; // ignore invalid airport
            this->replaceOrAdd(&CAirport::getIcao, addedOrReplacedAirport.getIcao(), addedOrReplacedAirport);
        }

        CAirport CAirportList::findFirstByIcao(const CAirportIcaoCode &icao, const CAirport &ifNotFound) const
        {
            return this->findFirstByOrDefault(&CAirport::getIcao, icao, ifNotFound);
        }

        QStringList CAirportList::allIcaoCodes(bool sorted) const
        {
            QStringList icaos;
            for (const CAirport &airport : *this)
            {
                if (!airport.getIcaoAsString().isEmpty())
                {
                    icaos.push_back(airport.getIcaoAsString());
                }
            }
            if (sorted)
            {
                icaos.sort();
            }
            return icaos;
        }

        CAirportList CAirportList::fromDatabaseJson(const QJsonArray &array)
        {
            CAirportList airports;
            for (const QJsonValue &value : array)
            {
                const CAirport airport(CAirport::fromDatabaseJson(value.toObject()));
                airports.push_back(airport);
            }
            return airports;
        }
    } // namespace
} // namespace
