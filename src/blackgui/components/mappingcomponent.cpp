/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/context/contextnetwork.h"
#include "blackcore/context/contextsimulator.h"
#include "blackcore/network.h"
#include "blackgui/components/mappingcomponent.h"
#include "blackgui/guiapplication.h"
#include "blackgui/guiutility.h"
#include "blackgui/models/aircraftmodellistmodel.h"
#include "blackgui/models/simulatedaircraftlistmodel.h"
#include "blackgui/views/aircraftmodelview.h"
#include "blackgui/views/checkboxdelegate.h"
#include "blackgui/views/simulatedaircraftview.h"
#include "blackgui/views/viewbase.h"
#include "blackmisc/aviation/callsign.h"
#include "blackmisc/icons.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/pixmap.h"
#include "blackmisc/simulation/aircraftmodel.h"
#include "blackmisc/simulation/simulatedaircraft.h"
#include "blackmisc/simulation/simulatedaircraftlist.h"
#include "blackmisc/statusmessage.h"
#include "mappingcomponent.h"
#include "ui_mappingcomponent.h"

#include <QCheckBox>
#include <QCompleter>
#include <QLabel>
#include <QLineEdit>
#include <QPixmap>
#include <QPushButton>
#include <QStringListModel>
#include <QtGlobal>

using namespace BlackMisc;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Network;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackCore;
using namespace BlackCore::Context;
using namespace BlackGui;
using namespace BlackGui::Views;
using namespace BlackGui::Models;
using namespace BlackGui::Filters;
using namespace BlackGui::Settings;

