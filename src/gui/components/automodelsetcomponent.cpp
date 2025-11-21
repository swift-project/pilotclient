// SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/components/automodelsetcomponent.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QPointer>
#include <QSet>
#include <QStringList>

#include "ui_automodelsetcomponent.h"

#include "config/buildconfig.h"
#include "core/aircraftmatcher.h"
#include "core/webdataservices.h"
#include "gui/components/dbownmodelscomponent.h"
#include "gui/components/dbownmodelsdialog.h"
#include "gui/components/dbownmodelsetcomponent.h"
#include "gui/components/dbownmodelsetdialog.h"
#include "gui/guiapplication.h"
#include "gui/views/distributorview.h"
#include "misc/directoryutils.h"
#include "misc/verify.h"
// #include "core/application.h"
#include "core/modelsetbuilder.h"

// #include "core/webdataservices.h"
// #include "core/webdataservicesms.h"
// #include "gui/components/dbmappingcomponent.h"
// #include "gui/guiapplication.h"
// #include "gui/models/distributorlistmodel.h"

using namespace swift::config;
using namespace swift::core;

using namespace swift::misc;
using namespace swift::misc::network;
using namespace swift::misc::simulation;
using namespace swift::misc::simulation::data;
using namespace swift::misc::simulation::settings;

namespace swift::gui::components
{
    const QStringList &CAutoModelSetComponent::getLogCategories()
    {
        static const QStringList cats { CLogCategories::modelGui() };
        return cats;
    }

    CAutoModelSetComponent::CAutoModelSetComponent(QWidget *parent)
        : COverlayMessagesFrame(parent), ui(new Ui::CAutoModelSetComponent)
    {
        ui->setupUi(this);
        this->preselectOptions();
        // ui->comp_Distributors->view()->setSelectionMode(QAbstractItemView::MultiSelection);
        ui->comp_SimulatorSelector->setMode(CSimulatorSelector::RadioButtons);
        ui->comp_SimulatorSelector->setRememberSelectionAndSetToLastSelection();

        // we use the powerful component to access own models
        m_modelsDialog.reset(new CDbOwnModelsDialog(this));
        m_modelSetDialog.reset(new CDbOwnModelSetDialog(this));

        this->onSimulatorChanged(ui->comp_SimulatorSelector->getValue());

        bool s = connect(ui->comp_SimulatorSelector, &CSimulatorSelector::changed, this,
                         &CAutoModelSetComponent::onSimulatorChanged);
        Q_ASSERT_X(s, Q_FUNC_INFO, "Cannot connect selector signal");
        connect(&m_simulatorSettings, &CMultiSimulatorSettings::settingsChanged, this,
                &CAutoModelSetComponent::onSettingsChanged, Qt::QueuedConnection);
        Q_ASSERT_X(s, Q_FUNC_INFO, "Cannot connect settings signal");
        connect(m_modelsDialog.data(), &CDbOwnModelsDialog::successfullyLoadedModels, this,
                &CAutoModelSetComponent::onModelsLoaded, Qt::QueuedConnection);
        Q_ASSERT_X(s, Q_FUNC_INFO, "Cannot connect models signal");

        connect(ui->pb_ModelSet, &QPushButton::clicked, this, &CAutoModelSetComponent::openOwnModelSetDialog);
        connect(ui->pb_Models, &QPushButton::clicked, this, &CAutoModelSetComponent::openOwnModelsDialog);
        connect(ui->pb_ModelsTriggerReload, &QPushButton::clicked, this, &CAutoModelSetComponent::openOwnModelsDialog);
        connect(ui->pb_ChangeModelDir, &QPushButton::clicked, this, &CAutoModelSetComponent::changeModelDirectory);
        connect(ui->pb_ClearModelDir, &QPushButton::clicked, this, &CAutoModelSetComponent::changeModelDirectory);
        connect(ui->pb_CreateModelSet, &QPushButton::clicked, this, &CAutoModelSetComponent::createModelSet);

        connect(ui->pb_CreateAutoModelSet, &QPushButton::clicked, this, &CAutoModelSetComponent::createAutoModelsets);
        connect(ui->pb_ShowExpertMode, &QPushButton::clicked, this, &CAutoModelSetComponent::toggleExportMode);

        ui->gb_Distributors->setVisible(m_expertmode);
        ui->fr_ModelsAndSimulator->setVisible(m_expertmode);
    }

