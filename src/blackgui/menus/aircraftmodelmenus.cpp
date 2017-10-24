/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "aircraftmodelmenus.h"
#include "blackgui/guiapplication.h"
#include "blackgui/components/dbmappingcomponent.h"
#include "blackgui/components/dbmappingcomponentaware.h"
#include "blackcore/webdataservices.h"
#include "blackcore/db/databaseutils.h"
#include "blackmisc/verify.h"
#include "blackmisc/icons.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/simulation/aircraftmodelutils.h"
#include "blackmisc/simulation/modelconverterx.h"
#include "blackconfig/buildconfig.h"
#include <QDesktopServices>

using namespace BlackConfig;
using namespace BlackMisc;
using namespace BlackMisc::Simulation;
using namespace BlackGui;
using namespace BlackGui::Views;
using namespace BlackGui::Models;
using namespace BlackGui::Components;
using namespace BlackCore::Db;

namespace BlackGui
{
    namespace Menus
    {
        const CLogCategoryList &IAircraftModelViewMenu::getLogCategories()
        {
            static const CLogCategoryList cats { CLogCategory::guiComponent() };
            return cats;
        }

        CAircraftModelView *IAircraftModelViewMenu::modelView() const
        {
            CAircraftModelView *mv = qobject_cast<CAircraftModelView *>(parent());
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

        CShowSimulatorFileMenu::CShowSimulatorFileMenu(CAircraftModelView *modelView, COverlayMessagesFrame *messageFrame, bool separator) :
            IAircraftModelViewMenu(modelView, separator), m_messageFrame(messageFrame)
        { }

        const CLogCategoryList &CShowSimulatorFileMenu::getLogCategories()
        {
            static const CLogCategoryList cats { CLogCategory::guiComponent() };
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
                    m_fileAction = menuActions.addAction(m_fileAction, CIcons::text16(), "Open simulator file", CMenuAction::pathSimulator(), { this, &CShowSimulatorFileMenu::showSimulatorFile });
                    added = true;
                    if (CModelConverterX::supportsModelConverterX())
                    {
                        m_modelConverterX = menuActions.addAction(m_modelConverterX, CIcons::modelConverterX(), "ModelConverterX", CMenuAction::pathSimulator(), { this, &CShowSimulatorFileMenu::startModelConverterX });
                    }
                }

                if (m_messageFrame && !model.getIconPath().isEmpty())
                {
                    added = true;
                    menuActions.addMenuSimulator();
                    m_iconAction = menuActions.addAction(m_iconAction, CIcons::appAircraft16(), "Display icon", CMenuAction::pathSimulator(), { this, &CShowSimulatorFileMenu::displayIcon });
                }

                if (added)
                {
                    menuActions.addSeparator(CMenuAction::pathSimulator());
                }
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
        }

