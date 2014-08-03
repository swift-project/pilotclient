/* Copyright (C) 2014
 * swift Project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_MAININFOAREACOMPONENT_H
#define BLACKGUI_MAININFOAREACOMPONENT_H

#include "../dockwidgetinfoarea.h"
#include "atcstationcomponent.h"
#include "aircraftcomponent.h"
#include "usercomponent.h"
#include "textmessagecomponent.h"
#include "simulatorcomponent.h"
#include "flightplancomponent.h"
#include "settingscomponent.h"
#include "logcomponent.h"
#include <QMainWindow>
#include <QTabBar>
#include <QPixmap>

namespace Ui { class CMainInfoAreaComponent; }

namespace BlackGui
{
    namespace Components
    {

        //! Main info area
        class CMainInfoAreaComponent : public QMainWindow
        {
            Q_OBJECT

        public:
            //! Constructor
            explicit CMainInfoAreaComponent(QWidget *parent = nullptr);

            //! Destructor
            ~CMainInfoAreaComponent();

            //! Info areas
            enum InfoArea
            {
                // index must match tab index!
                InfoAreaAircrafts = 0,
                InfoAreaAtc = 1,
                InfoAreaUsers = 2,
                InfoAreaTextMessages = 3,
                InfoAreaSimulator = 4,
                InfoAreaFlightPlan = 5,
                InfoAreaWeather = 6,
                InfoAreaMappings = 7,
                InfoAreaLog = 8,
                InfoAreaSettings = 9
            };

            //! Add items to context menu
            void addToContextMenu(QMenu *menu) const;

            //! Is this area floating?
            bool isFloating() const { return this->m_infoAreaFloating; }

            //! ATC stations
            CAtcStationComponent *getAtcStationComponent();

            //! Aircrafts
            CAircraftComponent *getAircraftComponent();

            //! User component
            CUserComponent *getUserComponent();

            //! Flight plan
            CFlightPlanComponent *getFlightPlanComponent();

            //! Settings
            CSettingsComponent *getSettingsComponent();

            //! Log messages
            CLogComponent *getLogComponent();

            //! Simulator
            CSimulatorComponent *getSimulatorComponent();

            //! Text messages
            CTextMessageComponent *getTextMessageComponent();

        public slots:
            //! Dock all widgets
            void dockAllWidgets();

            //! All widgets floating
            void floatAllWidgets();

            //! Toggle dock / floating of the whole info area
            void toggleFloating();

            //! Toggle floating of given area
            void toggleFloating(InfoArea infoArea);

            //! Toggle floating of index
            void toggleFloating(int index);

            //! Select area
            void selectArea(InfoArea infoArea);

            //! Select area
            void selectArea(int index);

        protected:
            //! Override close event
            virtual void closeEvent(QCloseEvent *event) override;

        private:
            Ui::CMainInfoAreaComponent *ui = nullptr;
            QList<CDockWidgetInfoArea *> m_dockableWidgets ;
            QTabBar *m_tabBar = nullptr;
            bool m_showTabTexts = true;
            bool m_infoAreaFloating = false; //!< whole info area floating
            bool m_autoAdjustFloatingWidgets = true; //!< auto ajdust the floating widgets

            //! Tabify the widgets
            void tabifyAllWidgets();

            //! Untabify
            void unTabifyAllWidgets();

            //! The tab bar of the docked widgets
            QTabBar *tabBarDockedWidgets() const;

            //! Corresponding dockable widgets
            QList<CDockWidgetInfoArea *> dockableWidgets() const;

            //! Corresponding dockable widget
            CDockWidgetInfoArea *getDockableWidgetByIndex(int index) const;

            //! Selected dockable widget
            CDockWidgetInfoArea *selectedDockableWidget() const;

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

            //! Info area to icon
            static const QPixmap &infoAreaToIcon(InfoArea infoArea);

        private slots:
            //! Tab bar has been double clicked
            void ps_tabBarDoubleClicked(int index);

            //! A widget has changed its top level
            void ps_onWidgetTopLevelChanged(CDockWidget *widget, bool topLevel);

            //! Style sheet has been changed
            void ps_onStyleSheetChanged();

            //! Context menu
            void ps_showContextMenu(const QPoint &pos);

            //! Show the tab texts, or just the icons
            void ps_showTabTexts(bool show);

            //! Toggle checkable setting
            void ps_toggleAutoAdjustFloatingWidget(bool adjust);

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
}

#endif // guard
