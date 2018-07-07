/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackgui/menus/menuaction.h"
#include "blackgui/guiapplication.h"
#include "blackcore/webdataservices.h"
#include "blackmisc/icons.h"
#include "blackmisc/slot.h"
#include "blackmisc/verify.h"

#include <QAction>
#include <QMap>
#include <QMenu>
#include <QStringList>
#include <QtGlobal>
#include <algorithm>

using namespace BlackMisc;
using namespace BlackGui;

namespace BlackGui
{
    namespace Menus
    {
        CMenuAction::CMenuAction(const QIcon &icon, const QString &title, const QString &path, bool separator) :
            m_icon(icon), m_title(title), m_path(path.trimmed()), m_separator(separator)
        { }

        CMenuAction::CMenuAction(const QString &path, bool separator) :
            m_path(path.trimmed()), m_separator(separator)
        { }

        CMenuAction::CMenuAction(QAction *action, const QString &path, bool separator) :
            m_action(action), m_icon(action->icon()), m_title(action->text()), m_path(path.trimmed()), m_separator(separator)
        { }

        bool CMenuAction::isCheckableQAction() const
        {
            if (!m_action) { return false; }
            return m_action->isCheckable();
        }

        void CMenuAction::setActionChecked(bool checked)
        {
            if (m_action) { m_action->setChecked(checked); }
        }

        bool CMenuAction::isActionEnabled() const
        {
            if (!m_action) { return false; }
            return m_action->isEnabled();
        }

        void CMenuAction::setActionEnabled(bool enabled)
        {
            if (m_action) { m_action->setEnabled(enabled); }
        }

        bool CMenuAction::hasNoPathWithSeparator() const
        {
            return m_separator && this->hasNoPath();
        }

        bool CMenuAction::hasNoPath() const
        {
            return m_path.isEmpty() || m_path == pathNone();
        }

        QPixmap CMenuAction::getPixmap() const
        {
            if (m_icon.isNull()) { return QPixmap(); }
            return m_icon.pixmap(m_icon.actualSize(QSize(16, 16)));
        }

        void CMenuAction::setEnabled(bool enabled)
        {
            Q_ASSERT_X(m_action, Q_FUNC_INFO, "No action");
            m_action->setEnabled(enabled);
        }

        const CMenuAction &CMenuAction::subMenuDatabase()
        {
            static const CMenuAction subdir(CIcons::appDatabase16(), "Database", CMenuAction::pathViewDatabase());
            return subdir;
        }

        const CMenuAction &CMenuAction::subMenuConsolidateModels()
        {
            static const CMenuAction subdir(CIcons::appModels16(), "Consolidate models", CMenuAction::pathViewModelsConsolidate());
            return subdir;
        }

        const CMenuAction &CMenuAction::subMenuSimulator()
        {
            static const CMenuAction subdir(CIcons::appSimulator16(), "Simulator", CMenuAction::pathSimulator());
            return subdir;
        }

        QString CMenuAction::getLastPathPart() const
        {
            if (m_path.contains('/'))
            {
                if (m_path.endsWith('/')) { return ""; }
                const int i = m_path.lastIndexOf('/');
                return m_path.mid(i + 1);
            }
            return "";
        }

        void CMenuActions::splitSubMenus(const QString &key, QList<CMenuAction> &actions, QList<CMenuAction> &menus) const
        {
            QList<CMenuAction> myActions(m_actions.values(key));
            QList<CMenuAction> checkableActions;
            std::reverse(myActions.begin(), myActions.end()); // the order is reverse because of the insert multi value
            for (const CMenuAction &action : myActions)
            {
                if (action.isSeparator())
                {
                    actions.append(action);
                }
                else if (action.isSubMenu() || !action.getQAction())
                {
                    menus.append(action);
                }
                else
                {
                    if (action.isCheckableQAction())
                    {
                        checkableActions.append(action);
                    }
                    else
                    {
                        actions.append(action);
                    }
                }
            }
            actions.append(checkableActions); // checkable actions ar end
        }

        CMenuActions::CMenuActions(const QList<CMenuAction> &actions)
        {
            for (const CMenuAction &action : actions)
            {
                this->addAction(action);
            }
        }

        QList<QAction *> CMenuActions::getQActions() const
        {
            QList<QAction *> qActions;
            for (const CMenuAction &a : this->toQList())
            {
                qActions.append(a.getQAction());
            }
            return qActions;
        }