namespace BlackGui
{
    namespace Components
    {
        CMappingComponent::CMappingComponent(QWidget *parent) :
            COverlayMessagesFrame(parent),
            CIdentifiable(this),
            ui(new Ui::CMappingComponent)
        {
            ui->setupUi(this);
            ui->tvp_AircraftModels->setAircraftModelMode(CAircraftModelListModel::OwnAircraftModelClient);
            ui->tvp_AircraftModels->setResizeMode(CAircraftModelView::ResizingOff);
            ui->tvp_AircraftModels->addFilterDialog();
            ui->tvp_AircraftModels->menuRemoveItems(CViewBaseNonTemplate::MenuBackend);

            ui->tvp_RenderedAircraft->setAircraftMode(CSimulatedAircraftListModel::RenderedMode);
            ui->tvp_RenderedAircraft->setResizeMode(CAircraftModelView::ResizingOnce);

            connect(sGui->getIContextNetwork(), &IContextNetwork::connectionStatusChanged, this, &CMappingComponent::ps_connectionStatusChanged);

            connect(ui->tvp_AircraftModels, &CAircraftModelView::requestUpdate, this, &CMappingComponent::ps_onModelsUpdateRequested);
            connect(ui->tvp_AircraftModels, &CAircraftModelView::modelDataChanged, this, &CMappingComponent::ps_onRowCountChanged);
            connect(ui->tvp_AircraftModels, &CAircraftModelView::clicked, this, &CMappingComponent::ps_onModelSelectedInView);

            connect(ui->tvp_RenderedAircraft, &CSimulatedAircraftView::modelDataChanged, this, &CMappingComponent::ps_onRowCountChanged);
            connect(ui->tvp_RenderedAircraft, &CSimulatedAircraftView::clicked, this, &CMappingComponent::ps_onAircraftSelectedInView);
            connect(ui->tvp_RenderedAircraft, &CSimulatedAircraftView::requestUpdate, this, &CMappingComponent::ps_tokenBucketUpdate);
            connect(ui->tvp_RenderedAircraft, &CSimulatedAircraftView::requestTextMessageWidget, this, &CMappingComponent::requestTextMessageWidget);
            connect(ui->tvp_RenderedAircraft, &CSimulatedAircraftView::requestEnableAircraft, this, &CMappingComponent::ps_onMenuToggleEnableAircraft);
            connect(ui->tvp_RenderedAircraft, &CSimulatedAircraftView::requestFastPositionUpdates, this, &CMappingComponent::ps_onMenuChangeFastPositionUpdates);
            connect(ui->tvp_RenderedAircraft, &CSimulatedAircraftView::requestHighlightInSimulator, this, &CMappingComponent::ps_onMenuHighlightInSimulator);

            connect(ui->pb_SaveAircraft, &QPushButton::clicked, this, &CMappingComponent::ps_onSaveAircraft);
            connect(ui->pb_ResetAircraft, &QPushButton::clicked, this, &CMappingComponent::ps_onResetAircraft);
            connect(ui->pb_LoadModels, &QPushButton::clicked, this, &CMappingComponent::ps_onModelsUpdateRequested);

            m_currentMappingsViewDelegate = new CCheckBoxDelegate(":/diagona/icons/diagona/icons/tick.png", ":/diagona/icons/diagona/icons/cross.png", this);
            ui->tvp_RenderedAircraft->setItemDelegateForColumn(0, m_currentMappingsViewDelegate);
            this->showKillButton(false);

            // Aircraft previews
            connect(ui->cb_AircraftIconDisplayed, &QCheckBox::stateChanged, this, &CMappingComponent::ps_onModelPreviewChanged);

            // model string completer
            ui->completer_ModelStrings->setSourceVisible(CAircraftModelStringCompleter::OwnModels, false);
            ui->completer_ModelStrings->selectSource(CAircraftModelStringCompleter::ModelSet);

            // Updates
            connect(&m_updateTimer, &QTimer::timeout, this, &CMappingComponent::ps_timerUpdate);
            ui->tvp_AircraftModels->setDisplayAutomatically(false);
            this->ps_settingsChanged();

            connect(sGui->getIContextSimulator(), &IContextSimulator::modelSetChanged, this, &CMappingComponent::ps_onModelSetChanged);
            connect(sGui->getIContextSimulator(), &IContextSimulator::modelMatchingCompleted, this, &CMappingComponent::ps_tokenBucketUpdateAircraft);
            connect(sGui->getIContextSimulator(), &IContextSimulator::aircraftRenderingChanged, this, &CMappingComponent::ps_tokenBucketUpdateAircraft);
            connect(sGui->getIContextSimulator(), &IContextSimulator::airspaceSnapshotHandled, this, &CMappingComponent::ps_tokenBucketUpdate);
            connect(sGui->getIContextSimulator(), &IContextSimulator::addingRemoteModelFailed, this, &CMappingComponent::ps_addingRemoteAircraftFailed);
            connect(sGui->getIContextNetwork(), &IContextNetwork::changedRemoteAircraftModel, this, &CMappingComponent::ps_onRemoteAircraftModelChanged);
            connect(sGui->getIContextNetwork(), &IContextNetwork::changedRemoteAircraftEnabled, this, &CMappingComponent::ps_tokenBucketUpdateAircraft);
            connect(sGui->getIContextNetwork(), &IContextNetwork::changedFastPositionUpdates, this, &CMappingComponent::ps_tokenBucketUpdateAircraft);
            connect(sGui->getIContextNetwork(), &IContextNetwork::removedAircraft, this, &CMappingComponent::ps_tokenBucketUpdate);
            connect(sGui->getIContextNetwork(), &IContextNetwork::connectionStatusChanged, this, &CMappingComponent::ps_onConnectionStatusChanged);

            // requires simulator context
            connect(ui->tvp_RenderedAircraft, &CAircraftModelView::objectChanged, this, &CMappingComponent::ps_onChangedSimulatedAircraftInView);

            // with external core models might be already available
            this->ps_onModelSetChanged();
        }

        CMappingComponent::~CMappingComponent()
        { }

        int CMappingComponent::countCurrentMappings() const
        {
            Q_ASSERT(ui->tvp_RenderedAircraft);
            return ui->tvp_RenderedAircraft->rowCount();
        }

