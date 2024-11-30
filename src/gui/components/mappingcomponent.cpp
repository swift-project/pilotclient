// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/components/mappingcomponent.h"

#include <QCheckBox>
#include <QCompleter>
#include <QLabel>
#include <QLineEdit>
#include <QPixmap>
#include <QPointer>
#include <QPushButton>
#include <QStringListModel>
#include <QtGlobal>

#include "mappingcomponent.h"
#include "ui_mappingcomponent.h"

#include "core/context/contextnetwork.h"
#include "core/context/contextsimulator.h"
#include "gui/guiapplication.h"
#include "gui/guiutility.h"
#include "gui/models/aircraftmodellistmodel.h"
#include "gui/models/simulatedaircraftlistmodel.h"
#include "gui/views/aircraftmodelview.h"
#include "gui/views/checkboxdelegate.h"
#include "gui/views/simulatedaircraftview.h"
#include "gui/views/viewbase.h"
#include "misc/aviation/callsign.h"
#include "misc/icons.h"
#include "misc/logmessage.h"
#include "misc/network/connectionstatus.h"
#include "misc/network/server.h"
#include "misc/pixmap.h"
#include "misc/simulation/aircraftmodel.h"
#include "misc/simulation/simulatedaircraft.h"
#include "misc/simulation/simulatedaircraftlist.h"
#include "misc/statusmessage.h"

using namespace swift::misc;
using namespace swift::misc::simulation;
using namespace swift::misc::network;
using namespace swift::misc::aviation;
using namespace swift::misc::physical_quantities;
using namespace swift::core;
using namespace swift::core::context;
using namespace swift::gui::views;
using namespace swift::gui::models;
using namespace swift::gui::filters;
using namespace swift::gui::settings;

namespace swift::gui::components
{
    const QStringList &CMappingComponent::getLogCategories()
    {
        static const QStringList cats({ CLogCategories::mapping(), CLogCategories::guiComponent() });
        return cats;
    }

