#include "cockpitv1component.h"
#include "ui_cockpitv1component.h"

#include "blackgui/atcstationlistmodel.h"
#include "blackcore/context_ownaircraft.h"
#include "blackcore/context_ownaircraft_impl.h"
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


namespace BlackGui
{
    CCockpitV1Component::CCockpitV1Component(QWidget *parent) :
        QWidget(parent), CRuntimeBasedComponent(nullptr, false), ui(new Ui::CCockpitV1Component), pb_ExternalCockpitIdent(nullptr), pb_ExternalCockpitSelected(nullptr)
    {
        ui->setupUi(this);

        // init encapsulated table views / models
        this->ui->tvp_CockpitVoiceRoom1->setUserMode(CUserListModel::UserShort);
        this->ui->tvp_CockpitVoiceRoom2->setUserMode(CUserListModel::UserShort);

        // SELCAL pairs in cockpit
        this->ui->cb_CockpitSelcal1->clear();
        this->ui->cb_CockpitSelcal2->clear();
        this->ui->cb_CockpitSelcal1->addItems(BlackMisc::Aviation::CSelcal::codePairs());
        this->ui->cb_CockpitSelcal2->addItems(BlackMisc::Aviation::CSelcal::codePairs());


        // cockpit
        bool connected = this->connect(this->ui->cbp_CockpitTransponderMode, SIGNAL(currentIndexChanged(QString)), this, SLOT(cockpitValuesChanged()));
        Q_ASSERT(connected);
        this->connect(this->ui->ds_CockpitCom1Active, &QDoubleSpinBox::editingFinished, this, &CCockpitV1Component::cockpitValuesChanged);
        this->connect(this->ui->ds_CockpitCom2Active, &QDoubleSpinBox::editingFinished, this, &CCockpitV1Component::cockpitValuesChanged);
        this->connect(this->ui->ds_CockpitCom1Standby, &QDoubleSpinBox::editingFinished, this, &CCockpitV1Component::cockpitValuesChanged);
        this->connect(this->ui->ds_CockpitCom2Standby, &QDoubleSpinBox::editingFinished, this, &CCockpitV1Component::cockpitValuesChanged);
        this->connect(this->ui->ds_CockpitTransponder, &QDoubleSpinBox::editingFinished, this, &CCockpitV1Component::cockpitValuesChanged);

        this->connect(this->ui->cb_CockpitVoiceRoom1Override, &QCheckBox::clicked, this, &CCockpitV1Component::setAudioVoiceRooms);
        this->connect(this->ui->cb_CockpitVoiceRoom2Override, &QCheckBox::clicked, this, &CCockpitV1Component::setAudioVoiceRooms);
        this->connect(this->ui->le_CockpitVoiceRoomCom1, &QLineEdit::returnPressed, this, &CCockpitV1Component::setAudioVoiceRooms);
        this->connect(this->ui->le_CockpitVoiceRoomCom2, &QLineEdit::returnPressed, this, &CCockpitV1Component::setAudioVoiceRooms);
        this->connect(this->ui->pb_CockpitToggleCom1, &QPushButton::clicked, this, &CCockpitV1Component::cockpitValuesChanged);
        this->connect(this->ui->pb_CockpitToggleCom2, &QPushButton::clicked, this, &CCockpitV1Component::cockpitValuesChanged);
        this->connect(this->ui->pb_CockpitSelcalTest, &QPushButton::clicked, this, &CCockpitV1Component::testSelcal);
        this->connect(this->ui->cbp_CockpitTransponderMode, &CTransponderModeSelector::identEnded, this, &CCockpitV1Component::resetTransponderMode);

        Q_UNUSED(connected);
    }

    CCockpitV1Component::~CCockpitV1Component()
    {
        delete ui;
    }

    void CCockpitV1Component::setExternalButtons(QPushButton *cockpitIdent, QPushButton *cockpitSelected)
    {
        if (this->pb_ExternalCockpitIdent) disconnect(this->pb_ExternalCockpitIdent);

        this->pb_ExternalCockpitIdent = cockpitIdent;
        this->pb_ExternalCockpitSelected = cockpitSelected;

        if (this->pb_ExternalCockpitIdent)
            this->connect(this->pb_ExternalCockpitIdent, &QPushButton::clicked, this, &CCockpitV1Component::cockpitValuesChanged);
    }

    const QString &CCockpitV1Component::cockpitOriginator()
    {
        static const QString o("cockpit v1");
        return o;
    }

