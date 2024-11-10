// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "misc/aviation/airportlist.h"
#include "misc/mixin/mixincompare.h"
#include "misc/setbuilder.h"

#include <QString>

using namespace swift::misc::physical_quantities;

BLACK_DEFINE_SEQUENCE_MIXINS(swift::misc::aviation, CAirport, CAirportList)

namespace swift::misc::aviation
{
    CAirportList::CAirportList(const CSequence<CAirport> &other) : CSequence<CAirport>(other)
    {}

    CAirportList CAirportList::findByIcao(const CAirportIcaoCode &icao) const
    {
        return this->findBy(&CAirport::getIcao, icao);
    }

    bool CAirportList::containsAirportWithIcaoCode(const CAirportIcaoCode &icao) const
    {
        if (icao.isEmpty()) { return false; }
        return this->contains(&CAirport::getIcao, icao);
    }

    void CAirportList::replaceOrAddByIcao(const CAirport &addedOrReplacedAirport)
    {
        if (!addedOrReplacedAirport.hasValidIcaoCode()) return; // ignore invalid airport
        this->replaceOrAdd(&CAirport::getIcao, addedOrReplacedAirport.getIcao(), addedOrReplacedAirport);
    }

    void CAirportList::updateMissingParts(const CAirportList &updateFromList)
    {
        if (updateFromList.isEmpty()) { return; }
        for (CAirport &airport : *this)
        {
            const CAirport fromAirport = updateFromList.findFirstByIcao(airport.getIcao());
            if (fromAirport.hasValidIcaoCode())
            {
                airport.updateMissingParts(fromAirport);
            }
        }
    }

    CAirport CAirportList::findFirstByIcao(const CAirportIcaoCode &icao) const
    {
        return this->findFirstByOrDefault(&CAirport::getIcao, icao, CAirport());
    }

    CAirport CAirportList::findFirstByNameOrLocation(const QString &nameOrLocation) const
    {
        if (this->isEmpty() || nameOrLocation.isEmpty()) { return CAirport(); }
        CAirportList airports = this->findBy([&](const CAirport &airport) {
            return airport.matchesDescriptiveName(nameOrLocation);
        });
        if (!airports.isEmpty()) { return airports.frontOrDefault(); }

        airports = this->findBy([&](const CAirport &airport) {
            return airport.matchesLocation(nameOrLocation);
        });
        if (!airports.isEmpty()) { return airports.frontOrDefault(); }
        return CAirport();
    }

    QStringList CAirportList::allIcaoCodes(bool sorted) const
    {
        QStringList icaos;
        for (const CAirport &airport : *this)
        {
            if (airport.getIcaoAsString().isEmpty()) { continue; }
            icaos.push_back(airport.getIcaoAsString());
        }
        if (sorted) { icaos.sort(); }
        return icaos;
    }

    QStringList CAirportList::allDescriptivesNames(bool sorted) const
    {
        QStringList names;
        for (const CAirport &airport : *this)
        {
            if (airport.getDescriptiveName().isEmpty()) { continue; }
            names.push_back(airport.getDescriptiveName());
        }
        if (sorted) { names.sort(); }
        return names;
    }

    QStringList CAirportList::allLocations() const
    {
        CSetBuilder<QString> locations;
        for (const CAirport &airport : *this)
        {
            if (airport.getLocation().isEmpty()) { continue; }
            locations.insert(airport.getLocation());
        }

        return locations;
    }

    QStringList CAirportList::allLocationsPlusOptionalDescription(bool sorted) const
    {
        QStringList locations;
        for (const CAirport &airport : *this)
        {
            const QString l = airport.getLocationPlusOptionalName();
            if (l.isEmpty()) { continue; }
            locations.push_back(l);
        }
        if (sorted) { locations.sort(); }
        return locations;
    }

    CAirportList CAirportList::fromDatabaseJson(const QJsonArray &array, CAirportList *inconsistent)
    {
        CAirportList airports;
        Q_UNUSED(inconsistent); // not yet implemented, but signature already prepared
        for (const QJsonValue &value : array)
        {
            const CAirport airport(CAirport::fromDatabaseJson(value.toObject()));
            airports.push_back(airport);
        }
        return airports;
    }
} // namespace
