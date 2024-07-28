// SPDX-FileCopyrightText: Copyright (C) 2019 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "modelbrowsercomponent.h"
#include "ui_modelbrowsercomponent.h"
#include "blackgui/guiapplication.h"
#include "blackcore/context/contextownaircraft.h"
#include "blackcore/context/contextsimulator.h"
#include "blackmisc/aviation/heading.h"
#include "blackmisc/pq/angle.h"
#include "blackmisc/pq/speed.h"

using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackMisc::Geo;
using namespace BlackGui::Editors;
using namespace BlackGui::Views;
using namespace BlackCore;
using namespace BlackCore::Context;

namespace BlackGui::Components
{
    CModelBrowserComponent::CModelBrowserComponent(QWidget *parent) : QFrame(parent),
                                                                      ui(new Ui::CModelBrowserComponent)
    {
        ui->setupUi(this);
        ui->tw_Tab->setCurrentIndex(0);
        ui->tvp_AircraftModels->acceptClickSelection(false);
        ui->tvp_AircraftModels->acceptDoubleClickSelection(true);

        const CLength relDistance(40.0, CLengthUnit::m());
        ui->editor_RelativePosition->setDistance(relDistance);

        connect(ui->pb_SetRelativePosition, &QPushButton::released, this, &CModelBrowserComponent::onSetRelativePosition, Qt::QueuedConnection);
        connect(ui->pb_LoadModelSet, &QPushButton::released, this, &CModelBrowserComponent::loadModelSet, Qt::QueuedConnection);
        connect(ui->pb_Display, &QPushButton::released, this, &CModelBrowserComponent::display, Qt::QueuedConnection);
        connect(ui->pb_Remove, &QPushButton::released, this, &CModelBrowserComponent::remove, Qt::QueuedConnection);
        connect(ui->editor_Coordinate, &CCoordinateForm::changedCoordinate, this, &CModelBrowserComponent::onSetAbsolutePosition, Qt::QueuedConnection);
        connect(ui->editor_Pbh, &CPbhsForm::changeValues, this, &CModelBrowserComponent::onSetPBH, Qt::QueuedConnection);
        connect(ui->editor_AircraftParts, &CAircraftPartsForm::changeAircraftParts, this, &CModelBrowserComponent::onSetParts, Qt::QueuedConnection);
        connect(ui->tvp_AircraftModels, &CAircraftModelView::objectDoubleClicked, this, &CModelBrowserComponent::onModelDblClicked, Qt::QueuedConnection);

        connect(ui->cb_OverrideCG, &QCheckBox::clicked, this, &CModelBrowserComponent::onCGChecked);
        connect(ui->cb_UseCG, &QCheckBox::clicked, this, &CModelBrowserComponent::onCGChecked);

        this->loadModelSet();
        this->onSetRelativePosition();
    }

    CModelBrowserComponent::~CModelBrowserComponent()
    {
        // void
    }

    void CModelBrowserComponent::close()
    {
        this->remove();
    }

    void CModelBrowserComponent::onSetRelativePosition()
    {
        if (!this->hasContexts()) { return; }
        const CAircraftSituation s = sGui->getIContextOwnAircraft()->getOwnAircraftSituation();
        ui->editor_RelativePosition->setOriginCoordinate(s);
        const CCoordinateGeodetic rel = ui->editor_RelativePosition->getRelativeCoordinate(s.getHeading());
        ui->editor_RelativePosition->displayInfo(rel);
        m_situation.setPosition(rel);
    }

    void CModelBrowserComponent::onSetAbsolutePosition()
    {
        m_situation.setPosition(ui->editor_Coordinate->getCoordinate());
    }

    void CModelBrowserComponent::onSetPBH()
    {
        ui->cb_UsePBH->setChecked(true);
        if (!this->hasContexts()) { return; }
        this->updatePartsAndPBH(true, false);
    }

    void CModelBrowserComponent::onSetParts()
    {
        ui->cb_UseParts->setChecked(true);
        if (!this->hasContexts()) { return; }
        this->updatePartsAndPBH(false, true);
    }

