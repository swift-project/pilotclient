// SPDX-FileCopyrightText: Copyright (C) 2015 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#ifndef SWIFT_GUI_MENUS_MENUACTION_H
#define SWIFT_GUI_MENUS_MENUACTION_H

#include <QIcon>
#include <QKeySequence>
#include <QList>
#include <QMap>
#include <QMultiMap>
#include <QString>

#include "misc/slot.h"

class QAction;
class QMenu;
class QObject;

namespace swift::gui::menus
{
    /*!
     * Wraps a QAction with extra metadata to allow proper sorting for a QMenu
     */
    class CMenuAction
    {
    public:
        //! Constructor
        CMenuAction(const QIcon &icon, const QString &title, const QString &path = pathNone(), bool separator = false);

        //! Constructor
        CMenuAction(const QString &path = pathNone(), bool separator = false);

        //! Constructor
        CMenuAction(QAction *action, const QString &path = pathNone(), bool separator = false);

        //! Title
        void setTitle(const QString &title) { m_title = title; }

        //! Icon
        void setIcon(const QIcon &icon) { m_icon = icon; }

        //! Path
        void setPath(const QString &path) { m_path = path; }

        //! Action
        QAction *getQAction() const { return m_action; }

        //! Checkable QAction
        bool isCheckableQAction() const;

        //! Set a checkable action, QAction::setChecked
        void setActionChecked(bool checked);

        //! Enabled action?
        bool isActionEnabled() const;

        //! Enable action (allow gray out)
        void setActionEnabled(bool enabled);

        //! Path
        const QString &getPath() const { return m_path; }

        //! Last part of the path, e.g. "Foo/Bar" -> "Bar"
        QString getLastPathPart() const;

        //! Title
        const QString &getTitle() const { return m_title; }

        //! Has title?
        bool hasTitle() const { return !m_title.isEmpty(); }

        //! Is menu?
        bool isSubMenu() const { return m_isMenu; }

        //! Is separator?
        bool isSeparator() const { return m_title == "_SEP_"; }

        //! Is menu?
        void setSubMenu(bool menu) { m_isMenu = menu; }

        //! No path and separator wanted
        bool hasNoPathWithSeparator() const;

        //! No path?
        bool hasNoPath() const;

        //! Icon
        const QIcon &getIcon() const { return m_icon; }

        //! Icon as pixmap
        QPixmap getPixmap() const;

        //! Has icon?
        bool hasIcon() const { return !m_icon.isNull(); }

        //! Set enabled / disabled (allows to gray out)
        void setEnabled(bool enabled);

        //! Conversion
        operator QAction *() const { return this->m_action; }

        // ---- paths for menus ----
        //! \name Menu path definitions
        //! @{

        //! No key
        static const QString &pathNone()
        {
            static const QString p("_NONE");
            return p;
        }

        //! Path converter into separator
        static const QString &pathSeparator()
        {
            static const QString p("_SEPARATOR");
            return p;
        }

        //! Simulator sub menu
        static const QString &pathSimulator()
        {
            static const QString p("Custom.10.Simulator/Simulator");
            return p;
        }

        //! Simulator sub menu reload models
        static const QString &pathSimulatorModelsReload()
        {
            static const QString p("Custom.10.Simulator/Simulator/Reload models");
            return p;
        }

        //! Simulator sub menu reload models
        static const QString &pathSimulatorModelsClearCache()
        {
            static const QString p("Custom.10.Simulator/Simulator/Clear model caches");
            return p;
        }

        //! Model
        static const QString &pathModel()
        {
            static const QString p("Custom.11.Model");
            return p;
        }

        //! Model set
        static const QString &pathModelSet()
        {
            static const QString p("Custom.11.Model/Model set");
            return p;
        }

        //! Model set, new set
        static const QString &pathModelSetNew()
        {
            static const QString p("Custom.11.Model/Model set/New set");
            return p;
        }

        //! Consolidate
        static const QString &pathModelConsolidate()
        {
            static const QString p("Custom.11.Model/Consolidate");
            return p;
        }

        //! Stash sub menu
        static const QString &pathModelStash()
        {
            static const QString p("Custom.11.Model/Stash/Stash");
            return p;
        }

        //! Stash editor sub menu
        static const QString &pathModelStashEditor()
        {
            static const QString p("Custom.11.Model/Stash/Editor");
            return p;
        }

        //! vPilot data
        //! \deprecated vPilot functionality likely to be removed in the future
        static const QString &pathVPilot()
        {
            static const QString p("Custom.14.vPilot/vPilot");
            return p;
        }

        //! Log functionality
        static const QString &pathLog()
        {
            static const QString p("Custom15.Log");
            return p;
        }

        //! Font menus (font size etc.)
        static const QString &pathFont()
        {
            static const QString p("Custom20.Font");
            return p;
        }

        // ---- client ----

