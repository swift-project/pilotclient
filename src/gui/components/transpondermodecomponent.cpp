// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "transpondermodecomponent.h"

#include <QPointer>
#include <QTimer>

#include "ui_transpondermodecomponent.h"

#include "core/context/contextownaircraft.h"
#include "gui/guiapplication.h"
#include "misc/aviation/transponder.h"

using namespace swift::misc;
using namespace swift::misc::aviation;
using namespace swift::misc::simulation;
using namespace swift::core::context;

namespace swift::gui::components
{
    CTransponderModeComponent::CTransponderModeComponent(QWidget *parent)
        : QFrame(parent), CIdentifiable(this), ui(new Ui::CTransponderModeComponent)
    {
        ui->setupUi(this);

        Q_ASSERT_X(sGui, Q_FUNC_INFO, "Need sApp");
        Q_ASSERT_X(sGui->getIContextOwnAircraft(), Q_FUNC_INFO, "Need own aircraft");

        connect(ui->tb_TransponderMode, &QToolButton::released, this, &CTransponderModeComponent::onClicked,
                Qt::QueuedConnection);
        connect(sGui->getIContextOwnAircraft(), &IContextOwnAircraft::changedAircraftCockpit, this,
                &CTransponderModeComponent::onChangedAircraftCockpit, Qt::QueuedConnection);

        this->onChangedAircraftCockpit(sGui->getIContextOwnAircraft()->getOwnAircraft(), CIdentifier::null());
        this->init();

        QPointer<CTransponderModeComponent> myself(this);
        QTimer::singleShot(10 * 1000, this, [=] {
            if (!myself) { return; }
            myself->onChangedAircraftCockpit(sGui->getIContextOwnAircraft()->getOwnAircraft(), CIdentifier::null());
        });
    }

    CTransponderModeComponent::~CTransponderModeComponent() = default;

    void CTransponderModeComponent::init()
    {
        if (!sGui || sGui->isShuttingDown()) { return; }
        if (!sGui->supportsContexts() || !sGui->getIContextOwnAircraft())
        {
            this->setVisible(false);
            return;
        }

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

    void CTransponderModeComponent::onChangedAircraftCockpit(const CSimulatedAircraft &aircraft,
                                                             const CIdentifier &originator)
    {
        if (this->identifier() == originator) { return; }
        if (m_transponder == aircraft.getTransponder()) { return; }
        m_transponder = aircraft.getTransponder();
        this->init();
        emit this->changed();
    }
} // namespace swift::gui::components