        int CMappingComponent::countAircraftModels() const
        {
            Q_ASSERT(ui->tvp_AircraftModels);
            return ui->tvp_AircraftModels->rowCount();
        }

        CAircraftModelList CMappingComponent::findModelsStartingWith(const QString modelName, Qt::CaseSensitivity cs)
        {
            Q_ASSERT(ui->tvp_AircraftModels);
            return ui->tvp_AircraftModels->container().findModelsStartingWith(modelName, cs);
        }

        void CMappingComponent::ps_onModelSetChanged()
        {
            if (ui->tvp_AircraftModels->displayAutomatically())
            {
                this->ps_onModelsUpdateRequested();
            }
            else
            {
                CLogMessage(this).info("Models loaded, you can update the model view");
            }
        }

        void CMappingComponent::ps_onRowCountChanged(int count, bool withFilter)
        {
            Q_UNUSED(count);
            Q_UNUSED(withFilter);
            const int am = ui->tw_ListViews->indexOf(ui->tb_AircraftModels);
            const int cm = ui->tw_ListViews->indexOf(ui->tb_CurrentMappings);
            const QString amf = ui->tvp_AircraftModels->derivedModel()->hasFilter() ? "F" : "";
            QString a = ui->tw_ListViews->tabBar()->tabText(am);
            QString c = ui->tw_ListViews->tabBar()->tabText(cm);
            a = CGuiUtility::replaceTabCountValue(a, this->countAircraftModels()) + amf;
            c = CGuiUtility::replaceTabCountValue(c, this->countCurrentMappings());
            ui->tw_ListViews->tabBar()->setTabText(am, a);
            ui->tw_ListViews->tabBar()->setTabText(cm, c);
        }

        void CMappingComponent::ps_onChangedSimulatedAircraftInView(const CVariant &object, const CPropertyIndex &index)
        {
            const CSimulatedAircraft sa = object.to<CSimulatedAircraft>(); // changed in GUI
            const CSimulatedAircraft saFromBackend = sGui->getIContextNetwork()->getAircraftInRangeForCallsign(sa.getCallsign());
            if (!saFromBackend.hasValidCallsign()) { return; } // obviously deleted
            if (index.contains(CSimulatedAircraft::IndexEnabled))
            {
                const bool enabled = sa.propertyByIndex(index).toBool();
                if (saFromBackend.isEnabled() == enabled) { return; }
                CLogMessage(this).info("Request to %1 aircraft %2") << (enabled ? "enable" : "disable") << saFromBackend.getCallsign().toQString();
                sGui->getIContextNetwork()->updateAircraftEnabled(saFromBackend.getCallsign(), enabled);
            }
            else
            {
                Q_ASSERT_X(false, "ps_onChangedSimulatedAircraftInView", "Index not supported");
            }
        }

        void CMappingComponent::ps_onAircraftSelectedInView(const QModelIndex &index)
        {
            const CSimulatedAircraft simAircraft = ui->tvp_RenderedAircraft->at(index);
            ui->cb_AircraftEnabled->setChecked(simAircraft.isEnabled());
            ui->le_Callsign->setText(simAircraft.getCallsign().asString());
            ui->completer_ModelStrings->setModel(simAircraft.getModel());
        }

        void CMappingComponent::ps_onModelSelectedInView(const QModelIndex &index)
        {
            const CAircraftModel model = ui->tvp_AircraftModels->at(index);
            ui->completer_ModelStrings->setModel(model);

            if (ui->cb_AircraftIconDisplayed->isChecked())
            {
                const QString modelString(model.getModelString());
                const CPixmap pm = sGui->getIContextSimulator()->iconForModel(modelString);
                if (pm.isNull())
                {
                    this->closeOverlay();
                }
                else
                {
                    this->showOverlayImage(pm);
                }
            }
            else
            {
                this->ps_onModelPreviewChanged(Qt::Unchecked);
            }
        }

