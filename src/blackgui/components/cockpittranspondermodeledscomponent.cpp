/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "cockpittranspondermodeledscomponent.h"
#include "blackcore/context_ownaircraft.h"
#include <QVBoxLayout>
#include <QHBoxLayout>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackCore;

namespace BlackGui
{
    namespace Components
    {
        CCockpitTransponderModeLedsComponent::CCockpitTransponderModeLedsComponent(QWidget *parent) :
            QFrame(parent),
            m_ledStandby(new CLedWidget(false, CLedWidget::Blue, CLedWidget::Black, CLedWidget::Rounded, "standby", "", LedWidth, this)),
            m_ledModes(new CLedWidget(false, CLedWidget::Green, CLedWidget::Black, CLedWidget::Rounded, "mode C", "", LedWidth, this)),
            m_ledIdent(new CLedWidget(false, CLedWidget::Yellow, CLedWidget::Black, CLedWidget::Rounded, "ident", "", LedWidth, this))
        {
            this->init(true);
        }

        void CCockpitTransponderModeLedsComponent::runtimeHasBeenSet()
        {
            Q_ASSERT(this->getIContextOwnAircraft());
            connect(this->getIContextOwnAircraft(), &IContextOwnAircraft::changedAircraftCockpit, this, &CCockpitTransponderModeLedsComponent::ps_onAircraftCockpitChanged);
            this->setMode(getOwnTransponder().getTransponderMode());
        }

        void CCockpitTransponderModeLedsComponent::ps_onAircraftCockpitChanged(const CAircraft &aircraft, const BlackMisc::COriginator &originator)
        {
            if (ledsOriginator() == originator) { return; }
            this->setMode(aircraft.getTransponderMode());
        }

        void CCockpitTransponderModeLedsComponent::ps_onLedClicked()
        {
            QWidget *w = qobject_cast<QWidget *>(QObject::sender());
            if (!w) { return; }
            if (!this->getIContextOwnAircraft()) { return; }
            CTransponder::TransponderMode mode;
            if (this->m_ledStandby.data() == w)
            {
                mode = CTransponder::StateStandby;
            }
            else if (this->m_ledIdent.data() == w)
            {
                mode = CTransponder::StateIdent;
            }
            else if (this->m_ledModes.data() == w)
            {
                mode = CTransponder::ModeC;
            }
            else
            {
                return;
            }
            CAircraft ownAircraft = this->getOwnAircraft();
            if (ownAircraft.getTransponderMode() == mode) { return; }

            this->setMode(mode);
            CTransponder xpdr = ownAircraft.getTransponder();
            xpdr.setTransponderMode(mode);
            this->getIContextOwnAircraft()->updateCockpit(ownAircraft.getCom1System(), ownAircraft.getCom2System(), xpdr, ledsOriginator());
        }

        void CCockpitTransponderModeLedsComponent::init(bool horizontal)
        {
            QBoxLayout *ledLayout = nullptr;
            if (horizontal) { ledLayout = new QHBoxLayout(this); }
            else { ledLayout = new QVBoxLayout(this); }

            ledLayout->setMargin(0);
            ledLayout->addWidget(m_ledStandby.data());
            ledLayout->addWidget(m_ledModes.data());
            ledLayout->addWidget(m_ledIdent.data());
            connect(this->m_ledIdent.data(), &CLedWidget::clicked, this, &CCockpitTransponderModeLedsComponent::ps_onLedClicked);
            connect(this->m_ledModes.data(), &CLedWidget::clicked, this, &CCockpitTransponderModeLedsComponent::ps_onLedClicked);
            connect(this->m_ledStandby.data(), &CLedWidget::clicked, this, &CCockpitTransponderModeLedsComponent::ps_onLedClicked);
            this->setLayout(ledLayout);

            // if context is already available set mode
            if (this->getIContextOwnAircraft()) { this->setMode(getOwnTransponder().getTransponderMode()); }
        }

        void CCockpitTransponderModeLedsComponent::setMode(BlackMisc::Aviation::CTransponder::TransponderMode mode)
        {
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
            Q_ASSERT(getIContextOwnAircraft());
            return getIContextOwnAircraft()->getOwnAircraft().getTransponder();
        }

        CAircraft CCockpitTransponderModeLedsComponent::getOwnAircraft() const
        {
            Q_ASSERT(getIContextOwnAircraft());
            return getIContextOwnAircraft()->getOwnAircraft();
        }

        BlackMisc::COriginator CCockpitTransponderModeLedsComponent::ledsOriginator()
        {
            if (m_originator.getName().isEmpty())
                m_originator = COriginator(QStringLiteral("XPDRLEDSCOMCOMPONENT"));

            return m_originator;
        }

    } // namespace
} // namespace