    CAircraftParts CModelBrowserComponent::getParts() const
    {
        return ui->editor_AircraftParts->getAircraftPartsFromGui();
    }

    void CModelBrowserComponent::updatePartsAndPBH(bool setPbh, bool setParts)
    {
        if (!this->hasContexts()) { return; }
        CAircraftParts parts = CAircraftParts::null();
        if (setParts || ui->cb_UseParts->isChecked()) { parts = this->getParts(); }

        if (setPbh || ui->cb_UsePBH->isChecked())
        {
            ui->editor_Pbh->updateSituation(m_situation);
        }
        else
        {
            m_situation.setZeroPBHandGs();
        }

        sGui->getISimulator()->testSendSituationAndParts(ISimulator::getTestCallsign(), m_situation, parts);
    }

    void CModelBrowserComponent::fetchSimulatorValues()
    {
        if (!this->hasContexts()) { return; }
    }

    void CModelBrowserComponent::onModelDblClicked(const CVariant &object)
    {
        Q_UNUSED(object);
        this->display();
    }

    void CModelBrowserComponent::display()
    {
        if (!this->hasContexts()) { return; }
        CAircraftModel model = ui->tvp_AircraftModels->selectedObject();
        if (!model.hasModelString()) { return; }

        this->remove(); // only 1 model at a time

        if (ui->cb_OverrideCG->isChecked())
        {
            CLength cg;
            cg.parseFromString(ui->le_CG->text(), CPqString::SeparatorBestGuess);
            if (!cg.isNull()) { model.setCG(cg); }
        }
        else
        {
            ui->le_CG->setText(model.getCG().valueRoundedWithUnit(CLengthUnit::ft(), 1));
        }

        m_situation.setZeroPBHandGs();
        if (ui->cb_UsePBH->isChecked())
        {
            ui->editor_Pbh->updateSituation(m_situation);
        }

        if (ui->cb_UseParts->isChecked())
        {
            const CAircraftParts parts = ui->editor_AircraftParts->getAircraftPartsFromGui();
            if (!parts.isNull()) { m_aircraft.setParts(parts); }
        }

        m_aircraft.setModel(model);
        m_aircraft.setCallsign(ISimulator::getTestCallsign());
        m_aircraft.setSituation(m_situation);

        ui->le_ModelInfo->setText(model.getModelStringAndDbKey());
        ui->le_Info->setText(m_situation.toQString(true));

        sGui->getIContextSimulator()->requestElevationBySituation(m_situation);
        sGui->getIContextSimulator()->testRemoteAircraft(m_aircraft, true);
    }

    void CModelBrowserComponent::remove()
    {
        ui->le_ModelInfo->clear();
        ui->le_Info->clear();

        if (!this->hasContexts()) { return; }
        if (!m_aircraft.hasCallsign()) { return; }
        sGui->getIContextSimulator()->testRemoteAircraft(m_aircraft, false);
        m_aircraft = CSimulatedAircraft(); // reset
    }

    void CModelBrowserComponent::loadModelSet()
    {
        if (!this->hasContexts()) { return; }

        const CAircraftModelList modelSet = sGui->getIContextSimulator()->getModelSet();
        ui->tvp_AircraftModels->updateContainerMaybeAsync(modelSet);
        const QString sim = sGui->getIContextSimulator()->getSimulatorPluginInfo().getSimulatorInfo().toQString(true);
        ui->lbl_ModelSetInfo->setText(QStringLiteral("'%1' model set with %2 models").arg(sim).arg(modelSet.sizeInt()));
    }

    bool CModelBrowserComponent::hasContexts() const
    {
        if (!sGui || sGui->isShuttingDown()) { return false; }
        if (!sGui->getIContextOwnAircraft()) { return false; }
        if (!sGui->getIContextSimulator()) { return false; }
        return true;
    }

    void CModelBrowserComponent::onCGChecked(bool checked)
    {
        if (ui->cb_OverrideCG->isChecked() != checked) { ui->cb_OverrideCG->setChecked(true); }
        if (ui->cb_UseCG->isChecked() != checked) { ui->cb_UseCG->setChecked(true); }
    }
} // ns
