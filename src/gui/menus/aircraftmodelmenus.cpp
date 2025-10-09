// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "aircraftmodelmenus.h"

#include <QDesktopServices>

#include "config/buildconfig.h"
#include "core/db/databaseutils.h"
#include "core/webdataservices.h"
#include "gui/components/dbmappingcomponent.h"
#include "gui/components/dbmappingcomponentaware.h"
#include "gui/guiapplication.h"
#include "misc/icons.h"
#include "misc/logmessage.h"
#include "misc/simulation/modelconverterx.h"

using namespace swift::config;
using namespace swift::misc;
using namespace swift::misc::simulation;
using namespace swift::gui;
using namespace swift::gui::views;
using namespace swift::gui::models;
using namespace swift::gui::components;
using namespace swift::core::db;

namespace swift::gui::menus
{
    void IAircraftModelViewMenu::anchor() {}

    const QStringList &IAircraftModelViewMenu::getLogCategories()
    {
        static const QStringList cats { CLogCategories::guiComponent() };
        return cats;
    }

    CAircraftModelView *IAircraftModelViewMenu::modelView() const
    {
        auto *mv = qobject_cast<CAircraftModelView *>(parent());
        Q_ASSERT_X(mv, Q_FUNC_INFO, "no view");
        return mv;
    }

    const CAircraftModelList &IAircraftModelViewMenu::getAircraftModels() const
    {
        const CAircraftModelView *mv = modelView();
        Q_ASSERT_X(mv, Q_FUNC_INFO, "no view");
        return mv->container();
    }

    const CAircraftModelList &IAircraftModelViewMenu::getAllOrAllFilteredAircraftModels(bool *filtered) const
    {
        const CAircraftModelView *mv = modelView();
        Q_ASSERT_X(mv, Q_FUNC_INFO, "no view");
        return mv->containerOrFilteredContainer(filtered);
    }

    CAircraftModelList IAircraftModelViewMenu::getSelectedAircraftModels() const
    {
        const CAircraftModelView *mv = modelView();
        Q_ASSERT_X(mv, Q_FUNC_INFO, "no view");
        return mv->selectedObjects();
    }

    CShowSimulatorFileMenu::CShowSimulatorFileMenu(CAircraftModelView *modelView, COverlayMessagesFrame *messageFrame)
        : IAircraftModelViewMenu(modelView), m_messageFrame(messageFrame)
    {}

    const QStringList &CShowSimulatorFileMenu::getLogCategories()
    {
        static const QStringList cats { CLogCategories::guiComponent() };
        return cats;
    }

    void CShowSimulatorFileMenu::customMenu(CMenuActions &menuActions)
    {
        CAircraftModelView *mv = modelView();
        Q_ASSERT_X(mv, Q_FUNC_INFO, "no view");

        if (mv->hasSingleSelectedRow())
        {
            const CAircraftModel model(mv->selectedObject());
            bool added = false;
            if (model.hasFileName())
            {
                menuActions.addMenuSimulator();
                m_fileAction = menuActions.addAction(m_fileAction, CIcons::text16(), "Open simulator file",
                                                     CMenuAction::pathSimulator(),
                                                     { this, &CShowSimulatorFileMenu::showSimulatorFile });
                added = true;
                if (CModelConverterX::supportsModelConverterX())
                {
                    m_modelConverterX =
                        menuActions.addAction(m_modelConverterX, "ModelConverterX", CMenuAction::pathSimulator(),
                                              { this, &CShowSimulatorFileMenu::startModelConverterX });
                }
            }

            if (added) { menuActions.addSeparator(CMenuAction::pathSimulator()); }
        }
        this->nestedCustomMenu(menuActions);
    }

    void CShowSimulatorFileMenu::showSimulatorFile()
    {
        const CAircraftModelView *mv = modelView();
        if (!mv->hasSingleSelectedRow()) { return; }
        const CAircraftModel model(mv->selectedObject());
        if (!model.hasFileName()) { return; }
        if (QFile::exists(model.getFileName()))
        {
            const QUrl url = QUrl::fromLocalFile(model.getFileName());
            QDesktopServices::openUrl(url);
        }
        else
        {
            const CStatusMessage m =
                CStatusMessage(this, CStatusMessage::SeverityError,
                               QStringLiteral("No file for model '&1'").arg(model.getFileName()), true);
            if (m_messageFrame) { m_messageFrame->showOverlayHTMLMessage(m); }
            CLogMessage::preformatted(m);
        }
    }

    void CShowSimulatorFileMenu::startModelConverterX()
    {
        if (!CModelConverterX::supportsModelConverterX()) { return; }
        const CAircraftModelView *mv = modelView();
        if (!mv->hasSingleSelectedRow()) { return; }
        const CAircraftModel model(mv->selectedObject());
        CModelConverterX::startModelConverterX(model, sApp);
    }

