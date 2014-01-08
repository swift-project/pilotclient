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


/*
 * Read booked stations
 */
void MainWindow::reloadAtcStationsBooked()
{
    if (!this->isContextNetworkAvailableCheck()) return;
    this->m_atcListBooked->update(this->m_contextNetwork->getAtcStationsBooked());
    this->ui->tv_AtcStationsBooked->resizeColumnsToContents();
}

/*
 * Read online stations
 */
void MainWindow::reloadAtcStationsOnline()
{
    if (!this->isContextNetworkAvailableCheck()) return;
    this->m_atcListOnline->update(this->m_contextNetwork->getAtcStationsOnline());
    this->ui->tv_AtcStationsOnline->resizeColumnsToContents();
}

/*
 * Station selected
 */
void MainWindow::onlineAtcStationSelected(QModelIndex index)
{
    this->ui->te_AtcStationsOnlineInfo->setText(""); // reset
    const CAtcStation stationClicked = this->m_atcListOnline->at(index);
    QString infoMessage;

    if (stationClicked.hasAtis())
    {
        infoMessage.append(stationClicked.getAtis().getMessage());
    }
    if (stationClicked.hasMetar())
    {
        if (!infoMessage.isEmpty()) infoMessage.append("\n\n");
        infoMessage.append(stationClicked.getMetar().getMessage());
    }

    this->ui->te_AtcStationsOnlineInfo->setText(infoMessage);
}

/*
 * Get METAR
 */
void MainWindow::getMetar(const QString &airportIcaoCode)
{
    if (!this->isContextNetworkAvailableCheck()) return;
    if (!this->m_contextNetwork->isConnected()) return;
    QString icao = airportIcaoCode.isEmpty() ? this->ui->le_AtcStationsOnlineMetar->text().trimmed().toUpper() : airportIcaoCode.trimmed().toUpper();
    this->ui->le_AtcStationsOnlineMetar->setText(icao);
    if (icao.length() != 4) return;
    CInformationMessage metar = this->m_contextNetwork->getMetar(icao);
    if (metar.getType() != CInformationMessage::METAR) return;
    if (metar.isEmpty()) return;
    this->ui->te_AtcStationsOnlineInfo->setText(metar.getMessage());
}

/*
 * ATC station tab changed are changed
 */
void MainWindow::atcStationTabChanged(int /** tabIndex **/)
{
    if (this->isContextNetworkAvailableCheck())
    {
        if (this->ui->tw_AtcStations->currentWidget() == this->ui->tb_AtcStationsBooked)
        {
            if (this->m_atcListBooked->rowCount() < 1)
                this->reloadAtcStationsBooked();
        }
        else if (this->ui->tw_AtcStations->currentWidget() == this->ui->tb_AtcStationsOnline)
        {
            this->reloadAtcStationsOnline();
        }
    }
}
