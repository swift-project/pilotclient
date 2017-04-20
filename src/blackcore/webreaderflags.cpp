/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "webreaderflags.h"

using namespace BlackMisc::Network;

namespace BlackCore
{
    CWebReaderFlags::WebReader CWebReaderFlags::entityToReader(CEntityFlags::Entity entity)
    {
        WebReader f = None;
        if (entity.testFlag(CEntityFlags::AircraftIcaoEntity) || entity.testFlag(CEntityFlags::AirlineIcaoEntity) || entity.testFlag(CEntityFlags::CountryEntity))
        {
            f |= IcaoDataReader;
        }

        if (entity.testFlag(CEntityFlags::ModelEntity) || entity.testFlag(CEntityFlags::DistributorEntity) || entity.testFlag(CEntityFlags::LiveryEntity))
        {
            f |= ModelReader;
        }

        if (entity.testFlag(CEntityFlags::AirportEntity))
        {
            f |= AirportReader;
        }

        if (entity.testFlag(CEntityFlags::InfoObjectEntity)) { f |= DbInfoDataReader; }
        if (entity.testFlag(CEntityFlags::BookingEntity)) { f |= VatsimBookingReader; }
        if (entity.testFlag(CEntityFlags::VatsimDataFile)) { f |= VatsimDataReader; }
        if (entity.testFlag(CEntityFlags::VatsimStatusFile)) { f |= VatsimStatusReader; }
        if (entity.testFlag(CEntityFlags::MetarEntity)) { f |= VatsimMetarReader; }

        return f;
    }

    CWebReaderFlags::WebReader CWebReaderFlags::webReaderFlagToWebReader(CWebReaderFlags::WebReaderFlag flag)
    {
        return static_cast<WebReader>(flag);
    }

    CEntityFlags::Entity CWebReaderFlags::allEntitiesForReaders(WebReader readers)
    {
        CEntityFlags::Entity entities = CEntityFlags::NoEntity;
        if (readers.testFlag(IcaoDataReader)) { entities |= CEntityFlags::AllIcaoAndCountries; }
        if (readers.testFlag(ModelReader)) { entities |= CEntityFlags::DistributorLiveryModel; }
        if (readers.testFlag(AirportReader)) { entities |= CEntityFlags::AirportEntity; }
        if (readers.testFlag(DbInfoDataReader)) { entities |= CEntityFlags::InfoObjectEntity; }
        if (readers.testFlag(VatsimBookingReader)) { entities |= CEntityFlags::BookingEntity; }
        if (readers.testFlag(VatsimMetarReader)) { entities |= CEntityFlags::MetarEntity; }
        if (readers.testFlag(VatsimDataReader)) { entities |= CEntityFlags::VatsimDataFile; }
        if (readers.testFlag(VatsimStatusReader)) { entities |= CEntityFlags::VatsimStatusFile; }
        return entities;
    }

    bool CWebReaderFlags::isFromSwiftDb(BlackMisc::Network::CEntityFlags::Entity entity)
    {
        return isFromSwiftDb(entityToReader(entity));
    }

    bool CWebReaderFlags::isFromSwiftDb(WebReader reader)
    {
        return reader.testFlag(ModelReader) || reader.testFlag(IcaoDataReader) || reader.testFlag(DbInfoDataReader);
    }

} // namespace