    CMappingComponent::CMappingComponent(QWidget *parent)
        : COverlayMessagesFrame(parent), CIdentifiable(this), ui(new Ui::CMappingComponent)
    {
        Q_ASSERT_X(sGui, Q_FUNC_INFO, "need sGui");
        Q_ASSERT_X(sGui->getIContextSimulator(), Q_FUNC_INFO, "need simulator context");
        Q_ASSERT_X(sGui->getIContextNetwork(), Q_FUNC_INFO, "need network context");

        ui->setupUi(this);
        ui->tw_SpecializedViews->setCurrentIndex(0);

        ui->tvp_AircraftModels->setAircraftModelMode(CAircraftModelListModel::OwnAircraftModelClient);
        ui->tvp_AircraftModels->setResizeMode(CAircraftModelView::ResizingOff);
        ui->tvp_AircraftModels->addFilterDialog();
        ui->tvp_AircraftModels->menuRemoveItems(CAircraftModelView::MenuBackend);
        ui->tvp_AircraftModels->menuAddItems(CAircraftModelView::MenuDisableModelsTemp);

        ui->tvp_RenderedAircraft->setAircraftMode(CSimulatedAircraftListModel::RenderedMode);
        ui->tvp_RenderedAircraft->setResizeMode(CAircraftModelView::ResizingOnce);
        ui->tvp_RenderedAircraft->menuAddItems(CAircraftModelView::MenuDisableModelsTemp);

        connect(sGui->getIContextNetwork(), &IContextNetwork::connectionStatusChanged, this,
                &CMappingComponent::onNetworkConnectionStatusChanged);

        connect(ui->tvp_AircraftModels, &CAircraftModelView::requestUpdate, this,
                &CMappingComponent::onModelsUpdateRequested);
        connect(ui->tvp_AircraftModels, &CAircraftModelView::modelDataChanged, this,
                &CMappingComponent::onRowCountChanged);
        connect(ui->tvp_AircraftModels, &CAircraftModelView::clicked, this, &CMappingComponent::onModelSelectedInView);
        connect(ui->tvp_AircraftModels, &CAircraftModelView::requestTempDisableModelsForMatching, this,
                &CMappingComponent::onTempDisableModelsForMatchingRequested);

        connect(ui->tvp_RenderedAircraft, &CSimulatedAircraftView::modelDataChanged, this,
                &CMappingComponent::onRowCountChanged);
        connect(ui->tvp_RenderedAircraft, &CSimulatedAircraftView::clicked, this,
                &CMappingComponent::onAircraftSelectedInView);
        connect(ui->tvp_RenderedAircraft, &CSimulatedAircraftView::requestUpdate, this,
                &CMappingComponent::tokenBucketUpdate);
        connect(ui->tvp_RenderedAircraft, &CSimulatedAircraftView::requestTextMessageWidget, this,
                &CMappingComponent::requestTextMessageWidget);
        connect(ui->tvp_RenderedAircraft, &CSimulatedAircraftView::requestTempDisableModelsForMatching, this,
                &CMappingComponent::onTempDisableModelsForMatchingRequested);

        connect(ui->pb_SaveAircraft, &QPushButton::clicked, this, &CMappingComponent::onSaveAircraft);
        connect(ui->pb_ResetAircraft, &QPushButton::clicked, this, &CMappingComponent::onResetAircraft);
        connect(ui->pb_LoadModels, &QPushButton::clicked, this, &CMappingComponent::onModelsUpdateRequested);
        connect(ui->pb_DoMatchingAgain, &QPushButton::clicked, this, &CMappingComponent::doMatchingsAgain);
        connect(ui->pb_ValidateModelSet, &QPushButton::clicked, this, &CMappingComponent::requestValidationDialog);

        m_currentMappingsViewDelegate = new CCheckBoxDelegate(":/diagona/icons/diagona/icons/tick.png",
                                                              ":/diagona/icons/diagona/icons/cross.png", this);
        ui->tvp_RenderedAircraft->setItemDelegateForColumn(0, m_currentMappingsViewDelegate);

        // overlay
        this->setReducedInfo(true);
        this->setForceSmall(true);
        this->showKillButton(false);

        // Aircraft previews
        connect(ui->cb_AircraftIconDisplayed, &QCheckBox::stateChanged, this,
                &CMappingComponent::onModelPreviewChanged);

        // model string completer
        ui->completer_ModelStrings->setSourceVisible(CAircraftModelStringCompleter::OwnModels, false);
        ui->completer_ModelStrings->selectSource(CAircraftModelStringCompleter::ModelSet);

        // Updates
        connect(&m_updateTimer, &QTimer::timeout, this, &CMappingComponent::timerUpdate);
        m_updateTimer.setObjectName(this->objectName() + "::updateTimer");
        ui->tvp_AircraftModels->setDisplayAutomatically(false);
        this->settingsChanged();

        // selector
        ui->comp_SimulatorSelector->setRememberSelection(false); // pilot client UI
        ui->comp_SimulatorSelector->setMode(CSimulatorSelector::ComboBox);
        ui->comp_SimulatorSelector->setToConnectedSimulator();
        connect(ui->comp_SimulatorSelector, &CSimulatorSelector::changed, this,
                &CMappingComponent::onModelSetSimulatorChanged);

        // connect
        connect(sGui->getIContextSimulator(), &IContextSimulator::modelSetChanged, this,
                &CMappingComponent::onModelSetChanged, Qt::QueuedConnection);
        connect(sGui->getIContextSimulator(), &IContextSimulator::modelMatchingCompleted, this,
                &CMappingComponent::tokenBucketUpdateAircraft, Qt::QueuedConnection);
        connect(sGui->getIContextSimulator(), &IContextSimulator::aircraftRenderingChanged, this,
                &CMappingComponent::tokenBucketUpdateAircraft, Qt::QueuedConnection);
        connect(sGui->getIContextSimulator(), &IContextSimulator::airspaceSnapshotHandled, this,
                &CMappingComponent::tokenBucketUpdate, Qt::QueuedConnection);
        connect(sGui->getIContextSimulator(), &IContextSimulator::addingRemoteModelFailed, this,
                &CMappingComponent::onAddingRemoteAircraftFailed, Qt::QueuedConnection);
        connect(sGui->getIContextSimulator(), &IContextSimulator::simulatorPluginChanged, this,
                &CMappingComponent::onSimulatorPluginChanged, Qt::QueuedConnection);
        connect(sGui->getIContextSimulator(), &IContextSimulator::simulatorStatusChanged, this,
                &CMappingComponent::onSimulatorStatusChanged, Qt::QueuedConnection);
        connect(sGui->getIContextNetwork(), &IContextNetwork::changedRemoteAircraftModel, this,
                &CMappingComponent::onRemoteAircraftModelChanged, Qt::QueuedConnection);
        connect(sGui->getIContextNetwork(), &IContextNetwork::changedRemoteAircraftEnabled, this,
                &CMappingComponent::tokenBucketUpdateAircraft, Qt::QueuedConnection);
        connect(sGui->getIContextNetwork(), &IContextNetwork::changedFastPositionUpdates, this,
                &CMappingComponent::tokenBucketUpdateAircraft, Qt::QueuedConnection);
        connect(sGui->getIContextNetwork(), &IContextNetwork::changedGndFlagCapability, this,
                &CMappingComponent::tokenBucketUpdateAircraft, Qt::QueuedConnection);
        connect(sGui->getIContextNetwork(), &IContextNetwork::removedAircraft, this,
                &CMappingComponent::tokenBucketUpdate, Qt::QueuedConnection);
        connect(sGui->getIContextNetwork(), &IContextNetwork::connectionStatusChanged, this,
                &CMappingComponent::onConnectionStatusChanged, Qt::QueuedConnection);

        connect(ui->tw_SpecializedViews, &QTabWidget::currentChanged, this, &CMappingComponent::onTabWidgetChanged);

        // requires simulator context
        connect(ui->tvp_RenderedAircraft, &CAircraftModelView::objectChanged, this,
                &CMappingComponent::onChangedSimulatedAircraftInView, Qt::QueuedConnection);

        // with external core models might be already available
        // nevertheless, wait some time to allow to init
        QPointer<CMappingComponent> myself(this);
        QTimer::singleShot(10000, this, [=] {
            if (!myself || !sGui || sGui->isShuttingDown()) { return; }
            const CSimulatorInfo simulator(myself->getConnectedOrSelectedSimulator());
            myself->onModelSetSimulatorChanged(simulator);
            myself->onModelSetChanged(simulator);
        });
    }

