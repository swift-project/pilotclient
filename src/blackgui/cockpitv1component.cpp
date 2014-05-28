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
        QWidget(parent), CRuntimeBasedComponent(nullptr, false), ui(new Ui::CCockpitV1Component), m_externalCockpitIdentButton(nullptr), m_voiceRoomMembersTimer(nullptr)
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

        // cockpit GUI events
        this->connect(this->ui->ds_CockpitCom1Active, &QDoubleSpinBox::editingFinished, this, &CCockpitV1Component::cockpitValuesChanged);
        this->connect(this->ui->ds_CockpitCom2Active, &QDoubleSpinBox::editingFinished, this, &CCockpitV1Component::cockpitValuesChanged);
        this->connect(this->ui->ds_CockpitCom1Standby, &QDoubleSpinBox::editingFinished, this, &CCockpitV1Component::cockpitValuesChanged);
        this->connect(this->ui->ds_CockpitCom2Standby, &QDoubleSpinBox::editingFinished, this, &CCockpitV1Component::cockpitValuesChanged);
        this->connect(this->ui->ds_CockpitTransponder, &QDoubleSpinBox::editingFinished, this, &CCockpitV1Component::cockpitValuesChanged);

        this->connect(this->ui->cb_CockpitVoiceRoom1Override, &QCheckBox::clicked, this, &CCockpitV1Component::setAudioVoiceRoomUrls);
        this->connect(this->ui->cb_CockpitVoiceRoom2Override, &QCheckBox::clicked, this, &CCockpitV1Component::setAudioVoiceRoomUrls);
        this->connect(this->ui->le_CockpitVoiceRoomCom1, &QLineEdit::returnPressed, this, &CCockpitV1Component::setAudioVoiceRoomUrls);
        this->connect(this->ui->le_CockpitVoiceRoomCom2, &QLineEdit::returnPressed, this, &CCockpitV1Component::setAudioVoiceRoomUrls);
        this->connect(this->ui->pb_CockpitToggleCom1, &QPushButton::clicked, this, &CCockpitV1Component::cockpitValuesChanged);
        this->connect(this->ui->pb_CockpitToggleCom2, &QPushButton::clicked, this, &CCockpitV1Component::cockpitValuesChanged);
        this->connect(this->ui->pb_CockpitSelcalTest, &QPushButton::clicked, this, &CCockpitV1Component::testSelcal);

        this->connect(this->ui->cbp_CockpitTransponderMode, &CTransponderModeSelector::valueChanged, this, &CCockpitV1Component::cockpitValuesChanged);

        this->connect(this->ui->di_CockpitCom1Volume, &QDial::valueChanged, this, &CCockpitV1Component::setCom1Volume);
        this->connect(this->ui->di_CockpitCom2Volume, &QDial::valueChanged, this, &CCockpitV1Component::setCom2Volume);

        // timer
        this->m_voiceRoomMembersTimer  = new QTimer(this);
        this->connect(this->m_voiceRoomMembersTimer,  &QTimer::timeout, this, &CCockpitV1Component::updateVoiceRoomMembers);
        this->m_voiceRoomMembersTimer->start(10 * 1000);
    }

    void CCockpitV1Component::runtimeHasBeenSet()
    {
        // hook up with changes from own aircraft context
        Q_ASSERT(this->getIContextOwnAircraft());
        this->connect(this->getIContextOwnAircraft(), &IContextOwnAircraft::changedAircraftCockpit, this, &CCockpitV1Component::updateCockpitFromContext);

        // Audio is optional
        if (this->getIContextAudio())
        {
            this->connect(this->getIContextAudio(), &IContextAudio::changedVoiceRooms, this, &CCockpitV1Component::updateAudioVoiceRoomsFromObject);
        }
    }

    CCockpitV1Component::~CCockpitV1Component()
    {
        delete ui;
    }

    void CCockpitV1Component::setExternalIdentButton(QPushButton *cockpitIdent)
    {
        if (this->m_externalCockpitIdentButton) disconnect(this->m_externalCockpitIdentButton);
        this->m_externalCockpitIdentButton = cockpitIdent;
        if (this->m_externalCockpitIdentButton) this->connect(this->m_externalCockpitIdentButton, &QPushButton::clicked, this, &CCockpitV1Component::cockpitValuesChanged);
    }

    void CCockpitV1Component::setCom1Volume(int volume)
    {
        if (volume > 100) volume = 100;
        if (volume < 0) volume = 0;
        if (QObject::sender() != ui->di_CockpitCom1Volume)
            this->ui->di_CockpitCom1Volume->setValue(volume);
        this->getIContextOwnAircraft()->setAudioOutputVolumes(volume, this->ui->di_CockpitCom2Volume->value());
        emit this->audioVolumeChanged();
    }

    void CCockpitV1Component::setCom2Volume(int volume)
    {
        if (volume > 100) volume = 100;
        if (volume < 0) volume = 0;
        if (QObject::sender() != ui->di_CockpitCom2Volume)
            this->ui->di_CockpitCom2Volume->setValue(volume);
        this->getIContextOwnAircraft()->setAudioOutputVolumes(this->ui->di_CockpitCom1Volume->value(), volume);
        emit this->audioVolumeChanged();
    }

    int CCockpitV1Component::getCom1Volume() const
    {
        return this->ui->di_CockpitCom1Volume->value();
    }

    int CCockpitV1Component::getCom2Volume() const
    {
        return this->ui->di_CockpitCom2Volume->value();
    }

    void CCockpitV1Component::setCockpitVoiceStatusPixmap(const QPixmap &pixmap)
    {
        this->ui->lbl_CockpitVoiceStatus->setPixmap(pixmap);
    }

    bool CCockpitV1Component::isCockpitVolumeWidget(const QObject *sender) const
    {
        return
            sender == this->ui->di_CockpitCom1Volume ||
            sender == this->ui->di_CockpitCom2Volume;
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
        return CAircraft(); // anything better here, or status?
    }

    void CCockpitV1Component::cockpitValuesChanged()
    {
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
        else if (sender == this->ui->cbp_CockpitTransponderMode)
        {
            // toggle the external button
            if (this->ui->cbp_CockpitTransponderMode->isIdentSelected())
            {
                if (this->m_externalCockpitIdentButton) this->m_externalCockpitIdentButton->setStyleSheet("background: red");
            }
            else
            {
                if (this->m_externalCockpitIdentButton) this->m_externalCockpitIdentButton->setStyleSheet("");
            }
        }
        else if (sender == this->m_externalCockpitIdentButton)
        {
            // toggle the combo box
            if (this->ui->cbp_CockpitTransponderMode->isIdentSelected())
            {
                this->ui->cbp_CockpitTransponderMode->resetTransponderMode();
            }
            else
            {
                this->ui->cbp_CockpitTransponderMode->setSelectedTransponderModeStateIdent(); // trigger real button and whole process
            }
        }

        CAircraft ownAircraft = this->cockpitValuesToObject();
        this->sendCockpitUpdates(ownAircraft);
    }

    CAircraft CCockpitV1Component::cockpitValuesToObject()
    {

        CAircraft ownAircraft = this->getOwnAircraft();
        CTransponder transponder = ownAircraft.getTransponder();
        CComSystem com1 = ownAircraft.getCom1System();
        CComSystem com2 = ownAircraft.getCom2System();

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

        //
        // COM units
        //
        com1.setFrequencyActiveMHz(this->ui->ds_CockpitCom1Active->value());
        com1.setFrequencyStandbyMHz(this->ui->ds_CockpitCom1Standby->value());
        com2.setFrequencyActiveMHz(this->ui->ds_CockpitCom2Active->value());
        com2.setFrequencyStandbyMHz(this->ui->ds_CockpitCom2Standby->value());
        this->updateComFrequencyDisplaysFromObjects(com1, com2); // back annotation after rounding

        ownAircraft.setCom1System(com1);
        ownAircraft.setCom2System(com2);
        ownAircraft.setTransponder(transponder);
        return ownAircraft;
    }

    void CCockpitV1Component::updateComFrequencyDisplaysFromObjects(const CComSystem &com1, const CComSystem &com2)
    {
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

    void CCockpitV1Component::updateCockpitFromObject(const CAircraft &ownAircraft)
    {
        // update GUI elements
        // avoid unnecessary change events as far as possible
        const CComSystem com1 = ownAircraft.getCom1System(); // aircraft just updated or set from context
        const CComSystem com2 = ownAircraft.getCom2System();
        const CTransponder transponder = ownAircraft.getTransponder();

        // update the frequencies
        this->updateComFrequencyDisplaysFromObjects(com1, com2);

        // update transponder
        qint32 tc = transponder.getTransponderCode();
        if (tc != static_cast<qint32>(this->ui->ds_CockpitTransponder->value()))
            this->ui->ds_CockpitTransponder->setValue(tc);

        this->ui->cbp_CockpitTransponderMode->setSelectedTransponderMode(transponder.getTransponderMode());
    }

    void CCockpitV1Component::updateCockpitFromContext(const CAircraft &ownAircraft, const QString &originator)
    {
        if (originator == CCockpitV1Component::cockpitOriginator()) return; // comes from myself

        // update GUI elements
        // avoid unnecessary change events as far as possible
        const CComSystem com1 = ownAircraft.getCom1System(); // aircraft just updated or set from context
        const CComSystem com2 = ownAircraft.getCom2System();
        const CTransponder transponder = ownAircraft.getTransponder();

        // update the frequencies
        this->updateComFrequencyDisplaysFromObjects(com1, com2);

        // update transponder
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
    }

    bool CCockpitV1Component::sendCockpitUpdates(const CAircraft &ownAircraft)
    {
        //
        // Send to context
        //
        bool changedCockpit = false;
        if (this->getIContextOwnAircraft())
        {
            changedCockpit =  this->getIContextOwnAircraft()->updateOwnCockpit(ownAircraft.getCom1System(), ownAircraft.getCom2System(), ownAircraft.getTransponder(), CCockpitV1Component::cockpitOriginator());
        }
        return changedCockpit;
    }

    void CCockpitV1Component::setAudioVoiceRoomUrls()
    {
        Q_ASSERT(this->getIContextOwnAircraft());

        // make fields readonly if not overriding
        this->ui->le_CockpitVoiceRoomCom1->setReadOnly(!this->ui->cb_CockpitVoiceRoom1Override->isChecked());
        this->ui->le_CockpitVoiceRoomCom2->setReadOnly(!this->ui->cb_CockpitVoiceRoom2Override->isChecked());

        QString room1;
        QString room2;
        if (this->ui->cb_CockpitVoiceRoom1Override->isChecked()) room1 = ui->le_CockpitVoiceRoomCom1->text();
        if (this->ui->cb_CockpitVoiceRoom2Override->isChecked()) room2 = ui->le_CockpitVoiceRoomCom2->text();
        this->getIContextOwnAircraft()->setAudioVoiceRoomOverrideUrls(room1, room2);
    }

    void CCockpitV1Component::updateAudioVoiceRoomsFromObject(const CVoiceRoomList &selectedVoiceRooms)
    {
        Q_ASSERT(selectedVoiceRooms.size() == 2);
        CVoiceRoom room1 = selectedVoiceRooms[0];
        CVoiceRoom room2 = selectedVoiceRooms[1];

        // remark
        // isAudioPlaying() is not set, as this is only a temporary value when really "something is playing"

        this->ui->le_CockpitVoiceRoomCom1->setText(room1.getVoiceRoomUrl());
        if (room1.isConnected())
        {
            this->ui->le_CockpitVoiceRoomCom1->setStyleSheet("background: green");
            if (this->getIContextAudio()) this->ui->tvp_CockpitVoiceRoom1->update(this->getIContextAudio()->getCom1RoomUsers());
        }
        else
        {
            this->ui->le_CockpitVoiceRoomCom1->setStyleSheet("");
            this->ui->tvp_CockpitVoiceRoom1->clear();
        }

        this->ui->le_CockpitVoiceRoomCom2->setText(room2.getVoiceRoomUrl());
        if (room2.isConnected())
        {
            this->ui->le_CockpitVoiceRoomCom2->setStyleSheet("background: green");
        }
        else
        {
            this->ui->le_CockpitVoiceRoomCom2->setStyleSheet("");
            this->ui->tvp_CockpitVoiceRoom2->clear();
        }
    }

    void CCockpitV1Component::updateVoiceRoomMembers()
    {
        if (!this->getIContextAudio()) return;
        if (!this->ui->le_CockpitVoiceRoomCom1->text().trimmed().isEmpty())
            this->ui->tvp_CockpitVoiceRoom1->update(this->getIContextAudio()->getCom1RoomUsers());
        else
            this->ui->tvp_CockpitVoiceRoom1->clear();

        if (!this->ui->le_CockpitVoiceRoomCom2->text().trimmed().isEmpty())
            this->ui->tvp_CockpitVoiceRoom2->update(this->getIContextAudio()->getCom2RoomUsers());
        else
            this->ui->tvp_CockpitVoiceRoom2->clear();

    }

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

    QString CCockpitV1Component::getSelcalCode() const
    {
        QString selcal = this->ui->cb_CockpitSelcal1->currentText().append(this->ui->cb_CockpitSelcal2->currentText());
        return selcal;
    }

} // guard