    CAutoModelSetComponent::~CAutoModelSetComponent() = default;

    void CAutoModelSetComponent::onSimulatorChanged(const CSimulatorInfo &simulator)
    {
        if (!simulator.isSingleSimulator())
        {
            //! \fixme KB 2019-01 reported by RR/crash dump sometimes happening and leading to ASSERT/CTD avoiding the
            //! "crash" for better infos
            if (CBuildConfig::isLocalDeveloperDebugBuild())
            {
                SWIFT_VERIFY_X(false, Q_FUNC_INFO, "Need single simulator");
            }
            CLogMessage(this).error(u"Changing to non-single simulator %1 ignored") << simulator.toQString();
            return;
        }

        Q_ASSERT_X(m_modelsDialog, Q_FUNC_INFO, "No models dialog");
        m_modelsDialog->setSimulator(simulator);

        Q_ASSERT_X(m_modelSetDialog, Q_FUNC_INFO, "No model set dialog");
        m_modelSetDialog->setSimulator(simulator);

        // distributor component
        // ui->comp_Distributors->filterBySimulator(simulator);

        const QStringList dirs = m_simulatorSettings.getModelDirectoriesOrDefault(simulator);
        ui->le_ModelDirectories->setText(dirs.join(", "));

        // kind of hack, but simplest solution
        // we us the loader of the components directly,
        // avoid to fully init a loader logic here
        static const QString modelsNo("No models so far");
        const int modelsCount = this->modelLoader()->getCachedModelsCount(simulator);
        if (modelsCount > 0)
        {
            static const QString modelsInfo("%1 included %2 DB key %3");
            const CAircraftModelList modelsInCache = this->modelLoader()->getCachedModels(simulator);
            const int modelsIncluded = modelsInCache.countByMode(CAircraftModel::Include);
            const int modelsDbKey = modelsInCache.countWithValidDbKey(true);
            ui->le_ModelsInfo->setText(modelsInfo.arg(this->modelLoader()->getCacheCountAndTimestamp(simulator))
                                           .arg(modelsIncluded)
                                           .arg(modelsDbKey));
        }
        else { ui->le_ModelsInfo->setText(modelsNo); }

        ui->pb_CreateModelSet->setEnabled(modelsCount > 0);

        static const QString modelsSetNo("Model set is empty");
        const int modelsSetCount = m_modelSetDialog->modelSetComponent()->getModelSetCount();
        ui->le_ModelSetInfo->setText(
            modelsSetCount > 0 ? m_modelSetDialog->modelSetComponent()->getModelCacheCountAndTimestamp() : modelsSetNo);
    }

    void CAutoModelSetComponent::onSettingsChanged(const CSimulatorInfo &simulator)
    {
        const CSimulatorInfo currentSimulator = ui->comp_SimulatorSelector->getValue();
        if (simulator != currentSimulator) { return; } // ignore changes not for my selected simulator
        this->onSimulatorChanged(simulator);
    }

    void CAutoModelSetComponent::onModelsLoaded(const CSimulatorInfo &simulator, int count)
    {
        Q_UNUSED(count);
        const CSimulatorInfo currentSimulator = ui->comp_SimulatorSelector->getValue();
        if (simulator != currentSimulator) { return; } // ignore changes not for my selected simulator
        this->onSimulatorChanged(simulator);
    }

    void CAutoModelSetComponent::triggerSettingsChanged(const CSimulatorInfo &simulator)
    {
        if (!sGui || sGui->isShuttingDown()) { return; }
        QPointer<CAutoModelSetComponent> myself(this);
        QTimer::singleShot(0, this, [=] {
            if (!myself || !sGui || sGui->isShuttingDown()) { return; }
            myself->onSettingsChanged(simulator);
        });
    }

    const CDbOwnModelsComponent *CAutoModelSetComponent::modelsComponent() const
    {
        Q_ASSERT_X(m_modelsDialog, Q_FUNC_INFO, "No models dialog");
        Q_ASSERT_X(m_modelsDialog->modelsComponent(), Q_FUNC_INFO, "No models component");
        return m_modelsDialog->modelsComponent();
    }

