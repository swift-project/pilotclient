// SPDX-FileCopyrightText: Copyright (C) 2013 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackcore/context/contextownaircraft.h"
#include "blackgui/components/cockpittranspondermodeledscomponent.h"
#include "blackgui/guiapplication.h"
#include "blackgui/led.h"

#include <QBoxLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>

using namespace swift::misc;
using namespace swift::misc::aviation;
using namespace swift::misc::simulation;
using namespace BlackCore;
using namespace BlackCore::Context;

namespace BlackGui::Components
{
    CCockpitTransponderModeLedsComponent::CCockpitTransponderModeLedsComponent(QWidget *parent) : QFrame(parent),
                                                                                                  CIdentifiable(this),
                                                                                                  m_ledStandby(new CLedWidget(false, CLedWidget::Blue, CLedWidget::Black, CLedWidget::Rounded, "standby", "", LedWidth, this)),
                                                                                                  m_ledModes(new CLedWidget(false, CLedWidget::Green, CLedWidget::Black, CLedWidget::Rounded, "mode C", "", LedWidth, this)),
                                                                                                  m_ledIdent(new CLedWidget(false, CLedWidget::Yellow, CLedWidget::Black, CLedWidget::Rounded, "ident", "", LedWidth, this))
    {
        this->init(true);

        Q_ASSERT_X(sGui, Q_FUNC_INFO, "Need sGui");
        connect(sGui->getIContextOwnAircraft(), &IContextOwnAircraft::changedAircraftCockpit, this, &CCockpitTransponderModeLedsComponent::onAircraftCockpitChanged);
    }

    void CCockpitTransponderModeLedsComponent::onAircraftCockpitChanged(const CSimulatedAircraft &aircraft, const swift::misc::CIdentifier &originator)
    {
        if (isMyIdentifier(originator)) { return; }
        this->setMode(aircraft.getTransponderMode(), true);
    }

    void CCockpitTransponderModeLedsComponent::onLedClicked()
    {
        QWidget *w = qobject_cast<QWidget *>(QObject::sender());
        if (!w) { return; }
        if (!sGui || sGui->isShuttingDown() || !sGui->getIContextOwnAircraft()) { return; }
        CTransponder::TransponderMode mode;
        if (m_ledStandby.data() == w)
        {
            mode = CTransponder::StateStandby;
        }
        else if (m_ledIdent.data() == w)
        {
            mode = CTransponder::StateIdent;
        }
        else if (m_ledModes.data() == w)
        {
            mode = CTransponder::ModeC;
        }
        else
        {
            return;
        }
        CSimulatedAircraft ownAircraft(this->getOwnAircraft());
        if (ownAircraft.getTransponderMode() == mode) { return; }

        this->setMode(mode, true);
        CTransponder xpdr = ownAircraft.getTransponder();
        xpdr.setTransponderMode(mode);

        if (sGui)
        {
            sGui->getIContextOwnAircraft()->updateCockpit(ownAircraft.getCom1System(), ownAircraft.getCom2System(), xpdr, identifier());
        }
    }

    void CCockpitTransponderModeLedsComponent::init(bool horizontal)
    {
        QBoxLayout *ledLayout = nullptr;
        if (horizontal) { ledLayout = new QHBoxLayout(this); }
        else { ledLayout = new QVBoxLayout(this); }

        ledLayout->setContentsMargins(0, 0, 0, 0);
        ledLayout->addWidget(m_ledStandby.data());
        ledLayout->addWidget(m_ledModes.data());
        ledLayout->addWidget(m_ledIdent.data());
        connect(m_ledIdent.data(), &CLedWidget::clicked, this, &CCockpitTransponderModeLedsComponent::onLedClicked);
        connect(m_ledModes.data(), &CLedWidget::clicked, this, &CCockpitTransponderModeLedsComponent::onLedClicked);
        connect(m_ledStandby.data(), &CLedWidget::clicked, this, &CCockpitTransponderModeLedsComponent::onLedClicked);
        this->setLayout(ledLayout);

        // if context is already available set mode
        if (sGui && sGui->getIContextOwnAircraft()) { this->setMode(getOwnTransponder().getTransponderMode(), true); }
    }

    void CCockpitTransponderModeLedsComponent::setMode(CTransponder::TransponderMode mode, bool force)
    {
        if (!force && m_mode == mode) { return; }
        m_ledStandby->setOn(false);
        m_ledModes->setOn(false);
        m_ledIdent->setOn(false);

        switch (mode)
        {
        case CTransponder::ModeA:
        case CTransponder::ModeC:
        case CTransponder::ModeS:
        case CTransponder::ModeMil1:
        case CTransponder::ModeMil2:
        case CTransponder::ModeMil3:
        case CTransponder::ModeMil4:
        case CTransponder::ModeMil5:
            m_ledModes->setOn(true);
            break;
        case CTransponder::StateIdent:
            m_ledModes->setOn(true);
            m_ledIdent->setOn(true);
            break;
        default:
        case CTransponder::StateStandby:
            m_ledStandby->setOn(true);
            break;
        }
    }

    CTransponder CCockpitTransponderModeLedsComponent::getOwnTransponder() const
    {
        if (!sGui || sGui->isShuttingDown() || !sGui->getIContextOwnAircraft()) { return CTransponder(); }
        return sGui->getIContextOwnAircraft()->getOwnAircraft().getTransponder();
    }

    CSimulatedAircraft CCockpitTransponderModeLedsComponent::getOwnAircraft() const
    {
        if (!sGui || sGui->isShuttingDown() || !sGui->getIContextOwnAircraft()) { return CSimulatedAircraft(); }
        return sGui->getIContextOwnAircraft()->getOwnAircraft();
    }
} // namespace
