// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef BLACKGUI_INFOAREA_H
#define BLACKGUI_INFOAREA_H

#include "blackgui/blackguiexport.h"
#include "blackgui/enableforframelesswindow.h"
#include "misc/statusmessage.h"
#include "misc/statusmessagelist.h"

#include <QDockWidget>
#include <QList>
#include <QMainWindow>
#include <QObject>
#include <QSize>
#include <QString>
#include <QTabWidget>
#include <Qt>

class QAction;
class QCloseEvent;
class QKeyEvent;
class QMenu;
class QPaintEvent;
class QPixmap;
class QPoint;
class QTabBar;
class QWidget;

namespace BlackGui
{
    class CDockWidget;
    class CDockWidgetInfoArea;

    //! Info area, hosting dockable widgets
    //! \sa CDockWidgetInfoArea
    class BLACKGUI_EXPORT CInfoArea : public QMainWindow, CEnableForFramelessWindow
    {
        Q_OBJECT

    public:
        //! Destructor
        virtual ~CInfoArea() override;

        //! Add items to context menu
        void addToContextMenu(QMenu *menu) const;

        //! Is the area floating?
        bool isFloating() const { return m_infoAreaFloating; }

        //! Is given widget selected. Means it is not floating, and the one selected
        bool isSelectedDockWidgetInfoArea(const CDockWidgetInfoArea *infoArea) const;

        //! Get the selected info area (non floating, selected in tabbar)
        const CDockWidgetInfoArea *getSelectedDockInfoArea() const;

        //! Get the selected info area (non floating, selected in tabbar)
        int getSelectedDockInfoAreaIndex() const;

        //! Own dockable widgets
        QList<const CDockWidgetInfoArea *> getDockWidgetInfoAreas() const;

        //! Create a list of actions to select the info areas.
        //! This could be used in a menu or somewhere else.
        //! \param withShortcut automatically add a shortcut
        //! \param parent which will own the action (deletion)
        QList<QAction *> getInfoAreaSelectActions(bool withShortcut, QWidget *parent) const;

        //! Create a list of actions to select the info areas and toogle its floating state.
        //! This could be used in a menu or somewhere else.
        //! \param parent which will own the action (deletion)
        QList<QAction *> getInfoAreaToggleFloatingActions(QWidget *parent) const;

        //! Create a list of actions to restore the info areas.
        //! This could be used in a menu or somewhere else.
        //! \param parent which will own the action (deletion)
        QList<QAction *> getInfoAreaRestoreActions(QWidget *parent) const;

        //! Create a list of actions to reset the position the info areas.
        //! This could be used in a menu or somewhere else.
        //! \param parent which will own the action (deletion)
        QList<QAction *> getInfoAreaResetPositionActions(QWidget *parent) const;

        //! Docked area indexes
        QList<int> getAreaIndexesDockedOrFloating(bool floating) const;

        //! Dock all widgets
        void dockAllWidgets();

        //! Adjust size for all dock widgets
        void adjustSizeForAllDockWidgets();

        //! All widgets floating
        void floatAllWidgets();

        //! All floating info areas on top
        void allFloatingOnTop();

        //! Reset all floating widget settings
        void resetAllFloatingWidgetSettings();

        //! Reset all widget settings
        void resetAllWidgetSettings();

        //! Toggle dock / floating of the whole info area
        virtual void toggleFloatingWholeInfoArea();

        //! Toggle floating of index
        void toggleFloatingByIndex(int areaIndex);

        //! Toggle visibilty
        void toggleVisibility(int areaIndex);

        //! Select area
        void selectArea(int areaIndex);

        //! Reset position
        void resetPosition(int areaIndex);

        //! Set current tab bar index by given widget
        void selectArea(const CDockWidgetInfoArea *dockWidgetInfoArea);

        //! Select area (sender is QAction)
        void selectAreaByAction();

        //! Reset window position of area (sender is QAction)
        void resetPositionByAction();

        //! Toggle area floating (sender is QAction)
        void toggleAreaFloatingByAction();

        //! Restore dock widget`s state (from settings)
        void restoreDockWidgetInfoArea();

        //! Restore dock widget`s state (from settings)
        void restoreDockWidgetInfoAreaByIndex(int areaIndex);

        //! Select next left tab
        void selectLeftTab();

        //! Select next right tab
        void selectRightTab();

        //! Display status message in all info areas (according their state)
        void displayStatusMessage(const swift::misc::CStatusMessage &statusMessage);