        CMenuActions CMenuActions::getMenuActions(const QString &path) const
        {
            if (m_actions.contains(path)) { return QList<CMenuAction>(); };
            const QList<CMenuAction> allActions(m_actions.values(path));
            QList<CMenuAction> menuActions;
            for (const CMenuAction &a : allActions)
            {
                if (a.isSubMenu() || !a.getQAction())
                {
                    menuActions.append(a);
                }
            }
            return menuActions;
        }

        bool CMenuActions::containsMenu(const QString &path) const
        {
            if (!m_actions.contains(path)) { return false; }
            return getMenuActions(path).size() > 0;
        }

        CMenuAction CMenuActions::addMenu(const CMenuAction &subdirAction)
        {
            return this->addMenu(subdirAction.getIcon(), subdirAction.getTitle(), subdirAction.getPath());
        }

        CMenuAction CMenuActions::addMenu(const QString &title, const QString &path)
        {
            return this->addMenu(QIcon(), title, path);
        }

        void CMenuActions::addSeparator(const QString &path)
        {
            static const CMenuAction separatorDummy(QIcon(), "_SEP_", path);
            this->addAction(separatorDummy);
        }

        CMenuAction CMenuActions::addMenu(const QIcon &icon, const QString &title, const QString &path)
        {
            const QList<CMenuAction> exisitingMenu(this->getMenuActions(path));
            if (!exisitingMenu.isEmpty())
            {
                const CMenuAction existingAction(exisitingMenu.first());
                Q_ASSERT_X(exisitingMenu.size() > 1, Q_FUNC_INFO, "Redundant menu entries");
                Q_ASSERT_X(existingAction.getTitle() != title, Q_FUNC_INFO, "Title mismatch");
                if (icon.isNull() || existingAction.hasIcon()) { return existingAction.getQAction(); }
                return existingAction;
            }

            CMenuAction menuAction(icon, title, path);
            menuAction.setSubMenu(true);
            return this->addAction(menuAction);
        }

        CMenuAction CMenuActions::addAction(const CMenuAction &menuAction)
        {
            Q_ASSERT_X(!menuAction.getPath().isEmpty(), Q_FUNC_INFO, "Need path");
            m_actions.insertMulti(menuAction.getPath(), menuAction);
            return menuAction;
        }

        CMenuActions CMenuActions::addActions(const CMenuActions &actions)
        {
            CMenuAction a;
            CMenuActions menuActions;
            for (const CMenuAction &action : actions.toQList())
            {
                a = this->addAction(action);
                menuActions.addAction(a);
            }
            return menuActions;
        }

        CMenuAction CMenuActions::addAction(QAction *action, const QString &path)
        {
            return this->addAction(CMenuAction(action, path));
        }

        CMenuActions CMenuActions::addActions(const QList<QAction *> &actions, const QString &path)
        {
            if (actions.isEmpty()) { return CMenuActions(); }
            CMenuAction menuAction;
            CMenuActions menuActions;
            for (QAction *a : actions)
            {
                menuAction = this->addAction(a, path);
                menuActions.addAction(menuAction);
            }
            return menuActions;
        }

        CMenuAction CMenuActions::addAction(QAction *action, const QString &text, const QString &path, const BlackMisc::CSlot<void ()> &slot, const QKeySequence &shortcut)
        {
            if (action) { return this->addAction(action, path); }
            return this->addAction(text, path, slot, shortcut);
        }

        CMenuAction CMenuActions::addAction(QAction *action, const QString &text, const QString &path, QObject *actionOwner, const BlackMisc::CSlot<void ()> &slot, const QKeySequence &shortcut)
        {
            if (action) { return this->addAction(action, path); }
            return this->addAction(text, path, actionOwner, slot, shortcut);
        }

        CMenuAction CMenuActions::addAction(QAction *action, const QIcon &icon, const QString &text, const QString &path, const BlackMisc::CSlot<void ()> &slot, const QKeySequence &shortcut)
        {
            if (action) { return this->addAction(action, path); }
            return this->addAction(icon, text, path, slot, shortcut);
        }

        CMenuAction CMenuActions::addAction(QAction *action, const QIcon &icon, const QString &text, const QString &path, QObject *actionOwner, const BlackMisc::CSlot<void ()> &slot, const QKeySequence &shortcut)
        {
            if (action) { return this->addAction(action, path); }
            Q_ASSERT_X(actionOwner, Q_FUNC_INFO, "Need action owner"); // in this case nullptr as actionOwner is not allowed
            return this->addAction(icon, text, path, actionOwner, slot, shortcut);
        }

        CMenuAction CMenuActions::addAction(const QString &text, const QString &path, QObject *actionOwner, const QKeySequence &shortcut)
        {
            return this->addAction(QIcon(), text, path, actionOwner, shortcut);
        }

