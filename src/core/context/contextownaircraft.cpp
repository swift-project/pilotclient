// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "core/context/contextownaircraft.h"

#include "core/application.h"
#include "core/context/contextownaircraftempty.h"
#include "core/context/contextownaircraftimpl.h"
#include "core/context/contextownaircraftproxy.h"
#include "core/webdataservices.h"
#include "misc/aviation/aircraftsituation.h"
#include "misc/dbusserver.h"

using namespace swift::misc;
using namespace swift::misc::aviation;
using namespace swift::misc::physical_quantities;
using namespace swift::misc::geo;
using namespace swift::misc::simulation;

namespace swift::core::context
{
    IContextOwnAircraft *IContextOwnAircraft::create(CCoreFacade *parent, CCoreFacadeConfig::ContextMode mode,
                                                     CDBusServer *server, QDBusConnection &connection)
    {
        switch (mode)
        {
        case CCoreFacadeConfig::Local: return new CContextOwnAircraft(mode, parent);
        case CCoreFacadeConfig::LocalInDBusServer:
        {
            auto *context = new CContextOwnAircraft(mode, parent);
            context->registerWithDBus(server);
            return context;
        }
        case CCoreFacadeConfig::Remote:
            return new CContextOwnAircraftProxy(CDBusServer::coreServiceName(connection), connection, mode, parent);
        case CCoreFacadeConfig::NotUsed:
        default: return new CContextOwnAircraftEmpty(parent);
        }
    }

    const CAircraftSituation &IContextOwnAircraft::getDefaultSituation()
    {
        static const CAircraftSituation situation(
            CCoordinateGeodetic(CLatitude::fromWgs84("N 049° 18' 17"), CLongitude::fromWgs84("E 008° 27' 05"),
                                CAltitude(312, CAltitude::MeanSeaLevel, CLengthUnit::ft())));
        return situation;
    }

    CAircraftModel IContextOwnAircraft::getDefaultOwnAircraftModel()
    {
        // if all fails
        static const CAircraftModel defaultModel(
            "", CAircraftModel::TypeOwnSimulatorModel, "default model",
            CAircraftIcaoCode("C172", "L1P", "Cessna", "172", CWakeTurbulenceCategory::LIGHT, true, false, false, 0));

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
} // namespace swift::core::context
