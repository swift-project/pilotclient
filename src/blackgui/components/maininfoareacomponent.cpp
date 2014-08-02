/* Copyright (C) 2014
 * swift Project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of Swift Project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "maininfoareacomponent.h"
#include "ui_maininfoareacomponent.h"
#include "../stylesheetutility.h"
#include "../guiutility.h"
#include "blackmisc/icons.h"
#include <QMenu>
#include <QListIterator>
#include <QSignalMapper>
#include <QCloseEvent>

using namespace BlackMisc;

namespace BlackGui
{
    namespace Components
    {
        CMainInfoAreaComponent::CMainInfoAreaComponent(QWidget *parent) :
            QMainWindow(parent), ui(new Ui::CMainInfoAreaComponent)
        {
            this->ps_setInfoAreaFloating(this->m_infoAreaFloating);
            ui->setupUi(this);
            this->setWindowIcon(CIcons::swift24());

            // after setup, GUI established
            if (this->m_dockableWidgets.isEmpty())
            {
                this->m_dockableWidgets = this->findChildren<CDockWidgetInfoArea *>();
                Q_ASSERT(!this->m_dockableWidgets.isEmpty());
            }

            this->ps_setDockArea(Qt::TopDockWidgetArea);
            this->setMarginsWhenFloating(5, 5, 5, 5); // left, top, right bottom
            this->setMarginsWhenDocked(1, 1, 1, 1); // top has no effect
            this->connectAllWidgets();
            this->setFeaturesForDockableWidgets(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetClosable);
            this->tabifyAllWidgets();

            // context menu
            this->setContextMenuPolicy(Qt::CustomContextMenu);
            connect(this, &CMainInfoAreaComponent::customContextMenuRequested, this, &CMainInfoAreaComponent::ps_showContextMenu);
            connect(&CStyleSheetUtility::instance(), &CStyleSheetUtility::styleSheetsChanged, this, &CMainInfoAreaComponent::ps_onStyleSheetChanged);

            // initial style sheet setting
            this->ps_onStyleSheetChanged();
        }

        CMainInfoAreaComponent::~CMainInfoAreaComponent()
        {
            delete ui;
        }

        void CMainInfoAreaComponent::addToContextMenu(QMenu *menu) const
        {
            if (!menu) return;
            menu->addAction(CIcons::dockTop16(), "Dock all", this, SLOT(dockAllWidgets()));
            menu->addAction(CIcons::floatAll16(), "Float all", this, SLOT(floatAllWidgets()));
            menu->addAction(CIcons::floatOne16(), "Dock / float info area", this, SLOT(toggleFloating()));

            bool c = false;
            if (!this->m_dockableWidgets.isEmpty())
            {
                menu->addSeparator();
                QMenu *subMenuToggleFloat = new QMenu("Toggle Float/Dock", menu);
                QMenu *subMenuDisplay = new QMenu("Display", menu);

                QSignalMapper *signalMapperToggleFloating = new QSignalMapper(menu);
                QSignalMapper *signalMapperDisplay = new QSignalMapper(menu);

                for (int i = 0; i < this->m_dockableWidgets.size(); i++)
                {
                    const CDockWidgetInfoArea *dw = this->m_dockableWidgets.at(i);
                    const QPixmap pm = infoAreaToIcon(static_cast<InfoArea>(i));
                    const QString t = dw->windowTitleBackup();
                    QAction *checkableMenuAction = new QAction(menu);
                    checkableMenuAction->setObjectName(QString(t).append("ToggleFloatingAction"));
                    checkableMenuAction->setIconText(t);
                    checkableMenuAction->setIcon(pm);
                    checkableMenuAction->setData(QVariant(i));
                    checkableMenuAction->setCheckable(true);
                    checkableMenuAction->setChecked(!dw->isFloating());
                    subMenuToggleFloat->addAction(checkableMenuAction);
                    c = connect(checkableMenuAction, SIGNAL(toggled(bool)), signalMapperToggleFloating, SLOT(map()));
                    Q_ASSERT(c);
                    signalMapperToggleFloating->setMapping(checkableMenuAction, i);

                    QAction *displayMenuAction = new QAction(menu);
                    displayMenuAction->setObjectName(QString(t).append("DisplayAction"));
                    displayMenuAction->setIconText(t);
                    displayMenuAction->setIcon(pm);
                    displayMenuAction->setData(QVariant(i));
                    displayMenuAction->setCheckable(false);

                    subMenuDisplay->addAction(displayMenuAction);
                    c = connect(displayMenuAction, SIGNAL(triggered(bool)), signalMapperDisplay, SLOT(map()));
                    Q_ASSERT(c);
                    signalMapperDisplay->setMapping(displayMenuAction, i);
                }
                c = connect(signalMapperToggleFloating, SIGNAL(mapped(int)), this, SLOT(toggleFloating(int)));
                Q_ASSERT(c);

                c = connect(signalMapperDisplay, SIGNAL(mapped(int)), this, SLOT(selectArea(int)));
                Q_ASSERT(c);

                menu->addMenu(subMenuDisplay);
                menu->addMenu(subMenuToggleFloat);

                // where and how to display tab
                menu->addSeparator();
                QAction *showMenuText = new QAction(menu);
                showMenuText->setObjectName("ShowDockedWidgetTextAction");
                showMenuText->setIconText("Show tab text");
                showMenuText->setIcon(CIcons::headingOne16());
                showMenuText->setCheckable(true);
                showMenuText->setChecked(this->m_showTabTexts);
                menu->addAction(showMenuText);
                connect(showMenuText, &QAction::toggled, this, &CMainInfoAreaComponent::ps_showTabTexts);

                menu->addAction(CIcons::dockBottom16(), "Toogle tabbar position", this, SLOT(ps_toggleTabBarPosition()));
            }
        }

        CAtcStationComponent *CMainInfoAreaComponent::getAtcStationComponent()
        {
            return this->ui->comp_AtcStations;
        }

        CAircraftComponent *CMainInfoAreaComponent::getAircraftComponent()
        {
            return this->ui->comp_Aircrafts;
        }

        CUserComponent *CMainInfoAreaComponent::getUserComponent()
        {
            return this->ui->comp_Users;
        }

        CFlightPlanComponent *CMainInfoAreaComponent::getFlightPlanComponent()
        {
            return this->ui->comp_FlightPlan;
        }

        CSettingsComponent *CMainInfoAreaComponent::getSettingsComponent()
        {
            return this->ui->comp_Settings;
        }

        CLogComponent *CMainInfoAreaComponent::getLogComponent()
        {
            return this->ui->comp_Log;
        }

        CSimulatorComponent *CMainInfoAreaComponent::getSimulatorComponent()
        {
            return this->ui->comp_Simulator;
        }

        CTextMessageComponent *CMainInfoAreaComponent::getTextMessageComponent()
        {
            return this->ui->comp_TextMessages;
        }

        void CMainInfoAreaComponent::dockAllWidgets()
        {
            this->tabifyAllWidgets();
        }

        void CMainInfoAreaComponent::floatAllWidgets()
        {
            // I fake the double click here, which queues the events in the queue
            // and hence fires all depending signals in order
            if (!this->m_tabBar) return;
            for (int i = 0; i < this->m_tabBar->count(); i++)
            {
                emit this->m_tabBar->tabBarDoubleClicked(i);
            }
        }

        void CMainInfoAreaComponent::toggleFloating()
        {
            this->ps_setInfoAreaFloating(!this->m_infoAreaFloating);
        }

        void CMainInfoAreaComponent::toggleFloating(CMainInfoAreaComponent::InfoArea infoArea)
        {
            CDockWidgetInfoArea *dw = this->m_dockableWidgets.at(static_cast<int>(infoArea));
            Q_ASSERT(dw);
            if (!dw) return;
            dw->toggleFloating();
        }

        void CMainInfoAreaComponent::toggleFloating(int index)
        {
            if (index < 0 || index >= this->m_dockableWidgets.size()) return;
            CDockWidgetInfoArea *dw = this->m_dockableWidgets.at(index);
            Q_ASSERT(dw);
            if (!dw) return;
            dw->toggleFloating();
        }

        void CMainInfoAreaComponent::selectArea(CMainInfoAreaComponent::InfoArea infoArea)
        {
            CDockWidgetInfoArea *dw = this->m_dockableWidgets.at(static_cast<int>(infoArea));
            Q_ASSERT(dw);
            if (!dw) return;
            Q_ASSERT(this->m_tabBar);
            if (m_tabBar->count() < 1) return;

            if (dw->isFloating())
            {
                dw->show();
            }
            else
            {
                int index = this->widgetToTabBarIndex(dw);
                Q_ASSERT(index >= 0);
                if (index >= 0 && index < m_tabBar->count())
                {
                    m_tabBar->setCurrentIndex(index);
                }
            }
        }

        void CMainInfoAreaComponent::selectArea(int index)
        {
            if (index < 0 || index >= this->m_dockableWidgets.count()) return;
            this->selectArea(static_cast<InfoArea>(index));
        }

        void CMainInfoAreaComponent::ps_setDockArea(Qt::DockWidgetArea area)
        {
            QList<CDockWidgetInfoArea *>::iterator i;
            for (i = this->m_dockableWidgets.begin(); i != this->m_dockableWidgets.end(); ++i)
            {
                CDockWidgetInfoArea *dw = (*i);
                Qt::DockWidgetAreas newAreas = static_cast<Qt::DockWidgetAreas>(area);
                Qt::DockWidgetAreas oldAreas = dw->allowedAreas();
                if (oldAreas == newAreas) continue;
                dw->setAllowedAreas(newAreas);
                this->addDockWidget(area, dw);
            }
        }

        void CMainInfoAreaComponent::ps_setInfoAreaFloating(bool floating)
        {
            this->m_infoAreaFloating = floating;
            if (this->m_infoAreaFloating)
            {
                QPoint p = CGuiUtility::mainWindowPosition();
                this->setWindowFlags(Qt::Dialog);
                this->move(p.rx() + 20, p.ry() + 20);
                this->show(); // not working without show
            }
            else
            {
                // make this compliant as QWidget
                // https://qt-project.org/forums/viewthread/17519
                // http://www.qtcentre.org/threads/12569-QMainWindow-as-a-child-of-QMainWindow

                // this->setParent(this->m_originalParent, this->windowFlags() & ~Qt::Window);
                this->setWindowFlags(this->windowFlags() & ~Qt::Window);
                this->setVisible(true); // after redocking this is required
            }
        }

        void CMainInfoAreaComponent::tabifyAllWidgets()
        {
            // this->setDockArea(Qt::LeftDockWidgetArea);
            this->setTabPosition(Qt::LeftDockWidgetArea, QTabWidget::East);

            bool init = this->m_tabBar ? false : true;
            QPoint pos = CGuiUtility::assumedMainWindowPosition(); //main window pos. not initialized yet

            if (!this->m_dockableWidgets.isEmpty())
            {
                for (int i = 0; i < this->m_dockableWidgets.size(); i++)
                {
                    CDockWidgetInfoArea *first = i > 0 ? this->m_dockableWidgets.at(i - 1) : nullptr;
                    CDockWidgetInfoArea *after = this->m_dockableWidgets.at(i);
                    Q_ASSERT(after);

                    // trick, init widget as floating
                    if (init)
                    {
                        QPoint initPoint(pos.rx() + i * 25, pos.ry() + i * 20);
                        after->setVisible(false);
                        after->setFloating(true);
                        after->move(initPoint);
                        after->setVisible(true);
                    }
                    after->setFloating(false);
                    if (!first) { continue; }
                    this->tabifyDockWidget(first, after);
                }
            }

            // as now tabified, now set tab
            if (!this->m_tabBar)
            {
                this->m_tabBar = this->findChild<QTabBar *>();
                Q_ASSERT(m_tabBar);
                QString qss = CStyleSheetUtility::instance().style(CStyleSheetUtility::fileNameDockWidgetTab());
                this->m_tabBar->setStyleSheet(qss);
                this->m_tabBar->setObjectName("comp_MainInfoAreaDockWidgetTab");
                this->m_tabBar->setMovable(false);
                this->m_tabBar->setElideMode(Qt::ElideNone);
                this->m_tabBar->setShape(QTabBar::TriangularEast);

                // East / West does not work (shown, but area itself empty)
                // South does not have any effect
                this->m_tabBar->setShape(QTabBar::TriangularSouth);
                connect(this->m_tabBar, &QTabBar::tabBarDoubleClicked, this, &CMainInfoAreaComponent::ps_tabBarDoubleClicked);
            }

            this->setTabPixmaps();
            if (this->countDockedWidgets() > 0)
            {
                this->m_tabBar->setCurrentIndex(0);
            }
        }

        void CMainInfoAreaComponent::unTabifyAllWidgets()
        {
            if (this->m_dockableWidgets.size() < 2) return;
            CDockWidgetInfoArea *first = this->m_dockableWidgets.first();
            for (int i = 1; i < this->m_dockableWidgets.size(); i++)
            {
                CDockWidgetInfoArea *after = this->m_dockableWidgets.at(i);
                Q_ASSERT(after);
                this->splitDockWidget(first, after, Qt::Horizontal);
            }
        }

        void CMainInfoAreaComponent::connectAllWidgets()
        {
            QList<CDockWidgetInfoArea *>::iterator i;
            for (i = this->m_dockableWidgets.begin(); i != this->m_dockableWidgets.end(); ++i)
            {
                connect(*i, &CDockWidgetInfoArea::widgetTopLevelChanged, this, &CMainInfoAreaComponent::ps_onWidgetTopLevelChanged);
            }
        }

        void CMainInfoAreaComponent::setMarginsWhenFloating(int left, int top, int right, int bottom)
        {
            QList<CDockWidgetInfoArea *>::iterator i;
            for (i = this->m_dockableWidgets.begin(); i != this->m_dockableWidgets.end(); ++i)
            {
                //! Margins when window is floating
                (*i)->setMarginsWhenFloating(left, top, right, bottom);
            }
        }

        void CMainInfoAreaComponent::setMarginsWhenDocked(int left, int top, int right, int bottom)
        {
            QList<CDockWidgetInfoArea *>::iterator i;
            for (i = this->m_dockableWidgets.begin(); i != this->m_dockableWidgets.end(); ++i)
            {
                //! Margins when window is floating
                (*i)->setMarginsWhenDocked(left, top, right, bottom);
            }
        }

        const QPixmap &CMainInfoAreaComponent::infoAreaToIcon(CMainInfoAreaComponent::InfoArea infoArea)
        {
            switch (infoArea)
            {
            case InfoAreaUsers:
                return CIcons::appUsers16();
            case InfoAreaWeather:
                return CIcons::appWeather16();
            case InfoAreaAtc:
                return CIcons::appAtc16();
            case InfoAreaAircrafts:
                return CIcons::appAircrafts16();
            case InfoAreaSettings:
                return CIcons::appSettings16();
            case InfoAreaFlightPlan:
                return CIcons::appFlightPlan16();
            case InfoAreaTextMessages:
                return CIcons::appTextMessages16();
            case InfoAreaSimulator:
                return CIcons::appSimulator16();
            case InfoAreaMappings:
                return CIcons::appMappings16();
            case InfoAreaLog:
                return CIcons::appLog16();
            default:
                return CIcons::empty();
            }
        }

        CDockWidgetInfoArea *CMainInfoAreaComponent::getDockableWidgetByIndex(int index) const
        {
            if (index >= this->m_dockableWidgets.count() || index < 0) return nullptr;
            return this->m_dockableWidgets.at(index);
        }

        CDockWidgetInfoArea *CMainInfoAreaComponent::selectedDockableWidget() const
        {
            if (!this->m_tabBar) return nullptr;
            int i = this->m_tabBar->currentIndex();
            return getDockableWidgetByIndex(i);
        }

        int CMainInfoAreaComponent::countDockedWidgets() const
        {
            if (!this->m_tabBar) return 0;
            return this->m_tabBar->count();
        }

        int CMainInfoAreaComponent::widgetToTabBarIndex(const CDockWidgetInfoArea *dockWidget)
        {
            if (!dockWidget) return -1;
            if (dockWidget->isFloating()) return -1;
            int tabBarIndex = 0;
            QList<CDockWidgetInfoArea *>::iterator i;
            for (i = this->m_dockableWidgets.begin(); i != this->m_dockableWidgets.end(); ++i)
            {
                CDockWidgetInfoArea *dw = (*i);
                if (dw->isFloating()) continue; // not in tab bar
                if (dw == dockWidget) return tabBarIndex;
                tabBarIndex++;
            }
            return -1;
        }

        void CMainInfoAreaComponent::setFeaturesForDockableWidgets(QDockWidget::DockWidgetFeatures features)
        {
            for (int i = 0; i < this->m_dockableWidgets.size(); i++)
            {
                CDockWidgetInfoArea *dw = this->m_dockableWidgets.at(i);
                dw->setFeatures(features);
            }
        }

        void CMainInfoAreaComponent::setTabPixmaps()
        {
            if (!this->m_tabBar) return;
            for (int i = 0; i < this->m_tabBar->count(); i++)
            {
                InfoArea area = static_cast<InfoArea>(i);
                const QPixmap p(infoAreaToIcon(area));
                this->m_tabBar->setTabIcon(i, p);
            }
        }

        void CMainInfoAreaComponent::ps_tabBarDoubleClicked(int index)
        {
            CDockWidgetInfoArea *dw = (index >= 0) ?
                                      this->getDockableWidgetByIndex(index) :
                                      this->selectedDockableWidget();
            if (!dw) return;
            dw->toggleFloating();
        }

        void CMainInfoAreaComponent::ps_onWidgetTopLevelChanged(CDockWidget *widget, bool topLevel)
        {
            Q_ASSERT(widget);
            Q_UNUSED(topLevel);
            if (!widget) return;

            // fix pixmaps
            this->setTabPixmaps();
        }

        void CMainInfoAreaComponent::ps_onStyleSheetChanged()
        {
            QString qss = CStyleSheetUtility::instance().style(CStyleSheetUtility::fileNameMainInfoArea());
            this->setStyleSheet(qss);

            if (this->m_tabBar)
            {
                QString qss = CStyleSheetUtility::instance().style(CStyleSheetUtility::fileNameDockWidgetTab());
                this->m_tabBar->setStyleSheet(qss);
            }
        }

        void CMainInfoAreaComponent::ps_showContextMenu(const QPoint &pos)
        {
            QPoint globalPos = this->mapToGlobal(pos);
            QScopedPointer<QMenu> contextMenu(new QMenu(this));
            this->addToContextMenu(contextMenu.data());

            QAction *selectedItem = contextMenu.data()->exec(globalPos);
            Q_UNUSED(selectedItem);

        }

        void CMainInfoAreaComponent::ps_showTabTexts(bool show)
        {
            if (show == this->m_showTabTexts) return;
            this->m_showTabTexts = show;
            QList<CDockWidgetInfoArea *>::iterator i;
            for (i = this->m_dockableWidgets.begin(); i != this->m_dockableWidgets.end(); ++i)
            {
                CDockWidgetInfoArea *dw = (*i);
                dw->showTitleWhenDocked(show);
            }
        }

        void CMainInfoAreaComponent::ps_setTabBarPosition(QTabWidget::TabPosition position)
        {
            Q_ASSERT(position == QTabWidget::North || position == QTabWidget::South);
            this->setTabPosition(Qt::TopDockWidgetArea, position);
        }

        void CMainInfoAreaComponent::ps_toggleTabBarPosition()
        {
            QTabWidget::TabPosition p = (this->tabPosition(Qt::TopDockWidgetArea) == QTabWidget::North) ?
                                        QTabWidget::South : QTabWidget::North;
            this->ps_setTabBarPosition(p);
        }

        void CMainInfoAreaComponent::closeEvent(QCloseEvent *event)
        {
            if (this->isFloating())
            {
                this->toggleFloating();
                event->setAccepted(false); // refuse -> do not close
            }
            else
            {
                QMainWindow::closeEvent(event);
            }
        }
    }
}
