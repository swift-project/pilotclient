// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/components/modelmatchercomponent.h"

#include <QCheckBox>
#include <QCompleter>
#include <QLineEdit>
#include <QPushButton>
#include <QStringBuilder>
#include <QStringList>
#include <QTabWidget>
#include <QTextEdit>
#include <QtGlobal>

#include "ui_modelmatchercomponent.h"

#include "core/webdataservices.h"
#include "gui/components/aircraftcombinedtypeselector.h"
#include "gui/components/dbaircrafticaoselectorcomponent.h"
#include "gui/components/dbairlineicaoselectorcomponent.h"
#include "gui/components/dbliveryselectorcomponent.h"
#include "gui/components/settingsmatchingdialog.h"
#include "gui/components/simulatorselector.h"
#include "gui/guiapplication.h"
#include "gui/guiutility.h"
#include "gui/models/statusmessagelistmodel.h"
#include "gui/uppercasevalidator.h"
#include "gui/views/aircraftmodelview.h"
#include "gui/views/statusmessageview.h"
#include "misc/aviation/aircrafticaocode.h"
#include "misc/aviation/airlineicaocode.h"
#include "misc/aviation/callsign.h"
#include "misc/aviation/livery.h"
#include "misc/network/user.h"
#include "misc/simulation/data/modelcaches.h"
#include "misc/simulation/matchingutils.h"
#include "misc/statusmessagelist.h"

using namespace swift::misc;
using namespace swift::misc::aviation;
using namespace swift::misc::simulation;
using namespace swift::misc::simulation::data;
using namespace swift::misc::network;
using namespace swift::gui::models;
using namespace swift::gui::views;
using namespace swift::core;

namespace swift::gui::components
{
    CModelMatcherComponent::CModelMatcherComponent(QWidget *parent) : QFrame(parent), ui(new Ui::CModelMatcherComponent)
    {
        Q_ASSERT_X(sGui, Q_FUNC_INFO, "Missing sGui");
        Q_ASSERT_X(sGui->getWebDataServices(), Q_FUNC_INFO, "Missing web services");

        ui->setupUi(this);
        ui->comp_SimulatorSelector->setMode(CSimulatorSelector::RadioButtons);
        ui->comp_SimulatorSelector->setRememberSelectionAndSetToLastSelection();
        ui->comp_AirlineSelector->displayWithIcaoDescription(false);
        ui->comp_AircraftSelector->displayWithIcaoDescription(false);
        ui->comp_LiverySelector->withLiveryDescription(false);
        ui->tvp_ResultMessages->setMode(CStatusMessageListModel::Simplified);
        ui->tvp_ResultMessages->menuAddItems(CViewBaseNonTemplate::MenuSave);

        const CUpperCaseValidator *validator = new CUpperCaseValidator(this);
        ui->le_ModelString->setValidator(validator);
        ui->le_Manufacturer->setValidator(validator);
        ui->le_Callsign->setValidator(validator);

        connect(ui->comp_SimulatorSelector, &CSimulatorSelector::changed, this,
                &CModelMatcherComponent::onSimulatorChanged);
        connect(sGui->getWebDataServices(), &CWebDataServices::dataRead, this, &CModelMatcherComponent::onWebDataRead,
                Qt::QueuedConnection);

        connect(ui->pb_ModelMatching, &QPushButton::pressed, this, &CModelMatcherComponent::testModelMatching);
        connect(ui->pb_ReverseLookup, &QPushButton::pressed, this, &CModelMatcherComponent::reverseLookup);
        connect(ui->pb_Settings, &QPushButton::pressed, this, &CModelMatcherComponent::displaySettingsDialog);

        connect(ui->cb_UseWorkbench, &QCheckBox::toggled, this, &CModelMatcherComponent::onWorkbenchToggled);

        // initial settings
        m_matcher.setSetup(m_matchingSettings.get());

        this->redisplay();
        ui->cb_UseWorkbench->setVisible(false);
    }

    CModelMatcherComponent::~CModelMatcherComponent() {}

    void CModelMatcherComponent::tabIndexChanged(int index)
    {
        if (index < 0) { return; }
        const QTabWidget *tw = CGuiUtility::parentTabWidget(this);
        Q_ASSERT_X(tw, Q_FUNC_INFO, "Cannot find parent tab widget");
        const QWidget *tabWidget = tw->currentWidget();
        const QWidget *myselfTabWidget = this->parentWidget();
        if (tabWidget != myselfTabWidget) { return; }
        this->redisplay();
    }

