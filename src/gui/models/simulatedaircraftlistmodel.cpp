// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/models/simulatedaircraftlistmodel.h"

#include <Qt>
#include <QtDebug>
#include <QtGlobal>

#include "gui/models/columnformatters.h"
#include "gui/models/columns.h"
#include "misc/aviation/aircraftsituation.h"
#include "misc/aviation/callsign.h"
#include "misc/aviation/comsystem.h"
#include "misc/aviation/modulator.h"
#include "misc/aviation/transponder.h"
#include "misc/network/user.h"
#include "misc/simulation/aircraftmodel.h"
#include "misc/simulation/simulatedaircraft.h"

using namespace swift::misc;
using namespace swift::misc::aviation;
using namespace swift::misc::network;
using namespace swift::misc::physical_quantities;
using namespace swift::misc::simulation;

namespace swift::gui::models
{
    CSimulatedAircraftListModel::CSimulatedAircraftListModel(QObject *parent)
        : CListModelCallsignObjects("ModelSimulatedAircraftList", parent)
    {
        this->setAircraftMode(NetworkMode);

        // force strings for translation in resource files
        (void)QT_TRANSLATE_NOOP("ModelSimulatedAircraftList", "callsign");
        (void)QT_TRANSLATE_NOOP("ModelSimulatedAircraftList", "pilotrealname");
        (void)QT_TRANSLATE_NOOP("ModelSimulatedAircraftList", "latitude");
        (void)QT_TRANSLATE_NOOP("ModelSimulatedAircraftList", "longitude");
        (void)QT_TRANSLATE_NOOP("ModelSimulatedAircraftList", "altitude");
        (void)QT_TRANSLATE_NOOP("ModelSimulatedAircraftList", "distance");
        (void)QT_TRANSLATE_NOOP("ModelSimulatedAircraftList", "height");
        (void)QT_TRANSLATE_NOOP("ModelSimulatedAircraftList", "transponder");
        (void)QT_TRANSLATE_NOOP("ModelSimulatedAircraftList", "groundspeed");
        (void)QT_TRANSLATE_NOOP("ModelSimulatedAircraftList", "icao");
        (void)QT_TRANSLATE_NOOP("ModelSimulatedAircraftList", "model");
    }

