// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/components/dbmappingcomponent.h"
#include "gui/components/dbownmodelsetcomponent.h"
#include "gui/components/dbownmodelsetformdialog.h"
#include "gui/components/dbreducemodelduplicates.h"
#include "gui/components/firstmodelsetdialog.h"
#include "gui/components/copymodelsfromotherswiftversionsdialog.h"
#include "gui/menus/aircraftmodelmenus.h"
#include "gui/menus/menuaction.h"
#include "gui/models/aircraftmodellistmodel.h"
#include "gui/views/aircraftmodelview.h"
#include "gui/views/viewbase.h"
#include "gui/views/aircraftmodelstatisticsdialog.h"
#include "gui/guiapplication.h"
#include "gui/guiutility.h"
#include "misc/simulation/aircraftmodelutils.h"
#include "misc/simulation/aircraftmodellist.h"
#include "misc/simulation/distributorlist.h"
#include "misc/simulation/distributorlistpreferences.h"
#include "misc/mixin/mixincompare.h"
#include "misc/icons.h"
#include "misc/logmessage.h"
#include "misc/orderable.h"
#include "ui_dbownmodelsetcomponent.h"

#include <QAction>
#include <QDialog>
#include <QFlags>
#include <QIcon>
#include <QLineEdit>
#include <QPushButton>
#include <QStringBuilder>
#include <QString>
#include <QWidget>
#include <Qt>
#include <QtGlobal>
#include <QPointer>
#include <QDesktopServices>

using namespace swift::misc;
using namespace swift::misc::simulation;
using namespace swift::gui::models;
using namespace swift::gui::menus;
using namespace swift::gui::views;

namespace swift::gui::components
{
    CDbOwnModelSetComponent::CDbOwnModelSetComponent(QWidget *parent) : QFrame(parent),
                                                                        CDbMappingComponentAware(parent),
                                                                        ui(new Ui::CDbOwnModelSetComponent)
    {
        ui->setupUi(this);
        ui->tvp_OwnModelSet->setAircraftModelMode(CAircraftModelListModel::OwnModelSet);
        ui->tvp_OwnModelSet->menuAddItems(CAircraftModelView::MenuStashing);
        ui->tvp_OwnModelSet->menuRemoveItems(CAircraftModelView::MenuDisplayAutomaticallyAndRefresh | CAircraftModelView::MenuBackend);
        ui->tvp_OwnModelSet->menuAddItems(CAircraftModelView::MenuRemoveSelectedRows | CAircraftModelView::MenuClear | CAircraftModelView::MenuMaterializeFilter);
        ui->tvp_OwnModelSet->addFilterDialog();
        ui->tvp_OwnModelSet->setCustomMenu(new CLoadModelSetMenu(this));
        ui->tvp_OwnModelSet->setCustomMenu(new CConsolidateWithDbDataMenu(ui->tvp_OwnModelSet, this));
        ui->tvp_OwnModelSet->setCustomMenu(new CConsolidateWithSimulatorModels(ui->tvp_OwnModelSet, this));
        ui->tvp_OwnModelSet->menuAddItems(CAircraftModelView::MenuLoadAndSave | CAircraftModelView::MenuRefresh | CAircraftModelView::MenuOrderable);
        ui->tvp_OwnModelSet->setSorting(CAircraftModel::IndexOrderString);
        ui->tvp_OwnModelSet->initAsOrderable();

        const CSimulatorInfo sim = ui->comp_SimulatorSelector->getValue();
        ui->tvp_OwnModelSet->setCorrespondingSimulator(sim, m_simulatorSettings.getSimulatorDirectoryOrDefault(sim));
        ui->comp_SimulatorSelector->setMode(CSimulatorSelector::RadioButtons);
        ui->comp_SimulatorSelector->setRememberSelectionAndSetToLastSelection();
        const CSimulatorInfo simulator = ui->comp_SimulatorSelector->getValue();
        ui->le_Simulator->setText(simulator.toQString(true));

        //! \fixme hardcoded style sheet
        ui->pb_SaveAsSetForSimulator->setStyleSheet("padding-left: 3px; padding-right: 3px;");

        connect(ui->pb_CreateNewSet, &QPushButton::clicked, this, &CDbOwnModelSetComponent::buttonClicked, Qt::QueuedConnection);
        connect(ui->pb_LoadExistingSet, &QPushButton::clicked, this, &CDbOwnModelSetComponent::buttonClicked, Qt::QueuedConnection);
        connect(ui->pb_ShowStatistics, &QPushButton::clicked, this, &CDbOwnModelSetComponent::buttonClicked, Qt::QueuedConnection);
        connect(ui->pb_FirstSet, &QPushButton::clicked, this, &CDbOwnModelSetComponent::buttonClicked, Qt::QueuedConnection);
        connect(ui->pb_Clear, &QPushButton::clicked, this, &CDbOwnModelSetComponent::buttonClicked, Qt::QueuedConnection);
        connect(ui->pb_CopyFromAnotherSwift, &QPushButton::clicked, this, &CDbOwnModelSetComponent::buttonClicked, Qt::QueuedConnection);
        connect(ui->pb_SaveAsSetForSimulator, &QPushButton::clicked, this, &CDbOwnModelSetComponent::buttonClicked, Qt::QueuedConnection);

        connect(ui->comp_SimulatorSelector, &CSimulatorSelector::changed, this, &CDbOwnModelSetComponent::setSimulator, Qt::QueuedConnection);
        connect(ui->tvp_OwnModelSet, &CAircraftModelView::modelDataChanged, this, &CDbOwnModelSetComponent::onRowCountChanged, Qt::QueuedConnection);
        connect(ui->tvp_OwnModelSet, &CAircraftModelView::modelChanged, this, &CDbOwnModelSetComponent::viewModelChanged, Qt::QueuedConnection);
        connect(ui->tvp_OwnModelSet, &CAircraftModelView::requestUpdate, this, &CDbOwnModelSetComponent::updateViewToCurrentModels, Qt::QueuedConnection);
        connect(ui->tvp_OwnModelSet, &CAircraftModelView::jsonModelsForSimulatorLoaded, this, &CDbOwnModelSetComponent::onJsonDataLoaded, Qt::QueuedConnection);

        this->triggerSetSimulatorDeferred(simulator);
    }