    // --------------------------------- with DB data ---------------------------------

    CConsolidateWithDbDataMenu::CConsolidateWithDbDataMenu(CAircraftModelView *modelView, QObject *modelsTarget)
        : IAircraftModelViewMenu(modelView), m_modelsTarget(modelsTarget)
    {
        // it can be the target is not yet known
        if (modelsTarget)
        {
            const bool ok = modelsTargetSetable() || modelsTargetUpdatable();
            Q_ASSERT_X(ok, Q_FUNC_INFO, "Neither setable nor updatable");
            Q_UNUSED(ok)
        }
    }

    const QStringList &CConsolidateWithDbDataMenu::getLogCategories()
    {
        static const QStringList cats { CLogCategories::mapping(), CLogCategories::guiComponent() };
        return cats;
    }

    void CConsolidateWithDbDataMenu::customMenu(CMenuActions &menuActions)
    {
        const CAircraftModelView *mv = modelView();
        if (!mv || mv->isEmpty())
        {
            this->nestedCustomMenu(menuActions);
            return;
        }
        if (!sGui->hasWebDataServices())
        {
            this->nestedCustomMenu(menuActions);
            return;
        }

        menuActions.addMenuConsolidateModels();

        m_consolidateAll = menuActions.addAction(m_consolidateAll, CIcons::databaseEdit16(), "All with DB data",
                                                 CMenuAction::pathModelConsolidate(),
                                                 { this, &CConsolidateWithDbDataMenu::consolidateData });
        if (mv->hasSelection())
        {
            m_consolidateSelected = menuActions.addAction(
                m_consolidateSelected, CIcons::databaseEdit16(), "Selected with DB data",
                CMenuAction::pathModelConsolidate(), { this, &CConsolidateWithDbDataMenu::consolidateSelectedData });
        }
        this->nestedCustomMenu(menuActions);
    }

    void CConsolidateWithDbDataMenu::consolidateData()
    {
        if (!sGui || sGui->isShuttingDown() || !sGui->hasWebDataServices()) { return; }
        const CAircraftModelList dbModels(sGui->getWebDataServices()->getModels());
        if (dbModels.isEmpty())
        {
            CLogMessage(this).warning(u"No DB models to consolidate with");
            return;
        }
        if (!this->modelsTargetSetable())
        {
            CLogMessage(this).warning(u"No setable target");
            return;
        }

        this->modelView()->showLoadIndicator();
        bool filtered = false;
        CAircraftModelList models(this->getAllOrAllFilteredAircraftModels(&filtered));

        const int c = CDatabaseUtils::consolidateModelsWithDbDataAllowsGuiRefresh(models, true, true);
        if (c > 0 && this->modelsTargetSetable() && this->modelsTargetUpdatable())
        {
            if (filtered)
            {
                this->modelsTargetUpdatable()->updateModels(models);
                CLogMessage(this).info(u"Consolidated %1/%2 filtered models with DB") << c << models.size();
            }
            else
            {
                this->modelsTargetSetable()->setModels(models);
                CLogMessage(this).info(u"Consolidated %1/%2 models with DB") << c << models.size();
            }
        }
        else
        {
            CLogMessage(this).info(u"No data consolidated with DB");
            this->modelView()->hideLoadIndicator();
        }
    }

    void CConsolidateWithDbDataMenu::consolidateSelectedData()
    {
        Q_ASSERT_X(sGui, Q_FUNC_INFO, "Missing sGui");
        if (!sGui->hasWebDataServices()) { return; }

        CAircraftModelList models(this->getSelectedAircraftModels());
        if (models.isEmpty()) { return; }
        if (!this->modelsTargetUpdatable())
        {
            CLogMessage(this).warning(u"No updatable target");
            return;
        }
        const int c = CDatabaseUtils::consolidateModelsWithDbDataAllowsGuiRefresh(models, true, true);
        if (c > 0 && this->modelsTargetUpdatable()) { this->modelsTargetUpdatable()->updateModels(models); }
    }

    IModelsSetable *CConsolidateWithDbDataMenu::modelsTargetSetable() const
    {
        return qobject_cast<IModelsSetable *>(m_modelsTarget);
    }

    IModelsUpdatable *CConsolidateWithDbDataMenu::modelsTargetUpdatable() const
    {
        return qobject_cast<IModelsUpdatable *>(m_modelsTarget);
    }

    // --------------------------------- with simulator models ---------------------------------

