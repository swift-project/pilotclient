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
            Q_ASSERT_X(sGui, Q_FUNC_INFO, "need sGui");
            Q_ASSERT_X(sGui->getIContextSimulator(), Q_FUNC_INFO, "need simulator context");
            Q_ASSERT_X(sGui->getIContextNetwork(), Q_FUNC_INFO, "need network context");

            ui->setupUi(this);
            ui->tw_ListViews->setCurrentIndex(0);

            ui->tvp_AircraftModels->setAircraftModelMode(CAircraftModelListModel::OwnAircraftModelClient);
            ui->tvp_AircraftModels->setResizeMode(CAircraftModelView::ResizingOff);
            ui->tvp_AircraftModels->addFilterDialog();
            ui->tvp_AircraftModels->menuRemoveItems(CViewBaseNonTemplate::MenuBackend);

            ui->tvp_RenderedAircraft->setAircraftMode(CSimulatedAircraftListModel::RenderedMode);
            ui->tvp_RenderedAircraft->setResizeMode(CAircraftModelView::ResizingOnce);

            connect(sGui->getIContextNetwork(), &IContextNetwork::connectionStatusChanged, this, &CMappingComponent::connectionStatusChanged);

            connect(ui->tvp_AircraftModels, &CAircraftModelView::requestUpdate, this, &CMappingComponent::onModelsUpdateRequested);
            connect(ui->tvp_AircraftModels, &CAircraftModelView::modelDataChanged, this, &CMappingComponent::onRowCountChanged);
            connect(ui->tvp_AircraftModels, &CAircraftModelView::clicked, this, &CMappingComponent::onModelSelectedInView);

            connect(ui->tvp_RenderedAircraft, &CSimulatedAircraftView::modelDataChanged, this, &CMappingComponent::onRowCountChanged);
            connect(ui->tvp_RenderedAircraft, &CSimulatedAircraftView::clicked, this, &CMappingComponent::onAircraftSelectedInView);
            connect(ui->tvp_RenderedAircraft, &CSimulatedAircraftView::requestUpdate, this, &CMappingComponent::tokenBucketUpdate);
            connect(ui->tvp_RenderedAircraft, &CSimulatedAircraftView::requestTextMessageWidget, this, &CMappingComponent::requestTextMessageWidget);
            connect(ui->tvp_RenderedAircraft, &CSimulatedAircraftView::requestEnableAircraft, this, &CMappingComponent::onMenuToggleEnableAircraft);
            connect(ui->tvp_RenderedAircraft, &CSimulatedAircraftView::requestFastPositionUpdates, this, &CMappingComponent::onMenuChangeFastPositionUpdates);
            connect(ui->tvp_RenderedAircraft, &CSimulatedAircraftView::requestHighlightInSimulator, this, &CMappingComponent::onMenuHighlightInSimulator);

            connect(ui->pb_SaveAircraft, &QPushButton::clicked, this, &CMappingComponent::onSaveAircraft);
            connect(ui->pb_ResetAircraft, &QPushButton::clicked, this, &CMappingComponent::onResetAircraft);
            connect(ui->pb_LoadModels, &QPushButton::clicked, this, &CMappingComponent::onModelsUpdateRequested);

            m_currentMappingsViewDelegate = new CCheckBoxDelegate(":/diagona/icons/diagona/icons/tick.png", ":/diagona/icons/diagona/icons/cross.png", this);
            ui->tvp_RenderedAircraft->setItemDelegateForColumn(0, m_currentMappingsViewDelegate);

            // overlay
            this->setReducedInfo(true);
            this->setForceSmall(true);
            this->showKillButton(false);

            // Aircraft previews
            connect(ui->cb_AircraftIconDisplayed, &QCheckBox::stateChanged, this, &CMappingComponent::onModelPreviewChanged);

            // model string completer
            ui->completer_ModelStrings->setSourceVisible(CAircraftModelStringCompleter::OwnModels, false);
            ui->completer_ModelStrings->selectSource(CAircraftModelStringCompleter::ModelSet);

            // Updates
            connect(&m_updateTimer, &QTimer::timeout, this, &CMappingComponent::timerUpdate);
            ui->tvp_AircraftModels->setDisplayAutomatically(false);
            this->settingsChanged();

            connect(sGui->getIContextSimulator(), &IContextSimulator::modelSetChanged, this, &CMappingComponent::onModelSetChanged);
            connect(sGui->getIContextSimulator(), &IContextSimulator::modelMatchingCompleted, this, &CMappingComponent::tokenBucketUpdateAircraft);
            connect(sGui->getIContextSimulator(), &IContextSimulator::aircraftRenderingChanged, this, &CMappingComponent::tokenBucketUpdateAircraft);
            connect(sGui->getIContextSimulator(), &IContextSimulator::airspaceSnapshotHandled, this, &CMappingComponent::tokenBucketUpdate);
            connect(sGui->getIContextSimulator(), &IContextSimulator::addingRemoteModelFailed, this, &CMappingComponent::addingRemoteAircraftFailed);
            connect(sGui->getIContextNetwork(), &IContextNetwork::changedRemoteAircraftModel, this, &CMappingComponent::onRemoteAircraftModelChanged);
            connect(sGui->getIContextNetwork(), &IContextNetwork::changedRemoteAircraftEnabled, this, &CMappingComponent::tokenBucketUpdateAircraft);
            connect(sGui->getIContextNetwork(), &IContextNetwork::changedFastPositionUpdates, this, &CMappingComponent::tokenBucketUpdateAircraft);
            connect(sGui->getIContextNetwork(), &IContextNetwork::removedAircraft, this, &CMappingComponent::tokenBucketUpdate);
            connect(sGui->getIContextNetwork(), &IContextNetwork::connectionStatusChanged, this, &CMappingComponent::onConnectionStatusChanged);

            // requires simulator context
            connect(ui->tvp_RenderedAircraft, &CAircraftModelView::objectChanged, this, &CMappingComponent::onChangedSimulatedAircraftInView);

            // with external core models might be already available
            this->onModelSetChanged();
        }

        CMappingComponent::~CMappingComponent()
        { }

        const CLogCategoryList &CMappingComponent::getLogCategories()
        {
            static const CLogCategoryList cats({ CLogCategory::mapping(), CLogCategory::guiComponent() });
            return cats;
        }

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

        void CMappingComponent::onModelSetChanged()
        {
            if (ui->tvp_AircraftModels->displayAutomatically())
            {
                this->onModelsUpdateRequested();
            }
            else
            {
                CLogMessage(this).info("Models loaded, you can update the model view");
            }
        }

        void CMappingComponent::onRowCountChanged(int count, bool withFilter)
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

        void CMappingComponent::onChangedSimulatedAircraftInView(const CVariant &object, const CPropertyIndex &index)
        {
            if (!index.contains(CSimulatedAircraft::IndexEnabled)) { return; } // we only deal with enabled/disabled here
            const CSimulatedAircraft sa = object.to<CSimulatedAircraft>(); // changed in GUI
            const CSimulatedAircraft saFromBackend = sGui->getIContextNetwork()->getAircraftInRangeForCallsign(sa.getCallsign());
            if (!saFromBackend.hasValidCallsign()) { return; } // obviously deleted
            const bool nowEnabled = sa.isEnabled();
            if (saFromBackend.isEnabled() == nowEnabled) { return; } // already same value
            CLogMessage(this).info("Request to %1 aircraft '%2'") << (nowEnabled ? "enable" : "disable") << saFromBackend.getCallsign().toQString();
            sGui->getIContextNetwork()->updateAircraftEnabled(saFromBackend.getCallsign(), nowEnabled);
        }

        void CMappingComponent::onAircraftSelectedInView(const QModelIndex &index)
        {
            const CSimulatedAircraft simAircraft = ui->tvp_RenderedAircraft->at(index);
            ui->cb_AircraftEnabled->setChecked(simAircraft.isEnabled());
            ui->le_Callsign->setText(simAircraft.getCallsign().asString());
            ui->completer_ModelStrings->setModel(simAircraft.getModel());
        }

        void CMappingComponent::onModelSelectedInView(const QModelIndex &index)
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
                this->onModelPreviewChanged(Qt::Unchecked);
            }
        }

        CCallsign CMappingComponent::validateRenderedCallsign()
        {
            const QString cs = ui->le_Callsign->text().trimmed();
            if (!CCallsign::isValidAircraftCallsign(cs))
            {
                this->showOverlayMessage(CStatusMessage(this).validationError("Invalid callsign for mapping"), OverlayMessageMs);
                return CCallsign();
            }

            const CCallsign callsign(cs);
            const bool hasCallsign = ui->tvp_RenderedAircraft->container().containsCallsign(callsign);
            if (!hasCallsign)
            {
                const CStatusMessage msg = CStatusMessage(this).validationError("Unmapped callsign '%1' for mapping") << callsign.asString();
                this->showOverlayMessage(msg);
                return CCallsign();
            }
            return callsign;
        }

        void CMappingComponent::onSaveAircraft()
        {
            if (!sGui->getIContextSimulator()->isSimulatorSimulating()) { return; }
            const CCallsign callsign(this->validateRenderedCallsign());
            if (callsign.isEmpty()) { return; }
            const QString modelString = ui->completer_ModelStrings->getModelString();
            if (modelString.isEmpty())
            {
                this->showOverlayMessage(CStatusMessage(this).validationError("Missing model for mapping"), OverlayMessageMs);
                return;
            }

            const bool hasModel = ui->tvp_AircraftModels->container().containsModelString(modelString);
            if (!hasModel)
            {
                this->showOverlayMessage(CStatusMessage(this).validationError("Invalid model for mapping, reloading model set"), OverlayMessageMs);
                if (ui->tvp_AircraftModels->isEmpty())
                {
                    this->onModelsUpdateRequested();
                }
                return;
            }

            const CSimulatedAircraft aircraftFromBackend = sGui->getIContextNetwork()->getAircraftInRangeForCallsign(callsign);
            const bool enabled = ui->cb_AircraftEnabled->isChecked();
            bool changed = false;
            if (aircraftFromBackend.getModelString() != modelString)
            {
                const CAircraftModelList models = sGui->getIContextSimulator()->getModelSetModelsStartingWith(modelString);
                if (models.isEmpty())
                {
                    const CStatusMessage msg = CStatusMessage(this).validationError("No model for title: '%1'") << modelString;
                    this->showOverlayMessage(msg, OverlayMessageMs);
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
                        const CStatusMessage msg = CStatusMessage(this).validationInfo("Ambigious title: '%1', using '%2'") << modelString << model.getModelString();
                        this->showOverlayMessage(msg, OverlayMessageMs);
                    }
                }
                model.setModelType(CAircraftModel::TypeManuallySet);
                CLogMessage(this).info("Requesting changes for '%1'") << callsign.asString();
                changed = sGui->getIContextNetwork()->updateAircraftModel(aircraftFromBackend.getCallsign(), model, identifier());
            }
            if (aircraftFromBackend.isEnabled() != enabled)
            {
                changed = sGui->getIContextNetwork()->updateAircraftEnabled(aircraftFromBackend.getCallsign(), enabled);
            }

            if (!changed)
            {
                this->showOverlayMessage(CLogMessage(this).info("Model mapping, nothing to change"), OverlayMessageMs);
            }
        }

        void CMappingComponent::onResetAircraft()
        {
            if (!sGui->getIContextSimulator()->isSimulatorSimulating()) { return; }
            const CCallsign callsign(this->validateRenderedCallsign());
            if (callsign.isEmpty()) { return; }
            const bool reset = sGui->getIContextSimulator()->resetToModelMatchingAircraft(callsign);
            const CStatusMessage msg = reset ?
                                       CStatusMessage(this).info("Model reset for '%1'") << callsign.toQString() :
                                       CStatusMessage(this).info("Reset failed for '%1'") << callsign.toQString();
            this->showOverlayMessage(msg);
        }

        void CMappingComponent::onModelPreviewChanged(int state)
        {
            Q_UNUSED(state);
            this->closeOverlay();
        }

        void CMappingComponent::onModelsUpdateRequested()
        {
            const CAircraftModelList ml(sGui->getIContextSimulator()->getModelSet());
            ui->tvp_AircraftModels->updateContainerMaybeAsync(ml);
        }

        void CMappingComponent::onRemoteAircraftModelChanged(const CSimulatedAircraft &aircraft, const CIdentifier &originator)
        {
            if (CIdentifiable::isMyIdentifier(originator)) { return; }
            this->tokenBucketUpdateAircraft(aircraft);
        }

        void CMappingComponent::onConnectionStatusChanged(INetwork::ConnectionStatus from, INetwork::ConnectionStatus to)
        {
            Q_UNUSED(from);
            if (INetwork::isDisconnectedStatus(to))
            {
                this->tokenBucketUpdate();
                ui->tvp_RenderedAircraft->clear();
            }
        }

        void CMappingComponent::onMenuChangeFastPositionUpdates(const CSimulatedAircraft &aircraft)
        {
            if (sGui->getIContextNetwork())
            {
                sGui->getIContextNetwork()->updateFastPositionEnabled(aircraft.getCallsign(), aircraft.fastPositionUpdates());
            }
        }

        void CMappingComponent::onMenuHighlightInSimulator(const CSimulatedAircraft &aircraft)
        {
            if (sGui->getIContextSimulator())
            {
                sGui->getIContextSimulator()->highlightAircraft(aircraft, true, IContextSimulator::HighlightTime());
            }
        }

        void CMappingComponent::addingRemoteAircraftFailed(const CSimulatedAircraft &aircraft, const CStatusMessage &message)
        {
            this->tokenBucketUpdate();
            Q_UNUSED(aircraft);
            Q_UNUSED(message);
        }

        void CMappingComponent::onMenuToggleEnableAircraft(const CSimulatedAircraft &aircraft)
        {
            if (sGui->getIContextNetwork())
            {
                sGui->getIContextNetwork()->updateAircraftEnabled(aircraft.getCallsign(), aircraft.isEnabled());
            }
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

        void CMappingComponent::timerUpdate()
        {
            // timer update to update position, speed ...
            this->updateRenderedAircraftView(false); // unforced
        }

        void CMappingComponent::tokenBucketUpdateAircraft(const CSimulatedAircraft &aircraft)
        {
            Q_UNUSED(aircraft);
            this->tokenBucketUpdate();
        }

        void CMappingComponent::tokenBucketUpdate()
        {
            if (!m_bucket.tryConsume()) { return; }
            this->updateRenderedAircraftView(true); // forced update
        }

        void CMappingComponent::settingsChanged()
        {
            const CViewUpdateSettings settings = m_settings.get();
            const int ms = settings.getRenderingUpdateTime().toMs();
            m_updateTimer.setInterval(ms);
        }

        void CMappingComponent::connectionStatusChanged(BlackCore::INetwork::ConnectionStatus from, BlackCore::INetwork::ConnectionStatus to)
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
