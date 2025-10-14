// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/components/dbownmodelscomponent.h"

#include <QAction>
#include <QDir>
#include <QFileDialog>
#include <QIcon>
#include <QMessageBox>
#include <QPointer>
#include <QtGlobal>

#include "ui_dbownmodelscomponent.h"

#include "config/buildconfig.h"
#include "core/db/databaseutils.h"
#include "core/webdataservices.h"
#include "gui/components/simulatorselector.h"
#include "gui/guiapplication.h"
#include "gui/menus/aircraftmodelmenus.h"
#include "gui/menus/menuaction.h"
#include "gui/models/aircraftmodellistmodel.h"
#include "gui/views/aircraftmodelview.h"
#include "misc/icons.h"
#include "misc/logmessage.h"
#include "misc/processctrl.h"
#include "misc/simulation/aircraftmodelloaderprovider.h"
#include "misc/statusmessage.h"
#include "misc/swiftdirectories.h"

using namespace swift::config;
using namespace swift::misc;
using namespace swift::misc::simulation;
using namespace swift::core::db;
using namespace swift::gui::menus;
using namespace swift::gui::views;
using namespace swift::gui::models;

namespace swift::gui::components
{
    CDbOwnModelsComponent::CDbOwnModelsComponent(QWidget *parent)
        : COverlayMessagesFrame(parent), ui(new Ui::CDbOwnModelsComponent)
    {
        ui->setupUi(this);
        ui->comp_SimulatorSelector->setMode(CSimulatorSelector::RadioButtons);
        ui->tvp_OwnAircraftModels->setAircraftModelMode(CAircraftModelListModel::OwnAircraftModelMappingTool);
        ui->tvp_OwnAircraftModels->addFilterDialog();
        ui->tvp_OwnAircraftModels->setDisplayAutomatically(true);
        ui->tvp_OwnAircraftModels->setCustomMenu(new CLoadModelsMenu(this));

        const CSimulatorInfo sim = ui->comp_SimulatorSelector->getValue();
        ui->tvp_OwnAircraftModels->setCorrespondingSimulator(sim,
                                                             m_simulatorSettings.getSimulatorDirectoryOrDefault(sim));

        bool c = connect(ui->tvp_OwnAircraftModels, &CAircraftModelView::requestUpdate, this,
                         &CDbOwnModelsComponent::requestOwnModelsUpdate);
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");
        c = connect(ui->tvp_OwnAircraftModels, &CAircraftModelView::jsonLoadCompleted, this,
                    &CDbOwnModelsComponent::onViewDiskLoadingFinished, Qt::QueuedConnection);
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");
        c = connect(ui->comp_SimulatorSelector, &CSimulatorSelector::changed, this,
                    &CDbOwnModelsComponent::onSimulatorSelectorChanged);
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");
        c = connect(&CMultiAircraftModelLoaderProvider::multiModelLoaderInstance(),
                    &CMultiAircraftModelLoaderProvider::loadingFinished, this,
                    &CDbOwnModelsComponent::onModelLoaderLoadingFinished, Qt::QueuedConnection);
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");
        c = connect(&CMultiAircraftModelLoaderProvider::multiModelLoaderInstance(),
                    &CMultiAircraftModelLoaderProvider::diskLoadingStarted, this,
                    &CDbOwnModelsComponent::onModelLoaderDiskLoadingStarted, Qt::QueuedConnection);
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");
        c = connect(&CMultiAircraftModelLoaderProvider::multiModelLoaderInstance(),
                    &CMultiAircraftModelLoaderProvider::loadingProgress, this,
                    &CDbOwnModelsComponent::onModelLoadingProgress, Qt::QueuedConnection);
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");
        c = connect(&CMultiAircraftModelLoaderProvider::multiModelLoaderInstance(),
                    &CMultiAircraftModelLoaderProvider::cacheChanged, this, &CDbOwnModelsComponent::onCacheChanged,
                    Qt::QueuedConnection);
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");
        c = connect(ui->pb_ForceReload, &QPushButton::released, this,
                    &CDbOwnModelsComponent::confirmedForcedReloadCurrentSimulator, Qt::QueuedConnection);
        Q_ASSERT_X(c, Q_FUNC_INFO, "Connect failed");

        // Last selection isPinned -> no sync needed
        ui->comp_SimulatorSelector->setRememberSelectionAndSetToLastSelection();
        const CSimulatorInfo simulator = ui->comp_SimulatorSelector->getValue();
        if (simulator.isSingleSimulator())
        {
            m_simulator = simulator;
            this->setUiSimulatorString(simulator);
            const bool success = this->initModelLoader(simulator, IAircraftModelLoader::CacheOnly);
            if (!success) { CLogMessage(this).error(u"Init of model loader failed in component"); }
        }

        // menu
        ui->tvp_OwnAircraftModels->setCustomMenu(new CConsolidateWithDbDataMenu(ui->tvp_OwnAircraftModels, this));
    }