        CCallsign CMappingComponent::validateRenderedCallsign() const
        {
            const QString cs = ui->le_Callsign->text().trimmed();
            if (!CCallsign::isValidAircraftCallsign(cs))
            {
                CLogMessage(this).validationError("Invalid callsign for mapping");
                return CCallsign();
            }

            const CCallsign callsign(cs);
            const bool hasCallsign = ui->tvp_RenderedAircraft->container().containsCallsign(callsign);
            if (!hasCallsign)
            {
                CLogMessage(this).validationError("Unmapped callsign %1 for mapping") << callsign.asString();
                return CCallsign();
            }
            return callsign;
        }

        void CMappingComponent::ps_onSaveAircraft()
        {
            if (!sGui->getIContextSimulator()->isSimulatorSimulating()) { return; }
            const CCallsign callsign(this->validateRenderedCallsign());
            if (callsign.isEmpty()) { return; }
            const QString modelString = ui->completer_ModelStrings->getModelString();
            if (modelString.isEmpty())
            {
                CLogMessage(this).validationError("Missing model for mapping");
                return;
            }

            const bool hasModel = ui->tvp_AircraftModels->container().containsModelString(modelString);
            if (!hasModel)
            {
                CLogMessage(this).validationError("Invalid model for mapping, reload models");
                if (ui->tvp_AircraftModels->isEmpty())
                {
                    this->ps_onModelsUpdateRequested();
                }
                return;
            }

            const CSimulatedAircraft aircraftFromBackend = sGui->getIContextNetwork()->getAircraftInRangeForCallsign(callsign);
            const bool enabled = ui->cb_AircraftEnabled->isChecked();
            bool changed = false;
            if (aircraftFromBackend.getModelString() != modelString)
            {
                CAircraftModelList models = sGui->getIContextSimulator()->getModelSetModelsStartingWith(modelString);
                if (models.isEmpty())
                {
                    CLogMessage(this).validationError("No model for title: %1") << modelString;
                    return;
                }

                CAircraftModel model(models.front());
                if (models.size() > 1)
                {
                    if (models.containsModelString(modelString))
                    {
                        model = models.findByModelString(modelString).front(); // exact match
                    }
                    else
                    {
                        CLogMessage(this).validationInfo("Ambigious title: %1, using %2") << modelString << model.getModelString();
                    }
                }
                model.setModelType(CAircraftModel::TypeManuallySet);
                CLogMessage(this).info("Requesting changes for %1") << callsign.asString();
                changed = sGui->getIContextNetwork()->updateAircraftModel(aircraftFromBackend.getCallsign(), model, identifier());
            }
            if (aircraftFromBackend.isEnabled() != enabled)
            {
                changed = sGui->getIContextNetwork()->updateAircraftEnabled(aircraftFromBackend.getCallsign(), enabled);
            }

            if (!changed)
            {
                CLogMessage(this).info("Model mapping, nothing to change");
            }
        }

        void CMappingComponent::ps_onResetAircraft()
        {
            if (!sGui->getIContextSimulator()->isSimulatorSimulating()) { return; }
            const CCallsign callsign(this->validateRenderedCallsign());
            if (callsign.isEmpty()) { return; }
            bool reset = sGui->getIContextSimulator()->resetToModelMatchingAircraft(callsign);
            if (reset)
            {
                CLogMessage(this).info("Model reset for '%1'") << callsign.toQString();
            }
            else
            {
                CLogMessage(this).info("Reset failed for '%1'") << callsign.toQString();
            }
        }

        void CMappingComponent::ps_onModelPreviewChanged(int state)
        {
            Q_UNUSED(state);
            this->closeOverlay();
        }

        void CMappingComponent::ps_onModelsUpdateRequested()
        {
            const CAircraftModelList ml(sGui->getIContextSimulator()->getModelSet());
            ui->tvp_AircraftModels->updateContainerMaybeAsync(ml);
        }

        void CMappingComponent::ps_onRemoteAircraftModelChanged(const CSimulatedAircraft &aircraft, const CIdentifier &originator)
        {
            if (CIdentifiable::isMyIdentifier(originator)) { return; }
            this->ps_tokenBucketUpdateAircraft(aircraft);
        }