        CMenuAction CMenuActions::addAction(const QIcon &actionIcon, const QString &text, const QString &path, QObject *actionOwner, const QKeySequence &shortcut)
        {
            QAction *action = actionIcon.isNull() ? new QAction(text, actionOwner) : new QAction(actionIcon, text, actionOwner);
            action->setShortcut(shortcut);
            const CMenuAction ma(action, path);
            return this->addAction(ma);
        }

        CMenuAction CMenuActions::addAction(const QIcon &actionIcon, const QString &text, const QString &path, QObject *actionOwner, const BlackMisc::CSlot<void ()> &slot, const QKeySequence &shortcut)
        {
            CMenuAction action = this->addAction(actionIcon, text, path, actionOwner, shortcut);
            QAction::connect(action.getQAction(), &QAction::triggered, [slot](bool checked)
            {
                slot();
                Q_UNUSED(checked);
            });
            return action;
        }

        CMenuAction CMenuActions::addAction(const QIcon &actionIcon, const QString &text, const QString &path)
        {
            return this->addAction(actionIcon, text, path, nullptr);
        }

        CMenuAction CMenuActions::addAction(const QIcon &actionIcon, const QString &text, const QString &path, const BlackMisc::CSlot<void ()> &slot, const QKeySequence &shortcut)
        {
            return this->addAction(actionIcon, text, path, slot.object(), slot, shortcut);
        }

        CMenuAction CMenuActions::addAction(const QString &text, const QString &path, const BlackMisc::CSlot<void ()> &slot, const QKeySequence &shortcut)
        {
            return this->addAction(QIcon(), text, path, slot.object(), slot, shortcut);
        }

        CMenuAction CMenuActions::addAction(const QString &text, const QString &path, QObject *actionOwner, const BlackMisc::CSlot<void ()> &slot, const QKeySequence &shortcut)
        {
            return this->addAction(QIcon(), text, path, actionOwner, slot, shortcut);
        }

        void CMenuActions::toQMenu(QMenu &menu, bool separateGroups) const
        {
            if (m_actions.isEmpty()) { return; }
            const QStringList keys = m_actions.uniqueKeys(); // Sorted ascending

            QMap<QString, QMenu *> subMenus; // all sub menus
            for (const QString &key : keys)
            {
                bool addedSeparator = false;
                const int pathDepth = CMenuActions::pathDepth(key); // 0 based

                QList<CMenuAction> actions;
                QList<CMenuAction> menus;
                this->splitSubMenus(key, actions, menus);
                if (actions.isEmpty())
                {
                    // No actions directly for that level
                    continue;
                }
                if (!menu.isEmpty() && separateGroups)
                {
                    menu.addSeparator();
                    addedSeparator = true;
                }

                int noActionsWithoutPath = 0;
                QMenu *currentMenu = nullptr;

                // reverse iteration because same key values are inserted and have reverse order
                for (const CMenuAction &menuAction : actions)
                {
                    // create submenu if required
                    if (!currentMenu)
                    {
                        currentMenu = currentMenuForAction(menu, menuAction, menus, subMenus, key, pathDepth);
                    }
                    Q_ASSERT_X(currentMenu, Q_FUNC_INFO, "Missing menu");
                    Q_ASSERT_X(menuAction.isSubMenu() || menuAction.isSeparator() || menuAction.getQAction(), Q_FUNC_INFO, "Wrong  type");

                    if (menuAction.isSeparator())
                    {
                        if (menu.isEmpty()) continue;
                        currentMenu->addSeparator();
                        continue;
                    }

                    if (menuAction.hasNoPath())
                    {
                        noActionsWithoutPath++;

                        // separator for unclassfied items
                        if ((!addedSeparator || noActionsWithoutPath > 1) && menuAction.hasNoPathWithSeparator())
                        {
                            menu.addSeparator();
                            addedSeparator = false;
                        }
                    }

                    // either add to submenu or menu
                    currentMenu->addAction(menuAction.getQAction());

                    // menu has ownership if there is no other parent
                    if (menuAction.getQAction() && !menuAction.getQAction()->parent())
                    {
                        menuAction.getQAction()->setParent(&menu);
                    }
                } // actions

                // clean up empty sub menus
                if (currentMenu && currentMenu->isEmpty())
                {
                    menu.removeAction(currentMenu->menuAction());
                }
            } // keys
        }

        QList<CMenuAction> CMenuActions::toQList() const
        {
            return m_actions.values();
        }

