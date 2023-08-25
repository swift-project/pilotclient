// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackcore/application.h"
#include "blackcore/webdataservices.h"
#include "blackcore/context/contextownaircraft.h"
#include "blackcore/context/contextownaircraftempty.h"
#include "blackcore/context/contextownaircraftimpl.h"
#include "blackcore/context/contextownaircraftproxy.h"
#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/dbusserver.h"

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Simulation;

namespace BlackCore::Context
{
    IContextOwnAircraft *IContextOwnAircraft::create(CCoreFacade *parent, CCoreFacadeConfig::ContextMode mode, CDBusServer *server, QDBusConnection &connection)
    {
        switch (mode)
        {
        case CCoreFacadeConfig::Local:
        case CCoreFacadeConfig::LocalInDBusServer:
            return (new CContextOwnAircraft(mode, parent))->registerWithDBus(server);
        case CCoreFacadeConfig::Remote:
            return new CContextOwnAircraftProxy(CDBusServer::coreServiceName(connection), connection, mode, parent);
        case CCoreFacadeConfig::NotUsed:
        default:
            return new CContextOwnAircraftEmpty(parent);
        }
    }

    const CAircraftSituation &IContextOwnAircraft::getDefaultSituation()
    {
        static const CAircraftSituation situation(
            CCoordinateGeodetic(
                CLatitude::fromWgs84("N 049° 18' 17"),
                CLongitude::fromWgs84("E 008° 27' 05"),
                CAltitude(312, CAltitude::MeanSeaLevel, CLengthUnit::ft())));
        return situation;
    }

    CAircraftModel IContextOwnAircraft::getDefaultOwnAircraftModel()
    {
        // if all fails
        static const CAircraftModel defaultModel(
            "", CAircraftModel::TypeOwnSimulatorModel, "default model",
            CAircraftIcaoCode("C172", "L1P", "Cessna", "172", "L", true, false, false, 0));

        // create one from DB data
        if (sApp && sApp->hasWebDataServices())
        {
            static const CAircraftIcaoCode icao = sApp->getWebDataServices()->getAircraftIcaoCodeForDesignator("C172");
            static const CLivery livery = sApp->getWebDataServices()->getLiveryForCombinedCode("_CC_WHITE_WHITE");
            static const CAircraftModel model("", CAircraftModel::TypeOwnSimulatorModel, icao, livery);
            return model;
        }
        return defaultModel;
    }
} // namespace