        void CShowSimulatorFileMenu::displayIcon()
        {
            const CAircraftModelView *mv = modelView();
            if (!mv->hasSingleSelectedRow()) { return; }
            const CAircraftModel model(mv->selectedObject());
            if (model.getIconPath().isEmpty()) { return; }
            CStatusMessage msg(this);
            const CPixmap pm(model.loadIcon(msg));
            if (msg.isSuccess())
            {
                m_messageFrame->showOverlayImage(pm);
            }
            else
            {
                CLogMessage::preformatted(msg);
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

        CConsolidateWithDbDataMenu::CConsolidateWithDbDataMenu(CAircraftModelView *modelView, QObject *modelsTarget, bool separator) :
            IAircraftModelViewMenu(modelView, separator), m_modelsTarget(modelsTarget)
        {
            // it can be the target is not yet known
            if (modelsTarget)
            {
                const bool ok = modelsTargetSetable() || modelsTargetUpdatable();
                Q_ASSERT_X(ok, Q_FUNC_INFO, "Neither setable nor updatable");
                Q_UNUSED(ok);
            }
        }

        const CLogCategoryList &CConsolidateWithDbDataMenu::getLogCategories()
        {
            static const CLogCategoryList cats { CLogCategory::mapping(), CLogCategory::guiComponent() };
            return cats;
        }

        void CConsolidateWithDbDataMenu::customMenu(CMenuActions &menuActions)
        {
            const CAircraftModelView *mv = modelView();
            if (mv->isEmpty()) { this->nestedCustomMenu(menuActions); return; }
            if (!sGui->hasWebDataServices()) { this->nestedCustomMenu(menuActions); return; }

            menuActions.addMenuConsolidateModels();

            m_consolidateAll = menuActions.addAction(m_consolidateAll, CIcons::databaseEdit16(), "All with DB data", CMenuAction::pathViewModelsConsolidate(), { this, &CConsolidateWithDbDataMenu::consolidateData });
            if (mv->hasSelection())
            {
                m_consolidateSelected = menuActions.addAction(m_consolidateSelected, CIcons::databaseEdit16(), "Selected with DB data", CMenuAction::pathViewModelsConsolidate(), { this, &CConsolidateWithDbDataMenu::consolidateSelectedData });
            }
            this->nestedCustomMenu(menuActions);
        }

        void CConsolidateWithDbDataMenu::consolidateData()
        {
            BLACK_VERIFY_X(sGui, Q_FUNC_INFO, "Missing sGui");
            if (!sGui->hasWebDataServices()) { return; }

            const CAircraftModelList dbModels(sGui->getWebDataServices()->getModels());
            if (dbModels.isEmpty())
            {
                CLogMessage(this).warning("No DB models to consolidate with");
                return;
            }
            if (!this->modelsTargetSetable())
            {
                CLogMessage(this).warning("No setable target");
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
                    CLogMessage(this).info("Consolidated %1/%2 filtered models with DB") << c << models.size();
                }
                else
                {
                    this->modelsTargetSetable()->setModels(models);
                    CLogMessage(this).info("Consolidated %1/%2 models with DB") << c << models.size();
                }
            }
            else
            {
                CLogMessage(this).info("No data consolidated with DB");
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
                CLogMessage(this).warning("No updatable target");
                return;
            }
            const int c = CDatabaseUtils::consolidateModelsWithDbDataAllowsGuiRefresh(models, true, true);
            if (c > 0 && this->modelsTargetUpdatable())
            {
                this->modelsTargetUpdatable()->updateModels(models);
            }
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

        CConsolidateWithSimulatorModels::CConsolidateWithSimulatorModels(CAircraftModelView *modelView, QObject *modelsTarget, bool separator) :
            IAircraftModelViewMenu(modelView, separator), m_modelsTarget(modelsTarget)
        {
            // it can be the target is not yet known
            if (modelsTarget)
            {
                const bool ok = modelsTargetSetable() || modelsTargetUpdatable();
                Q_ASSERT_X(ok, Q_FUNC_INFO, "Neither setable nor updatable");
                Q_UNUSED(ok);
            }
        }

        const CLogCategoryList &CConsolidateWithSimulatorModels::getLogCategories()
        {
            static const CLogCategoryList cats { CLogCategory::mapping(), CLogCategory::guiComponent() };
            return cats;
        }

        void CConsolidateWithSimulatorModels::customMenu(CMenuActions &menuActions)
        {
            const CAircraftModelView *mv = modelView();
            if (mv->isEmpty()) { this->nestedCustomMenu(menuActions); return; }
            if (!sGui->hasWebDataServices()) { this->nestedCustomMenu(menuActions); return; }

            menuActions.addMenuConsolidateModels();

            m_consolidateAll = menuActions.addAction(m_consolidateAll, CIcons::appModels16(), "All with simulator models", CMenuAction::pathViewModelsConsolidate(), { this, &CConsolidateWithSimulatorModels::consolidateData });
            if (mv->hasSelection())
            {
                m_consolidateSelected = menuActions.addAction(m_consolidateSelected, CIcons::appModels16(), "Selected with simulator models", CMenuAction::pathViewModelsConsolidate(), { this, &CConsolidateWithSimulatorModels::consolidateSelectedData });
            }
            this->nestedCustomMenu(menuActions);
        }

        void CConsolidateWithSimulatorModels::consolidateData()
        {
            bool filtered = false;
            const CAircraftModelList models(this->getAllOrAllFilteredAircraftModels(&filtered));
            if (models.isEmpty()) { return; }
            const int i = this->modelView()->showLoadIndicator();
            const CAircraftModelList consolidated = CDatabaseUtils::consolidateModelsWithSimulatorModelsAllowsGuiRefresh(models, this->getSimulatorModels(), true);
            const CSimulatorInfo sim(this->getSimulator());

            if (!filtered)
            {
                this->modelsTargetSetable()->setModels(consolidated, sim);
            }
            else
            {
                if (!this->modelsTargetUpdatable())
                {
                    CLogMessage(this).warning("No updatable target");
                }
                else
                {
                    this->modelsTargetUpdatable()->updateModels(consolidated, sim);
                }
            }
            this->modelView()->hideLoadIndicator(i);
        }

        void CConsolidateWithSimulatorModels::consolidateSelectedData()
        {
            Q_ASSERT_X(sGui, Q_FUNC_INFO, "Missing sGui");
            const CAircraftModelList models(this->getSelectedAircraftModels());
            if (models.isEmpty()) { return; }
            if (!this->modelsTargetUpdatable())
            {
                CLogMessage(this).warning("No updatable target");
                return;
            }

            const int i = this->modelView()->showLoadIndicator();
            const CAircraftModelList consolidated = CDatabaseUtils::consolidateModelsWithSimulatorModelsAllowsGuiRefresh(models, this->getSimulatorModels(), true);
            const CSimulatorInfo sim(this->getSimulator());

            this->modelsTargetUpdatable()->updateModels(consolidated, sim);
            this->modelView()->hideLoadIndicator(i);
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

        IModelsPerSimulatorSetable *CConsolidateWithSimulatorModels::modelsTargetSetable() const
        {
            return qobject_cast<IModelsPerSimulatorSetable *>(m_modelsTarget);
        }

        IModelsPerSimulatorUpdatable *CConsolidateWithSimulatorModels::modelsTargetUpdatable() const
        {
            return qobject_cast<IModelsPerSimulatorUpdatable *>(m_modelsTarget);
        }

        ISimulatorSelectable *CConsolidateWithSimulatorModels::simulatorSelectable() const
        {
            return qobject_cast<ISimulatorSelectable *>(m_modelsTarget);
        }

        Components::CDbMappingComponent *CConsolidateWithSimulatorModels::getMappingComponent() const
        {
            // try to cast target
            CDbMappingComponent *mc = nullptr;
            CDbMappingComponentAware *mca = qobject_cast<CDbMappingComponentAware *>(m_modelsTarget);
            if (mca)
            {
                mc = mca->getMappingComponent();
            }
            if (!mc)
            {
                mc = qobject_cast<CDbMappingComponent *>(m_modelsTarget);
            }
            return mc;
        }
    } // ns
} // ns
