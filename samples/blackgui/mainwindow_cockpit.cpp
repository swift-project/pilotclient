#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "blackgui/atcstationlistmodel.h"
#include "blackcore/dbus_server.h"
#include "blackcore/context_network.h"
#include "blackmisc/voiceroom.h"

using namespace BlackCore;
using namespace BlackMisc;
using namespace BlackGui;
using namespace BlackMisc::Network;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Geo;
using namespace BlackMisc::Settings;
using namespace BlackMisc::Math;
using namespace BlackMisc::Audio;

/*
 * Cockpit values
 */
void MainWindow::cockpitValuesChanged()
{
    Q_ASSERT(this->m_timerCollectedCockpitUpdates);

    // frequency switch?
    QObject *sender = QObject::sender();
    if (sender == this->ui->pb_CockpitToggleCom1)
    {
        if (this->ui->ds_CockpitCom1Standby->value() == this->ui->ds_CockpitCom1Active->value()) return;
        double f = this->ui->ds_CockpitCom1Active->value();
        this->ui->ds_CockpitCom1Active->setValue(this->ui->ds_CockpitCom1Standby->value());
        this->ui->ds_CockpitCom1Standby->setValue(f);
    }
    else if (sender == this->ui->pb_CockpitToggleCom2)
    {
        if (this->ui->ds_CockpitCom2Standby->value() == this->ui->ds_CockpitCom2Active->value()) return;
        double f = this->ui->ds_CockpitCom2Active->value();
        this->ui->ds_CockpitCom2Active->setValue(this->ui->ds_CockpitCom2Standby->value());
        this->ui->ds_CockpitCom2Standby->setValue(f);
    }
    else if (sender == this->ui->pb_CockpitIdent)
    {
        // trigger the real button
        if (this->ui->cbp_CockpitTransponderMode->isIdentSelected())
        {
            this->ui->pb_CockpitIdent->setStyleSheet("");
            this->ui->cbp_CockpitTransponderMode->resetTransponderMode();
        }
        else
        {
            this->ui->pb_CockpitIdent->setStyleSheet("background: red");
            this->ui->cbp_CockpitTransponderMode->setSelectedTransponderModeStateIdent(); // trigger real button and whole process
        }
        return;
    }

    // this will call send cockpit updates with all changes made
    // send cockpit updates
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
    const CComSystem com1 = this->m_ownAircraft.getCom1System(); // aircraft just updated or set from context
    const CComSystem com2 = this->m_ownAircraft.getCom2System();
    const CTransponder transponder = this->m_ownAircraft.getTransponder();

    // update the frequencies
    this->updateComFrequencyDisplays(com1, com2);

    if (this->m_inputFocusedWidget != this->ui->ds_CockpitTransponder)
    {
        // update transponder if this is not input focused
        qint32 tc = transponder.getTransponderCode();
        if (tc != static_cast<qint32>(this->ui->ds_CockpitTransponder->value()))
            this->ui->ds_CockpitTransponder->setValue(tc);
    }

    this->ui->cbp_CockpitTransponderMode->setSelectedTransponderMode(transponder.getTransponderMode());

    if (this->m_contextNetworkAvailable)
    {
        CAtcStationList selectedStations = this->getIContextNetwork()->getSelectedAtcStations();
        CAtcStation com1Station = selectedStations[0];
        CAtcStation com2Station = selectedStations[1];
        if (com1Station.getCallsign().isEmpty())
            this->ui->lbl_CockpitCom1->setToolTip("");
        else
            this->ui->lbl_CockpitCom1->setToolTip(com1Station.getCallsign().getStringAsSet());
        if (com2Station.getCallsign().isEmpty())
            this->ui->lbl_CockpitCom2->setToolTip("");
        else
            this->ui->lbl_CockpitCom2->setToolTip(com2Station.getCallsign().getStringAsSet());
    }

    //
    // Voice room override
    //
    if (this->m_contextAudioAvailable)
    {
        // get all rooms, it is important to get the rooms from voice context here
        // these are the ones featuring the real audio status
        CVoiceRoomList rooms = this->getIContextAudio()->getComVoiceRoomsWithAudioStatus();
        Q_ASSERT(rooms.size() == 2);

        CVoiceRoom room1 = rooms[0];
        CVoiceRoom room2 = rooms[1];
        bool com1Connected = room1.isConnected();
        bool com2Connected = room2.isConnected();

        // update views
        this->ui->tvp_CockpitVoiceRoom1->update(this->getIContextAudio()->getCom1RoomUsers());
        this->ui->tvp_CockpitVoiceRoom1->update(this->getIContextAudio()->getCom1RoomUsers());

        // highlite voice room according to status
        QString vrStyle1;
        QString vrStyle2;
        if (!room1.getVoiceRoomUrl(true).isEmpty()) vrStyle1 = com1Connected ? "background: green" : "background: red";
        if (!room2.getVoiceRoomUrl(true).isEmpty()) vrStyle2 = com2Connected ? "background: green" : "background: red";
        this->ui->le_CockpitVoiceRoomCom1->setStyleSheet(vrStyle1);
        this->ui->le_CockpitVoiceRoomCom2->setStyleSheet(vrStyle2);

        // display URL if not override mode
        if (!this->ui->cb_CockpitVoiceRoom1Override->isChecked())
        {
            // no override
            QString s = room1.getVoiceRoomUrl();
            this->ui->le_CockpitVoiceRoomCom1->setText(s);
        }

        // display URL if not override mode
        if (!this->ui->cb_CockpitVoiceRoom2Override->isChecked())
        {
            // no overrride
            QString s = room2.getVoiceRoomUrl();
            this->ui->le_CockpitVoiceRoomCom2->setText(s);
        }
    }

    // update some other GUI elements
    this->ui->comp_TextMessages->setTabToolTip(BlackGui::CTextMessageComponent::TextMessagesCom1, com1.getFrequencyActive().valueRoundedWithUnit(3));
    this->ui->comp_TextMessages->setTabToolTip(BlackGui::CTextMessageComponent::TextMessagesCom2, com2.getFrequencyActive().valueRoundedWithUnit(3));
}

