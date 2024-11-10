// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "interpolationsetupcomponent.h"
#include "ui_interpolationsetupcomponent.h"
#include "blackgui/views/interpolationsetupview.h"
#include "blackgui/guiapplication.h"
#include "blackcore/context/contextsimulator.h"
#include "misc/simulation/interpolation/interpolationsetuplist.h"
#include "misc/statusmessage.h"
#include <QPointer>

using namespace BlackGui::Views;
using namespace swift::misc;
using namespace swift::misc::aviation;
using namespace swift::misc::simulation;
using namespace BlackCore::Context;

namespace BlackGui::Components
{
    CInterpolationSetupComponent::CInterpolationSetupComponent(QWidget *parent) : COverlayMessagesFrame(parent),
                                                                                  ui(new Ui::CInterpolationSetupComponent)
    {
        ui->setupUi(this);
        ui->cb_IgnoreGlobal->setChecked(true);
        ui->tvp_InterpolationSetup->menuAddItems(CInterpolationSetupView::MenuRemoveSelectedRows);

        connect(ui->pb_RenderingSetup, &QPushButton::clicked, this, &CInterpolationSetupComponent::requestRenderingRestrictionsWidget);
        connect(ui->pb_Save, &QPushButton::clicked, this, &CInterpolationSetupComponent::saveSetup);
        connect(ui->pb_DeleteOrReset, &QPushButton::clicked, this, &CInterpolationSetupComponent::removeOrResetSetup);
        connect(ui->pb_Reload, &QPushButton::clicked, this, &CInterpolationSetupComponent::reloadSetup, Qt::QueuedConnection);
        connect(ui->tvp_InterpolationSetup, &CInterpolationSetupView::doubleClicked, this, &CInterpolationSetupComponent::onRowDoubleClicked);
        connect(ui->tvp_InterpolationSetup, &CInterpolationSetupView::modelChanged, this, &CInterpolationSetupComponent::onModelChanged, Qt::QueuedConnection);
        connect(ui->tvp_InterpolationSetup, &CInterpolationSetupView::modelDataChanged, this, &CInterpolationSetupComponent::onModelChanged, Qt::QueuedConnection);
        connect(ui->tvp_InterpolationSetup, &CInterpolationSetupView::objectsDeleted, this, &CInterpolationSetupComponent::onObjectsDeleted, Qt::QueuedConnection);
        connect(ui->rb_Callsign, &QRadioButton::released, this, &CInterpolationSetupComponent::onModeChanged);
        connect(ui->rb_Global, &QRadioButton::released, this, &CInterpolationSetupComponent::onModeChanged);
        if (sGui && sGui->getIContextSimulator())
        {
            connect(sGui->getIContextSimulator(), &IContextSimulator::interpolationAndRenderingSetupChanged, this, &CInterpolationSetupComponent::onSetupChanged, Qt::QueuedConnection);
        }

        ui->rb_Global->setChecked(true);

        QPointer<CInterpolationSetupComponent> myself(this);
        QTimer::singleShot(1000, this, [=] {
            if (!sGui || sGui->isShuttingDown()) { return; }
            if (!myself) { return; }
            this->onModeChanged();
        });

        QTimer::singleShot(30 * 1000, this, [=] {
            if (!sGui || sGui->isShuttingDown()) { return; }
            if (!myself) { return; }
            this->onSetupChanged();
        });
    }

    CInterpolationSetupComponent::~CInterpolationSetupComponent()
    {}

    CInterpolationSetupComponent::Mode CInterpolationSetupComponent::getSetupMode() const
    {
        return ui->rb_Callsign->isChecked() ? CInterpolationSetupComponent::SetupPerCallsign : CInterpolationSetupComponent::SetupGlobal;
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
        if (!sGui || sGui->isShuttingDown()) { return; }
        bool enableCallsign = false;
        if (this->getSetupMode() == CInterpolationSetupComponent::SetupGlobal)
        {
            this->setUiValuesFromGlobal();
        }
        else
        {
            enableCallsign = true;
        }
        this->displaySetupsPerCallsign();
        ui->comp_CallsignCompleter->setReadOnly(!enableCallsign);
        ui->pb_DeleteOrReset->setText(enableCallsign ? "delete" : "reset");
    }

    void CInterpolationSetupComponent::onModelChanged()
    {
        // void
    }

    void CInterpolationSetupComponent::onReloadSetup()
    {
        this->reloadSetup();
        this->displaySetupsPerCallsign();
    }

    void CInterpolationSetupComponent::reloadSetup()
    {
        const bool global = (this->getSetupMode() == CInterpolationSetupComponent::SetupGlobal);
        const bool overlay = QObject::sender() == ui->pb_Reload;
        if (!this->checkPrerequisites(!global, overlay)) { return; }
        if (global)
        {
            CInterpolationAndRenderingSetupGlobal gs = sGui->getIContextSimulator()->getInterpolationAndRenderingSetupGlobal();
            ui->form_InterpolationSetup->setValue(gs);
        }
        else
        {
            const CCallsign cs = ui->comp_CallsignCompleter->getCallsign(false);
            if (!cs.isValid()) { return; }
            const CInterpolationAndRenderingSetupPerCallsign setup = sGui->getIContextSimulator()->getInterpolationAndRenderingSetupPerCallsignOrDefault(cs);
            ui->form_InterpolationSetup->setValue(setup);
        }
    }

