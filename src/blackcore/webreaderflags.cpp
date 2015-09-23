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
    CWebReaderFlags::WebReader CWebReaderFlags::entityToReader(CDbFlags::Entity entity)
    {
        WebReader f = None;
        if (entity.testFlag(CDbFlags::AircraftIcaoEntity) ||
                entity.testFlag(CDbFlags::AirlineIcaoEntity) ||
                entity.testFlag(CDbFlags::CountryEntity))
        {
            f |= IcaoDataReader;
        }

        if (entity.testFlag(CDbFlags::ModelEntity) ||
                entity.testFlag(CDbFlags::DistributorEntity) ||
                entity.testFlag(CDbFlags::LiveryEntity))
        {
            f |= ModelReader;
        }

        if (entity.testFlag(CDbFlags::VatsimBookings))
        {
            f |= VatsimBookingReader;
        }

        if (entity.testFlag(CDbFlags::VatsimDataFile))
        {
            f |= VatsimDataReader;
        }
        return f;
    }

    bool CWebReaderFlags::isFromSwiftDb(CDbFlags::Entity entity)
    {
        return isFromSwiftDb(entityToReader(entity));
    }

    bool CWebReaderFlags::isFromSwiftDb(WebReader reader)
    {
        return reader.testFlag(ModelReader) || reader.testFlag(IcaoDataReader);
    }

} // namespace
