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
#include "blackmisc/icons.h"
#include "blackmisc/logmessage.h"
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

        void CShowSimulatorFileMenu::customMenu(QMenu &menu) const
        {
            CAircraftModelView *mv = modelView();
            Q_ASSERT_X(mv, Q_FUNC_INFO, "no view");

            if (mv->hasSingleSelectedRow())
            {
                const CAircraftModel model(mv->selectedObject());
                if (model.hasFileName() || (!model.getIconPath().isEmpty() && this->m_messageFrame))
                {
                    this->addSeparator(menu);
                    if (this->m_messageFrame)
                    {
                        const CAircraftModel model(mv->selectedObject());
                        if (!model.getIconPath().isEmpty())
                        {
                            this->addSeparator(menu);
                            menu.addAction(CIcons::appAircraft16(), "Display icon", this, &CShowSimulatorFileMenu::ps_displayIcon);
                        }
                    }
                    menu.addAction(CIcons::text16(), "Open simulator file", this, &CShowSimulatorFileMenu::ps_showSimulatorFile);
                }
            }
            this->nestedCustomMenu(menu);
        }

        void CShowSimulatorFileMenu::ps_showSimulatorFile() const
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

        void CMergeWithDbDataMenu::customMenu(QMenu &menu) const
        {
            const CAircraftModelView *mv = modelView();
            if (mv->isEmpty()) { this->nestedCustomMenu(menu); return; }
            if (!sGui->hasWebDataServices()) { this->nestedCustomMenu(menu); return; }

            this->addSeparator(menu);
            QMenu *mm = menu.addMenu(CIcons::databaseEdit16(), "Merge with DB data");
            mm->addAction("All", this, &CMergeWithDbDataMenu::ps_mergeData);
            if (mv->hasSelection())
            {
                mm->addAction("Selected only", this, &CMergeWithDbDataMenu::ps_mergeSelectedData);
            }
            this->nestedCustomMenu(menu);
        }

        void CMergeWithDbDataMenu::ps_mergeData()
        {
            Q_ASSERT_X(sGui, Q_FUNC_INFO, "Missing sGui");
            if (!sGui->hasWebDataServices()) { return; }

            const CAircraftModelList dbModels(sGui->getWebDataServices()->getModels());
            CAircraftModelList models(this->getAircraftModels());
            CAircraftModelUtilities::mergeWithDbData(models, dbModels, true);
            if (this->modelsTargetSetable())
            {
                this->modelsTargetSetable()->setModels(models);
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

        void CMergeWithDbDataMenu::addSeparator(QMenu &menu) const
        {
            // when the menu before us is a DB menu, we ignore the separator
            if (!this->m_separator) { return; }
            if (this->previousMenuItemContains("DB", menu)) { return; }
            IAircraftModelViewMenu::addSeparator(menu);
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