        //! Display status messages in all info areas (according their state)
        void displayStatusMessages(const swift::misc::CStatusMessageList &statusMessages);

    signals:
        //! Tab bar changed
        void changedInfoAreaTabBarIndex(int index);

        //! Status of info area changed
        void changedInfoAreaStatus(int currentTabIndex, QList<int> dockedAreas, QList<int> floatingAreas);

        //! Whole info area floating
        void changedWholeInfoAreaFloating(bool floating);

    protected:
        //! Constructor
        explicit CInfoArea(QWidget *parent = nullptr);

        //! \copydoc QWidget::closeEvent
        virtual void closeEvent(QCloseEvent *event) override;

        //! \copydoc QWidget::paintEvent
        virtual void paintEvent(QPaintEvent *event) override;

        //! \copydoc QWidget::keyPressEvent
        //! \remarks nor fully sufficient, as the info area is hardly having focus
        virtual void keyPressEvent(QKeyEvent *event) override;

        //! Preferred size when floating (size hint)
        virtual QSize getPreferredSizeWhenFloating(int areaIndex) const = 0;

        //! Info area (index) to icon
        virtual const QPixmap &indexToPixmap(int areaIndex) const = 0;

        //! Init area after(!) GUI is initialized
        void initInfoArea();

        //! Tab position for docked widgets tab
        //! \remarks North or South working, East / West not
        void setTabBarPosition(QTabWidget::TabPosition position);

        //! Toogle lock tabbar
        void toggleTabBarLocked(bool locked);

    private:
        QList<CDockWidgetInfoArea *> m_dockWidgetInfoAreas;
        QList<CInfoArea *> m_childInfoAreas;
        QTabBar *m_tabBar = nullptr;
        bool m_showTabTexts = true; //!< texts for tabs
        bool m_infoAreaFloating = false; //!< whole info area floating?
        bool m_showTabBar = true; //!< show the tab bar?
        bool m_lockTabBar = false; //!< locked: means double clicks ignored

        //! Toggle tab position North - South
        void toggleTabBarPosition();

        //! Tabify the widgets
        void tabifyAllWidgets();

        //! Untabify
        void unTabifyAllWidgets();

        //! Valid area index?
        bool isValidAreaIndex(int areaIndex) const;

        //! Corresponding dockable widget for given window title
        int getAreaIndexByWindowTitle(const QString &title) const;

        //! Tab bar index by title
        int getTabBarIndexByTitle(const QString &title) const;

        //! Corresponding dockable widget for given tab index
        CDockWidgetInfoArea *getDockWidgetInfoAreaByTabBarIndex(int tabBarIndex) const;

        //! Corresponding dockable widget for given window title
        CDockWidgetInfoArea *getDockWidgetInfoAreaByWindowTitle(const QString &title) const;

        //! Widget to tab bar index
        int dockWidgetInfoAreaToTabBarIndex(const CDockWidgetInfoArea *dockWidgetInfoArea) const;

        //! Features of the dockable widgets
        void setFeaturesForDockableWidgets(QDockWidget::DockWidgetFeatures features);

        //! Number of tabbed widgets
        int countDockedWidgetInfoAreas() const;

        //! Set the tab's icons
        void setTabPixmaps();

        //! Connect all widgets
        void connectTopLevelChanged();

        //! Tab bar has been double clicked
        void onTabBarDoubleClicked(int tabBarIndex);

        //! A widget has changed its top level
        void onWidgetTopLevelChanged(CDockWidget *dockWidget, bool topLevel);

        //! Style sheet has been changed
        void onStyleSheetChanged();

        //! Context menu
        void showContextMenu(const QPoint &pos);

        //! Show the tab texts, or just the icons
        void showTabTexts(bool show);

        //! Show tab bar
        void showTabBar(bool show);

        //! Set dock area used
        void setDockArea(Qt::DockWidgetArea area);

        //! Dock / floating of the whole info area
        void setWholeInfoAreaFloating(bool floating);

        //! Emit current status, \sa changedInfoAreaStatus
        void emitInfoAreaStatus();

        //! Tab bar index changed
        void onTabBarIndexChanged(int tabBarIndex);

        //! Nested info areas
        //! \remark weak performance as discussed in T184
        //! \remark result stored in m_childInfoAreas
        QList<CInfoArea *> findOwnChildInfoAreas() const;

        //! Direct dock widget areas, not the nested dock widget areas
        //! \remark result stored in m_dockableWidgets
        QList<CDockWidgetInfoArea *> findOwnDockWidgetInfoAreas() const;
    };
} // namespace

#endif // guard
