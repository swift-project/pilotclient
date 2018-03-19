/* Copyright (C) 2018
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "interpolationsetupcomponent.h"
#include "ui_interpolationsetupcomponent.h"
#include "blackgui/views/interpolationsetupview.h"
#include "blackgui/guiapplication.h"
#include "blackcore/context/contextsimulator.h"
#include "blackmisc/simulation/interpolationsetuplist.h"
#include "blackmisc/statusmessage.h"

using namespace BlackGui::Views;
using namespace BlackMisc;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::Simulation;

namespace BlackGui
{
    namespace Components
    {
        CInterpolationSetupComponent::CInterpolationSetupComponent(QWidget *parent) :
            COverlayMessagesFrame(parent),
            ui(new Ui::CInterpolationSetupComponent)
        {
            ui->setupUi(this);
            ui->tvp_InterpolationSetup->menuAddItems(CInterpolationSetupView::MenuRemoveSelectedRows);

            connect(ui->pb_RenderingSetup, &QPushButton::clicked, this, &CInterpolationSetupComponent::requestRenderingRestrictionsWidget);
            connect(ui->pb_Save, &QPushButton::clicked, this, &CInterpolationSetupComponent::saveSetup);
            connect(ui->pb_Delete, &QPushButton::clicked, this, &CInterpolationSetupComponent::removeSetup);
            connect(ui->tvp_InterpolationSetup, &CInterpolationSetupView::doubleClicked, this, &CInterpolationSetupComponent::onRowDoubleClicked);
            connect(ui->tvp_InterpolationSetup, &CInterpolationSetupView::modelChanged, this, &CInterpolationSetupComponent::onModelChanged);

            connect(ui->rb_Callsign, &QRadioButton::released, this, &CInterpolationSetupComponent::onModeChanged);
            connect(ui->rb_Global, &QRadioButton::released, this, &CInterpolationSetupComponent::onModeChanged);

            ui->rb_Global->setChecked(true);
            QTimer::singleShot(250, this, &CInterpolationSetupComponent::onModeChanged);
        }

        CInterpolationSetupComponent::~CInterpolationSetupComponent()
        { }

        CInterpolationSetupComponent::Mode CInterpolationSetupComponent::getSetupMode() const
        {
            return ui->rb_Callsign->isChecked() ? CInterpolationSetupComponent::PerCallsign : CInterpolationSetupComponent::Global;
        }

        void CInterpolationSetupComponent::onRowDoubleClicked(const QModelIndex &index)
        {
            if (!index.isValid()) { return; }
            const CInterpolationAndRenderingSetupPerCallsign setup = ui->tvp_InterpolationSetup->at(index);
            ui->form_InterpolationSetup->setValue(setup);
            ui->comp_CallsignCompleter->setCallsign(setup.getCallsign());
            ui->comp_CallsignCompleter->setReadOnly(false);
            ui->rb_Callsign->setChecked(true);
        }

        void CInterpolationSetupComponent::onModeChanged()
        {
            bool enableCallsign = false;
            if (this->getSetupMode() == CInterpolationSetupComponent::Global)
            {
                this->setUiValuesFromGlobal();
            }
            else
            {
                this->displaySetupsPerCallsign();
                enableCallsign = true;
            }
            ui->comp_CallsignCompleter->setReadOnly(!enableCallsign);
            ui->pb_Delete->setEnabled(enableCallsign);
        }

        void CInterpolationSetupComponent::onModelChanged()
        {
            const CInterpolationSetupList setups = ui->tvp_InterpolationSetup->container();
            this->setSetupsToContext(setups);
        }

        void CInterpolationSetupComponent::saveSetup()
        {
            if (!this->checkPrerequisites()) { return; }
            CInterpolationAndRenderingSetupPerCallsign setup = ui->form_InterpolationSetup->getValue();
            if (this->getSetupMode() == CInterpolationSetupComponent::Global)
            {
                CInterpolationAndRenderingSetupGlobal gs = sGui->getIContextSimulator()->getInterpolationAndRenderingSetupGlobal();
                gs.setBaseValues(setup);
                sGui->getIContextSimulator()->setInterpolationAndRenderingSetupGlobal(gs);
            }
            else
            {
                const CCallsign cs = ui->comp_CallsignCompleter->getCallsign(false);
                if (!cs.isValid()) { return; }
                setup.setCallsign(cs);
                CInterpolationSetupList setups = ui->tvp_InterpolationSetup->container();
                const int replaced = setups.replaceOrAddObjectByCallsign(setup);
                if (replaced < 1) { return; }
                const bool set = this->setSetupsToContext(setups);
                if (!set) { return; }
                QTimer::singleShot(250, this, &CInterpolationSetupComponent::displaySetupsPerCallsign);
            }
        }

        void CInterpolationSetupComponent::removeSetup()
        {
            if (!this->checkPrerequisites()) { return; }
            if (this->getSetupMode() == CInterpolationSetupComponent::Global) { return; }
            const CCallsign cs = ui->comp_CallsignCompleter->getCallsign(false);
            CInterpolationSetupList setups = ui->tvp_InterpolationSetup->container();
            const int removed = setups.removeByCallsign(cs);
            if (removed < 1) { return; } // nothing done
            const bool set = this->setSetupsToContext(setups);
            if (!set) { return; }
            QTimer::singleShot(100, this, &CInterpolationSetupComponent::displaySetupsPerCallsign);
        }

        void CInterpolationSetupComponent::setUiValuesFromGlobal()
        {
            Q_ASSERT_X(sGui, Q_FUNC_INFO, "Missing sGui");
            const CInterpolationAndRenderingSetupGlobal setup = sGui->getIContextSimulator()->getInterpolationAndRenderingSetupGlobal();
            ui->form_InterpolationSetup->setValue(setup);
        }

        void CInterpolationSetupComponent::displaySetupsPerCallsign()
        {
            Q_ASSERT_X(sGui, Q_FUNC_INFO, "Missing sGui");
            const CInterpolationSetupList setups = sGui->getIContextSimulator()->getInterpolationAndRenderingSetupsPerCallsign();
            ui->tvp_InterpolationSetup->updateContainerMaybeAsync(setups);
        }

        bool CInterpolationSetupComponent::checkPrerequisites()
        {
            if (!sGui || !sGui->getIContextSimulator())
            {
                const CStatusMessage m = CStatusMessage(this).validationError("No context");
                this->showOverlayMessage(m);
                return false;
            }
            if (!sGui->getIContextSimulator()->isSimulatorAvailable())
            {
                const CStatusMessage m = CStatusMessage(this).validationError("No simulator avialable");
                this->showOverlayMessage(m);
                return false;
            }
            return true;
        }

        bool CInterpolationSetupComponent::setSetupsToContext(const CInterpolationSetupList &setups)
        {
            if (!sGui || !sGui->getIContextSimulator()) { return false; }
            if (setups == m_lastSetSetups) { return false; }
            sGui->getIContextSimulator()->setInterpolationAndRenderingSetupsPerCallsign(setups);
            m_lastSetSetups = setups;
            return true;
        }
    } // ns
} // ns