        CMenuActions::operator QList<QAction *>() const
        {
            QList<QAction *> qActions;
            for (const CMenuAction &a : toQList())
            {
                if (!a.getQAction()) { continue; }
                qActions.append(a.getQAction());
            }
            return qActions;
        }

        CMenuAction CMenuActions::addMenuViewOrder()
        {
            if (this->containsMenu(CMenuAction::pathViewOrder())) { return CMenuAction(); }
            return this->addMenu(CIcons::arrowMediumEast16(), "Order", CMenuAction::pathViewOrder());
        }

        CMenuAction CMenuActions::addMenuSimulator()
        {
            if (this->containsMenu(CMenuAction::pathSimulator())) { return CMenuAction(); }
            return this->addMenu(CIcons::appSimulator16(), "Simulator", CMenuAction::pathSimulator());
        }

        CMenuAction CMenuActions::addMenuStash()
        {
            if (this->containsMenu(CMenuAction::pathStash())) { return CMenuAction(); }
            const bool canConnectDb = sGui->getWebDataServices()->hasSuccesfullyConnectedSwiftDb();
            const QString txt(canConnectDb ? "Stash tools" : "Stash tools (Warning: no DB!)");
            return this->addMenu(CIcons::appDbStash16(), txt, CMenuAction::pathStash());
        }

        CMenuAction CMenuActions::addMenuStashEditor()
        {
            if (this->containsMenu(CMenuAction::pathStashEditor())) { return CMenuAction(); }
            return this->addMenu(CIcons::appDbStash16(), "Edit models", CMenuAction::pathStashEditor());
        }

        CMenuAction CMenuActions::addMenuDatabase()
        {
            if (this->containsMenu(CMenuAction::pathViewDatabase())) { CMenuAction(); }
            return this->addMenu(CMenuAction::subMenuDatabase());
        }

        CMenuAction CMenuActions::addMenuConsolidateModels()
        {
            if (this->containsMenu(CMenuAction::pathViewModelsConsolidate())) { CMenuAction(); }
            return this->addMenu(CMenuAction::subMenuConsolidateModels());
        }

        CMenuAction CMenuActions::addMenuModelSet()
        {
            if (this->containsMenu(CMenuAction::pathModelSet())) { CMenuAction(); }
            return this->addMenu(CIcons::appModels16(), "Model set", CMenuAction::pathModelSet());
        }

        const CMenuActions &CMenuActions::predefinedSubmenus()
        {
            static const CMenuActions pd(
            {
                CMenuAction::subMenuDatabase(),
                CMenuAction::subMenuSimulator()
            });
            return pd;
        }

        QMenu *CMenuActions::currentMenuForAction(QMenu &menu, const CMenuAction &menuAction, const QList<CMenuAction> &menus, QMap<QString, QMenu *> &subMenus, const QString &key, int pathDepth)
        {
            if (pathDepth < 1) { return &menu; }
            QMenu *parentMenu = &menu;
            if (pathDepth > 1)
            {
                // find the corresponding submenu. If this is empty the next higher level will be choosen
                // if not found at all, use top level menu
                parentMenu = findUpwardsInMenus(key, subMenus);
                if (!parentMenu)
                {
                    parentMenu = &menu;
                }
            }

            // explicity menu?
            QMenu *subMenu = nullptr;
            if (menus.isEmpty())
            {
                subMenu = parentMenu->addMenu(menuAction.getLastPathPart());
            }
            else
            {
                const CMenuAction menuFound(menus.first());
                subMenu = parentMenu->addMenu(menuFound.getIcon(), menuFound.getTitle());
            }
            Q_ASSERT_X(subMenu, Q_FUNC_INFO, "Could not create sub menu");

            subMenu->setParent(parentMenu);
            if (pathDepth > 0 && subMenu)
            {
                subMenus.insert(key, subMenu);
            }
            return subMenu;
        }

        int CMenuActions::pathDepth(const QString &path)
        {
            if (path.isEmpty()) { return 0; }
            int c = path.count('/');
            return c > 0 ? c : 0;
        }

        QString CMenuActions::parentPath(const QString &currentPath)
        {
            if (!currentPath.contains('/')) { return ""; }
            const int i = currentPath.lastIndexOf('/');
            return currentPath.left(i);
        }

        QMenu *CMenuActions::findUpwardsInMenus(const QString &key, const QMap<QString, QMenu *> &menus)
        {
            QString k = key;
            while (!k.isEmpty() && !menus.isEmpty())
            {
                if (menus.contains(k))
                {
                    return menus[k];
                }
                k = parentPath(k);
            }
            return nullptr;
        }
    } // ns
} // ns