    CConsolidateWithSimulatorModels::CConsolidateWithSimulatorModels(CAircraftModelView *modelView,
                                                                     QObject *modelsTarget)
        : IAircraftModelViewMenu(modelView), m_modelsTarget(modelsTarget)
    {
        // it can be the target is not yet known
        if (modelsTarget)
        {
            const bool ok = modelsTargetSetable() || modelsTargetUpdatable();
            Q_ASSERT_X(ok, Q_FUNC_INFO, "Neither setable nor updatable");
            Q_UNUSED(ok)
        }
    }

    const QStringList &CConsolidateWithSimulatorModels::getLogCategories()
    {
        static const QStringList cats { CLogCategories::mapping(), CLogCategories::guiComponent() };
        return cats;
    }

    void CConsolidateWithSimulatorModels::customMenu(CMenuActions &menuActions)
    {
        const CAircraftModelView *mv = modelView();
        if (mv->isEmpty())
        {
            this->nestedCustomMenu(menuActions);
            return;
        }
        if (!sGui->hasWebDataServices())
        {
            this->nestedCustomMenu(menuActions);
            return;
        }

        menuActions.addMenuConsolidateModels();

        // consolidate
        m_consolidateAll = menuActions.addAction(m_consolidateAll, CIcons::appModels16(), "All with simulator models",
                                                 CMenuAction::pathModelConsolidate(),
                                                 { this, &CConsolidateWithSimulatorModels::consolidateData });
        if (mv->hasSelection())
        {
            m_consolidateSelected =
                menuActions.addAction(m_consolidateSelected, CIcons::appModels16(), "Selected with simulator models",
                                      CMenuAction::pathModelConsolidate(),
                                      { this, &CConsolidateWithSimulatorModels::consolidateSelectedData });
        }

        // update directories
        m_updateDirsAll = menuActions.addAction(m_updateDirsAll, CIcons::disk16(), "Update all directories",
                                                CMenuAction::pathModelConsolidate(),
                                                { this, &CConsolidateWithSimulatorModels::updateDirectoryData });
        if (mv->hasSelection())
        {
            m_updateDirsSelected =
                menuActions.addAction(m_updateDirsSelected, CIcons::disk16(), "Update directories for selected",
                                      CMenuAction::pathModelConsolidate(),
                                      { this, &CConsolidateWithSimulatorModels::updateDirectorySelectedData });
        }

        this->nestedCustomMenu(menuActions);
    }

    void CConsolidateWithSimulatorModels::consolidateData()
    {
        using namespace std::chrono_literals;

        bool filtered = false;
        const CAircraftModelList models(this->getAllOrAllFilteredAircraftModels(&filtered));
        if (models.isEmpty()) { return; }
        QStringList removedModelStrings;
        const int i = this->modelView()->showLoadIndicator();
        const CAircraftModelList consolidated = CDatabaseUtils::consolidateModelsWithSimulatorModelsAllowsGuiRefresh(
            models, this->getSimulatorModels(), removedModelStrings, true);
        const CSimulatorInfo sim(this->getSimulator());

        if (!filtered) { this->modelsTargetSetable()->setModelsForSimulator(consolidated, sim); }
        else
        {
            if (!this->modelsTargetUpdatable()) { CLogMessage(this).warning(u"No updatable target"); }
            else { this->modelsTargetUpdatable()->updateModelsForSimulator(consolidated, sim); }
        }
        this->modelView()->hideLoadIndicator(i);
        if (!removedModelStrings.isEmpty() && this->getMappingComponent())
        {
            const CStatusMessage m = CStatusMessage(this).info(u"Removed %1 model(s)") << removedModelStrings.size();
            this->getMappingComponent()->showOverlayMessage(m, 5s);
        }
    }

    void CConsolidateWithSimulatorModels::consolidateSelectedData()
    {
        using namespace std::chrono_literals;

        Q_ASSERT_X(sGui, Q_FUNC_INFO, "Missing sGui");
        const CAircraftModelList models(this->getSelectedAircraftModels());
        if (models.isEmpty()) { return; }
        if (!this->modelsTargetUpdatable())
        {
            CLogMessage(this).warning(u"No updatable target");
            return;
        }

        QStringList removedModelStrings;
        const int i = this->modelView()->showLoadIndicator();
        const CAircraftModelList consolidated = CDatabaseUtils::consolidateModelsWithSimulatorModelsAllowsGuiRefresh(
            models, this->getSimulatorModels(), removedModelStrings, true);
        const CSimulatorInfo sim(this->getSimulator());

        this->modelsTargetUpdatable()->updateModelsForSimulator(consolidated, sim);
        this->modelView()->hideLoadIndicator(i);
        if (!removedModelStrings.isEmpty() && this->getMappingComponent())
        {
            const CStatusMessage m = CStatusMessage(this).info(u"Removed %1 model(s)") << removedModelStrings.size();
            this->getMappingComponent()->showOverlayMessage(m, 5s);
        }
    }