    CMappingComponent::~CMappingComponent() {}

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

    void CMappingComponent::setTab(CMappingComponent::TabWidget tab)
    {
        const int tabIndex = static_cast<int>(tab);
        ui->tw_SpecializedViews->setCurrentIndex(tabIndex);
    }

    CAircraftModelList CMappingComponent::findModelsStartingWith(const QString &modelName, Qt::CaseSensitivity cs)
    {
        Q_ASSERT(ui->tvp_AircraftModels);
        return ui->tvp_AircraftModels->container().findModelsStartingWith(modelName, cs);
    }

    void CMappingComponent::onModelSetChanged(const CSimulatorInfo &dummy)
    {
        // changed model set, which can be any model set
        Q_UNUSED(dummy); // we do not use the passed simulator

        const CSimulatorInfo simulator(ui->comp_SimulatorSelector->getValue()); // UI value
        const bool changed = ui->completer_ModelStrings->setSimulator(simulator);
        if (!changed) { return; }

        if (ui->tvp_AircraftModels->displayAutomatically()) { this->onModelsUpdateRequested(); }
        else
        {
            CLogMessage(this).info(u"Model set loaded ('%1'), you can update the model view")
                << simulator.toQString(true);
        }
    }

    void CMappingComponent::onRowCountChanged(int count, bool withFilter)
    {
        Q_UNUSED(count);
        Q_UNUSED(withFilter);
        const int am = ui->tw_SpecializedViews->indexOf(ui->tb_AircraftModels);
        const int cm = ui->tw_SpecializedViews->indexOf(ui->tb_CurrentMappings);
        const QString amf = ui->tvp_AircraftModels->derivedModel()->hasFilter() ? "F" : "";
        QString a = ui->tw_SpecializedViews->tabBar()->tabText(am);
        QString c = ui->tw_SpecializedViews->tabBar()->tabText(cm);
        a = CGuiUtility::replaceTabCountValue(a, this->countAircraftModels()) + amf;
        c = CGuiUtility::replaceTabCountValue(c, this->countCurrentMappings());
        ui->tw_SpecializedViews->tabBar()->setTabText(am, a);
        ui->tw_SpecializedViews->tabBar()->setTabText(cm, c);
    }