        void CMappingComponent::ps_onConnectionStatusChanged(INetwork::ConnectionStatus from, INetwork::ConnectionStatus to)
        {
            Q_UNUSED(from);
            if (INetwork::isDisconnectedStatus(to))
            {
                this->ps_tokenBucketUpdate();
                ui->tvp_RenderedAircraft->clear();
            }
        }

        void CMappingComponent::ps_onMenuChangeFastPositionUpdates(const CSimulatedAircraft &aircraft)
        {
            if (sGui->getIContextNetwork())
            {
                sGui->getIContextNetwork()->updateFastPositionEnabled(aircraft.getCallsign(), aircraft.fastPositionUpdates());
            }
        }

        void CMappingComponent::ps_onMenuHighlightInSimulator(const CSimulatedAircraft &aircraft)
        {
            if (sGui->getIContextSimulator())
            {
                sGui->getIContextSimulator()->highlightAircraft(aircraft, true, IContextSimulator::HighlightTime());
            }
        }

        void CMappingComponent::ps_addingRemoteAircraftFailed(const CSimulatedAircraft &aircraft, const CStatusMessage &message)
        {
            this->ps_tokenBucketUpdate();
            Q_UNUSED(aircraft);
            Q_UNUSED(message);
        }

        void CMappingComponent::ps_onMenuToggleEnableAircraft(const CSimulatedAircraft &aircraft)
        {
            if (sGui->getIContextNetwork())
            {
                sGui->getIContextNetwork()->updateAircraftEnabled(aircraft.getCallsign(), aircraft.isEnabled());
            }
        }

        CIdentifier CMappingComponent::mappingIdentifier()
        {
            if (m_identifier.getName().isEmpty())
            {
                m_identifier = CIdentifier(QStringLiteral("MAPPINGCOMPONENT"));
            }
            return m_identifier;
        }

        void CMappingComponent::updateRenderedAircraftView(bool forceUpdate)
        {
            m_updateTimer.start(); // restart
            if (!forceUpdate && !this->isVisibleWidget())
            {
                m_missedRenderedAircraftUpdate = true;
                return;
            }

            m_missedRenderedAircraftUpdate = false;
            m_updateTimer.start(); // restart
            if (sGui->getIContextSimulator()->getSimulatorStatus() > 0)
            {
                const CSimulatedAircraftList aircraft(sGui->getIContextNetwork()->getAircraftInRange());
                ui->tvp_RenderedAircraft->updateContainer(aircraft);
            }
            else
            {
                ui->tvp_RenderedAircraft->clear();
            }
        }

        void CMappingComponent::ps_timerUpdate()
        {
            // timer update to update position, speed ...
            this->updateRenderedAircraftView(false); // unforced
        }

        void CMappingComponent::ps_tokenBucketUpdateAircraft(const CSimulatedAircraft &aircraft)
        {
            Q_UNUSED(aircraft);
            this->ps_tokenBucketUpdate();
        }

        void CMappingComponent::ps_tokenBucketUpdate()
        {
            if (!m_bucket.tryConsume()) { return; }
            this->updateRenderedAircraftView(true); // forced update
        }

        void CMappingComponent::ps_settingsChanged()
        {
            const CViewUpdateSettings settings = m_settings.get();
            const int ms = settings.getRenderingUpdateTime().toMs();
            m_updateTimer.setInterval(ms);
        }

        void CMappingComponent::ps_connectionStatusChanged(BlackCore::INetwork::ConnectionStatus from, BlackCore::INetwork::ConnectionStatus to)
        {
            Q_UNUSED(from);
            if (INetwork::isDisconnectedStatus(to))
            {
                ui->tvp_RenderedAircraft->clear();
                m_updateTimer.stop();
            }
            else if (INetwork::isConnectedStatus(to))
            {
                m_updateTimer.start();
            }
        }
    } // namespace
} // namespace
