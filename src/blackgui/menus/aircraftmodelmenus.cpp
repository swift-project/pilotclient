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
#include "blackcore/webdataservices.h"
#include "blackmisc/verify.h"
#include "blackmisc/icons.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/simulation/aircraftmodelinterfaces.h"
#include "blackmisc/simulation/aircraftmodelutils.h"

#include <QDesktopServices>

using namespace BlackMisc;
using namespace BlackMisc::Simulation;
using namespace BlackGui;
using namespace BlackGui::Views;
using namespace BlackGui::Models;

namespace BlackGui
{
    namespace Menus
    {
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

        CAircraftModelList IAircraftModelViewMenu::getSelectedAircraftModels() const
        {
            const CAircraftModelView *mv = modelView();
            Q_ASSERT_X(mv, Q_FUNC_INFO, "no view");
            return mv->selectedObjects();
        }

        CShowSimulatorFileMenu::CShowSimulatorFileMenu(CAircraftModelView *modelView, COverlayMessagesFrame *messageFrame, bool separator) :
            IAircraftModelViewMenu(modelView, separator), m_messageFrame(messageFrame)
        { }

        void CShowSimulatorFileMenu::customMenu(CMenuActions &menuActions)
        {
            CAircraftModelView *mv = modelView();
            Q_ASSERT_X(mv, Q_FUNC_INFO, "no view");

            if (mv->hasSingleSelectedRow())
            {
                const CAircraftModel model(mv->selectedObject());
                if (model.hasFileName() || (!model.getIconPath().isEmpty() && this->m_messageFrame))
                {
                    menuActions.addMenuSimulator();
                    if (this->m_messageFrame)
                    {
                        const CAircraftModel model(mv->selectedObject());
                        if (!model.getIconPath().isEmpty())
                        {
                            this->m_iconAction = menuActions.addAction(this->m_iconAction, CIcons::appAircraft16(), "Display icon", CMenuAction::pathSimulator(), { this, &CShowSimulatorFileMenu::ps_displayIcon });
                        }
                    }
                    this->m_fileAction = menuActions.addAction(this->m_fileAction, CIcons::text16(), "Open simulator file", CMenuAction::pathSimulator(), { this, &CShowSimulatorFileMenu::ps_showSimulatorFile });
                }
            }
            this->nestedCustomMenu(menuActions);
        }

        void CShowSimulatorFileMenu::ps_showSimulatorFile()
        {
            const CAircraftModelView *mv = modelView();
            if (!mv->hasSingleSelectedRow()) { return; }
            const CAircraftModel model(mv->selectedObject());
            if (!model.hasFileName()) { return; }
            if (QFile::exists(model.getFileName()))
            {
                const QString url("file:///" + model.getFileName());
                QDesktopServices::openUrl(QUrl(url));
            }
        }

        void CShowSimulatorFileMenu::ps_displayIcon()
        {
            const CAircraftModelView *mv = modelView();
            if (!mv->hasSingleSelectedRow()) { return; }
            const CAircraftModel model(mv->selectedObject());
            if (model.getIconPath().isEmpty()) { return; }
            CStatusMessage msg;
            const CPixmap pm(model.loadIcon(msg));
            if (msg.isSuccess())
            {
                this->m_messageFrame->showOverlayImage(pm);
            }
            else
            {
                msg.setCategories(getLogCategories());
                CLogMessage::preformatted(msg);
            }
        }

        CMergeWithDbDataMenu::CMergeWithDbDataMenu(CAircraftModelView *modelView, QObject *modelsTarget, bool separator) :
            IAircraftModelViewMenu(modelView, separator), m_modelsTarget(modelsTarget)
        {
            if (modelsTarget)
            {
                bool ok = modelsTargetSetable() || modelsTargetUpdatable();
                Q_ASSERT_X(ok, Q_FUNC_INFO, "Neither setable nor updatable");
                Q_UNUSED(ok);
            }
        }

        void CMergeWithDbDataMenu::customMenu(CMenuActions &menuActions)
        {
            const CAircraftModelView *mv = modelView();
            if (mv->isEmpty()) { this->nestedCustomMenu(menuActions); return; }
            if (!sGui->hasWebDataServices()) { this->nestedCustomMenu(menuActions); return; }

            menuActions.addMenuDatabase();
            menuActions.addMenu(CIcons::databaseEdit16(), "Merge with DB data", CMenuAction::pathViewDatabaseMerge());

            this->m_mergeAll = menuActions.addAction(this->m_mergeAll, "All", CMenuAction::pathViewDatabaseMerge(), { this, &CMergeWithDbDataMenu::ps_mergeData });
            if (mv->hasSelection())
            {
                this->m_mergeSelected = menuActions.addAction(this->m_mergeSelected, "Selected only", CMenuAction::pathViewDatabaseMerge(), { this, &CMergeWithDbDataMenu::ps_mergeSelectedData });
            }
            this->nestedCustomMenu(menuActions);
        }

        void CMergeWithDbDataMenu::ps_mergeData()
        {
            BLACK_VERIFY_X(sGui, Q_FUNC_INFO, "Missing sGui");
            if (!sGui->hasWebDataServices()) { return; }

            const CAircraftModelList dbModels(sGui->getWebDataServices()->getModels());
            if (dbModels.isEmpty())
            {
                CLogMessage().warning("No DB models to merge with");
                return;
            }

            this->modelView()->showLoadIndicator();
            CAircraftModelList models(this->getAircraftModels());
            int c = CAircraftModelUtilities::mergeWithDbData(models, dbModels, true);
            if (c > 0 && this->modelsTargetSetable())
            {
                this->modelsTargetSetable()->setModels(models);
                CLogMessage().info("Merged %1/%2 models with DB") << c << models.size();
            }
            else
            {
                CLogMessage().info("No data merged with DB");
            }
        }

        void CMergeWithDbDataMenu::ps_mergeSelectedData()
        {
            Q_ASSERT_X(sGui, Q_FUNC_INFO, "Missing sGui");
            if (!sGui->hasWebDataServices()) { return; }

            CAircraftModelList models(this->getSelectedAircraftModels());
            if (models.isEmpty()) { return; }
            const CAircraftModelList dbModels(sGui->getWebDataServices()->getModels());

            CAircraftModelUtilities::mergeWithDbData(models, dbModels, true);
            if (this->modelsTargetUpdatable())
            {
                this->modelsTargetUpdatable()->updateModels(models);
            }
        }

        IModelsSetable *CMergeWithDbDataMenu::modelsTargetSetable() const
        {
            return qobject_cast<IModelsSetable *>(this->m_modelsTarget);
        }

        IModelsUpdatable *CMergeWithDbDataMenu::modelsTargetUpdatable() const
        {
            return qobject_cast<IModelsUpdatable *>(this->m_modelsTarget);
        }
    } // ns
} // ns