    void CMappingComponent::onChangedSimulatedAircraftInView(const CVariant &object, const CPropertyIndex &index)
    {
        if (!index.contains(CSimulatedAircraft::IndexEnabled)) { return; } // we only deal with enabled/disabled here
        const CSimulatedAircraft sa = object.to<CSimulatedAircraft>(); // changed in GUI
        const CSimulatedAircraft saFromBackend =
            sGui->getIContextNetwork()->getAircraftInRangeForCallsign(sa.getCallsign());
        if (!saFromBackend.hasValidCallsign()) { return; } // obviously deleted
        const bool nowEnabled = sa.isEnabled();
        if (saFromBackend.isEnabled() == nowEnabled) { return; } // already same value
        CLogMessage(this).info(u"Request to %1 aircraft '%2'")
            << (nowEnabled ? "enable" : "disable") << saFromBackend.getCallsign().toQString();
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
            if (pm.isNull()) { this->closeOverlay(); }
            else { this->showOverlayImage(pm); }
        }
        else { this->onModelPreviewChanged(Qt::Unchecked); }
    }

    CCallsign CMappingComponent::validateRenderedCallsign()
    {
        const QString cs = ui->le_Callsign->text().trimmed();
        if (!CCallsign::isValidAircraftCallsign(cs))
        {
            this->showOverlayMessage(CStatusMessage(this).validationError(u"Invalid callsign for mapping"),
                                     OverlayMessageMs);
            return CCallsign();
        }

        const CCallsign callsign(cs);
        const bool hasCallsign = ui->tvp_RenderedAircraft->container().containsCallsign(callsign);
        if (!hasCallsign)
        {
            const CStatusMessage msg = CStatusMessage(this).validationError(u"Unmapped callsign '%1' for mapping")
                                       << callsign.asString();
            this->showOverlayMessage(msg);
            return CCallsign();
        }
        return callsign;
    }

    void CMappingComponent::onModelSetSimulatorChanged(const CSimulatorInfo &simulator)
    {
        if (!sGui || this->isSimulatorAvailable()) { return; }
        sGui->getIContextSimulator()->setModelSetLoaderSimulator(simulator);

        // completer will be changed in onModelSetChanged
    }

    void CMappingComponent::onSimulatorPluginChanged(const CSimulatorPluginInfo &pluginInfo)
    {
        Q_UNUSED(pluginInfo);
        ui->comp_SimulatorSelector->setToConnectedSimulator(50);
    }

    void CMappingComponent::onSimulatorStatusChanged(int status)
    {
        const ISimulator::SimulatorStatus simStatus = static_cast<ISimulator::SimulatorStatus>(status);

        // make sure the selector represents connected simulator
        if (simStatus.testFlag(ISimulator::Connected) && sGui && sGui->getIContextSimulator())
        {
            ui->comp_SimulatorSelector->setToConnectedSimulator(50);
        }
    }

    void CMappingComponent::doMatchingsAgain()
    {
        using namespace std::chrono_literals;

        if (!sGui || sGui->isShuttingDown() || !sGui->getIContextSimulator()) { return; }
        if (!sGui->getIContextSimulator()->isSimulatorAvailable()) { return; }
        const int rematchedNumber = sGui->getIContextSimulator()->doMatchingsAgain();

        const QString info = QStringLiteral("Triggered re-matching of %1 aircraft").arg(rematchedNumber);
        this->showOverlayHTMLMessage(info, 5s);
        CLogMessage(this).info(info);
    }

    void CMappingComponent::onSaveAircraft()
    {
        if (!sGui || !sGui->getIContextSimulator() || !sGui->getIContextSimulator()->isSimulatorSimulating())
        {
            return;
        }
        const CCallsign callsign(this->validateRenderedCallsign());
        if (callsign.isEmpty()) { return; }
        const QString modelString = ui->completer_ModelStrings->getModelString();
        if (modelString.isEmpty())
        {
            this->showOverlayHTMLMessage(CStatusMessage(this).validationError(u"Missing model for mapping"),
                                         OverlayMessageMs);
            return;
        }

        // model in current view (data already in UI)
        bool hasModel = ui->tvp_AircraftModels->container().containsModelString(modelString);
        if (!hasModel && sGui && sGui->getIContextSimulator())
        {
            hasModel = sGui->getIContextSimulator()->isKnownModelInSet(modelString);
        }

        if (!hasModel)
        {
            this->showOverlayMessage(
                CStatusMessage(this).validationError(u"Invalid model for mapping, reloading model set"),
                OverlayMessageMs);
            if (ui->tvp_AircraftModels->isEmpty()) { this->onModelsUpdateRequested(); }
            return;
        }

        const CSimulatedAircraft aircraftFromBackend =
            sGui->getIContextNetwork()->getAircraftInRangeForCallsign(callsign);
        const bool enabled = ui->cb_AircraftEnabled->isChecked();
        bool changed = false;

        // changed model?
        if (aircraftFromBackend.getModelString() != modelString)
        {
            const CAircraftModelList models = sGui->getIContextSimulator()->getModelSetModelsStartingWith(modelString);
            if (models.isEmpty())
            {
                const CStatusMessage msg = CStatusMessage(this).validationError(u"No model for title: '%1'")
                                           << modelString;
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
                    const CStatusMessage msg = CStatusMessage(this).validationInfo(u"Ambigious title: '%1', using '%2'")
                                               << modelString << model.getModelString();
                    this->showOverlayMessage(msg, OverlayMessageMs);
                }
            }
            model.setModelType(CAircraftModel::TypeManuallySet);
            CLogMessage(this).info(u"Requesting changes for '%1'") << callsign.asString();

            // enable in any case
            sGui->getIContextNetwork()->updateAircraftEnabled(aircraftFromBackend.getCallsign(), true);
            changed =
                sGui->getIContextNetwork()->updateAircraftModel(aircraftFromBackend.getCallsign(), model, identifier());
        }
        if (aircraftFromBackend.isEnabled() != enabled)
        {
            changed = sGui->getIContextNetwork()->updateAircraftEnabled(aircraftFromBackend.getCallsign(), enabled);
        }

        if (!changed)
        {
            this->showOverlayHTMLMessage(CLogMessage(this).info(u"Model mapping, nothing to change"), OverlayMessageMs);
        }
    }

    void CMappingComponent::onResetAircraft()
    {
        using namespace std::chrono_literals;

        if (!sGui || !sGui->getIContextSimulator() || !sGui->getIContextSimulator()->isSimulatorSimulating())
        {
            return;
        }
        const CCallsign callsign(this->validateRenderedCallsign());
        if (callsign.isEmpty()) { return; }
        const bool reset = sGui->getIContextSimulator()->resetToModelMatchingAircraft(callsign);
        const CStatusMessage msg = reset ? CStatusMessage(this).info(u"Model reset for '%1'") << callsign.toQString() :
                                           CStatusMessage(this).info(u"Reset failed for '%1'") << callsign.toQString();
        this->showOverlayHTMLMessage(msg, 3s);
    }

    void CMappingComponent::onModelPreviewChanged(int state)
    {
        Q_UNUSED(state);
        this->closeOverlay();
    }

    void CMappingComponent::onModelsUpdateRequested()
    {
        if (!sGui || sGui->isShuttingDown() || !sGui->getIContextSimulator()) { return; }
        CAircraftModelList modelSet(sGui->getIContextSimulator()->getModelSet());

        const CAircraftModelList disabledModels = sGui->getIContextSimulator()->getDisabledModelsForMatching();
        const bool hasDisabledModels = !disabledModels.isEmpty();

        ui->tvp_AircraftModels->setHighlightColor(Qt::red);
        ui->tvp_AircraftModels->setHighlight(hasDisabledModels);
        ui->tvp_AircraftModels->setHighlightModels(disabledModels);
        ui->tvp_AircraftModels->updateContainerMaybeAsync(modelSet);
        ui->tw_SpecializedViews->setCurrentIndex(TabAircraftModels);
    }

    void CMappingComponent::onTempDisableModelsForMatchingRequested(const CAircraftModelList &models)
    {
        if (models.isEmpty()) { return; }
        if (sGui && sGui->getIContextSimulator())
        {
            sGui->getIContextSimulator()->disableModelsForMatching(models, true);
            const CStatusMessage m = CLogMessage(this).info(u"Disabled %1 model(s): %2")
                                     << models.size() << models.getCallsignsAsString(", ", true);
            this->showOverlayHTMLMessage(m, OverlayMessageMs);
            this->onModelsUpdateRequested();
        }
    }

    void CMappingComponent::onRemoteAircraftModelChanged(const CSimulatedAircraft &aircraft,
                                                         const CIdentifier &originator)
    {
        if (CIdentifiable::isMyIdentifier(originator)) { return; }
        this->tokenBucketUpdateAircraft(aircraft);
    }

    void CMappingComponent::onConnectionStatusChanged(const CConnectionStatus &from, const CConnectionStatus &to)
    {
        Q_UNUSED(from);
        if (to.isDisconnected())
        {
            this->tokenBucketUpdate();
            ui->tvp_RenderedAircraft->clear();
        }
        else if (to.isConnected())
        {
            if (sGui && sGui->getIContextNetwork())
            {
                const CServer server = sGui->getIContextNetwork()->getConnectedServer();
                const bool sendFast = server.getFsdSetup().sendInterimPositions();
                ui->tvp_RenderedAircraft->configureMenuFastPositionUpdates(sendFast);
            }
        }
    }

    CSimulatorInfo CMappingComponent::getConnectedOrSelectedSimulator() const
    {
        if (this->isSimulatorAvailable()) { return sGui->getIContextSimulator()->isSimulatorAvailable(); }
        return ui->comp_SimulatorSelector->getValue();
    }

    bool CMappingComponent::isSimulatorAvailable() const
    {
        if (!sGui || !sGui->getIContextSimulator()) { return false; }
        return sGui->getIContextSimulator()->isSimulatorAvailable();
    }

    void CMappingComponent::showAircraftModelDetails(bool show)
    {
        QList<int> sizes = ui->sp_MappingComponentSplitter->sizes();
        Q_ASSERT_X(sizes.size() == 2, Q_FUNC_INFO, "Wrong splitter sizes");
        const int total = sizes[0] + sizes[1];
        QList<int> newSizes({ 0, 0 });
        if (show)
        {
            newSizes[0] = qRound(total * 0.8);
            newSizes[1] = qRound(total * 0.2);
        }
        else
        {
            newSizes[0] = total;
            newSizes[1] = 0;
        }
        ui->sp_MappingComponentSplitter->setSizes(newSizes);
    }

    void CMappingComponent::onAddingRemoteAircraftFailed(const CSimulatedAircraft &aircraft, bool disabled,
                                                         bool failover, const CStatusMessage &message)
    {
        this->tokenBucketUpdate();
        Q_UNUSED(failover);
        Q_UNUSED(aircraft);
        Q_UNUSED(message);
        Q_UNUSED(disabled);
    }

    void CMappingComponent::onTabWidgetChanged(int index)
    {
        Q_UNUSED(index);
        const TabWidget w = static_cast<TabWidget>(index);
        const bool show = (w == TabAircraftModels) || (w == TabRenderedAircraft);
        this->showAircraftModelDetails(show);
    }

    void CMappingComponent::updateRenderedAircraftView(bool forceUpdate)
    {
        if (!sGui || sGui->isShuttingDown()) { return; }
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
            ui->tvp_RenderedAircraft->updateContainerMaybeAsync(aircraft);
        }
        else { ui->tvp_RenderedAircraft->clear(); }
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

    void CMappingComponent::onNetworkConnectionStatusChanged(const CConnectionStatus &from, const CConnectionStatus &to)
    {
        Q_UNUSED(from);
        if (to.isDisconnected())
        {
            ui->tvp_RenderedAircraft->clear();
            m_updateTimer.stop();
        }
        else if (to.isConnected())
        {
            ui->comp_SimulatorSelector->setReadOnly(true);
            m_updateTimer.start();
        }
    }
} // namespace swift::gui::components