    CAircraft CCockpitV1Component::getOwnAircraft() const
    {
        Q_ASSERT(this->getIContextOwnAircraft());
        // direct object from local context
        if (this->getIContextOwnAircraft()->usingLocalObjects()) return this->getRuntime()->getCContextOwnAircraft()->ownAircraft();

        // non local
        if (this->canPingApplicationContext()) return this->getIContextOwnAircraft()->getOwnAircraft();
        return this->m_ownAircraft;
    }

    /*
     * Cockpit values
     */
    void CCockpitV1Component::cockpitValuesChanged()
    {
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
        else if (this->pb_ExternalCockpitIdent && sender == this->pb_ExternalCockpitSelected)
        {
            // trigger the real button
            if (this->ui->cbp_CockpitTransponderMode->isIdentSelected())
            {
                this->pb_ExternalCockpitIdent->setStyleSheet("");
                this->ui->cbp_CockpitTransponderMode->resetTransponderMode();
            }
            else
            {
                this->pb_ExternalCockpitIdent->setStyleSheet("background: red");
                this->ui->cbp_CockpitTransponderMode->setSelectedTransponderModeStateIdent(); // trigger real button and whole process
            }
            return;
        }

    }

    /*
     * Own cockpit
     */
    void CCockpitV1Component::updateCockpitFromContext(const CAircraft &ownAircraft, const QString &originator)
    {
        Q_UNUSED(originator);

        // update GUI elements
        // avoid unnecessary change events as far as possible
        const CComSystem com1 = ownAircraft.getCom1System(); // aircraft just updated or set from context
        const CComSystem com2 = ownAircraft.getCom2System();
        const CTransponder transponder = ownAircraft.getTransponder();

        // update the frequencies
        this->updateComFrequencyDisplays(com1, com2);

        // update transponder if this is not input focused
        qint32 tc = transponder.getTransponderCode();
        if (tc != static_cast<qint32>(this->ui->ds_CockpitTransponder->value()))
            this->ui->ds_CockpitTransponder->setValue(tc);

        this->ui->cbp_CockpitTransponderMode->setSelectedTransponderMode(transponder.getTransponderMode());

        if (this->getIContextNetwork())
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
        if (this->getIContextAudio())
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
    }

    /*
     * Round the com frequency displays
     */
    void CCockpitV1Component::updateComFrequencyDisplays(const CComSystem &com1, const CComSystem &com2)
    {
        // do not just set! Leads to unwanted signals fired
        // only update if not focused

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
    }

    /*
     * Reset transponder mode to Standby / Charly
     */
    void CCockpitV1Component::resetTransponderMode()
    {
        if (!this->pb_ExternalCockpitIdent) return;
        this->pb_ExternalCockpitIdent->setStyleSheet("");
    }

    /*
     * Send cockpit updates
     */
    void CCockpitV1Component::sendCockpitUpdates()
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
            this->sendStatusMessage(CStatusMessage::getValidationError("Wrong transponder code, reset"));
            this->ui->ds_CockpitTransponder->setValue(transponder.getTransponderCode());
        }

        transponder.setTransponderMode(this->ui->cbp_CockpitTransponderMode->getSelectedTransponderMode());
        if (this->pb_ExternalCockpitIdent && this->ui->cbp_CockpitTransponderMode->isIdentSelected())
        {
            // ident shall be sent for some time, then reset
            this->pb_ExternalCockpitIdent->setStyleSheet("background: red");
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
        if (this->getIContextOwnAircraft())
        {
            CAircraft ownAircraft = this->getOwnAircraft();
            if (!ownAircraft.hasSameComData(com1, com2, transponder))
            {
                this->getIContextOwnAircraft()->updateOwnCockpit(com1, com2, transponder, CCockpitV1Component::cockpitOriginator());
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
    void CCockpitV1Component::setAudioVoiceRooms()
    {
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
    void CCockpitV1Component::testSelcal()
    {
        QString selcalCode = this->getSelcalCode();
        if (!CSelcal::isValidCode(selcalCode))
        {
            this->sendStatusMessage(CStatusMessage(CStatusMessage::TypeValidation, CStatusMessage::SeverityWarning, "invalid SELCAL codde"));
        }
        else if (this->getIContextAudio())
        {
            CSelcal selcal(selcalCode);
            this->getIContextAudio()->playSelcalTone(selcal);
        }
        else
        {
            this->sendStatusMessage(CStatusMessage(CStatusMessage::TypeAudio, CStatusMessage::SeverityError, "No audi available"));
        }
    }

    /*
     * SELCAL value selected
     */
    QString CCockpitV1Component::getSelcalCode() const
    {
        QString selcal = this->ui->cb_CockpitSelcal1->currentText().append(this->ui->cb_CockpitSelcal2->currentText());
        return selcal;
    }

} // guard
