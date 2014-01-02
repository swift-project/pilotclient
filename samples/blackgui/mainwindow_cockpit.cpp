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
using namespace BlackMisc::Math;

/*
 * Cockpit values
 */
void MainWindow::cockpitValuesChanged()
{
    Q_ASSERT(this->m_timerCollectedCockpitUpdates);
    this->m_timerCollectedCockpitUpdates->stop();
    this->m_timerCollectedCockpitUpdates->start(1000); // start
    this->m_timerCollectedCockpitUpdates->setSingleShot(true);
}

/*
 * Is pending update
 */
bool MainWindow::isCockpitUpdatePending() const
{
    return (this->m_timerCollectedCockpitUpdates && this->m_timerCollectedCockpitUpdates->isActive());
}

/*
 * Own cockpit
 */
void MainWindow::updateCockpitFromContext()
{
    // update GUI elements
    // avoid unnecessary change events as far as possible
    const CComSystem com1 = this->m_ownAircraft.getCom1System();
    const CComSystem com2 = this->m_ownAircraft.getCom2System();
    const CTransponder transponder = this->m_ownAircraft.getTransponder();

    double freq = com1.getFrequencyActive().valueRounded(3);
    if (freq != this->ui->ds_CockpitCom1Active->value())
        this->ui->ds_CockpitCom1Active->setValue(freq);

    freq = com2.getFrequencyActive().valueRounded(3);
    if (freq != this->ui->ds_CockpitCom2Active->value())
        this->ui->ds_CockpitCom2Active->setValue(freq);

    freq = com1.getFrequencyStandby().valueRounded(3);
    if (freq != this->ui->ds_CockpitCom1Standby->value())
        this->ui->ds_CockpitCom1Standby->setValue(freq);

    freq = com2.getFrequencyStandby().valueRounded(3);
    if (freq != this->ui->ds_CockpitCom2Standby->value())
        this->ui->ds_CockpitCom2Standby->setValue(freq);

    qint32 tc = transponder.getTransponderCode();
    if (tc != static_cast<qint32>(this->ui->ds_CockpitTransponder->value()))
        this->ui->ds_CockpitTransponder->setValue(tc);

    QString tm = this->ui->cb_CockpitTransponderMode->currentText().trimmed().toUpper();
    switch (transponder.getTransponderMode())
    {
    case CTransponder::StateStandby:
    case CTransponder::ModeS:
        if (tm != "S")
            this->ui->cb_CockpitTransponderMode->setCurrentText("S");
        break;
    case CTransponder::ModeC:
        if (tm != "C")
            this->ui->cb_CockpitTransponderMode->setCurrentText("C");
        break;
    case CTransponder::StateIdent:
        if (tm != "I")
            this->ui->cb_CockpitTransponderMode->setCurrentText("I");
        break;
    default:
        break;
    }
}

/*
 * Send cockpit updates
 */
void MainWindow::sendCockpitUpdates()
{
    CTransponder transponder = this->m_ownAircraft.getTransponder();
    CComSystem com1 = this->m_ownAircraft.getCom1System();
    CComSystem com2 = this->m_ownAircraft.getCom2System();

    QString transponderCode = QString::number(qRound(this->ui->ds_CockpitTransponder->value()));
    if (CTransponder::isValidTransponderCode(transponderCode))
    {
        transponder.setTransponderCode(transponderCode);
    }
    else
    {
        this->displayStatusMessage(CStatusMessage::getValidationError("Wrong transponder code"));
        return;
    }

    QString tm = this->ui->cb_CockpitTransponderMode->currentText().toUpper();
    if (tm == "S")
        transponder.setTransponderMode(CTransponder::ModeS);
    else if (tm == "C")
        transponder.setTransponderMode(CTransponder::ModeC);
    else if (tm == "I")
        transponder.setTransponderMode(CTransponder::StateIdent);

    com1.setFrequencyActiveMHz(this->ui->ds_CockpitCom1Active->value());
    com1.setFrequencyStandbyMHz(this->ui->ds_CockpitCom1Standby->value());
    com2.setFrequencyActiveMHz(this->ui->ds_CockpitCom2Active->value());
    com2.setFrequencyStandbyMHz(this->ui->ds_CockpitCom2Standby->value());

    if (this->m_contextNetworkAvailable)
    {
        if (this->m_ownAircraft.getCom1System() != com1 ||
                this->m_ownAircraft.getCom2System() != com2 ||
                this->m_ownAircraft.getTransponder() != transponder)
        {
            this->m_contextNetwork->updateOwnCockpit(com1, com2, transponder);
            this->reloadOwnAircraft();
        }
    }
}