    CDbOwnModelsComponent::~CDbOwnModelsComponent()
    {
        // void
    }

    const QStringList &CDbOwnModelsComponent::getLogCategories()
    {
        static const QStringList l({ CLogCategories::modelGui(), CLogCategories::guiComponent() });
        return l;
    }

    CAircraftModelView *CDbOwnModelsComponent::view() const { return ui->tvp_OwnAircraftModels; }

    CAircraftModelListModel *CDbOwnModelsComponent::model() const { return ui->tvp_OwnAircraftModels->derivedModel(); }

    bool CDbOwnModelsComponent::requestModelsInBackground(const CSimulatorInfo &simulator, bool onlyIfNotEmpty)
    {
        this->setSimulator(simulator);
        if (onlyIfNotEmpty && this->getOwnModelsCount() > 0) { return false; }
        const IAircraftModelLoader::LoadMode mode =
            onlyIfNotEmpty ? IAircraftModelLoader::InBackgroundNoCache : IAircraftModelLoader::LoadInBackground;
        this->requestSimulatorModels(simulator, mode);
        return true;
    }

    CAircraftModel CDbOwnModelsComponent::getOwnModelForModelString(const QString &modelString) const
    {
        return this->getOwnModels().findFirstByModelStringOrDefault(modelString);
    }

    CAircraftModelList CDbOwnModelsComponent::getOwnModels() const
    {
        return this->getOwnCachedModels(this->getOwnModelsSimulator());
    }

    CAircraftModelList CDbOwnModelsComponent::getOwnCachedModels(const CSimulatorInfo &simulator) const
    {
        static const CAircraftModelList empty;
        if (!m_modelLoader) { return empty; }
        return m_modelLoader->getCachedModels(simulator);
    }

    CAircraftModelList CDbOwnModelsComponent::getOwnSelectedModels() const
    {
        return ui->tvp_OwnAircraftModels->selectedObjects();
    }

    CSimulatorInfo CDbOwnModelsComponent::getOwnModelsSimulator() const
    {
        return ui->comp_SimulatorSelector->getValue();
    }

    bool CDbOwnModelsComponent::setSimulator(const CSimulatorInfo &simulator, bool forced)
    {
        Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "Need single simulator");

        const bool simUnchanged = (m_simulator == simulator);

        if (!simUnchanged) { emit this->ownModelsSimulatorChanged(simulator); }
        if (!forced && simUnchanged) { return false; }