    CDbOwnModelSetComponent::~CDbOwnModelSetComponent()
    {
        // void
    }

    views::CAircraftModelView *CDbOwnModelSetComponent::view() const
    {
        return ui->tvp_OwnModelSet;
    }

    void CDbOwnModelSetComponent::setModelSet(const CAircraftModelList &models, const CSimulatorInfo &simulator)
    {
        Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "Need single simulator");
        this->setSimulator(simulator);
        if (models.isEmpty())
        {
            ui->tvp_OwnModelSet->clear();
            return;
        }

        CAircraftModelList cleanModelList(models.matchesSimulator(simulator)); // remove those not matching the simulator
        const int diff = models.size() - cleanModelList.size();
        if (diff > 0)
        {
            const CStatusMessage m = CStatusMessage(this).warning(u"Removed %1 models from set, because not matching %2") << diff << simulator.toQString(true);
            this->showMappingComponentOverlayHtmlMessage(m, 5000);
        }
        cleanModelList.resetOrder();
        ui->tvp_OwnModelSet->updateContainerMaybeAsync(cleanModelList);
    }

    int CDbOwnModelSetComponent::replaceOrAddModelSet(const CAircraftModelList &models, const CSimulatorInfo &simulator)
    {
        Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "Need single simulator");
        if (models.isEmpty()) { return 0; }
        CAircraftModelList cleanModelList(models.matchesSimulator(simulator)); // remove those not matching the simulator
        const int diff = models.size() - cleanModelList.size();
        if (diff > 0)
        {
            const CStatusMessage m = CStatusMessage(this).warning(u"Removed %1 models from set, because not matching %2") << diff << simulator.toQString(true);
            this->showMappingComponentOverlayHtmlMessage(m, 5000);
        }
        if (cleanModelList.isEmpty()) { return 0; }
        CAircraftModelList updatedModels(ui->tvp_OwnModelSet->container());
        updatedModels.replaceOrAddModelsWithString(cleanModelList, Qt::CaseInsensitive);
        updatedModels.resetOrder();
        ui->tvp_OwnModelSet->updateContainerMaybeAsync(updatedModels);
        return diff;
    }

    const CAircraftModelList &CDbOwnModelSetComponent::getModelSetFromView() const
    {
        return ui->tvp_OwnModelSet->container();
    }

    int CDbOwnModelSetComponent::getModelSetCountFromView() const
    {
        return ui->tvp_OwnModelSet->container().size();
    }

    CStatusMessage CDbOwnModelSetComponent::addToModelSet(const CAircraftModel &model, const CSimulatorInfo &simulator)
    {
        return this->addToModelSet(CAircraftModelList({ model }), simulator);
    }

    CStatusMessage CDbOwnModelSetComponent::addToModelSet(const CAircraftModelList &models, const CSimulatorInfo &simulator)
    {
        Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "Need single simulator");
        if (models.isEmpty()) { return CStatusMessage(this, CStatusMessage::SeverityInfo, u"No data", true); }
        if (!this->getModelSetSimulator().isSingleSimulator())
        {
            // no sim yet, we set it
            this->setSimulator(simulator);
        }
        if (simulator != this->getModelSetSimulator())
        {
            // only currently selected sim allowed
            return CStatusMessage(this, CStatusMessage::SeverityError,
                                  u"Cannot add data for " % simulator.toQString(true) % u" to " % this->getModelSetSimulator().toQString(true), true);
        }

        const bool allowExcludedModels = m_modelSettings.get().getAllowExcludedModels();
        CAircraftModelList updateModels(this->getModelSetFromView());
        int d = updateModels.replaceOrAddModelsWithString(models, Qt::CaseInsensitive);
        if (d > 0)
        {
            if (!allowExcludedModels) { updateModels.removeIfExcluded(); }
            updateModels.resetOrder();
            ui->tvp_OwnModelSet->updateContainerMaybeAsync(updateModels);
            return CStatusMessage(this, CStatusMessage::SeverityInfo, u"Modified " % QString::number(d) % u" entries in model set " % this->getModelSetSimulator().toQString(true), true);
        }
        else
        {
            return CStatusMessage(this, CStatusMessage::SeverityInfo, u"No data modified in model set", true);
        }
    }

    void CDbOwnModelSetComponent::setMappingComponent(CDbMappingComponent *component)
    {
        CDbMappingComponentAware::setMappingComponent(component);
        if (component)
        {
            connect(this->getMappingComponent(), &CDbMappingComponent::tabIndexChanged, this, &CDbOwnModelSetComponent::tabIndexChanged);
        }
    }

    void CDbOwnModelSetComponent::enableButtons(bool firstSet, bool newSet)
    {
        ui->pb_FirstSet->setEnabled(firstSet);
        ui->pb_CreateNewSet->setEnabled(newSet);
    }

    void CDbOwnModelSetComponent::tabIndexChanged(int index)
    {
        Q_UNUSED(index)
    }

    void CDbOwnModelSetComponent::buttonClicked()
    {
        const QObject *sender = QObject::sender();
        if (!sender) { return; }

        if (sender == ui->pb_CreateNewSet)
        {
            this->createNewSet();
            return;
        }

        if (sender == ui->pb_LoadExistingSet)
        {
            ui->tvp_OwnModelSet->showFileLoadDialog();
            return;
        }

        if (sender == ui->pb_SaveAsSetForSimulator)
        {
            const CAircraftModelList ownModelSet(ui->tvp_OwnModelSet->container());
            if (!ownModelSet.isEmpty())
            {
                const CSimulatorInfo sim = this->getSelectedSimulator();
                const CStatusMessage m = this->setCachedModels(ownModelSet, sim);
                CLogMessage::preformatted(m);
                if (m.isSuccess())
                {
                    const QString msg = QStringLiteral("Saved model set for '%1'").arg(sim.toQString(true));

                    // display either as overlay of componet or view
                    if (!this->showMappingComponentOverlayHtmlMessage(msg, 5000))
                    {
                        ui->tvp_OwnModelSet->showOverlayHTMLMessage(msg, 5000);
                    }
                }
                else
                {
                    if (!this->showMappingComponentOverlayMessage(m))
                    {
                        ui->tvp_OwnModelSet->showOverlayMessage(m);
                    }
                }
            }
            return;
        }

        if (sender == ui->pb_ShowStatistics)
        {
            this->showModelStatistics();
            return;
        }

        if (sender == ui->pb_FirstSet)
        {
            this->firstSet();
            return;
        }

        if (sender == ui->pb_CopyFromAnotherSwift)
        {
            this->copyFromAnotherSwift();
            return;
        }

        if (sender == ui->pb_Clear)
        {
            ui->tvp_OwnModelSet->clear();
            return;
        }
    }

    void CDbOwnModelSetComponent::onRowCountChanged(int count, bool withFilter)
    {
        Q_UNUSED(count)
        Q_UNUSED(withFilter)

        const int realUnfilteredCount = ui->tvp_OwnModelSet->container().size();
        const bool canSave = this->getModelSetSimulator().isSingleSimulator() && (realUnfilteredCount > 0);
        ui->pb_SaveAsSetForSimulator->setEnabled(canSave);
        if (canSave)
        {
            this->setSaveFileName(this->getModelSetSimulator());
            ui->pb_SaveAsSetForSimulator->setText(QStringLiteral("save '%1'").arg(this->getModelSetSimulator().toQString(true)));
        }
        else
        {
            ui->pb_SaveAsSetForSimulator->setText("save");
        }
    }

    void CDbOwnModelSetComponent::onJsonDataLoaded(const CSimulatorInfo &simulator)
    {
        if (simulator.isSingleSimulator())
        {
            this->setSimulator(simulator);
        }
    }

    void CDbOwnModelSetComponent::distributorPreferencesChanged()
    {
        const CDistributorListPreferences preferences = m_distributorPreferences.getThreadLocal();
        const CSimulatorInfo simuulator = preferences.getLastUpdatedSimulator();
        if (simuulator.isSingleSimulator())
        {
            this->updateDistributorOrder(simuulator);
        }
    }

    void CDbOwnModelSetComponent::reduceModels()
    {
        if (!m_reduceModelsDialog)
        {
            m_reduceModelsDialog.reset(new CDbReduceModelDuplicates(this));
        }

        CAircraftModelList models = ui->tvp_OwnModelSet->containerOrFilteredContainer();
        const CSimulatorInfo simulator = this->getModelSetSimulator();
        m_reduceModelsDialog->setModels(models, simulator);
        const QDialog::DialogCode ret = static_cast<QDialog::DialogCode>(m_reduceModelsDialog->exec());
        if (ret != QDialog::Accepted) { return; }
        const CAircraftModelList removeModels = m_reduceModelsDialog->getRemoveCandidates();
        const CSimulatorInfo removeSimulator = m_reduceModelsDialog->getSimulator();
        if (removeModels.isEmpty()) { return; }
        const QStringList modelStrings = removeModels.getModelStringList(false);
        models.removeModelsWithString(modelStrings, Qt::CaseInsensitive); // by strings also removes if id is missing
        this->setModelSet(models, removeSimulator);
    }

    void CDbOwnModelSetComponent::removeExcludedModels()
    {
        const CSimulatorInfo simulator = this->getModelSetSimulator();
        CAircraftModelList models = ui->tvp_OwnModelSet->containerOrFilteredContainer();
        const int r = models.removeIfExcluded();
        if (r < 1) { return; }
        this->setModelSet(models, simulator);
    }

    void CDbOwnModelSetComponent::removeNonDBModels()
    {
        const CSimulatorInfo simulator = this->getModelSetSimulator();
        CAircraftModelList models = ui->tvp_OwnModelSet->containerOrFilteredContainer();
        const int r = models.removeObjectsWithoutDbKey();
        if (r < 1) { return; }
        this->setModelSet(models, simulator);
    }

    void CDbOwnModelSetComponent::viewModelChanged()
    {
        const bool hasData = ui->tvp_OwnModelSet->rowCount() > 0;
        ui->pb_SaveAsSetForSimulator->setEnabled(hasData);
    }

    void CDbOwnModelSetComponent::setSaveFileName(const CSimulatorInfo &simulator)
    {
        Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "Need single simulator");
        const QString name("modelset" + simulator.toQString(true));
        ui->tvp_OwnModelSet->setSaveFileName(name);
    }

    void CDbOwnModelSetComponent::updateViewToCurrentModels()
    {
        const CAircraftModelList models(this->getModelSet());
        ui->tvp_OwnModelSet->updateContainerMaybeAsync(models);
    }

    void CDbOwnModelSetComponent::createNewSet()
    {
        // make sure both tabs display the same simulator
        // since we use the component also in the launcher wizard, mc might not be existing
        const CSimulatorInfo simulator(this->getModelSetSimulator());
        if (!simulator.isSingleSimulator())
        {
            // Ref T630, avoid invalid simulator
            CLogMessage(this).error(u"No single simulator!");
            return;
        }

        CDbMappingComponent *mc = this->getMappingComponent();
        if (!mc)
        {
            CLogMessage(this).error(u"No mapping component available!");
            return;
        }

        mc->setOwnModelsSimulator(simulator);
        if (!m_modelSetFormDialog)
        {
            m_modelSetFormDialog.reset(new CDbOwnModelSetFormDialog(this));
            m_modelSetFormDialog->setMappingComponent(this->getMappingComponent());
        }

        if (mc->getOwnModelsCount() > 0)
        {
            m_modelSetFormDialog->setModal(true);
            m_modelSetFormDialog->reloadData();
            const QDialog::DialogCode rc = static_cast<QDialog::DialogCode>(m_modelSetFormDialog->exec());
            if (rc == QDialog::Accepted)
            {
                this->setModelSet(m_modelSetFormDialog->getModelSet(), m_modelSetFormDialog->getSimulatorInfo());
            }
        }
        else
        {
            const CStatusMessage m = CStatusMessage(this).error(u"No model data for %1") << simulator.toQString(true);
            mc->showOverlayMessage(m);
        }
    }

    void CDbOwnModelSetComponent::firstSet()
    {
        if (!m_firstModelSetDialog)
        {
            m_firstModelSetDialog.reset(new CFirstModelSetDialog(this));
        }
        m_firstModelSetDialog->show();
    }

    void CDbOwnModelSetComponent::copyFromAnotherSwift()
    {
        if (!m_copyFromAnotherSwiftDialog)
        {
            m_copyFromAnotherSwiftDialog.reset(new CCopyModelsFromOtherSwiftVersionsDialog(this));
        }
        m_copyFromAnotherSwiftDialog->show();
    }

    void CDbOwnModelSetComponent::setSimulator(const CSimulatorInfo &simulator)
    {
        if (m_simulator == simulator) { return; } // avoid unnecessary signals
        if (simulator.isNoSimulator()) { return; }
        Q_ASSERT_X(simulator.isSingleSimulator(), Q_FUNC_INFO, "Need single simulator");

        m_simulator = simulator;
        ui->comp_SimulatorSelector->setValue(simulator);
        ui->tvp_OwnModelSet->setCorrespondingSimulator(simulator, m_simulatorSettings.getSimulatorDirectoryOrDefault(m_simulator));
        ui->le_Simulator->setText(simulator.toQString(true));
        this->synchronizeCache(simulator);
        this->updateViewToCurrentModels();
    }

    void CDbOwnModelSetComponent::setSimulatorSelectorMode(CSimulatorSelector::Mode mode)
    {
        ui->comp_SimulatorSelector->setMode(mode);
    }

    void CDbOwnModelSetComponent::triggerSetSimulatorDeferred(const CSimulatorInfo &simulator)
    {
        this->admitCache(simulator); // trigger loading

        QPointer<CDbOwnModelSetComponent> myself(this);
        QTimer::singleShot(1000, this, [=] {
            if (!sApp || sApp->isShuttingDown()) { return; }
            if (!myself) { return; }
            this->setSimulator(simulator);
        });
    }

    void CDbOwnModelSetComponent::showModelStatistics()
    {
        const CAircraftModelList set(this->getModelSetFromView());
        if (!m_modelStatisticsDialog)
        {
            m_modelStatisticsDialog.reset(new CAircraftModelStatisticsDialog(this));
        }
        m_modelStatisticsDialog->analyzeModels(set);
        m_modelStatisticsDialog->show();
    }

    void CDbOwnModelSetComponent::updateDistributorOrder(const CSimulatorInfo &simulator)
    {
        CAircraftModelList modelSet = this->getCachedModels(simulator);
        if (modelSet.isEmpty()) { return; }
        const CDistributorListPreferences preferences = m_distributorPreferences.getThreadLocal();
        const CDistributorList distributors = preferences.getDistributors(simulator);
        if (distributors.isEmpty()) { return; }
        modelSet.updateDistributorOrder(distributors);
        this->setCachedModels(modelSet, simulator);

        // display?
        const CSimulatorInfo currentSimulator(this->getModelSetSimulator());
        if (simulator == currentSimulator)
        {
            ui->tvp_OwnModelSet->updateContainerAsync(modelSet);
        }
    }

    bool CDbOwnModelSetComponent::runsInDialog()
    {
        return CGuiUtility::findParentDialog(this, 5);
    }

    void CDbOwnModelSetComponent::CLoadModelSetMenu::customMenu(CMenuActions &menuActions)
    {
        // for the moment I use all sims, I could restrict to CSimulatorInfo::getLocallyInstalledSimulators();
        const CSimulatorInfo sims = CSimulatorInfo::allSimulators();
        const bool noSims = sims.isNoSimulator() || sims.isUnspecified();
        if (!noSims)
        {
            CDbOwnModelSetComponent *ownModelSetComp = qobject_cast<CDbOwnModelSetComponent *>(this->parent());
            Q_ASSERT_X(ownModelSetComp, Q_FUNC_INFO, "Cannot access parent");
            if (m_setActions.isEmpty())
            {
                if (sims.isFSX())
                {
                    QAction *a = new QAction(CIcons::appModels16(), "FSX models", this);
                    connect(a, &QAction::triggered, ownModelSetComp, [ownModelSetComp](bool checked) {
                        Q_UNUSED(checked)
                        ownModelSetComp->setSimulator(CSimulatorInfo(CSimulatorInfo::FSX));
                    });
                    m_setActions.append(a);

                    a = new QAction(CIcons::appModels16(), "New set FSX models", this);
                    connect(a, &QAction::triggered, ownModelSetComp, [ownModelSetComp](bool checked) {
                        Q_UNUSED(checked)
                        ownModelSetComp->setModelSet(CAircraftModelList(), CSimulatorInfo(CSimulatorInfo::FSX));
                    });
                    m_setNewActions.append(a);
                }
                if (sims.isP3D())
                {
                    QAction *a = new QAction(CIcons::appModels16(), "P3D models", this);
                    connect(a, &QAction::triggered, ownModelSetComp, [ownModelSetComp](bool checked) {
                        Q_UNUSED(checked)
                        ownModelSetComp->setSimulator(CSimulatorInfo(CSimulatorInfo::P3D));
                    });
                    m_setActions.append(a);

                    a = new QAction(CIcons::appModels16(), "New set P3D models", this);
                    connect(a, &QAction::triggered, ownModelSetComp, [ownModelSetComp](bool checked) {
                        Q_UNUSED(checked)
                        ownModelSetComp->setModelSet(CAircraftModelList(), CSimulatorInfo(CSimulatorInfo::P3D));
                    });
                    m_setNewActions.append(a);
                }
                if (sims.isFS9())
                {
                    QAction *a = new QAction(CIcons::appModels16(), "FS9 models", this);
                    connect(a, &QAction::triggered, ownModelSetComp, [ownModelSetComp](bool checked) {
                        Q_UNUSED(checked)
                        ownModelSetComp->setSimulator(CSimulatorInfo(CSimulatorInfo::FS9));
                    });
                    m_setActions.append(a);

                    a = new QAction(CIcons::appModels16(), "New set FS9 models", this);
                    connect(a, &QAction::triggered, ownModelSetComp, [ownModelSetComp](bool checked) {
                        Q_UNUSED(checked)
                        ownModelSetComp->setModelSet(CAircraftModelList(), CSimulatorInfo(CSimulatorInfo::FS9));
                    });
                    m_setNewActions.append(a);
                }
                if (sims.isXPlane())
                {
                    QAction *a = new QAction(CIcons::appModels16(), "XPlane models", this);
                    connect(a, &QAction::triggered, ownModelSetComp, [ownModelSetComp](bool checked) {
                        Q_UNUSED(checked)
                        ownModelSetComp->setSimulator(CSimulatorInfo(CSimulatorInfo::XPLANE));
                    });
                    m_setActions.append(a);

                    a = new QAction(CIcons::appModels16(), "New set XPlane models", this);
                    connect(a, &QAction::triggered, ownModelSetComp, [ownModelSetComp](bool checked) {
                        Q_UNUSED(checked)
                        ownModelSetComp->setModelSet(CAircraftModelList(), CSimulatorInfo(CSimulatorInfo::XPLANE));
                    });
                    m_setNewActions.append(a);
                }
                if (sims.isFG())
                {
                    QAction *a = new QAction(CIcons::appModels16(), "FG models", this);
                    connect(a, &QAction::triggered, ownModelSetComp, [ownModelSetComp](bool checked) {
                        Q_UNUSED(checked)
                        ownModelSetComp->setSimulator(CSimulatorInfo(CSimulatorInfo::FG));
                    });
                    m_setActions.append(a);

                    a = new QAction(CIcons::appModels16(), "New set FG models", this);
                    connect(a, &QAction::triggered, ownModelSetComp, [ownModelSetComp](bool checked) {
                        Q_UNUSED(checked)
                        ownModelSetComp->setModelSet(CAircraftModelList(), CSimulatorInfo(CSimulatorInfo::FG));
                    });
                    m_setNewActions.append(a);
                }
                if (sims.isMSFS())
                {
                    QAction *a = new QAction(CIcons::appModels16(), "MSFS models", this);
                    connect(a, &QAction::triggered, ownModelSetComp, [ownModelSetComp](bool checked) {
                        Q_UNUSED(checked)
                        ownModelSetComp->setSimulator(CSimulatorInfo(CSimulatorInfo::MSFS));
                    });
                    m_setActions.append(a);

                    a = new QAction(CIcons::appModels16(), "New set MSFS models", this);
                    connect(a, &QAction::triggered, ownModelSetComp, [ownModelSetComp](bool checked) {
                        Q_UNUSED(checked)
                        ownModelSetComp->setModelSet(CAircraftModelList(), CSimulatorInfo(CSimulatorInfo::MSFS));
                    });
                    m_setNewActions.append(a);
                }

                QAction *a = new QAction(CIcons::appDistributors16(), "Apply distributor preferences", this);
                connect(a, &QAction::triggered, ownModelSetComp, &CDbOwnModelSetComponent::distributorPreferencesChanged, Qt::QueuedConnection);
                m_setActions.append(a);

                a = new QAction(CIcons::delete16(), "Reduce models (remove duplicates)", this);
                connect(a, &QAction::triggered, ownModelSetComp, &CDbOwnModelSetComponent::reduceModels, Qt::QueuedConnection);
                m_setActions.append(a);

                a = new QAction(CIcons::delete16(), "Remove excluded models", this);
                connect(a, &QAction::triggered, ownModelSetComp, &CDbOwnModelSetComponent::removeExcludedModels, Qt::QueuedConnection);
                m_setActions.append(a);

                a = new QAction(CIcons::delete16(), "Remove non DB models", this);
                connect(a, &QAction::triggered, ownModelSetComp, &CDbOwnModelSetComponent::removeNonDBModels, Qt::QueuedConnection);
                m_setActions.append(a);
            }
            menuActions.addMenuModelSet();
            menuActions.addActions(m_setActions, CMenuAction::pathModelSet());
            menuActions.addActions(m_setNewActions, CMenuAction::pathModelSetNew());
        }
        this->nestedCustomMenu(menuActions);
    }
} // ns
