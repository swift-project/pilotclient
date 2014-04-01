#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "blackgui/atcstationlistmodel.h"
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
 * Read aircrafts
 */
void MainWindow::reloadAircraftsInRange()
{
    if (!this->isContextNetworkAvailableCheck()) return;
    this->m_modelAircraftsInRange->update(this->m_rt->getIContextNetwork()->getAircraftsInRange());
    this->ui->tv_AircraftsInRange->resizeColumnsToContents();
    this->ui->tv_AircraftsInRange->resizeRowsToContents();
}

/*
 * Read own aircraft
 */
bool MainWindow::reloadOwnAircraft()
{
    if (!this->isContextNetworkAvailableCheck()) return false;
    if (this->isCockpitUpdatePending()) return false;

    // check for changed aircraft
    bool changed = false;
    CAircraft loadedAircraft = this->m_rt->getIContextNetwork()->getOwnAircraft();
    if (loadedAircraft != this->m_ownAircraft)
    {
        this->m_ownAircraft = loadedAircraft;
        this->updateCockpitFromContext();
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
    this->m_rt->getIContextNetwork()->updateOwnPosition(
        coordinate,
        altitude
    );
}