    const CDbOwnModelSetComponent *CAutoModelSetComponent::modelSetComponent() const
    {
        Q_ASSERT_X(m_modelSetDialog, Q_FUNC_INFO, "No model set dialog");
        Q_ASSERT_X(m_modelSetDialog->modelSetComponent(), Q_FUNC_INFO, "No model set component");
        return m_modelSetDialog->modelSetComponent();
    }

    IAircraftModelLoader *CAutoModelSetComponent::modelLoader() const
    {
        Q_ASSERT_X(m_modelsDialog->modelsComponent()->modelLoader(), Q_FUNC_INFO, "No model loader");
        return m_modelsDialog->modelsComponent()->modelLoader();
    }

    void CAutoModelSetComponent::openOwnModelsDialog()
    {
        if (!m_modelsDialog) { return; }
        if (!sGui || sGui->isShuttingDown() || !sGui->getWebDataServices()) { return; }
        const bool reload = (QObject::sender() == ui->pb_ModelsTriggerReload);

        const CSimulatorInfo simulator = ui->comp_SimulatorSelector->getValue();
        m_modelsDialog->setSimulator(simulator);

        if (reload)
        {
            if (!sGui->getWebDataServices()->hasDbModelData())
            {
                const QMessageBox::StandardButton reply = QMessageBox::warning(
                    this->mainWindow(), "DB data", "No DB data, models cannot be consolidated. Load anyway?",
                    QMessageBox::Yes | QMessageBox::No);
                if (reply != QMessageBox::Yes) { return; }
            }

            bool loadOnlyIfNotEmpty = true;
            if (m_modelsDialog->getOwnModelsCount() > 0)
            {
                const QMessageBox::StandardButton reply =
                    QMessageBox::warning(this->mainWindow(), "Model loading",
                                         "Reload the models?\nThe existing cache data will we overridden.",
                                         QMessageBox::Yes | QMessageBox::No);
                if (reply == QMessageBox::Yes) { loadOnlyIfNotEmpty = false; }
            }
            m_modelsDialog->requestModelsInBackground(simulator, loadOnlyIfNotEmpty);
        }
        m_modelsDialog->exec();

        // force UI update
        this->triggerSettingsChanged(simulator);
    }

    void CAutoModelSetComponent::openOwnModelSetDialog()
    {
        CSimulatorInfo simulator;
        if (!m_GenerateModelsets)
            simulator = ui->comp_SimulatorSelector->getValue();
        else
            simulator = m_simulator;
        m_modelSetDialog->setSimulator(simulator);
        m_modelSetDialog->enableButtons(false, false);
        m_modelSetDialog->exec();

        // force UI update
        this->triggerSettingsChanged(simulator);
    }