        // changed simulator
        m_simulator = simulator;
        this->requestSimulatorModelsWithCacheInBackground(simulator);
        ui->comp_SimulatorSelector->setValue(simulator);
        this->setUiSimulatorString(simulator);
        ui->tvp_OwnAircraftModels->setCorrespondingSimulator(
            simulator, m_simulatorSettings.getSimulatorDirectoryOrDefault(simulator));
        return true;
    }

    void CDbOwnModelsComponent::setSimulatorSelectorMode(CSimulatorSelector::Mode mode)
    {
        ui->comp_SimulatorSelector->setMode(mode);
    }

    void CDbOwnModelsComponent::onSimulatorSelectorChanged()
    {
        const CSimulatorInfo simulator(ui->comp_SimulatorSelector->getValue());
        this->setSimulator(simulator);
    }

    int CDbOwnModelsComponent::getOwnModelsCount() const
    {
        if (!m_modelLoader) { return 0; }
        return m_modelLoader->getCachedModelsCount(this->getOwnModelsSimulator());
    }

    QString CDbOwnModelsComponent::getInfoString() const
    {
        if (!m_modelLoader) { return {}; }
        return m_modelLoader->getInfoString();
    }

    QString CDbOwnModelsComponent::getInfoStringFsFamily() const
    {
        if (!m_modelLoader) { return {}; }
        return m_modelLoader->getInfoStringFsFamily();
    }

    CStatusMessage CDbOwnModelsComponent::updateViewAndCache(const CAircraftModelList &models)
    {
        const CStatusMessage m = m_modelLoader->setCachedModels(models, this->getOwnModelsSimulator());
        if (m.isSuccess()) { ui->tvp_OwnAircraftModels->updateContainerMaybeAsync(models); }
        return m;
    }

    void CDbOwnModelsComponent::clearView() { ui->tvp_OwnAircraftModels->clear(); }

    void CDbOwnModelsComponent::gracefulShutdown()
    {
        // void
    }

    void CDbOwnModelsComponent::setModelsForSimulator(const CAircraftModelList &models, const CSimulatorInfo &simulator)
    {
        if (!m_modelLoader) { return; }
        m_modelLoader->setCachedModels(models, simulator);
        ui->tvp_OwnAircraftModels->replaceOrAddModelsWithString(models);
    }

    int CDbOwnModelsComponent::updateModelsForSimulator(const CAircraftModelList &models,
                                                        const CSimulatorInfo &simulator)
    {
        if (!m_modelLoader) { return 0; }
        const int c = m_modelLoader->updateModelsForSimulator(models, simulator);
        const CAircraftModelList allModels(m_modelLoader->getCachedModels(simulator));
        ui->tvp_OwnAircraftModels->updateContainerMaybeAsync(allModels);
        return c;
    }

    bool CDbOwnModelsComponent::initModelLoader(const CSimulatorInfo &simulator, IAircraftModelLoader::LoadMode load)
    {
        // called when simulator is changed / init
        Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "Need single simulator");

        // already loaded
        if (!m_modelLoader || !m_modelLoader->supportsSimulator(simulator))
        {
            m_modelLoader = CMultiAircraftModelLoaderProvider::multiModelLoaderInstance().loaderInstance(simulator);
            if (m_modelLoader) { m_modelLoader->startLoading(load); }
        }
        this->setSaveFileName(simulator);
        return m_modelLoader && m_modelLoader->supportsSimulator(simulator);
    }

    void CDbOwnModelsComponent::setSaveFileName(const CSimulatorInfo &sim)
    {
        Q_ASSERT_X(sim.isSingleSimulator(), Q_FUNC_INFO, "Need single simulator");
        const QString n("simulator models " + sim.toQString(true));
        ui->tvp_OwnAircraftModels->setSaveFileName(n);
    }

    QString CDbOwnModelsComponent::directorySelector(const CSimulatorInfo &simulatorInfo)
    {
        QString dir = m_directorySettings.get().getLastModelDirectoryOrDefault();
        dir = QFileDialog::getExistingDirectory(this,
                                                QStringLiteral("Open directory (%1)").arg(simulatorInfo.toQString()),
                                                dir, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
        const QDir d(dir);
        if (d.exists())
        {
            CDirectories dirs = m_directorySettings.get();
            dirs.setLastModelDirectory(dir);
            const CStatusMessage m = m_directorySettings.setAndSave(dirs);
            CLogMessage::preformatted(m);
        }
        return dir;
    }

    void CDbOwnModelsComponent::setUiSimulatorString(const CSimulatorInfo &simulatorInfo)
    {
        const QString s = simulatorInfo.toQString(true);
        ui->le_Simulator->setText(s);
        ui->pb_ForceReload->setText(QStringLiteral("force reload '%1'").arg(s));
    }

    void CDbOwnModelsComponent::confirmedForcedReloadCurrentSimulator() { this->confirmedForcedReload(m_simulator); }

    void CDbOwnModelsComponent::confirmedForcedReload(const CSimulatorInfo &simulator)
    {
        QMessageBox msgBox(QMessageBox::Question, "Reload models from disk",
                           QStringLiteral("Completely reload '%1' models from disk?").arg(simulator.toQString(true)),
                           QMessageBox::Yes | QMessageBox::No, this);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        const QMessageBox::StandardButton reply = static_cast<QMessageBox::StandardButton>(msgBox.exec());
        if (reply != QMessageBox::Yes) { return; }

        this->requestSimulatorModels(simulator, IAircraftModelLoader::InBackgroundNoCache);
    }

    void CDbOwnModelsComponent::runScriptCSL2XSB()
    {
        static const QString script = QDir(CSwiftDirectories::shareDirectory()).filePath("CSL2XSB/CSL2XSB.exe");
        for (const QString &modelDir : m_simulatorSettings.getModelDirectoriesOrDefault(CSimulatorInfo::xplane()))
        {
            CLogMessage(this).info(u"Running CSL2XSB on model directory %1") << modelDir;
            CProcessCtrl::startDetached(script, { QDir::cleanPath(modelDir) }, true);
        }
    }

    void CDbOwnModelsComponent::CLoadModelsMenu::customMenu(CMenuActions &menuActions)
    {
        if (!sGui || sGui->isShuttingDown()) { return; }

        // for the moment I use all sims, I could restrict to CSimulatorInfo::getLocallyInstalledSimulators();
        const CSimulatorInfo sims = CSimulatorInfo::allSimulators();
        const bool noSims = sims.isNoSimulator() || sims.isUnspecified();
        if (!noSims)
        {
            QPointer<CDbOwnModelsComponent> ownModelsComp(qobject_cast<CDbOwnModelsComponent *>(this->parent()));
            Q_ASSERT_X(ownModelsComp, Q_FUNC_INFO, "Cannot access parent");

            menuActions.addMenuSimulator();

            if (sims.isXPlane() && CBuildConfig::isRunningOnWindowsNtPlatform() && CBuildConfig::buildWordSize() == 64)
            {
                if (!m_csl2xsbAction)
                {
                    m_csl2xsbAction = new QAction(CIcons::appTerminal16(), "XPlane: run CSL2XSB on all models", this);
                    connect(m_csl2xsbAction, &QAction::triggered, ownModelsComp, [ownModelsComp](bool checked) {
                        if (!ownModelsComp) { return; }
                        Q_UNUSED(checked)
                        ownModelsComp->runScriptCSL2XSB();
                    });
                }
                menuActions.addAction(m_csl2xsbAction, CMenuAction::pathSimulator());
            }
        }
        this->nestedCustomMenu(menuActions);
    }

    void CDbOwnModelsComponent::requestOwnModelsUpdate()
    {
        if (!m_modelLoader) { return; }
        ui->tvp_OwnAircraftModels->updateContainerMaybeAsync(this->getOwnModels());
    }

    // TODO TZ this is a stub for SimConnect loading
    void CDbOwnModelsComponent::loadInstalledModelsSimConnect(const CSimulatorInfo &simulator,
                                                              IAircraftModelLoader::LoadMode mode,
                                                              const QStringList &modelDirectories)
    {
        Q_UNUSED(mode);
        Q_UNUSED(modelDirectories);

        using namespace std::chrono_literals;
        const CStatusMessage msg = CLogMessage(this).info(u"Start loading models for %1") << simulator.toQString();
        this->showOverlayHTMLMessage(msg, 2s);

        return;
    }

    void CDbOwnModelsComponent::loadInstalledModels(const CSimulatorInfo &simulator,
                                                    IAircraftModelLoader::LoadMode mode,
                                                    const QStringList &modelDirectories)
    {
        if (!m_modelLoader) { return; }

        // here m_modelLoader is still the "current" loader
        if (m_modelLoader->isLoadingInProgress())
        {
            if (m_modelLoader->supportsSimulator(simulator))
            {
                const CStatusMessage msg =
                    CLogMessage(this).warning(u"Loading for '%1' already in progress, will NOT load.")
                    << simulator.toQString();
                this->showOverlayMessage(msg);
                return;
            }
            else
            {
                const CStatusMessage msg =
                    CLogMessage(this).warning(
                        u"Loading for another simulator '%1' already in progress. Loading might be slow.")
                    << simulator.toQString();
                this->showOverlayMessage(msg);
            }
        }

        if (!this->initModelLoader(simulator))
        {
            const CStatusMessage msg = CLogMessage(this).error(u"Cannot init model loader for %1")
                                       << simulator.toQString();
            this->showOverlayMessage(msg);
            return;
        }

        // Do not check for empty models die here, as depending on mode we could still load
        // will be checked in model loader
        CLogMessage(this).info(u"Starting loading for '%1' in mode '%2'")
            << simulator.toQString() << IAircraftModelLoader::enumToString(mode);
        ui->tvp_OwnAircraftModels->showLoadIndicator();
        Q_ASSERT_X(sGui && sGui->getWebDataServices(), Q_FUNC_INFO, "missing web data services");
        m_modelLoader->startLoading(
            mode, static_cast<int (*)(CAircraftModelList &, bool)>(&CDatabaseUtils::consolidateModelsWithDbData),
            modelDirectories);
    }

    void CDbOwnModelsComponent::onModelLoaderDiskLoadingStarted(const CSimulatorInfo &simulator,
                                                                IAircraftModelLoader::LoadMode mode)
    {
        using namespace std::chrono_literals;
        const CStatusMessage msg = CLogMessage(this).info(u"Started disk loading for '%1' in mode '%2'")
                                   << simulator.toQString(true) << IAircraftModelLoader::enumToString(mode);
        this->showOverlayHTMLMessage(msg, 5s);
    }

    void CDbOwnModelsComponent::onModelLoadingProgress(const CSimulatorInfo &simulator, const QString &message,
                                                       int progress)
    {
        using namespace std::chrono_literals;

        const CStatusMessage loadingMsg = CStatusMessage(this).info(u"%1 loading: %2")
                                          << simulator.toQString(true) << message;
        this->showOverlayHTMLMessage(loadingMsg, 5s);
        ui->tvp_OwnAircraftModels->showLoadIndicatorWithTimeout(5s); // trigger new load indicator
        Q_UNUSED(progress)
    }

    void CDbOwnModelsComponent::onModelLoaderLoadingFinished(const CStatusMessageList &statusMessages,
                                                             const CSimulatorInfo &simulator,
                                                             IAircraftModelLoader::LoadFinishedInfo info)
    {
        using namespace std::chrono_literals;
        Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "Expect single simulator");

        bool hideIndicator = false; // hide in case loading failed
        CStatusMessage summaryMsg;

        if (IAircraftModelLoader::isLoadedInfo(info) && m_modelLoader)
        {
            const CAircraftModelList models(m_modelLoader->getCachedModels(simulator));
            const int modelsLoaded = models.size();
            ui->tvp_OwnAircraftModels->updateContainerMaybeAsync(models);

            if (modelsLoaded < 1)
            {
                // loading ok, but no data
                summaryMsg = CLogMessage(this).warning(u"Loading completed for simulator '%1', but no models")
                             << simulator;
                hideIndicator = true;
            }
            else
            {
                summaryMsg = CLogMessage(this).info(u"Loading completed for simulator '%1' with %2 models")
                             << simulator << modelsLoaded;
            }

            // overlay
            if (!summaryMsg.isEmpty() && info == IAircraftModelLoader::ParsedData)
            {
                this->showOverlayHTMLMessage(summaryMsg, 5s);
            }

            // signal
            emit this->successfullyLoadedModels(simulator, modelsLoaded);
        }
        else if (info == IAircraftModelLoader::LoadingSkipped)
        {
            CLogMessage(this).error(u"Loading of models skipped, simulator '%1'") << simulator.toQString();
            hideIndicator = true;
        }
        else
        {
            ui->tvp_OwnAircraftModels->clear();
            hideIndicator = true;
            CLogMessage(this).error(u"Loading of models failed, simulator '%1'") << simulator.toQString();
        }

        // with errors we make sure errors are on top
        if (statusMessages.hasErrorMessages())
        {
            this->setOverlayMessagesSorting(CStatusMessage::IndexSeverityAsIcon, Qt::DescendingOrder);
            this->showOverlayMessagesOrHTMLMessage(statusMessages, false, 0s);
        }
        else
        {
            // no issues, directly hide
            const std::chrono::milliseconds timeout { statusMessages.hasWarningOrErrorMessages() ? 0 : 7500 };
            if (statusMessages.size() < 50)
            {
                // small number of messages
                this->showOverlayMessagesOrHTMLMessage(statusMessages, false, timeout);
            }
            else
            {
                QPointer<CDbOwnModelsComponent> myself(this);
                const QString confirmMessage =
                    QStringLiteral("Do you want to see the %1 detailled messages?").arg(statusMessages.size());
                this->showOverlayMessagesWithConfirmation(summaryMsg, false, confirmMessage, [=] {
                    if (!myself) { return; }
                    myself->showOverlayMessagesOrHTMLMessage(statusMessages);
                });
            }
        }

        if (hideIndicator) { ui->tvp_OwnAircraftModels->hideLoadIndicatorForced(); }

        // cache loads may occur in background, do not adjust UI settings
        if (info == IAircraftModelLoader::CacheLoaded) { return; }

        // parsed loads normally explicit displaying this simulator
        this->setSimulator(simulator);
    }

    void CDbOwnModelsComponent::onViewDiskLoadingFinished(const CStatusMessage &status)
    {
        if (status.isFailure()) { return; }
        QMessageBox msgBox(
            QMessageBox::Question, "Loaded models from disk",
            "Loaded models from disk file.\nSave to cache or just temporarily keep them?\n\nHint: Saving them will "
            "override the loaded models from the simulator.\nNormally you would not want that (cancel).",
            QMessageBox::Save | QMessageBox::Cancel, this);
        msgBox.setDefaultButton(QMessageBox::Cancel);
        const QMessageBox::StandardButton reply = static_cast<QMessageBox::StandardButton>(msgBox.exec());
        if (reply != QMessageBox::Cancel) { return; }
        const CAircraftModelList models = ui->tvp_OwnAircraftModels->container();
        if (models.isEmpty()) { return; }
        const CSimulatorInfo simulator = ui->comp_SimulatorSelector->getValue();
        m_modelLoader->setModelsForSimulator(models, simulator);
    }

    void CDbOwnModelsComponent::onCacheChanged(const CSimulatorInfo &simulator)
    {
        const CAircraftModelList models(m_modelLoader->getCachedModels(simulator));
        ui->tvp_OwnAircraftModels->updateContainerMaybeAsync(models);
    }

    void CDbOwnModelsComponent::requestSimulatorModels(const CSimulatorInfo &simulator,
                                                       IAircraftModelLoader::LoadMode mode,
                                                       const QStringList &modelDirectories)
    {
        // At this point, we switch how the models should be loaded: SimConnect or classic file search
        if (simulator.isMSFS2024())
            this->loadInstalledModelsSimConnect(simulator, mode, modelDirectories);
        else
            this->loadInstalledModels(simulator, mode, modelDirectories);
    }

    void CDbOwnModelsComponent::requestSimulatorModelsWithCacheInBackground(const CSimulatorInfo &simulator)
    {
        this->requestSimulatorModels(simulator, IAircraftModelLoader::InBackgroundWithCache);
    }

    void CDbOwnModelsComponent::clearSimulatorCache(const CSimulatorInfo &simulator)
    {
        if (!m_modelLoader) { return; }
        m_modelLoader->clearCachedModels(simulator);
    }
} // namespace swift::gui::components
