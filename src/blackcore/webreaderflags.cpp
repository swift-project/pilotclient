/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackcore/webreaderflags.h"

using namespace BlackMisc::Network;

namespace BlackCore
{
    CWebReaderFlags::WebReader CWebReaderFlags::entitiesToReaders(CEntityFlags::Entity entities)
    {
        WebReader f = None;
        if (
            entities.testFlag(CEntityFlags::AircraftIcaoEntity) || entities.testFlag(CEntityFlags::AirlineIcaoEntity) ||
            entities.testFlag(CEntityFlags::CountryEntity) || entities.testFlag(CEntityFlags::AircraftCategoryEntity))
        {
            f |= IcaoDataReader;
        }

        if (entities.testFlag(CEntityFlags::ModelEntity) || entities.testFlag(CEntityFlags::DistributorEntity) || entities.testFlag(CEntityFlags::LiveryEntity))
        {
            f |= ModelReader;
        }

        if (entities.testFlag(CEntityFlags::AirportEntity))
        {
            f |= AirportReader;
        }

        if (entities.testFlag(CEntityFlags::DbInfoObjectEntity)) { f |= DbInfoDataReader; }
        if (entities.testFlag(CEntityFlags::BookingEntity))      { f |= VatsimBookingReader; }
        if (entities.testFlag(CEntityFlags::VatsimDataFile))     { f |= VatsimDataReader; }
        if (entities.testFlag(CEntityFlags::VatsimStatusFile))   { f |= VatsimStatusReader; }
        if (entities.testFlag(CEntityFlags::MetarEntity))        { f |= VatsimMetarReader; }

        return f;
    }

    CWebReaderFlags::WebReader CWebReaderFlags::webReaderFlagToWebReader(CWebReaderFlags::WebReaderFlag flag)
    {
        return static_cast<WebReader>(flag);
    }

    CEntityFlags::Entity CWebReaderFlags::allEntitiesForReaders(WebReader readers)
    {
        CEntityFlags::Entity entities = CEntityFlags::NoEntity;
        if (readers.testFlag(IcaoDataReader))      { entities |= CEntityFlags::AllIcaoCountriesCategory; }
        if (readers.testFlag(ModelReader))         { entities |= CEntityFlags::DistributorLiveryModel; }
        if (readers.testFlag(AirportReader))       { entities |= CEntityFlags::AirportEntity; }
        if (readers.testFlag(DbInfoDataReader))    { entities |= CEntityFlags::DbInfoObjectEntity; }
        if (readers.testFlag(VatsimBookingReader)) { entities |= CEntityFlags::BookingEntity; }
        if (readers.testFlag(VatsimMetarReader))   { entities |= CEntityFlags::MetarEntity; }
        if (readers.testFlag(VatsimDataReader))    { entities |= CEntityFlags::VatsimDataFile; }
        if (readers.testFlag(VatsimStatusReader))  { entities |= CEntityFlags::VatsimStatusFile; }
        return entities;
    }

    // some strange Doxygen warning, added KB 2017-11
    //! \cond PRIVATE
    bool CWebReaderFlags::isFromSwiftDb(CEntityFlags::Entity entity)
    {
        Q_ASSERT_X(CEntityFlags::isSingleEntity(entity), Q_FUNC_INFO, "Need single entity");
        return CWebReaderFlags::isFromSwiftDb(entitiesToReaders(entity));
    }
    //! \endcond

    bool CWebReaderFlags::isFromSwiftDb(WebReader reader)
    {
        return reader.testFlag(ModelReader) || reader.testFlag(IcaoDataReader) || reader.testFlag(DbInfoDataReader) || reader.testFlag(AirportReader);
    }

    int CWebReaderFlags::numberOfReaders(WebReader readers)
    {
        int n = 0;
        if (readers.testFlag(ModelReader))      { n++; }
        if (readers.testFlag(IcaoDataReader))   { n++; }
        if (readers.testFlag(AirportReader))    { n++; }
        if (readers.testFlag(DbInfoDataReader)) { n++; }
        return n;
    }

    bool CWebReaderFlags::isSingleReader(WebReader readers)
    {
        return numberOfReaders(readers) == 1;
    }
} // namespace