    void CAutoModelSetComponent::changeModelDirectory()
    {
        using namespace std::chrono_literals;

        if (!sGui || sGui->isShuttingDown()) { return; }
        const CSimulatorInfo simulator = ui->comp_SimulatorSelector->getValue();
        CSpecializedSimulatorSettings settings = m_simulatorSettings.getSpecializedSettings(simulator);
        const bool clear = (QObject::sender() == ui->pb_ClearModelDir);

        if (clear) { settings.clearModelDirectories(); }
        else
        {
            const QString dirOld = settings.getFirstModelDirectoryOrDefault();
            const QString newDir =
                QFileDialog::getExistingDirectory(this->mainWindow(), tr("Open model directory"), dirOld,
                                                  QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
            if (newDir.isEmpty() || CDirectoryUtils::isSameExistingDirectory(dirOld, newDir)) { return; }
            settings.addModelDirectory(newDir);
        }

        const CStatusMessage msg = m_simulatorSettings.setAndSaveSettings(settings, simulator);
        if (msg.isSuccess()) { this->triggerSettingsChanged(simulator); }
        else { this->showOverlayMessage(msg, 4s); }
    }

    void CAutoModelSetComponent::createModelSet()
    {
        using namespace std::chrono_literals;
        CSimulatorInfo simulator = ui->comp_SimulatorSelector->getValue();

        if (m_GenerateModelsets) simulator = m_simulator;
        m_simulator = simulator;
        if (m_simulator == 64)
        {
            this->showOverlayHTMLMessage(
                CStatusMessage(this).info(u"Automatic model set creation not supported for MSFS2024 yet. Start the "
                                          u"swiftgui or mapping tool to create a model set."),
                std::chrono::seconds(10));

            return;
        }

        const int modelsCount = this->modelLoader()->getCachedModelsCount(simulator);
        if (modelsCount < 1)
        {
            static const CStatusMessage msg =
                CStatusMessage(this).validationError(u"No models indexed so far. Try 'reload'!");
            this->showOverlayMessage(msg, 4s);
            return;
        }

        bool useAllModels = false;
        // if (!ui->comp_Distributors->hasSelectedDistributors())
        if (!m_GenerateModelsets)
        {
            const QMessageBox::StandardButton reply =
                QMessageBox::question(this->mainWindow(), "Models", "No distributors selected, use all models?",
                                      QMessageBox::Yes | QMessageBox::No);
            if (reply == QMessageBox::Yes) { useAllModels = true; }
            else
            {
                static const CStatusMessage msg = CStatusMessage(this).validationError(u"No distributors selected");
                this->showOverlayMessage(msg, 4s);
                return;
            }
        }
        else
            useAllModels = true;

        CAircraftModelList modelsForSet = this->modelLoader()->getCachedModels(simulator);
        if (!useAllModels)
        {
            // const CDistributorList distributors = ui->comp_Distributors->getSelectedDistributors();
            // modelsForSet = modelsForSet.findByDistributors(distributors);
        }

        if (ui->cb_DbDataOnly->isChecked() || m_GenerateModelsets) { modelsForSet.removeObjectsWithoutDbKey(); }
        if (modelsForSet.isEmpty())
        {
            this->showOverlayHTMLMessage("Selection yielded no result!");
            return;
        }

        // just in case, paranoia
        if (!m_modelSetDialog || !m_modelSetDialog->modelSetComponent())
        {
            this->showOverlayHTMLMessage("No model set dialog, cannot continue");
            return;
        }

        const int modelsSetCount = m_modelSetDialog->modelSetComponent()->getModelSetCount();
        if (modelsSetCount > 0 && !m_GenerateModelsets)
        {
            QMessageBox::StandardButton override = QMessageBox::question(
                this, "Override", "Override existing model set?", QMessageBox::Yes | QMessageBox::No);
            if (override != QMessageBox::Yes) { return; }
        }

        m_modelSetDialog->modelSetComponent()->setModelSet(modelsForSet, simulator);

        if (!m_GenerateModelsets)
        {
            ui->pb_ModelSet->click();
            return;
        };

        // TODO TZ store modelset
        // modelsForSet
        CAircraftModelList newModelList;
        CAircraftModelList currentSet;
        CAircraftModelList NewSet;

        // store the model in the new list
        newModelList.replaceOrAddModelsWithString(modelsForSet, Qt::CaseInsensitive);

        if (!newModelList.isEmpty())
        {

            /*    m_simulatorInfo = this->getSimulatorInfo();*/

            bool givenDistributorsOnly = false;
            bool dbDataOnly = true;
            bool dbIcaoOnly = false;
            bool incremnental = false;
            bool sortByDistributor = true;
            bool consolidateWithDb = true;
            bool ShowAllInstalledModells = true; // msfs20424 always show all installed models

            // CDistributorList distributorList;
            // for (const QString &name : distributorNames) { distributorList.push_back(CDistributor(name)); }
            CDistributorList distributorList = sGui->getWebDataServices()->getDistributors();

            CModelSetBuilder builder(nullptr);
            CModelSetBuilder::Builder options =
                givenDistributorsOnly ? CModelSetBuilder::GivenDistributorsOnly : CModelSetBuilder::NoOptions;
            if (dbDataOnly) { options |= CModelSetBuilder::OnlyDbData; }
            if (dbIcaoOnly) { options |= CModelSetBuilder::OnlyDbIcaoCodes; }
            if (incremnental) { options |= CModelSetBuilder::Incremental; }
            if (sortByDistributor) { options |= CModelSetBuilder::SortByDistributors; }
            if (consolidateWithDb) { options |= CModelSetBuilder::ConsolidateWithDb; }
            if (ShowAllInstalledModells) { options |= CModelSetBuilder::ShowAllInstalledModells; }

            CCentralMultiSimulatorModelSetCachesProvider::modelCachesInstance().synchronizeCache(simulator);
            currentSet = CCentralMultiSimulatorModelSetCachesProvider::modelCachesInstance().getCachedModels(simulator);

            NewSet = builder.buildModelSet(simulator, newModelList, currentSet, options, distributorList);

            swift::core::CAircraftMatcher matcher;
            swift::misc::simulation::CAircraftMatcherSetup mSetup = matcher.getSetup();

            NewSet.setSimulatorInfo(simulator);
            matcher.setModelSet(NewSet, simulator, true);

            const QDateTime latestDbModelsTs =
                NewSet.isEmpty() ? sApp->getWebDataServices()->getCacheTimestamp(CEntityFlags::ModelEntity) :
                                   NewSet.latestTimestamp();
            if (!latestDbModelsTs.isValid()) { return; }

            // for swiftgui it is enough to set the cache here
            // if (gui_application)
            CCentralMultiSimulatorModelSetCachesProvider::modelCachesInstance().setModelsForSimulator(NewSet,
                                                                                                      simulator);

            CCentralMultiSimulatorModelCachesProvider::modelCachesInstance().setCachedModels(NewSet, simulator);

            const CStatusMessage m = CStatusMessage(this, CStatusMessage::SeverityInfo,
                                                    u"Loading SimObjects and Liveries from the Simulator completed");

            CLogMessage(this).info(u"%1 SimObjects and Liveries in DbModelList") << NewSet.size();
        }

        this->showOverlayHTMLMessage(CStatusMessage(this).info(u"Modelset for simulator: '%1' created ")
                                         << simulator.getSimulator(),
                                     std::chrono::seconds(2));
    }

    void CAutoModelSetComponent::createAutoModelsets()
    {

        m_expertmode = false;
        ui->gb_Distributors->setVisible(m_expertmode); // hide for now
        ui->fr_ModelsAndSimulator->setVisible(m_expertmode); // hide for now}
        m_GenerateModelsets = true;

        // loop for all enabled simulators
        CSimulatorInfo sims = this->m_enabledSimulators.get();
        unsigned int number = 96;

        // Iteriere
        // for (const auto &simInfo : number)
        //{
        //    m_simulator = simInfo;
        //    // this->createModelSet();
        //}

        const int totalBits = sizeof(number) * 8;
        for (int i = 0; i < totalBits; ++i)
        {
            if (number & (1u << i))
            { // Prüfen, ob Bit i gesetzt ist
                m_simulator = std::pow(2.0, i);
                this->createModelSet();
            }
        }
    }

    void CAutoModelSetComponent::toggleExportMode()
    {
        if (m_expertmode) { m_expertmode = false; }
        else
        {
            m_expertmode = true;
            m_GenerateModelsets = false;
        }
        ui->gb_Distributors->setVisible(m_expertmode); // hide for now
        ui->fr_ModelsAndSimulator->setVisible(m_expertmode); // hide for now
    }

    QWidget *CAutoModelSetComponent::mainWindow()
    {
        QWidget *pw = CGuiApplication::mainApplicationWidget();
        return pw ? pw : this;
    }

    void CAutoModelSetComponent::preselectOptions()
    {
        QStringList opt = this->m_enabledConfigOptions.get(); // force reload
        QString cb_GenerateModelsets = "false";
        QString cb_PTT = "false";
        QString cb_SetExportMode = "false";
        if (!opt.isEmpty())
        {
            cb_GenerateModelsets = opt.first();
            opt.removeFirst();
            if (!opt.isEmpty()) cb_PTT = opt.first();
            opt.removeFirst();
            if (!opt.isEmpty()) cb_SetExportMode = opt.first();
        }
        m_expertmode = cb_SetExportMode.contains("true") ? true : false;
        m_GenerateModelsets = cb_GenerateModelsets.contains("true") ? true : false;
    }

    bool CAutoModelSetWizardPage::validatePage() { return true; }
} // namespace swift::gui::components