    void CConsolidateWithSimulatorModels::updateDirectoryData()
    {
        using namespace std::chrono_literals;

        bool filtered = false;
        const CAircraftModelList models(this->getAllOrAllFilteredAircraftModels(&filtered));
        if (models.isEmpty()) { return; }
        QStringList removedModelStrings;
        const int i = this->modelView()->showLoadIndicator();
        const CAircraftModelList consolidated = CDatabaseUtils::updateModelsDirectoriesAllowsGuiRefresh(
            models, this->getSimulatorModels(), removedModelStrings, true);
        const CSimulatorInfo sim(this->getSimulator());

        if (!filtered) { this->modelsTargetSetable()->setModelsForSimulator(consolidated, sim); }
        else
        {
            if (!this->modelsTargetUpdatable()) { CLogMessage(this).warning(u"No updatable target"); }
            else { this->modelsTargetUpdatable()->updateModelsForSimulator(consolidated, sim); }
        }
        this->modelView()->hideLoadIndicator(i);
        if (!removedModelStrings.isEmpty() && this->getMappingComponent())
        {
            const CStatusMessage m = CStatusMessage(this).info(u"Removed %1 model(s)") << removedModelStrings.size();
            this->getMappingComponent()->showOverlayMessage(m, 5s);
        }
    }

    void CConsolidateWithSimulatorModels::updateDirectorySelectedData()
    {
        using namespace std::chrono_literals;

        Q_ASSERT_X(sGui, Q_FUNC_INFO, "Missing sGui");
        const CAircraftModelList models(this->getSelectedAircraftModels());
        if (models.isEmpty()) { return; }
        if (!this->modelsTargetUpdatable())
        {
            CLogMessage(this).warning(u"No updatable target");
            return;
        }

        QStringList removedModelStrings;
        const int i = this->modelView()->showLoadIndicator();
        const CAircraftModelList consolidated = CDatabaseUtils::updateModelsDirectoriesAllowsGuiRefresh(
            models, this->getSimulatorModels(), removedModelStrings, true);
        const CSimulatorInfo sim(this->getSimulator());

        this->modelsTargetUpdatable()->updateModelsForSimulator(consolidated, sim);
        this->modelView()->hideLoadIndicator(i);
        if (!removedModelStrings.isEmpty() && this->getMappingComponent())
        {
            const CStatusMessage m = CStatusMessage(this).info(u"Removed %1 model(s)") << removedModelStrings.size();
            this->getMappingComponent()->showOverlayMessage(m, 5s);
        }
    }

    CAircraftModelList CConsolidateWithSimulatorModels::getSimulatorModels() const
    {
        CDbMappingComponent *mc = this->getMappingComponent();
        Q_ASSERT_X(mc, Q_FUNC_INFO, "No mapping component");
        const CSimulatorInfo sim = this->getSimulator();
        mc->setOwnModelsSimulator(sim);
        return mc->getOwnModels();
    }

    CSimulatorInfo CConsolidateWithSimulatorModels::getSimulator() const
    {
        const ISimulatorSelectable *s = this->simulatorSelectable();
        Q_ASSERT_X(s, Q_FUNC_INFO, "No ISimulatorSelectable");
        const CSimulatorInfo sim = s->getSelectedSimulator();
        Q_ASSERT_X(sim.isSingleSimulator(), Q_FUNC_INFO, "Need single simulator");
        return sim;
    }

    IModelsForSimulatorSetable *CConsolidateWithSimulatorModels::modelsTargetSetable() const
    {
        return qobject_cast<IModelsForSimulatorSetable *>(m_modelsTarget);
    }

    IModelsForSimulatorUpdatable *CConsolidateWithSimulatorModels::modelsTargetUpdatable() const
    {
        return qobject_cast<IModelsForSimulatorUpdatable *>(m_modelsTarget);
    }

    ISimulatorSelectable *CConsolidateWithSimulatorModels::simulatorSelectable() const
    {
        return qobject_cast<ISimulatorSelectable *>(m_modelsTarget);
    }

    components::CDbMappingComponent *CConsolidateWithSimulatorModels::getMappingComponent() const
    {
        // try to cast target
        CDbMappingComponent *mc = nullptr;
        CDbMappingComponentAware *mca = qobject_cast<CDbMappingComponentAware *>(m_modelsTarget);
        if (mca) { mc = mca->getMappingComponent(); }
        if (!mc) { mc = qobject_cast<CDbMappingComponent *>(m_modelsTarget); }
        return mc;
    }
} // namespace swift::gui::menus
