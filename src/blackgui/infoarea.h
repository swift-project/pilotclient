/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_INFOAREA_H
#define BLACKGUI_INFOAREA_H

#include "dockwidgetinfoarea.h"
#include <QMainWindow>
#include <QTabBar>
#include <QPixmap>

namespace Ui { class CInfoArea; }

namespace BlackGui
{
    //! Info area, hosting dockable widgets
    //! \sa CDockWidgetInfoArea
    class CInfoArea : public QMainWindow
    {
        // KB: TODO, make this an interface, IInfoArea?
        Q_OBJECT

    public:

        //! Destructor
        virtual ~CInfoArea() {}

        //! Add items to context menu
        void addToContextMenu(QMenu *menu) const;

        //! Is the area floating?
        bool isFloating() const { return this->m_infoAreaFloating; }

        //! Selected area of non floating areas
        //! \remarks -1 for no area
        int getSelectedInfoAreaIndex() const;

        //! Is given widget selected. Means it is not floating, and the one selected
        bool isSelectedInfoArea(const CDockWidgetInfoArea *infoArea) const;

    public slots:
        //! Dock all widgets
        void dockAllWidgets();

        //! Adjust size for all dock widgets
        void adjustSizeForAllDockWidgets();

        //! All widgets floating
        void floatAllWidgets();

        //! Toggle dock / floating of the whole info area
        void toggleFloating();

        //! Toggle floating of index
        void toggleFloating(int areaIndex);

        //! Select area
        void selectArea(int areaIndex);

    protected:
        //! Constructor
        explicit CInfoArea(QWidget *parent = nullptr);

        //! Override close event
        virtual void closeEvent(QCloseEvent *event) override;

        //! Preferred size when floating
        virtual QSize getPreferredSizeWhenFloating(int areaIndex) const = 0;

        //! Info area (index) to icon
        virtual const QPixmap &indexToPixmap(int areaIndex) const = 0;

        //! Init area after(!) GUI is initialized
        void initInfoArea();

    private:
        Ui::CInfoArea *ui = nullptr;
        QList<CDockWidgetInfoArea *> m_dockableWidgets ;
        QTabBar *m_tabBar = nullptr;
        bool m_showTabTexts = true;
        bool m_infoAreaFloating = false; //!< whole info area floating
        bool m_showTabBar = true;        //!< auto ajdust the floating widgets

        //! Tabify the widgets
        void tabifyAllWidgets();

        //! Untabify
        void unTabifyAllWidgets();

        //! The tab bar of the docked widgets
        QTabBar *tabBarDockedWidgets() const;

        //! Corresponding dockable widgets
        QList<CDockWidgetInfoArea *> dockableWidgets() const;

        //! Corresponding dockable widget for given tab index
        CDockWidgetInfoArea *getDockableWidgetByTabIndex(int tabBarIndex) const;

        //! Features of the dockable widgets
        void setFeaturesForDockableWidgets(QDockWidget::DockWidgetFeatures features);

        //! Number of tabbed widgets
        int countDockedWidgets() const;

        //! Widget to tab bar index
        int widgetToTabBarIndex(const CDockWidgetInfoArea *dockWidget);

        //! Set the tab's icons
        void setTabPixmaps();

        //! Connect all widgets
        void connectAllWidgets();

        //! Margins for the floating widgets
        void setMarginsWhenFloating(int left, int top, int right, int bottom);

        //! Margins for the dockable widgets
        void setMarginsWhenDocked(int left, int top, int right, int bottom);

    private slots:
        //! Tab bar has been double clicked
        void ps_tabBarDoubleClicked(int tabBarIndex);

        //! A widget has changed its top level
        void ps_onWidgetTopLevelChanged(CDockWidget *widget, bool topLevel);

        //! Style sheet has been changed
        void ps_onStyleSheetChanged();

        //! Context menu
        void ps_showContextMenu(const QPoint &pos);

        //! Show the tab texts, or just the icons
        void ps_showTabTexts(bool show);

        //! Show tab bar
        void ps_showTabBar(bool show);

        //! Tab position for docked widgets tab
        //! \remarks North or South working, East / West not
        void ps_setTabBarPosition(QTabWidget::TabPosition position);

        //! Toggle tab position North - South
        void ps_toggleTabBarPosition();

        //! Set dock area used
        void ps_setDockArea(Qt::DockWidgetArea area);

        //! Dock / floating of the whole info area
        void ps_setInfoAreaFloating(bool floating);
    };
}

#endif // guard