/*
 * Round the com frequency displays
 */
void MainWindow::updateComFrequencyDisplays(const CComSystem &com1, const CComSystem &com2)
{
    // do not just set! Leads to unwanted signals fired
    // only update if not focused

    if (this->m_inputFocusedWidget != ui->ds_CockpitCom1Active)
    {
        double freq = com1.getFrequencyActive().valueRounded(3);
        if (freq != this->ui->ds_CockpitCom1Active->value())
            this->ui->ds_CockpitCom1Active->setValue(freq);
    }

    if (this->m_inputFocusedWidget != ui->ds_CockpitCom2Active)
    {
        double freq = com2.getFrequencyActive().valueRounded(3);
        if (freq != this->ui->ds_CockpitCom2Active->value())
            this->ui->ds_CockpitCom2Active->setValue(freq);
    }

    if (this->m_inputFocusedWidget != ui->ds_CockpitCom1Standby)
    {
        double freq = com1.getFrequencyStandby().valueRounded(3);
        if (freq != this->ui->ds_CockpitCom1Standby->value())
            this->ui->ds_CockpitCom1Standby->setValue(freq);
    }

    if (this->m_inputFocusedWidget != ui->ds_CockpitCom2Standby)
    {
        double freq = com2.getFrequencyStandby().valueRounded(3);
        if (freq != this->ui->ds_CockpitCom2Standby->value())
            this->ui->ds_CockpitCom2Standby->setValue(freq);
    }
}

/*
 * Reset transponder mode to Standby / Charly
 */