    void CModelMatcherComponent::setWorkbenchView(views::CAircraftModelView *workbenchView)
    {
        if (workbenchView)
        {
            ui->cb_UseWorkbench->setVisible(true);
            m_workbenchView = workbenchView;
        }
        else
        {
            ui->cb_UseWorkbench->setVisible(false);
            m_workbenchView.clear();
        }
    }

    void CModelMatcherComponent::onSimulatorChanged(const CSimulatorInfo &simulator)
    {
        Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "Need single simulator");

        ui->tvp_ResultMessages->clear();
        if (this->useWorkbench())
        {
            const CAircraftModelList models = m_workbenchView->container();
            if (models.isEmpty())
            {
                CStatusMessage m(this, CStatusMessage::SeverityWarning, u"No models in workbench, disabled.");
                ui->tvp_ResultMessages->insert(m);
                return;
            }
            CSimulatorInfo si = models.simulatorsWithMaxEntries();
            m_matcher.setModelSet(models, si, true);
        }
        else
        {
            const CAircraftModelList models =
                CCentralMultiSimulatorModelSetCachesProvider::modelCachesInstance().getCachedModels(simulator);
            m_matcher.setModelSet(models, simulator, true);
        }
        this->redisplay();
    }

    void CModelMatcherComponent::onWorkbenchToggled(bool checked)
    {
        Q_UNUSED(checked);
        this->onSimulatorChanged(ui->comp_SimulatorSelector->getValue());
    }

    void CModelMatcherComponent::onCacheChanged(const CSimulatorInfo &simulator)
    {
        Q_UNUSED(simulator);
        this->redisplay();
    }

    void CModelMatcherComponent::testModelMatching()
    {
        ui->te_Results->clear();
        this->onSimulatorChanged(ui->comp_SimulatorSelector->getValue()); // update model set to latest version
        CStatusMessageList msgs;
        CSimulatedAircraft remoteAircraft(this->createAircraft());
        m_matcher.setDefaultModel(CModelMatcherComponent::defaultModel());

        if (ui->cb_withReverseLookup->isChecked())
        {
            const QString liveryString(ui->comp_LiverySelector->getRawCombinedCode());
            const CAircraftModelList modelSet = m_matcher.getModelSet();
            const CAircraftMatcherSetup setup = m_matcher.getSetup();
            const CAircraftModel reverseModel =
                CAircraftMatcher::reverseLookupModelMs(remoteAircraft.getModel(), liveryString, setup, modelSet, &msgs);
            remoteAircraft.setModel(reverseModel); // current model
        }

        CStatusMessageList matchingMsgs;
        const CAircraftModel matched =
            m_matcher.getClosestMatch(remoteAircraft, MatchingLogAll, &matchingMsgs, true); // test model matching
        msgs.push_back(matchingMsgs);

        ui->te_Results->setText(matched.toQString(true));
        ui->tvp_ResultMessages->updateContainer(msgs);
        ui->tvp_ResultMessages->fullResizeToContents();
        ui->tvp_ResultMessages->resizeRowsToContents();
    }

    void CModelMatcherComponent::reverseLookup()
    {
        // CAirspaceMonitor::reverseLookupModelWithFlightplanData is the real world lookup

        ui->te_Results->clear();
        CStatusMessageList msgs;
        m_matcher.setDefaultModel(CModelMatcherComponent::defaultModel());

        const CAircraftModelList modelSet = m_matcher.getModelSet();
        const CAircraftMatcherSetup setup = m_matcher.getSetup();
        const CSimulatedAircraft remoteAircraft(createAircraft());
        const QString livery(ui->comp_LiverySelector->getRawCombinedCode());
        const CAircraftModel matched =
            CAircraftMatcher::reverseLookupModelMs(remoteAircraft.getModel(), livery, setup, modelSet, &msgs);
        ui->te_Results->setText(matched.toQString(true));
        ui->tvp_ResultMessages->updateContainer(msgs);
    }

    void CModelMatcherComponent::onWebDataRead(CEntityFlags::Entity entity, CEntityFlags::ReadState state, int number,
                                               const QUrl &url)
    {
        Q_UNUSED(url)

        if (!sGui || sGui->isShuttingDown()) { return; }
        if (number > 0 && entity.testFlag(CEntityFlags::ModelEntity) && CEntityFlags::isFinishedReadState(state))
        {
            const QStringList modelStrings(sGui->getWebDataServices()->getModelStrings(true));
            ui->le_ModelString->setCompleter(new QCompleter(modelStrings, this));
        }
    }

    void CModelMatcherComponent::displaySettingsDialog()
    {
        if (!m_settingsDialog) { m_settingsDialog = new CSettingsMatchingDialog(this); }
        m_settingsDialog->setMatchingSetup(m_matcher.getSetup());
        const auto r = static_cast<QDialog::DialogCode>(m_settingsDialog->exec());
        if (r == QDialog::Accepted) { m_matcher.setSetup(m_settingsDialog->getMatchingSetup()); }
    }

    void CModelMatcherComponent::redisplay()
    {
        const int c = this->getMatcherModelsCount();
        ui->le_ModelSetCount->setText(QString::number(c) % (this->useWorkbench() ? u" (workbench)" : u""));
    }

    CAircraftModelList CModelMatcherComponent::getModelSetModels() const
    {
        const CSimulatorInfo simulator = ui->comp_SimulatorSelector->getValue();
        const CAircraftModelList models =
            CCentralMultiSimulatorModelSetCachesProvider::modelCachesInstance().getCachedModels(simulator);
        return models;
    }

    int CModelMatcherComponent::getMatcherModelsCount() const { return m_matcher.getModelSetCount(); }

    bool CModelMatcherComponent::useWorkbench() const { return ui->cb_UseWorkbench->isChecked() && m_workbenchView; }

    CSimulatedAircraft CModelMatcherComponent::createAircraft() const
    {
        const QString airline(ui->comp_AirlineSelector->getRawDesignator());
        const QString aircraft(ui->comp_AircraftSelector->getRawDesignator());
        const QString modelString(ui->le_ModelString->text().trimmed().toUpper());
        const QString combined(ui->comp_CombinedCode->getCombinedType());
        const QString manufacturer(ui->le_Manufacturer->text().trimmed().toUpper());
        const QString liveryCombinedCode(ui->comp_LiverySelector->getRawCombinedCode());
        const CCallsign cs(ui->le_Callsign->text().isEmpty() ? "SWIFT" : ui->le_Callsign->text()); // need callsign
        static const CUser pilot("123456", "swift Test", cs);

        CAircraftIcaoCode icao(aircraft, combined);
        icao.setManufacturer(manufacturer);

        const CAirlineIcaoCode airlineIcao(airline);
        const CLivery livery(liveryCombinedCode, airlineIcao, "");
        CAircraftModel m(modelString, CAircraftModel::TypeFSInnData);
        m.setLivery(livery);
        m.setCallsign(cs);
        m.setModelType(modelString.isEmpty() ? CAircraftModel::TypeQueriedFromNetwork : CAircraftModel::TypeFSInnData);
        CSimulatedAircraft sa(m);
        sa.setPilot(pilot);
        sa.setAircraftIcaoCode(icao);
        return sa;
    }

    CAircraftModel CModelMatcherComponent::defaultModel() const
    {
        // can somehow dynamilcally determine the models
        const CAircraftIcaoCode icaoAircraft("B737", "L2J", "FooBar", "Dummy", CWakeTurbulenceCategory::MEDIUM, false,
                                             false, false, 1);
        const CAirlineIcaoCode icaoAirline("Foo", "FooBar airlines", { "DE", "Germany" }, "FOO", true, true);
        const CLivery livery(CLivery::getStandardCode(icaoAirline), icaoAirline, "Standard Foo airlines", "red", "blue",
                             false);
        CAircraftModel model("default model", CAircraftModel::TypeOwnSimulatorModel, "dummy model", icaoAircraft,
                             livery);
        if (model.getCallsign().isEmpty()) { model.setCallsign("SWIFT"); }
        return model;
    }

    MatchingScriptReturnValues CModelMatcherComponent::matchingScript(const CAircraftModel &inModel,
                                                                      const CAircraftMatcherSetup &setup,
                                                                      const CAircraftModelList &modelSet,
                                                                      CStatusMessageList &msgs)
    {
        // Script
        if (setup.doRunMsReverseLookupScript())
        {
            const MatchingScriptReturnValues rv =
                CAircraftMatcher::reverseLookupScript(inModel, setup, modelSet, &msgs);
            if (rv.runScriptAndModified()) { return rv; }
            else
            {
                CCallsign::addLogDetailsToList(&msgs, inModel.getCallsign(),
                                               QStringLiteral("Matching script, no modification"));
            }
        }
        else
        {
            CCallsign::addLogDetailsToList(&msgs, inModel.getCallsign(),
                                           QStringLiteral("No reverse lookup script used"));
        }

        return inModel;
    }
} // namespace swift::gui::components
