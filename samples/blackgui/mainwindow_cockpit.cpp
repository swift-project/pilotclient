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

    // this will call send cockpit updates with all changes made
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

    //
    // voice
    //
    if (!this->ui->cb_CockpitVoiceRoom1Override->isChecked())
    {
        if (!this->ui->cb_CockpitVoiceRoom1Override->isChecked() && this->m_voiceRoomCom1.isValid())
        {
            QString s(this->m_voiceRoomCom1.isConnected() ? "*" : "");
            s.append(this->m_voiceRoomCom1.getVoiceRoomUrl());
            this->ui->le_CockpitVoiceRoomCom1->setText(s);
        }
        else
            this->ui->le_CockpitVoiceRoomCom1->setText("");
    }

    if (!this->ui->cb_CockpitVoiceRoom2Override->isChecked())
    {
        if (this->m_voiceRoomCom2.isValid())
        {
            QString s(this->m_voiceRoomCom2.isConnected() ? "*" : "");
            s.append(this->m_voiceRoomCom2.getVoiceRoomUrl());
            this->ui->le_CockpitVoiceRoomCom2->setText(s);
        }
        else
            this->ui->le_CockpitVoiceRoomCom2->setText("");
    }
}

/*
 * Reset transponder mode to Standby
 */
void MainWindow::resetTransponderModerToStandby()
{
    this->ui->cb_CockpitTransponderMode->setCurrentText("S");
}

/*
 * Reset transponder mode to Standby
 */
void MainWindow::resetTransponderModerToCharly()
{
    this->ui->cb_CockpitTransponderMode->setCurrentText("C");
}

/*
 * Send cockpit updates
 */
void MainWindow::sendCockpitUpdates()
{
    CTransponder transponder = this->m_ownAircraft.getTransponder();
    CComSystem com1 = this->m_ownAircraft.getCom1System();
    CComSystem com2 = this->m_ownAircraft.getCom2System();

    //
    // Transponder
    //
    QString transponderCode = QString::number(qRound(this->ui->ds_CockpitTransponder->value()));
    if (CTransponder::isValidTransponderCode(transponderCode))
    {
        transponder.setTransponderCode(transponderCode);
    }
    else
    {
        this->displayStatusMessage(CStatusMessage::getValidationError("Wrong transponder code, reset"));
        this->ui->ds_CockpitTransponder->setValue(transponder.getTransponderCode());
    }

    QString tm = this->ui->cb_CockpitTransponderMode->currentText().toUpper();
    if (tm == "S")
        transponder.setTransponderMode(CTransponder::ModeS);
    else if (tm == "C")
        transponder.setTransponderMode(CTransponder::ModeC);
    else if (tm == "I")
    {
        // ident shall be sent for some time, then reset
        transponder.setTransponderMode(CTransponder::StateIdent);
        if (this->m_ownAircraft.getTransponderMode() == CTransponder::ModeS)
            QTimer::singleShot(5000, this, SLOT(resetTransponderModerToStandby()));
        else
            QTimer::singleShot(5000, this, SLOT(resetTransponderModerToCharly()));
    }

    //
    // COM units
    //
    com1.setFrequencyActiveMHz(this->ui->ds_CockpitCom1Active->value());
    com1.setFrequencyStandbyMHz(this->ui->ds_CockpitCom1Standby->value());
    com2.setFrequencyActiveMHz(this->ui->ds_CockpitCom2Active->value());
    com2.setFrequencyStandbyMHz(this->ui->ds_CockpitCom2Standby->value());


    //
    // Send to context
    //
    bool changedCockpit = false;
    if (this->m_contextNetworkAvailable)
    {
        if (this->m_ownAircraft.getCom1System() != com1 ||
                this->m_ownAircraft.getCom2System() != com2 ||
                this->m_ownAircraft.getTransponder() != transponder)
        {
            this->m_contextNetwork->updateOwnCockpit(com1, com2, transponder);
            this->reloadOwnAircraft(); // also loads resolved voice rooms
            changedCockpit = true;
        }
    }

    //
    // Now with the new voice room data, really set the
    // voice rooms in the context
    //
    if (changedCockpit && this->m_contextVoiceAvailable)
    {
        // set voice rooms here, this allows to use local/remote
        // voice context
        this->m_contextVoice->setComVoiceRooms(this->m_voiceRoomCom1, this->m_voiceRoomCom2);
    }
}

/*
 * Voice room override
 */
void MainWindow::voiceRoomOverride()
{
    this->ui->le_CockpitVoiceRoomCom1->setReadOnly(!this->ui->cb_CockpitVoiceRoom1Override->isChecked());
    this->ui->le_CockpitVoiceRoomCom2->setReadOnly(!this->ui->cb_CockpitVoiceRoom2Override->isChecked());
    if (this->ui->cb_CockpitVoiceRoom1Override->isChecked())
    {
        this->m_voiceRoomCom1 = this->ui->cb_CockpitVoiceRoom1Override->text().trimmed();
    }

    if (this->ui->cb_CockpitVoiceRoom2Override->isChecked())
    {
        this->m_voiceRoomCom2 = this->ui->cb_CockpitVoiceRoom2Override->text().trimmed();
    }
}