        //! Client COM related
        static const QString &pathClientCom()
        {
            static const QString p("Client.ATC/COM");
            return p;
        }

        //! Client flight plan
        static const QString &pathClientFlightPlan()
        {
            static const QString p("Client.FlightPlan");
            return p;
        }

        //! Client follow in simulator
        static const QString &pathClientFollowInSim()
        {
            static const QString p("Client.FollowInSim");
            return p;
        }

        //! Client simulation related
        static const QString &pathClientSimulation()
        {
            static const QString p("ClientSimulation");
            return p;
        }

        //! Client simulation/display related
        static const QString &pathClientSimulationDisplay()
        {
            static const QString p("ClientSimulation.Display/Display");
            return p;
        }

        //! Client simulation/display related
        static const QString &pathClientSimulationRender()
        {
            static const QString p("ClientSimulation.Display/Render");
            return p;
        }

        //! Client simulation/display related
        static const QString &pathClientSimulationTransfer()
        {
            static const QString p("ClientSimulation.Transfer/Transfer");
            return p;
        }

        //! Client model set related
        static const QString &pathClientModelSet()
        {
            static const QString p("Client.Model set");
            return p;
        }

        //! Network data
        static const QString &pathClientNetwork()
        {
            static const QString p("ClientNetwork/Network");
            return p;
        }

        // ---- standard view paths --------

        //! Database
        static const QString &pathViewDatabase()
        {
            static const QString p("View.10.Database/Database");
            return p;
        }

        //! Select add remove
        static const QString &pathViewAddRemove()
        {
            static const QString p("View.12.AddRemove");
            return p;
        }

        //! View selection mode
        static const QString &pathViewSelection()
        {
            static const QString p("View.13.Selection/Selection");
            return p;
        }

        //! Order submenus
        static const QString &pathViewOrder()
        {
            static const QString p("View.14.Order/Order");
            return p;
        }

        //! View resizing
        static const QString &pathViewResize()
        {
            static const QString p("View.15.Resize");
            return p;
        }

        //! View word wrap
        static const QString &pathViewWordWrap()
        {
            static const QString p("View.16.WordWrap");
            return p;
        }

        //! View clear highlighting
        static const QString &pathViewClearHighlighting()
        {
            static const QString p("View.17.ClearHighlight");
            return p;
        }

        //! View filter
        static const QString &pathViewFilter()
        {
            static const QString p("View.18.Filter");
            return p;
        }

        //! View update
        static const QString &pathViewUpdates()
        {
            static const QString p("View.19.Updates");
            return p;
        }

        //! View load/save
        static const QString &pathViewLoadSave()
        {
            static const QString p("View.19.LoadSave");
            return p;
        }

        //! View cut and paste
        static const QString &pathViewCutPaste()
        {
            static const QString p("View.19.CutPaste");
            return p;
        }

        // ---- nested dock widgets ----

        //! Nested dock widget
        static const QString &pathDockWidgetNested()
        {
            static const QString p("DockWidget.Nested");
            return p;
        }
        //! @}

        //! @{
        //! Predefined sub sub menus
        static const CMenuAction &subMenuDatabase();
        static const CMenuAction &subMenuSimulator();
        static const CMenuAction &subMenuConsolidateModels();
        static const CMenuAction &subMenuCom();
        static const CMenuAction &subMenuDisplayModels();
        static const CMenuAction &subMenuRenderModels();
        static const CMenuAction &subMenuDataTransfer();
        //! @}

    private:
        QAction *m_action = nullptr; //!< the action
        QIcon m_icon; //!< icon
        QString m_title; //!< title
        QString m_path; //!< path in menu
        bool m_separator = false; //!< separator
        bool m_isMenu = false; //!< is menu?
    };

    /*!
     * Bunch of CMenuAction objects
     */
    class CMenuActions
    {
    public:
        //! Constructor
        CMenuActions() {}

        //! Constructor
        CMenuActions(const QList<CMenuAction> &actions);

        //! All actions
        CMenuActions getActions() const { return m_actions.values(); }

        //! QActions
        QList<QAction *> getQActions() const;

        //! All menu actions
        CMenuActions getMenuActions(const QString &path) const;

        //! Menu already available?
        bool containsMenu(const QString &path) const;

        //! Empty?
        bool isEmpty() const { return m_actions.isEmpty(); }

        //! Elements
        int size() const { return m_actions.size(); }

        //! Add a sub menu
        CMenuAction addMenu(const CMenuAction &subdirAction);

        //! Add a sub menu
        CMenuAction addMenu(const QString &title, const QString &path);

        //! Add a separator
        void addSeparator(const QString &path);

        //! Add a sub menu
        CMenuAction addMenu(const QIcon &icon, const QString &title, const QString &path);

        //! Add menu action
        CMenuAction addAction(const CMenuAction &menuAction);

        //! Add menu actions, returns last valid QAction
        CMenuActions addActions(const CMenuActions &actions);

