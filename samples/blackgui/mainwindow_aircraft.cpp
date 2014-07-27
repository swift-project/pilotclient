#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "blackgui/models/atcstationlistmodel.h"
#include "blackcore/dbus_server.h"
#include "blackcore/context_network.h"

using namespace BlackCore;
using namespace BlackMisc;
using namespace BlackGui;
using namespace BlackMisc::Network;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Settings;
using namespace BlackMisc::Audio;

/*
 * Load own aircraft
 */
bool MainWindow::ps_reloadOwnAircraft()
{
    if (!this->isContextNetworkAvailableCheck()) return false;

    // check for changed aircraft
    bool changed = false;
    CAircraft loadedAircraft = this->getIContextOwnAircraft()->getOwnAircraft();
    if (loadedAircraft != this->m_ownAircraft)
    {
        this->m_ownAircraft = loadedAircraft;
        this->ui->comp_MainInfoArea->getFlightPlanComponent()->prefillWithAircraftData(this->m_ownAircraft);
        changed = true;
    }
    return changed;
}

/*
* Position
*/
void MainWindow::setTestPosition(const QString &wgsLatitude, const QString &wgsLongitude, const CAltitude &altitude)
{
    CCoordinateGeodetic coordinate(
        CLatitude::fromWgs84(wgsLatitude),
        CLongitude::fromWgs84(wgsLongitude),
        CLength(0, CLengthUnit::m()));

    this->m_ownAircraft.setPosition(coordinate);
    this->m_ownAircraft.setAltitude(altitude);
    this->getIContextOwnAircraft()->updateOwnPosition(coordinate, altitude, MainWindow::sampleBlackGuiOriginator());
}
