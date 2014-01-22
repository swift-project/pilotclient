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
using namespace BlackMisc::Voice;

/*
 * Read aircrafts
 */
void MainWindow::reloadAircraftsInRange()
{
    if (!this->isContextNetworkAvailableCheck()) return;
    this->m_aircraftsInRange->update(this->m_contextNetwork->getAircraftsInRange());
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
    CAircraft loadedAircraft = this->m_contextNetwork->getOwnAircraft();

    // changed aircraft
    if (loadedAircraft == this->m_ownAircraft) return false;
    this->m_ownAircraft = loadedAircraft;

    // update voice rooms
    if (this->m_contextVoiceAvailable)
    {
        CVoiceRoomList selectedRooms = this->m_contextNetwork->getSelectedVoiceRooms();
        if (selectedRooms.size() == 2)
        {
            this->m_voiceRoomCom1 = this->ui->cb_CockpitVoiceRoom1Override->isChecked() ?
                                    CVoiceRoom(this->ui->le_CockpitVoiceRoomCom1->text().trimmed()) :
                                    selectedRooms[0];
            this->m_voiceRoomCom2 = this->ui->cb_CockpitVoiceRoom2Override->isChecked() ?
                                    CVoiceRoom(this->ui->le_CockpitVoiceRoomCom2->text().trimmed()) :
                                    selectedRooms[1];
        }
    }

    //
    this->updateCockpitFromContext();

    // something has changed
    return true;
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
    this->m_contextNetwork->updateOwnPosition(
        coordinate,
        altitude
    );
}
