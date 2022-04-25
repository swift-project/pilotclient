/* Copyright (C) 2019
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "transpondermodecomponent.h"
#include "ui_transpondermodecomponent.h"
#include "blackgui/guiapplication.h"
#include "blackcore/context/contextownaircraft.h"
#include "blackmisc/aviation/transponder.h"

#include <QTimer>
#include <QPointer>

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;
using namespace BlackCore::Context;

namespace BlackGui::Components
{
    CTransponderModeComponent::CTransponderModeComponent(QWidget *parent) :
        QFrame(parent), CIdentifiable(this),
        ui(new Ui::CTransponderModeComponent)
    {
        ui->setupUi(this);

        Q_ASSERT_X(sGui, Q_FUNC_INFO, "Need sApp");
        Q_ASSERT_X(sGui->getIContextOwnAircraft(), Q_FUNC_INFO, "Need own aircraft");

        connect(ui->tb_TransponderMode, &QToolButton::released, this, &CTransponderModeComponent::onClicked, Qt::QueuedConnection);
        connect(sGui->getIContextOwnAircraft(), &IContextOwnAircraft::changedAircraftCockpit, this, &CTransponderModeComponent::onChangedAircraftCockpit, Qt::QueuedConnection);

        this->onChangedAircraftCockpit(sGui->getIContextOwnAircraft()->getOwnAircraft(), CIdentifier::null());
        this->init();

        QPointer<CTransponderModeComponent> myself(this);
        QTimer::singleShot(10 * 1000, this, [ = ]
        {
            if (!myself) { return; }
            myself->onChangedAircraftCockpit(sGui->getIContextOwnAircraft()->getOwnAircraft(), CIdentifier::null());
        });
    }

    CTransponderModeComponent::~CTransponderModeComponent()
    { }

    void CTransponderModeComponent::init()
    {
        if (!sGui || sGui->isShuttingDown()) { return; }
        if (!sGui->supportsContexts() || !sGui->getIContextOwnAircraft()) { this->setVisible(false); return; }

        this->setVisible(true);
        ui->tb_TransponderMode->setText(m_transponder.getModeAsShortString());

        this->setProperty("xpdrmode", m_transponder.getTransponderMode());
        this->setProperty("xpdrmodeshort", m_transponder.getModeAsShortString());
        ui->tb_TransponderMode->setProperty("xpdrmode", m_transponder.getTransponderMode());
        ui->tb_TransponderMode->setProperty("xpdrmodeshort", m_transponder.getModeAsShortString());

        this->setToolTip(m_transponder.toQString());
    }

    void CTransponderModeComponent::onClicked()
    {
        if (!sGui || sGui->isShuttingDown()) { return; }
        CTransponder xpdr = m_transponder;
        xpdr.toggleTransponderMode();
        sGui->getIContextOwnAircraft()->setTransponderMode(xpdr.getTransponderMode());
    }

    void CTransponderModeComponent::onChangedAircraftCockpit(const CSimulatedAircraft &aircraft, const CIdentifier &originator)
    {
        if (this->identifier() == originator) { return; }
        if (m_transponder == aircraft.getTransponder()) { return; }
        m_transponder = aircraft.getTransponder();
        this->init();
        emit this->changed();
    }
} // ns