    void CInterpolationSetupComponent::saveSetup()
    {
        const bool global = (this->getSetupMode() == CInterpolationSetupComponent::SetupGlobal);
        if (!this->checkPrerequisites(!global, true)) { return; }
        CInterpolationAndRenderingSetupPerCallsign setup = ui->form_InterpolationSetup->getValue();
        CInterpolationAndRenderingSetupGlobal gs = sGui->getIContextSimulator()->getInterpolationAndRenderingSetupGlobal();
        if (global)
        {
            gs.setBaseValues(setup);
            gs.setLogInterpolation(false); // that would globally log all values
            sGui->getIContextSimulator()->setInterpolationAndRenderingSetupGlobal(gs);
            CLogMessage(this).info(u"Set global setup: '%1'") << gs.toQString(true);

            const QPointer<CInterpolationSetupComponent> myself(this);
            QTimer::singleShot(250, this, [=] {
                if (!myself) { return; }
                this->reloadSetup();
            });
        }
        else
        {
            const CCallsign cs = ui->comp_CallsignCompleter->getCallsign(false);
            if (!cs.isValid()) { return; }
            setup.setCallsign(cs);
            const bool ignoreGlobal = ui->cb_IgnoreGlobal->isChecked();
            if (ignoreGlobal && setup.isEqualToGlobal(gs))
            {
                static const CStatusMessage m = CStatusMessage(this).validationWarning(u"Same as global setup");
                this->showOverlayMessage(m);
                return;
            }

            CInterpolationSetupList setups = ui->tvp_InterpolationSetup->container();
            const int replaced = setups.replaceOrAddObjectByCallsign(setup);
            if (replaced < 1) { return; }
            const bool set = this->setSetupsToContext(setups);
            if (!set) { return; }

            const QPointer<CInterpolationSetupComponent> myself(this);
            QTimer::singleShot(250, this, [=] {
                if (!myself) { return; }
                this->displaySetupsPerCallsign();
            });
        }
    }

    void CInterpolationSetupComponent::removeOrResetSetup()
    {
        const bool global = (this->getSetupMode() == CInterpolationSetupComponent::SetupGlobal);
        if (!this->checkPrerequisites(!global, true)) {}
        if (global)
        {
            // reset
            CInterpolationAndRenderingSetupGlobal gs;
            sGui->getIContextSimulator()->setInterpolationAndRenderingSetupGlobal(gs);
            this->reloadSetup();
        }
        else
        {
            // delete/remove
            const CCallsign cs = ui->comp_CallsignCompleter->getCallsign(false);
            CInterpolationSetupList setups = ui->tvp_InterpolationSetup->container();
            const int removed = setups.removeByCallsign(cs);
            if (removed < 1) { return; } // nothing done
            const bool set = this->setSetupsToContext(setups);
            if (!set) { return; }

            const QPointer<CInterpolationSetupComponent> myself(this);
            QTimer::singleShot(100, this, [=] {
                if (!myself) { return; }
                this->displaySetupsPerCallsign();
            });
        }
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

    bool CInterpolationSetupComponent::checkPrerequisites(bool checkSim, bool showOverlay)
    {
        if (!sGui || !sGui->getIContextSimulator() || sGui->isShuttingDown())
        {
            if (showOverlay)
            {
                const CStatusMessage m = CStatusMessage(this).validationError(u"No context");
                this->showOverlayMessage(m);
            }
            return false;
        }
        if (checkSim && !sGui->getIContextSimulator()->isSimulatorAvailable())
        {
            if (showOverlay)
            {
                const CStatusMessage m = CStatusMessage(this).validationError(u"No simulator available");
                this->showOverlayMessage(m);
            }
            return false;
        }
        return true;
    }

    bool CInterpolationSetupComponent::setSetupsToContext(const CInterpolationSetupList &setups, bool force)
    {
        if (!sGui || sGui->isShuttingDown() || !sGui->getIContextSimulator()) { return false; }
        if (!force && setups == m_lastSetSetups) { return false; }
        const bool ignoreGlobal = ui->cb_IgnoreGlobal->isChecked();
        sGui->getIContextSimulator()->setInterpolationAndRenderingSetupsPerCallsign(setups, ignoreGlobal);
        m_lastSetSetups = setups;
        return true;
    }

    void CInterpolationSetupComponent::onSetupChanged()
    {
        if (!sGui || sGui->isShuttingDown()) { return; }
        this->displaySetupsPerCallsign();
    }

    void CInterpolationSetupComponent::onObjectsDeleted(const CVariant &deletedObjects)
    {
        if (deletedObjects.canConvert<CInterpolationSetupList>())
        {
            const CInterpolationSetupList deletedSetups = deletedObjects.value<CInterpolationSetupList>();
            if (deletedSetups.isEmpty()) { return; }

            // make sure the setups are really deleted
            // it can be they are already in the container, but there is no guarantee
            CInterpolationSetupList setups = ui->tvp_InterpolationSetup->container();
            setups.removeByCallsigns(deletedSetups.getCallsigns());
            const bool set = this->setSetupsToContext(setups, true);
            Q_UNUSED(set)
        }
    }
} // ns