        //! Add menu action
        CMenuAction addAction(QAction *action, const QString &path);

        //! Add menu action
        CMenuActions addActions(const QList<QAction *> &actions, const QString &path);

        //! Convenience function if method is also kept elsewhere
        CMenuAction addAction(QAction *action, const QString &text, const QString &path,
                              const swift::misc::CSlot<void()> &slot, const QKeySequence &shortcut = 0);

        //! Convenience function if method is also kept elsewhere
        CMenuAction addAction(QAction *action, const QString &text, const QString &path, QObject *actionOwner,
                              const swift::misc::CSlot<void()> &slot, const QKeySequence &shortcut = 0);

        //! Convenience function if method is also kept elsewhere
        CMenuAction addAction(QAction *action, const QIcon &icon, const QString &text, const QString &path,
                              const swift::misc::CSlot<void()> &slot, const QKeySequence &shortcut = 0);

        //! Convenience function if method is also kept elsewhere
        CMenuAction addAction(QAction *action, const QIcon &icon, const QString &text, const QString &path,
                              QObject *actionOwner, const swift::misc::CSlot<void()> &slot,
                              const QKeySequence &shortcut = 0);

        //! Add action which still needs to be connected
        CMenuAction addAction(const QString &text, const QString &path, QObject *actionOwner,
                              const QKeySequence &shortcut = 0);

        //! Add action which still needs to be connected
        CMenuAction addAction(const QIcon &actionIcon, const QString &text, const QString &path, QObject *actionOwner,
                              const QKeySequence &shortcut = 0);

        //! Add action
        CMenuAction addAction(const QIcon &actionIcon, const QString &text, const QString &path, QObject *actionOwner,
                              const swift::misc::CSlot<void()> &slot, const QKeySequence &shortcut = 0);

        //! Add action without slot
        CMenuAction addAction(const QIcon &actionIcon, const QString &text, const QString &path);

        //! Add action
        CMenuAction addAction(const QIcon &actionIcon, const QString &text, const QString &path,
                              const swift::misc::CSlot<void()> &slot, const QKeySequence &shortcut = 0);

        //! Add menu action
        CMenuAction addAction(const QString &text, const QString &path, const swift::misc::CSlot<void()> &slot,
                              const QKeySequence &shortcut = 0);

        //! Add menu action
        CMenuAction addAction(const QString &text, const QString &path, QObject *actionOwner,
                              const swift::misc::CSlot<void()> &slot, const QKeySequence &shortcut = 0);

        //! Insert the sorted actions to the menu
        void toQMenu(QMenu &menu, bool separateGroups) const;

        //! To QList
        QList<CMenuAction> toQList() const;

        //! First action
        CMenuAction first() const { return m_actions.first(); }

        //! Last action
        CMenuAction last() const { return m_actions.last(); }

        //! All actions;
        operator QList<QAction *>() const;

        //! As QList
        operator QList<CMenuAction>() const { return toQList(); }

        //! \name Add some typical sub menus
        //! @{

        //! View order menu
        CMenuAction addMenuViewOrder();

        //! Simulator menu
        CMenuAction addMenuSimulator();

        //! Stash menu
        CMenuAction addMenuStash();

        //! Stash menu
        CMenuAction addMenuStashEditor();

        //! Database menu
        CMenuAction addMenuDatabase();

        //! Consolidate models menu
        CMenuAction addMenuConsolidateModels();

        //! Model set menu
        CMenuAction addMenuModelSet();

        //! Add ModelConverterX menu (optional)
        CMenuAction addMenuModelConverterX();

        //! Add COM model menu
        CMenuAction addMenuCom();

        //! Add display model menu
        CMenuAction addMenuDisplayModels();

        //! Add render model menu
        CMenuAction addMenuRenderModels();

        //! Add data transfer menu
        CMenuAction addMenuDataTransfer();

        //! @}

    private:
        QMultiMap<QString, CMenuAction> m_actions; //!< actions sorted by path

        //! Split actions into submenus, normal actions and fix order
        void splitSubMenus(const QString &key, QList<CMenuAction> &actions, QList<CMenuAction> &menus) const;

        //! Path depth
        static int pathDepth(const QString &path);

        //! find current menu for given action
        static QMenu *currentMenuForAction(QMenu &menu, const CMenuAction &menuAction, const QList<CMenuAction> &menus,
                                           QMap<QString, QMenu *> &subMenus, const QString &key, int pd);

        //! Get parent path (one level up) for path
        static QString parentPath(const QString &cuurentPath);

        //! Root of "Custom.11.Model/Stash/Stash" is "Custom.11"
        static QString keyRoot(const QString &key);

        //! Same key root?
        static bool isSameKeyRoot(const QString &key1, const QString &key2);

        //! Find key in top level menus
        static QMenu *findUpwardsInMenus(const QString &key, const QMap<QString, QMenu *> &menus);
    };
} // namespace swift::gui::menus

#endif // guard