    void CSimulatedAircraftListModel::setAircraftMode(AircraftMode mode)
    {
        if (m_mode == mode) { return; }
        m_mode = mode;
        m_columns.clear();
        switch (mode)
        {
        case NotSet:
        case NetworkMode:
        {
            m_columns.addColumn(CColumn::standardValueObject("cs.", "callsign", CSimulatedAircraft::IndexCallsign,
                                                             CCallsign::IndexCallsignString));
            m_columns.addColumnIncognito(CColumn::standardString(
                "realname", "pilot's real name", { CSimulatedAircraft::IndexPilot, CUser::IndexRealName }));
            m_columns.addColumn(CColumn("dist.", "distance", CSimulatedAircraft::IndexRelativeDistance,
                                        new CAirspaceDistanceFormatter()));
            m_columns.addColumn(CColumn("altitude",
                                        { CSimulatedAircraft::IndexSituation, CAircraftSituation::IndexAltitude },
                                        new CAltitudeFormatter()));
            m_columns.addColumn(CColumn("gs.",
                                        { CSimulatedAircraft::IndexSituation, CAircraftSituation::IndexGroundSpeed },
                                        new CSpeedKtsFormatter()));
            m_columns.addColumn(CColumn::standardString(
                "icao", "icao and livery info", { CSimulatedAircraft::IndexCombinedIcaoLiveryStringNetworkModel }));

            // icon column for airline
            CPixmapFormatter *pmf = new CPixmapFormatter();
            pmf->setMaxHeight(25);
            pmf->setMaxWidth(100);
            CColumn col("airline",
                        { CSimulatedAircraft::IndexNetworkModel, CAircraftModel::IndexLivery,
                          CLivery::IndexAirlineIcaoCode, CAirlineIcaoCode::IndexIcon },
                        pmf);
            col.setSortPropertyIndex({ CSimulatedAircraft::IndexNetworkModel, CAircraftModel::IndexLivery,
                                       CLivery::IndexAirlineIcaoCode, CAirlineIcaoCode::IndexAirlineDesignator });
            m_columns.addColumn(col);

            m_columns.addColumn(CColumn("frequency",
                                        { CSimulatedAircraft::IndexCom1System, CComSystem::IndexActiveFrequency },
                                        new CComFrequencyFormatter()));
            m_columns.addColumn(
                CColumn::standardString("transponder", { CSimulatedAircraft::IndexTransponder,
                                                         CTransponder::IndexTransponderCodeAndModeFormatted }));
            m_columns.addColumn(CColumn("latitude",
                                        { CSimulatedAircraft::IndexSituation, CAircraftSituation::IndexLatitude },
                                        new CLatLonFormatter()));
            m_columns.addColumn(CColumn("longitude",
                                        { CSimulatedAircraft::IndexSituation, CAircraftSituation::IndexLongitude },
                                        new CLatLonFormatter()));

            // default sort order
            this->setSortColumnByPropertyIndex(CSimulatedAircraft::IndexRelativeDistance);
            m_sortOrder = Qt::AscendingOrder;
        }
        break;

        case RenderedMode:
        {
            m_columns.addColumn(CColumn("e.", "enabled", CSimulatedAircraft::IndexEnabled,
                                        new CBoolIconFormatter("enabled", "disabled"), true));
            m_columns.addColumn(CColumn("r.", "rendered", CSimulatedAircraft::IndexRendered,
                                        new CBoolIconFormatter("rendered", "skipped"), true));
            m_columns.addColumn(CColumn::standardValueObject(
                "cs.", "callsign", { CSimulatedAircraft::IndexCallsign, CCallsign::IndexCallsignString }));
            m_columns.addColumn(CColumn("dist.", "distance", CSimulatedAircraft::IndexRelativeDistance,
                                        new CAirspaceDistanceFormatter()));
            m_columns.addColumn(CColumn("altitude",
                                        { CSimulatedAircraft::IndexSituation, CAircraftSituation::IndexAltitude },
                                        new CAltitudeFormatter()));
            m_columns.addColumn(CColumn("CG", { CSimulatedAircraft::IndexModel, CAircraftModel::IndexCG },
                                        new CPhysiqalQuantiyFormatter<CLengthUnit, CLength>(CLengthUnit::ft(), 1)));
            m_columns.addColumn(CColumn("gs.",
                                        { CSimulatedAircraft::IndexSituation, CAircraftSituation::IndexGroundSpeed },
                                        new CSpeedKtsFormatter()));
            m_columns.addColumn(CColumn("p.", "parts", CSimulatedAircraft::IndexPartsSynchronized,
                                        new CBoolIconFormatter("parts", "no parts"), true));
            m_columns.addColumn(CColumn("fp.", "fast position updates", CSimulatedAircraft::IndexFastPositionUpdates,
                                        new CBoolIconFormatter("enabled", "disabled"), true));
            m_columns.addColumn(CColumn("gnd", "supports gnd.flag", CSimulatedAircraft::IndexSupportsGndFlag,
                                        new CBoolIconFormatter("yes", "no"), true));
            m_columns.addColumnIncognito(CColumn::standardString(
                "realname", "pilot's real name", { CSimulatedAircraft::IndexPilot, CUser::IndexRealName }));
            m_columns.addColumn(CColumn::standardString("icao", CSimulatedAircraft::IndexCombinedIcaoLiveryString));
            m_columns.addColumn(
                CColumn::standardString("model", { CSimulatedAircraft::IndexModel, CAircraftModel::IndexModelString }));
            m_columns.addColumn(CColumn::standardString(
                "type", { CSimulatedAircraft::IndexModel, CAircraftModel::IndexModelTypeAsString }));
            m_columns.addColumn(CColumn::standardString(
                "desc.", "description", { CSimulatedAircraft::IndexModel, CAircraftModel::IndexDescription }));
            m_columns.addColumn(CColumn::standardString("aircraft", "rendered vs. network aircraft ICAO",
                                                        CSimulatedAircraft::IndexNetworkModelAircraftIcaoDifference));
            m_columns.addColumn(CColumn::standardString("livery", "rendered vs. network livery",
                                                        CSimulatedAircraft::IndexNetworkModelLiveryDifference));

            // default sort order
            this->setSortColumnByPropertyIndex(CSimulatedAircraft::IndexRelativeDistance);
            m_sortOrder = Qt::AscendingOrder;
        }
        break;

        default: qFatal("Wrong mode"); break;
        }
    }
} // namespace swift::gui::models
