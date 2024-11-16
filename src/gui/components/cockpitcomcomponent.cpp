// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/components/cockpitcomcomponent.h"

#include <QDoubleSpinBox>
#include <QLabel>
#include <QPushButton>
#include <QString>
#include <QtGlobal>

#include "ui_cockpitcomcomponent.h"

#include "core/context/contextaudio.h"
#include "core/context/contextnetwork.h"
#include "core/context/contextownaircraft.h"
#include "core/context/contextsimulator.h"
#include "gui/components/selcalcodeselector.h"
#include "gui/components/transpondercodespinbox.h"
#include "gui/components/transpondermodeselector.h"
#include "gui/guiapplication.h"
#include "gui/led.h"
#include "gui/stylesheetutility.h"
#include "misc/aviation/atcstation.h"
#include "misc/aviation/atcstationlist.h"
#include "misc/aviation/callsign.h"
#include "misc/aviation/comsystem.h"
#include "misc/logmessage.h"
#include "misc/pq/frequency.h"
#include "misc/pq/units.h"
#include "misc/sequence.h"
#include "misc/verify.h"

using namespace swift::gui;
using namespace swift::gui::editors;
using namespace swift::core;
using namespace swift::core::context;
using namespace swift::misc;
using namespace swift::misc::audio;
using namespace swift::misc::aviation;
using namespace swift::misc::simulation;
using namespace swift::misc::physical_quantities;

namespace swift::gui::components
{
    CCockpitComComponent::CCockpitComComponent(QWidget *parent) : QFrame(parent),
                                                                  CIdentifiable(this),
                                                                  ui(new Ui::CCockpitComComponent)
    {
        ui->setupUi(this);

        // init from aircraft
        this->forceCockpitUpdateFromOwnAircraftContext();

        // COM form
        connect(ui->editor_Com, &CCockpitComForm::testSelcal, this, &CCockpitComComponent::testSelcal);
        connect(ui->editor_Com, &CCockpitComForm::changedCockpitValues, this, &CCockpitComComponent::updateOwnCockpitInContext);
        connect(ui->editor_Com, &CCockpitComForm::changedSelcal, this, &CCockpitComComponent::updateSelcalInContext);
        connect(ui->editor_Com, &CCockpitComForm::requestCom1TextMessage, this, &CCockpitComComponent::requestCom1TextMessage);
        connect(ui->editor_Com, &CCockpitComForm::requestCom2TextMessage, this, &CCockpitComComponent::requestCom2TextMessage);

        // Relay COM form signals
        connect(ui->editor_Com, &CCockpitComForm::transponderModeChanged, this, &CCockpitComComponent::transponderModeChanged);
        connect(ui->editor_Com, &CCockpitComForm::transponderStateIdentEnded, this, &CCockpitComComponent::transponderStateIdentEnded);

        // hook up with changes from own aircraft context
        if (sGui)
        {
            // own aircraft
            connect(sGui->getIContextOwnAircraft(), &IContextOwnAircraft::changedAircraftCockpit, this, &CCockpitComComponent::updateCockpitFromContext, Qt::QueuedConnection);
            connect(sGui->getIContextOwnAircraft(), &IContextOwnAircraft::changedSelcal, this, &CCockpitComComponent::updateSelcalFromContext, Qt::QueuedConnection);

            // hook up with audio context
            // connect(sGui->getIContextAudio(), &IContextAudio::changedVoiceRooms, this, &CCockpitComComponent::updateVoiceRoomStatusFromContext, Qt::QueuedConnection);

            // network
            connect(sGui->getIContextNetwork(), &IContextNetwork::changedAtcStationsOnlineDigest, this, &CCockpitComComponent::onAtcStationsChanged, Qt::QueuedConnection);

            QPointer<CCockpitComComponent> myself(this);
            QTimer::singleShot(10 * 1000, this, [=] {
                if (!sGui || sGui->isShuttingDown() || !myself) { return; }
                const CSimulatedAircraft aircraft = sGui->getIContextOwnAircraft()->getOwnAircraft();
                this->updateCockpitFromContext(aircraft, CIdentifier::fake());
            });
        }
    }

    CCockpitComComponent::~CCockpitComComponent()
    {}

    void CCockpitComComponent::setTransponderModeStateIdent()
    {
        ui->editor_Com->setTransponderModeStateIdent();
    }

    void CCockpitComComponent::paintEvent(QPaintEvent *event)
    {
        Q_UNUSED(event)
        CStyleSheetUtility::useStyleSheetInDerivedWidget(this);
    }

    void CCockpitComComponent::updateCockpitFromContext(const CSimulatedAircraft &ownAircraft, const CIdentifier &originator)
    {
        if (isMyIdentifier(originator)) { return; } // comes from myself

        // update GUI elements
        // avoid unnecessary change events as far as possible
        const CComSystem com1 = ownAircraft.getCom1System(); // aircraft just updated or set from context
        const CComSystem com2 = ownAircraft.getCom2System();
        const CTransponder transponder = ownAircraft.getTransponder();

        // update the frequencies
        ui->editor_Com->setFrequencies(com1, com2);

        // update transponder
        ui->editor_Com->setTransponder(transponder);
    }

    void CCockpitComComponent::testSelcal()
    {
        Q_ASSERT_X(sGui, Q_FUNC_INFO, "Need sGui");
        const CSelcal selcal = ui->editor_Com->getSelcal();
        if (!selcal.isValid())
        {
            CLogMessage().validationWarning(u"Invalid SELCAL code");
        }
        else if (sGui->getCContextAudioBase())
        {
            sGui->getCContextAudioBase()->playSelcalTone(selcal);
        }
        else
        {
            CLogMessage().validationWarning(u"No audio available");
        }
    }

    void CCockpitComComponent::updateSelcalFromContext(const CSelcal &selcal, const CIdentifier &originator)
    {
        if (isMyIdentifier(originator)) { return; } // comes from myself
        ui->editor_Com->setSelcal(selcal);
    }

    CSimulatedAircraft CCockpitComComponent::getOwnAircraft() const
    {
        // unavailable context during shutdown possible
        // mostly when client runs with DBus, but DBus is down
        if (!sGui || sGui->isShuttingDown() || !sGui->getIContextOwnAircraft()) { return CSimulatedAircraft(); }
        return sGui->getIContextOwnAircraft()->getOwnAircraft();
    }

    bool CCockpitComComponent::updateOwnCockpitInContext(const CSimulatedAircraft &ownAircraft)
    {
        // unavailable context during shutdown possible
        // mostly when client runs with DBus, but DBus is down
        if (!sGui || sGui->isShuttingDown() || !sGui->getIContextOwnAircraft()) { return false; }
        return sGui->getIContextOwnAircraft()->updateCockpit(ownAircraft.getCom1System(), ownAircraft.getCom2System(), ownAircraft.getTransponder(), identifier());
    }

    void CCockpitComComponent::forceCockpitUpdateFromOwnAircraftContext()
    {
        if (!sGui || sGui->isShuttingDown()) { return; }
        const CSimulatedAircraft ownAircraft = this->getOwnAircraft();
        this->updateCockpitFromContext(ownAircraft, CIdentifier("dummyInitialValues")); // intentionally different name here
    }

    void CCockpitComComponent::onAtcStationsChanged()
    {
        // void
    }

    void CCockpitComComponent::updateSelcalInContext(const CSelcal &selcal)
    {
        if (!sGui || sGui->isShuttingDown() || !sGui->getIContextOwnAircraft()) { return; }
        sGui->getIContextOwnAircraft()->updateSelcal(selcal, identifier());
    }
} // namespace swift::gui::components
