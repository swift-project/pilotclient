/* Copyright (C) 2015
 * swift project Community / Contributors
 *
 * This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackcore/contextsimulator.h"
#include "blackcore/contextnetwork.h"
#include "blackcore/network.h"
#include "blackgui/views/aircraftmodelview.h"
#include "blackgui/filters/aircraftmodelfilterdialog.h"
#include "blackgui/models/aircraftmodellistmodel.h"
#include "blackgui/guiutility.h"
#include "blackmisc/propertyindexlist.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/pixmap.h"
#include "mappingcomponent.h"
#include "ui_mappingcomponent.h"
#include <QStringListModel>

using namespace BlackMisc;
using namespace BlackMisc::Simulation;
using namespace BlackMisc::Network;
using namespace BlackMisc::Aviation;
using namespace BlackMisc::PhysicalQuantities;
using namespace BlackCore;
using namespace BlackGui;
using namespace BlackGui::Views;
using namespace BlackGui::Models;
using namespace BlackGui::Filters;

namespace BlackGui
{
    namespace Components
    {
        CMappingComponent::CMappingComponent(QWidget *parent) :
            QFrame(parent),
            ui(new Ui::CMappingComponent),
            m_updateTimer(new CUpdateTimer("CMappingComponent", &CMappingComponent::ps_backgroundUpdate, this))
        {
            ui->setupUi(this);
            this->ui->tvp_AircraftModels->setAircraftModelMode(CAircraftModelListModel::OwnSimulatorModel);
            this->ui->tvp_AircraftModels->setResizeMode(CAircraftModelView::ResizingOff);
            this->ui->tvp_AircraftModels->addFilterDialog();

            this->ui->tvp_SimulatedAircraft->setAircraftMode(CSimulatedAircraftListModel::ModelMode);
            this->ui->tvp_SimulatedAircraft->setResizeMode(CAircraftModelView::ResizingOnce);

            connect(this->ui->tvp_AircraftModels, &CAircraftModelView::requestUpdate, this, &CMappingComponent::ps_onModelsUpdateRequested);
            connect(this->ui->tvp_AircraftModels, &CAircraftModelView::rowCountChanged, this, &CMappingComponent::ps_onRowCountChanged);
            connect(this->ui->tvp_AircraftModels, &CAircraftModelView::clicked, this, &CMappingComponent::ps_onModelSelectedInView);
            connect(this->ui->tvp_AircraftModels, &CAircraftModelView::requestNewBackendData, this, &CMappingComponent::ps_onMenuRequestModelReload);

            connect(this->ui->tvp_SimulatedAircraft, &CSimulatedAircraftView::rowCountChanged, this, &CMappingComponent::ps_onRowCountChanged);
            connect(this->ui->tvp_SimulatedAircraft, &CSimulatedAircraftView::clicked, this, &CMappingComponent::ps_onAircraftSelectedInView);
            connect(this->ui->tvp_SimulatedAircraft, &CSimulatedAircraftView::requestUpdate, this, &CMappingComponent::ps_onSimulatedAircraftUpdateRequested);
            connect(this->ui->tvp_SimulatedAircraft, &CSimulatedAircraftView::requestTextMessageWidget, this, &CMappingComponent::requestTextMessageWidget);
            connect(this->ui->tvp_SimulatedAircraft, &CSimulatedAircraftView::requestEnableAircraft, this, &CMappingComponent::ps_onMenuEnableAircraft);
            connect(this->ui->tvp_SimulatedAircraft, &CSimulatedAircraftView::requestFastPositionUpdates, this, &CMappingComponent::ps_onMenuChangeFastPositionUpdates);
            connect(this->ui->tvp_SimulatedAircraft, &CSimulatedAircraftView::requestHighlightInSimulator, this, &CMappingComponent::ps_onMenuHighlightInSimulator);

            connect(this->ui->pb_SaveAircraft, &QPushButton::clicked, this, &CMappingComponent::ps_onSaveAircraft);

            this->m_modelCompleter = new QCompleter(this);
            this->m_currentMappingsViewDelegate = new CCheckBoxDelegate(":/diagona/icons/diagona/icons/tick.png", ":/diagona/icons/diagona/icons/cross.png", this);
            this->ui->tvp_SimulatedAircraft->setItemDelegateForColumn(0, this->m_currentMappingsViewDelegate);

            // Aircraft previews
            connect(this->ui->cb_AircraftIconDisplayed, &QCheckBox::stateChanged, this, &CMappingComponent::ps_onModelPreviewChanged);
            this->ui->lbl_AircraftIconDisplayed->setText("Icon displayed here");

            // Updates
            this->ui->tvp_AircraftModels->setDisplayAutomatically(false);
            this->m_updateTimer->setUpdateInterval(10 * 1000);
        }

        CMappingComponent::~CMappingComponent()
        { }

        int CMappingComponent::countCurrentMappings() const
        {
            Q_ASSERT(this->ui->tvp_SimulatedAircraft);
            return this->ui->tvp_SimulatedAircraft->rowCount();
        }

        int CMappingComponent::countAircraftModels() const
        {
            Q_ASSERT(this->ui->tvp_AircraftModels);
            return this->ui->tvp_AircraftModels->rowCount();
        }

        CAircraftModelList CMappingComponent::findModelsStartingWith(const QString modelName, Qt::CaseSensitivity cs)
        {
            Q_ASSERT(this->ui->tvp_AircraftModels);
            return this->ui->tvp_AircraftModels->container().findModelsStartingWith(modelName, cs);
        }

        void CMappingComponent::runtimeHasBeenSet()
        {
            Q_ASSERT(getIContextSimulator());
            Q_ASSERT(getIContextNetwork());
            connect(getIContextSimulator(), &IContextSimulator::installedAircraftModelsChanged, this, &CMappingComponent::ps_onAircraftModelsLoaded);
            connect(getIContextSimulator(), &IContextSimulator::modelMatchingCompleted, this, &CMappingComponent::ps_onModelMatchingCompleted);
            connect(getIContextSimulator(), &IContextSimulator::airspaceSnapshotHandled, this, &CMappingComponent::ps_onAirspaceSnapshotHandled);
            connect(getIContextNetwork(), &IContextNetwork::changedRemoteAircraftModel, this, &CMappingComponent::ps_onRemoteAircraftModelChanged);
            connect(getIContextNetwork(), &IContextNetwork::changedRemoteAircraftEnabled, this, &CMappingComponent::ps_onChangedAircraftEnabled);
            connect(getIContextNetwork(), &IContextNetwork::changedFastPositionUpdates, this, &CMappingComponent::ps_onFastPositionUpdatesEnabled);
            connect(getIContextNetwork(), &IContextNetwork::connectionStatusChanged, this, &CMappingComponent::ps_onConnectionStatusChanged);

            // requires simulator context
            connect(this->ui->tvp_SimulatedAircraft, &CAircraftModelView::objectChanged, this, &CMappingComponent::ps_onChangedSimulatedAircraftInView);

            // with external core models might be already available
            this->ps_onAircraftModelsLoaded();
        }

        void CMappingComponent::ps_onAircraftModelsLoaded()
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

        void CMappingComponent::ps_onModelMatchingCompleted(const BlackMisc::Simulation::CSimulatedAircraft &aircraft)
        {
            Q_UNUSED(aircraft);
            this->ps_onSimulatedAircraftUpdateRequested();
        }

        void CMappingComponent::ps_onRowCountChanged(int count, bool withFilter)
        {
            Q_UNUSED(count);
            Q_UNUSED(withFilter);
            int am = this->ui->tw_ListViews->indexOf(this->ui->tb_AircraftModels);
            int cm = this->ui->tw_ListViews->indexOf(this->ui->tb_CurrentMappings);
            QString amf = this->ui->tvp_AircraftModels->derivedModel()->hasFilter() ? "F" : "";
            QString a = this->ui->tw_ListViews->tabBar()->tabText(am);
            QString c = this->ui->tw_ListViews->tabBar()->tabText(cm);
            a = CGuiUtility::replaceTabCountValue(a, this->countAircraftModels()) + amf;
            c = CGuiUtility::replaceTabCountValue(c, this->countCurrentMappings());
            this->ui->tw_ListViews->tabBar()->setTabText(am, a);
            this->ui->tw_ListViews->tabBar()->setTabText(cm, c);
        }

        void CMappingComponent::ps_onChangedSimulatedAircraftInView(const BlackMisc::CVariant &object, const BlackMisc::CPropertyIndex &index)
        {
            const CSimulatedAircraft sa = object.to<CSimulatedAircraft>(); // changed in GUI
            const CSimulatedAircraft saFromBackend = this->getIContextNetwork()->getAircraftInRangeForCallsign(sa.getCallsign());
            if (!saFromBackend.hasValidCallsign()) { return; } // obviously deleted
            if (index.contains(CSimulatedAircraft::IndexEnabled))
            {
                bool enabled = sa.propertyByIndex(index).toBool();
                if (saFromBackend.isEnabled() == enabled) { return; }
                CLogMessage(this).info("Request to %1 aircraft %2") << (enabled ? "enable" : "disable") << saFromBackend.getCallsign().toQString();
                this->getIContextNetwork()->updateAircraftEnabled(saFromBackend.getCallsign(), enabled, mappingIdentifier());
            }
            else
            {
                Q_ASSERT_X(false, "ps_onChangedSimulatedAircraftInView", "Index not supported");
            }
        }

        void CMappingComponent::ps_onAircraftSelectedInView(const QModelIndex &index)
        {
            const CSimulatedAircraft simAircraft = this->ui->tvp_SimulatedAircraft->at(index);
            this->ui->cb_AircraftEnabled->setChecked(simAircraft.isEnabled());
            this->ui->le_Callsign->setText(simAircraft.getCallsign().asString());
            this->ui->le_AircraftModel->setText(simAircraft.getModel().getModelString());
        }

        void CMappingComponent::ps_onModelSelectedInView(const QModelIndex &index)
        {
            Q_ASSERT(this->getIContextSimulator());
            const CAircraftModel model = this->ui->tvp_AircraftModels->at(index);
            this->ui->le_AircraftModel->setText(model.getModelString());

            if (this->ui->cb_AircraftIconDisplayed->isChecked())
            {
                const int MaxHeight = 125;
                this->ui->lbl_AircraftIconDisplayed->setText("");
                this->ui->lbl_AircraftIconDisplayed->setToolTip(model.getDescription());
                QString modelString(model.getModelString());
                CPixmap pm =  this->getIContextSimulator()->iconForModel(modelString);
                if (pm.isNull())
                {
                    this->ui->lbl_AircraftIconDisplayed->setPixmap(CIcons::crossWhite16());
                }
                else
                {
                    QPixmap qpm = pm.pixmap();
                    if (qpm.height() > MaxHeight)
                    {
                        qpm = qpm.scaledToWidth(MaxHeight, Qt::SmoothTransformation);
                    }
                    this->ui->lbl_AircraftIconDisplayed->setPixmap(qpm);
                }
            }
            else
            {
                this->ps_onModelPreviewChanged(Qt::Unchecked);
            }
        }

        void CMappingComponent::ps_onSaveAircraft()
        {
            Q_ASSERT(getIContextSimulator());
            QString cs = ui->le_Callsign->text().trimmed();
            if (!CCallsign::isValidAircraftCallsign(cs))
            {
                CLogMessage(this).validationError("Invalid callsign for mapping");
                return;
            }

            const CCallsign callsign(cs);
            bool hasCallsign = this->ui->tvp_SimulatedAircraft->container().containsCallsign(callsign);
            if (!hasCallsign)
            {
                CLogMessage(this).validationError("Unmapped callsign %1 for mapping") << callsign.asString();
                return;
            }

            QString modelString = this->ui->le_AircraftModel->text().trimmed();
            if (modelString.isEmpty())
            {
                CLogMessage(this).validationError("Missing model for mapping");
                return;
            }

            bool hasModel = this->ui->tvp_AircraftModels->container().containsModelString(modelString);
            if (!hasModel)
            {
                CLogMessage(this).validationError("Invalid model for mapping");
                return;
            }

            CSimulatedAircraft aircraftFromBackend = this->getIContextNetwork()->getAircraftInRangeForCallsign(callsign);
            bool enabled = this->ui->cb_AircraftEnabled->isChecked();
            bool changed = false;
            if (aircraftFromBackend.getModelString() != modelString)
            {
                CAircraftModelList models = this->getIContextSimulator()->getInstalledModelsStartingWith(modelString);
                if (models.isEmpty())
                {
                    CLogMessage(this).validationError("No model for title: %1") << modelString;
                    return;
                }
                else if (models.size() > 1)
                {
                    CLogMessage(this).validationError("Ambigious title: %1") << modelString;
                    return;
                }
                CAircraftModel model(models.front());
                model.setModelType(CAircraftModel::TypeManuallySet);
                CLogMessage(this).info("Requesting changes for %1") << callsign.asString();
                this->getIContextNetwork()->updateAircraftModel(aircraftFromBackend.getCallsign(), model, mappingIdentifier());
                changed = true;
            }
            if (aircraftFromBackend.isEnabled() != enabled)
            {
                this->getIContextNetwork()->updateAircraftEnabled(aircraftFromBackend.getCallsign(), enabled, mappingIdentifier());
                changed = true;
            }

            if (!changed)
            {
                CLogMessage(this).info("Model mapping, nothing to change");
                return;
            }
        }

        void CMappingComponent::ps_onModelPreviewChanged(int state)
        {
            static const QPixmap empty;
            Qt::CheckState s = static_cast<Qt::CheckState>(state);
            if (s == Qt::Unchecked)
            {
                this->ui->lbl_AircraftIconDisplayed->setPixmap(empty);
                this->ui->lbl_AircraftIconDisplayed->setText("Icon disabled");
            }
            else if (s == Qt::Checked)
            {
                this->ui->lbl_AircraftIconDisplayed->setPixmap(empty);
                this->ui->lbl_AircraftIconDisplayed->setText("Icon will go here");
            }
        }

        void CMappingComponent::ps_onSimulatedAircraftUpdateRequested()
        {
            this->updateSimulatedAircraftView();
        }

        void CMappingComponent::ps_onModelsUpdateRequested()
        {
            Q_ASSERT(getIContextSimulator());
            CAircraftModelList ml(getIContextSimulator()->getInstalledModels());
            this->ui->tvp_AircraftModels->updateContainer(ml);

            // model completer
            this->m_modelCompleter->setModel(new QStringListModel(ml.getModelStrings(), this->m_modelCompleter));
            this->m_modelCompleter->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
            this->m_modelCompleter->setCaseSensitivity(Qt::CaseInsensitive);
            this->m_modelCompleter->setWrapAround(true);
            this->m_modelCompleter->setCompletionMode(QCompleter::InlineCompletion);
            this->ui->le_AircraftModel->setCompleter(this->m_modelCompleter);
        }

        void CMappingComponent::ps_onRemoteAircraftModelChanged(const CSimulatedAircraft &aircraft, const CIdentifier &originator)
        {
            this->updateSimulatedAircraftView();
            Q_UNUSED(originator);
            Q_UNUSED(aircraft);
        }

        void CMappingComponent::ps_onChangedAircraftEnabled(const CSimulatedAircraft &aircraft, const CIdentifier &originator)
        {
            this->updateSimulatedAircraftView();
            Q_UNUSED(originator);
            Q_UNUSED(aircraft);
        }

        void CMappingComponent::ps_onFastPositionUpdatesEnabled(const CSimulatedAircraft &aircraft, const CIdentifier &originator)
        {
            this->updateSimulatedAircraftView();
            Q_UNUSED(originator);
            Q_UNUSED(aircraft);
        }

        void CMappingComponent::ps_onConnectionStatusChanged(INetwork::ConnectionStatus from, INetwork::ConnectionStatus to)
        {
            Q_UNUSED(from);
            if (INetwork::isDisconnectedStatus(to))
            {
                this->ui->tvp_SimulatedAircraft->clear();
            }
        }

        void CMappingComponent::ps_onAirspaceSnapshotHandled()
        {
            this->updateSimulatedAircraftView();
        }

        void CMappingComponent::ps_onMenuChangeFastPositionUpdates(const CSimulatedAircraft &aircraft)
        {
            if (getIContextNetwork())
            {
                getIContextNetwork()->updateFastPositionEnabled(aircraft.getCallsign(), aircraft.fastPositionUpdates(), mappingIdentifier());
            }
        }

        void CMappingComponent::ps_onMenuHighlightInSimulator(const CSimulatedAircraft &aircraft)
        {
            if (getIContextSimulator())
            {
                getIContextSimulator()->highlightAircraft(aircraft, true, IContextSimulator::HighlightTime());
            }
        }

        void CMappingComponent::ps_onMenuEnableAircraft(const CSimulatedAircraft &aircraft)
        {
            if (getIContextNetwork())
            {
                getIContextNetwork()->updateAircraftEnabled(aircraft.getCallsign(), aircraft.isEnabled(), mappingIdentifier());
            }
        }

        void CMappingComponent::ps_onMenuRequestModelReload()
        {
            if (this->getIContextSimulator())
            {
                this->getIContextSimulator()->reloadInstalledModels();
                CLogMessage(this).info("Requested to reload simulator aircraft models");
            }
        }

        CIdentifier CMappingComponent::mappingIdentifier()
        {
            if (m_identifier.getName().isEmpty())
                m_identifier = CIdentifier(QStringLiteral("MAPPINGCOMPONENT"));

            return m_identifier;
        }

        void CMappingComponent::updateSimulatedAircraftView(bool forceUpdate)
        {
            Q_ASSERT_X(getIContextNetwork(), Q_FUNC_INFO, "missing network context");
            Q_ASSERT_X(getIContextSimulator(), Q_FUNC_INFO, "missing simulator context");
            if (!forceUpdate && !this->isVisibleWidget())
            {
                m_missedSimulatedAircraftUpdate = true;
                return;
            }

            m_missedSimulatedAircraftUpdate = false;
            if (getIContextSimulator()->getSimulatorStatus() > 0)
            {
                const CSimulatedAircraftList aircraft(getIContextNetwork()->getAircraftInRange());
                this->ui->tvp_SimulatedAircraft->updateContainer(aircraft);
            }
            else
            {
                this->ui->tvp_SimulatedAircraft->clear();
            }
        }

        void CMappingComponent::ps_backgroundUpdate()
        {
            if (this->m_missedSimulatedAircraftUpdate)
            {
                // update, normally when view is invisible,
                // but we want an update from time to have some data when user switches to view
                this->updateSimulatedAircraftView(true);
            }
        }

    } // namespace
} // namespace