void MainWindow::resetTransponderMode()
{
    this->ui->pb_CockpitIdent->setStyleSheet("");
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

    transponder.setTransponderMode(this->ui->cbp_CockpitTransponderMode->getSelectedTransponderMode());
    if (this->ui->cbp_CockpitTransponderMode->isIdentSelected())
    {
        // ident shall be sent for some time, then reset
        this->ui->pb_CockpitIdent->setStyleSheet("background: red");
    }

    //
    // COM units
    //
    com1.setFrequencyActiveMHz(this->ui->ds_CockpitCom1Active->value());
    com1.setFrequencyStandbyMHz(this->ui->ds_CockpitCom1Standby->value());
    com2.setFrequencyActiveMHz(this->ui->ds_CockpitCom2Active->value());
    com2.setFrequencyStandbyMHz(this->ui->ds_CockpitCom2Standby->value());
    this->updateComFrequencyDisplays(com1, com2);

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
            this->getIContextNetwork()->updateOwnCockpit(com1, com2, transponder);
            this->reloadOwnAircraft(); // also loads resolved voice rooms
            changedCockpit = true;
        }
    }

    //
    // Now with the new voice room data, really set the
    // voice rooms in the context
    //
    if (changedCockpit) this->setAudioVoiceRooms();
}

/*
 * Voice room override
 */
void MainWindow::setAudioVoiceRooms()
{
    if (!this->m_contextAudioAvailable) return;
    if (!this->m_contextNetworkAvailable) return;

    // make fields readonly if not overriding
    this->ui->le_CockpitVoiceRoomCom1->setReadOnly(!this->ui->cb_CockpitVoiceRoom1Override->isChecked());
    this->ui->le_CockpitVoiceRoomCom2->setReadOnly(!this->ui->cb_CockpitVoiceRoom2Override->isChecked());

    CVoiceRoom room1;
    CVoiceRoom room2;
    CVoiceRoomList selectedVoiceRooms = this->getIContextNetwork()->getSelectedVoiceRooms();
    Q_ASSERT(selectedVoiceRooms.size() == 2);

    if (this->ui->cb_CockpitVoiceRoom1Override->isChecked())
        room1 = CVoiceRoom(this->ui->le_CockpitVoiceRoomCom1->text().trimmed());
    else
    {
        room1 = selectedVoiceRooms[0];
        room1.setAudioPlaying(true);
        this->ui->le_CockpitVoiceRoomCom1->setText(room1.getVoiceRoomUrl());
    }

    if (this->ui->cb_CockpitVoiceRoom2Override->isChecked())
        room2 = CVoiceRoom(this->ui->le_CockpitVoiceRoomCom1->text().trimmed());
    else
    {
        room2 = selectedVoiceRooms[1];
        room2.setAudioPlaying(true);
        this->ui->le_CockpitVoiceRoomCom2->setText(room2.getVoiceRoomUrl());
    }

    // set the real voice rooms for audio output
    this->getIContextAudio()->setComVoiceRooms(room1, room2);
}

/*
 * Test SELCAL code
 */
void MainWindow::testSelcal()
{
    QString selcalCode = this->getSelcalCode();
    if (!CSelcal::isValidCode(selcalCode))
    {
        this->displayStatusMessage(
            CStatusMessage(CStatusMessage::TypeValidation, CStatusMessage::SeverityWarning, "invalid SELCAL codde"));
        return;
    }
    if (this->m_contextAudioAvailable)
    {
        CSelcal selcal(selcalCode);
        this->getIContextAudio()->playSelcalTone(selcal);
    }
    else
    {
        this->displayStatusMessage(CStatusMessage(CStatusMessage::TypeAudio, CStatusMessage::SeverityError, "No audi available"));
    }
}

/*
 * SELCAL value selected
 */
QString MainWindow::getSelcalCode() const
{
    QString selcal = this->ui->cb_CockpitSelcal1->currentText().append(this->ui->cb_CockpitSelcal2->currentText());
    return selcal;
}

/*
 * Current input focus
 */
void MainWindow::inputFocusChanged(QWidget *oldWidget, QWidget *newWidget)
{
    Q_UNUSED(oldWidget);
    this->m_inputFocusedWidget = newWidget;
}
