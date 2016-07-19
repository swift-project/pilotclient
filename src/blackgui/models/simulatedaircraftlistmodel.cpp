/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackgui/models/columnformatters.h"
#include "blackgui/models/columns.h"
#include "blackgui/models/simulatedaircraftlistmodel.h"
#include "blackmisc/aviation/aircraftsituation.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/aviation/comsystem.h"
#include "blackmisc/aviation/modulator.h"
#include "blackmisc/aviation/transponder.h"
#include "blackmisc/network/user.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/simulation/simulatedaircraft.h"

#include <Qt>
#include <QtDebug>
#include <QtGlobal>

using namespace BlackMisc::Simulation;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Network;
using namespace BlackMisc;

namespace BlackGui
{
    namespace Models
    {
        CSimulatedAircraftListModel::CSimulatedAircraftListModel(QObject *parent) : CListModelBase("ModelSimulatedAircraftList", parent)
        {
            this->setAircraftMode(InfoMode);

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
            if (this->m_mode == mode) { return; }
            this->m_mode = mode;
            this->m_columns.clear();
            switch (mode)
            {
            case NotSet:
            case InfoMode:
                {
                    this->m_columns.addColumn(CColumn::standardValueObject("cs.", "callsign", CSimulatedAircraft::IndexCallsign, CCallsign::IndexCallsignString));
                    this->m_columns.addColumn(CColumn::standardString("realname", "pilot's real name", { CSimulatedAircraft::IndexPilot, CUser::IndexRealName }));
                    this->m_columns.addColumn(CColumn("dist.", "distance", CSimulatedAircraft::IndexRelativeDistance, new CAirspaceDistanceFormatter()));
                    this->m_columns.addColumn(CColumn("altitude", { CSimulatedAircraft::IndexSituation, CAircraftSituation::IndexAltitude }, new CAltitudeFormatter()));
                    this->m_columns.addColumn(CColumn("gs.", { CSimulatedAircraft::IndexSituation, CAircraftSituation::IndexGroundspeed }, new CAircraftSpeedFormatter()));
                    this->m_columns.addColumn(CColumn::standardString("icao", "icao and livery info", { CSimulatedAircraft::IndexCombinedIcaoLiveryString}));

                    CColumn col("airline", { CSimulatedAircraft::IndexLivery, CLivery::IndexAirlineIcaoCode, CAirlineIcaoCode::IndexIcon });
                    col.setSortPropertyIndex({ CSimulatedAircraft::IndexLivery, CLivery::IndexAirlineIcaoCode, CAirlineIcaoCode::IndexAirlineDesignator});
                    this->m_columns.addColumn(col);

                    this->m_columns.addColumn(CColumn("frequency", { CSimulatedAircraft::IndexCom1System, CComSystem::IndexActiveFrequency }, new CComFrequencyFormatter()));
                    this->m_columns.addColumn(CColumn::standardString("transponder", { CSimulatedAircraft::IndexTransponder, CTransponder::IndexTransponderCodeAndModeFormatted }));
                    this->m_columns.addColumn(CColumn("latitude", { CSimulatedAircraft::IndexSituation, CAircraftSituation::IndexLatitude }, new CLatLonFormatter()));
                    this->m_columns.addColumn(CColumn("longitude", { CSimulatedAircraft::IndexSituation, CAircraftSituation::IndexLongitude }, new CLatLonFormatter()));

                    // default sort order
                    this->setSortColumnByPropertyIndex(CSimulatedAircraft::IndexRelativeDistance);
                    this->m_sortOrder = Qt::AscendingOrder;
                }
                break;

            case ModelMode:
                {
                    this->m_columns.addColumn(CColumn("e.", "enabled", CSimulatedAircraft::IndexEnabled, new CBoolIconFormatter("enabled", "disabled"), true));
                    this->m_columns.addColumn(CColumn("r.", "rendered", CSimulatedAircraft::IndexRendered, new CBoolIconFormatter("rendered", "skipped"), true));
                    this->m_columns.addColumn(CColumn::standardValueObject("cs.", "callsign", { CSimulatedAircraft::IndexCallsign, CCallsign::IndexCallsignString }));
                    this->m_columns.addColumn(CColumn("dist.", "distance", CSimulatedAircraft::IndexRelativeDistance, new CAirspaceDistanceFormatter()));
                    this->m_columns.addColumn(CColumn("altitude", { CSimulatedAircraft::IndexSituation, CAircraftSituation::IndexAltitude }, new CAltitudeFormatter()));
                    this->m_columns.addColumn(CColumn("gs.", { CSimulatedAircraft::IndexSituation, CAircraftSituation::IndexGroundspeed }, new CAircraftSpeedFormatter()));
                    this->m_columns.addColumn(CColumn("p.", "parts", CSimulatedAircraft::IndexPartsSynchronized, new CBoolIconFormatter("parts", "no parts"), true));
                    this->m_columns.addColumn(CColumn("fp.", "fast position updates", CSimulatedAircraft::IndexFastPositionUpdates, new CBoolIconFormatter("enabled", "disabled"), true));
                    this->m_columns.addColumn(CColumn::standardString("realname", "pilot's real name", { CSimulatedAircraft::IndexPilot, CUser::IndexRealName }));
                    this->m_columns.addColumn(CColumn::standardString("icao", CSimulatedAircraft::IndexCombinedIcaoLiveryString));
                    this->m_columns.addColumn(CColumn::standardString("model", { CSimulatedAircraft::IndexModel, CAircraftModel::IndexModelString}));
                    this->m_columns.addColumn(CColumn::standardString("desc.", "description", { CSimulatedAircraft::IndexModel, CAircraftModel::IndexDescription}));
                    this->m_columns.addColumn(CColumn::standardString("type", { CSimulatedAircraft::IndexModel, CAircraftModel::IndexModelTypeAsString}));

                    // default sort order
                    this->setSortColumnByPropertyIndex(CSimulatedAircraft::IndexRelativeDistance);
                    this->m_sortOrder = Qt::AscendingOrder;
                }
                break;

            default:
                qFatal("Wrong mode");
                break;
            }
        }
    } // namespace
} // namespace
