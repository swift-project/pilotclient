/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution. No part of swift project, including this file, may be copied, modified, propagated,
 * or distributed except according to the terms contained in the LICENSE file.
 */

#include "blackgui/dockwidget.h"
#include "blackgui/dockwidgetinfoarea.h"
#include "blackgui/guiapplication.h"
#include "blackgui/guiutility.h"
#include "blackgui/infoarea.h"
#include "blackgui/stylesheetutility.h"
#include "blackmisc/icons.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/verify.h"

#include <QAction>
#include <QCloseEvent>
#include <QFlags>
#include <QIcon>
#include <QKeyEvent>
#include <QMenu>
#include <QPixmap>
#include <QPoint>
#include <QPointer>
#include <QScopedPointer>
#include <QSignalMapper>
#include <QStatusBar>
#include <QStyle>
#include <QTabBar>
#include <QTimer>
#include <QVariant>
#include <QWidget>
#include <QtGlobal>

using namespace BlackMisc;

namespace BlackGui
{
    CInfoArea::CInfoArea(QWidget *parent) :
        QMainWindow(parent),
        CEnableForFramelessWindow(CEnableForFramelessWindow::WindowTool, false, "framelessInfoArea", this)
    {
        this->setWholeInfoAreaFloating(m_infoAreaFloating);
    }

    CInfoArea::~CInfoArea()
    { }

    void CInfoArea::initInfoArea()
    {
        // initInfoArea() needs be called after(!) GUI is setup

        // Ref T184, child areas are now "cached" in m_childInfoAreas
        // The original version did always use "findChildInfoAreas", so if there are ever any issues T184 might be reverted
        m_childInfoAreas = this->findOwnChildInfoAreas();
        m_dockWidgetInfoAreas = this->findOwnDockWidgetInfoAreas();

        this->setDockArea(Qt::TopDockWidgetArea);
        this->connectTopLevelChanged();
        this->setFeaturesForDockableWidgets(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetClosable);
        this->tabifyAllWidgets();

        // context menu
        this->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(this, &CInfoArea::customContextMenuRequested, this, &CInfoArea::showContextMenu);
        connect(sGui, &CGuiApplication::styleSheetsChanged, this, &CInfoArea::onStyleSheetChanged, Qt::QueuedConnection);

        // initial style sheet setting
        this->onStyleSheetChanged();

        // status bar
        if (this->statusBar())
        {
            this->statusBar()->hide();
            this->statusBar()->setMaximumHeight(0);
        }

        // fire an initial status
        QPointer<CInfoArea> myself(this);
        QTimer::singleShot(5000, this, [ = ]
        {
            if (myself) { myself->emitInfoAreaStatus(); }
        });
    }

    void CInfoArea::addToContextMenu(QMenu *menu) const
    {
        if (!menu) { return; }
        bool hasDockedWidgets = this->countDockedWidgetInfoAreas() > 0;
        if (hasDockedWidgets)
        {
            menu->addAction(CIcons::dockTop16(),  "Dock all",  this, &CInfoArea::dockAllWidgets);
            menu->addAction(CIcons::floatAll16(), "Float all", this, &CInfoArea::floatAllWidgets);
            menu->addAction(CIcons::refresh16(),  "Reset all floating to defaults", this, &CInfoArea::resetAllFloatingWidgetSettings);
            menu->addAction(CIcons::refresh16(),  "Reset all to defaults",          this, &CInfoArea::resetAllWidgetSettings);

            menu->addAction(CIcons::floatOne16(), QStringLiteral("Dock / float '%1'").arg(this->windowTitle()), this, &CInfoArea::toggleFloatingWholeInfoArea);
            QAction *lockTabBarMenuAction = new QAction(menu);
            lockTabBarMenuAction->setObjectName(this->objectName().append("LockTabBar"));
            lockTabBarMenuAction->setIconText("Lock tab bar");
            lockTabBarMenuAction->setIcon(CIcons::lockClosed16());
            lockTabBarMenuAction->setCheckable(true);
            lockTabBarMenuAction->setChecked(m_lockTabBar);
            menu->addAction(lockTabBarMenuAction);
            connect(lockTabBarMenuAction, &QAction::toggled, this, &CInfoArea::toggleTabBarLocked);

            menu->addSeparator();
            QMenu *subMenuToggleFloat = new QMenu("Toggle Float/Dock", menu);
            QMenu *subMenuDisplay = new QMenu("Display", menu);
            QMenu *subMenuRestore = new QMenu("Restore from settings", menu);
            QMenu *subMenuResetPositions = new QMenu("Reset position", menu);
            subMenuRestore->setIcon(CIcons::load16());
            subMenuResetPositions->setIcon(CIcons::refresh16());
            subMenuRestore->addActions(this->getInfoAreaRestoreActions(subMenuRestore));
            subMenuDisplay->addActions(this->getInfoAreaSelectActions(false, subMenuDisplay));
            subMenuResetPositions->addActions(this->getInfoAreaResetPositionActions(subMenuResetPositions));

            QSignalMapper *signalMapperToggleFloating = new QSignalMapper(menu);
            bool c = false; // check connections

            for (int i = 0; i < m_dockWidgetInfoAreas.size(); i++)
            {
                const CDockWidgetInfoArea *dw = m_dockWidgetInfoAreas.at(i);
                const QString t = dw->windowTitleBackup();
                const QPixmap pm = this->indexToPixmap(i);
                QAction *toggleFloatingMenuAction = new QAction(menu);
                toggleFloatingMenuAction->setObjectName(QString(t).append("ToggleFloatingAction"));
                toggleFloatingMenuAction->setIconText(t);
                toggleFloatingMenuAction->setIcon(pm);
                toggleFloatingMenuAction->setData(QVariant(i));
                toggleFloatingMenuAction->setCheckable(true);
                toggleFloatingMenuAction->setChecked(!dw->isFloating());
                subMenuToggleFloat->addAction(toggleFloatingMenuAction);
                c = connect(toggleFloatingMenuAction, &QAction::toggled, signalMapperToggleFloating, qOverload<>(&QSignalMapper::map));
                BLACK_VERIFY_X(c, Q_FUNC_INFO, "Cannot map floating action"); // do not make that shutdown reason in a release build
                signalMapperToggleFloating->setMapping(toggleFloatingMenuAction, i);
            }

            c = connect(signalMapperToggleFloating, qOverload<int>(&QSignalMapper::mapped), this, &CInfoArea::toggleFloatingByIndex);
            BLACK_VERIFY_X(c, Q_FUNC_INFO, "Cannot connect mapper"); // do not make that shutdown reason in a release build

            menu->addMenu(subMenuDisplay);
            if (c) { menu->addMenu(subMenuToggleFloat); }
            menu->addMenu(subMenuResetPositions);
            menu->addMenu(subMenuRestore);

            // where and how to display tab bar
            menu->addSeparator();
            QAction *showMenuText = new QAction(menu);
            showMenuText->setObjectName("ShowDockedWidgetTextAction");
            showMenuText->setIconText("Show tab text");
            showMenuText->setIcon(CIcons::headingOne16());
            showMenuText->setCheckable(true);
            showMenuText->setChecked(m_showTabTexts);
            menu->addAction(showMenuText);
            connect(showMenuText, &QAction::toggled, this, &CInfoArea::showTabTexts);

            // auto adjust floating widgets
            QAction *showTabbar = new QAction(menu);
            showTabbar->setObjectName("ShowTabBar");
            showTabbar->setIconText("Show tab bar");
            showTabbar->setIcon(CIcons::dockBottom16());
            showTabbar->setCheckable(true);
            showTabbar->setChecked(m_showTabBar);
            menu->addAction(showTabbar);
            connect(showTabbar, &QAction::toggled, this, &CInfoArea::showTabBar);

            // tab bar position
            menu->addAction(CIcons::dockBottom16(), "Toggle tabbar position", this, &CInfoArea::toggleTabBarPosition);
            Q_UNUSED(c)
        }
    }

    bool CInfoArea::isSelectedDockWidgetInfoArea(const CDockWidgetInfoArea *infoArea) const
    {
        if (!infoArea) { return false; }
        if (infoArea->isFloating()) { return false; }
        if (!infoArea->isWidgetVisible()) { return false; }
        if (!m_tabBar || m_tabBar->count() < 1) { return false; }
        return true;
    }

    const CDockWidgetInfoArea *CInfoArea::getSelectedDockInfoArea() const
    {
        // we assume that there can only be 1, non floating info area,
        // which is the only visible one
        // selecting is tab text independent (can be hidden)
        if (!m_tabBar || m_tabBar->count() < 1) { return nullptr; }
        for (const CDockWidgetInfoArea *ia : m_dockWidgetInfoAreas)
        {
            if (ia->isFloating()) { continue; }
            if (ia->isWidgetVisible()) { return ia; }
        }
        return nullptr;
    }

    int CInfoArea::getSelectedDockInfoAreaIndex() const
    {
        const CDockWidgetInfoArea *sel = getSelectedDockInfoArea();
        if (!sel) { return -1; }
        const QString t(sel->windowTitleBackup());
        int ia = getAreaIndexByWindowTitle(t);
        return ia;
    }

    QList<const CDockWidgetInfoArea *> CInfoArea::getDockWidgetInfoAreas() const
    {
        return makeRange(m_dockWidgetInfoAreas);
    }

    QList<QAction *> CInfoArea::getInfoAreaSelectActions(bool withShortcut, QWidget *parent) const
    {
        Q_ASSERT(parent);
        int i = 0;
        QList<QAction *> actions;
        for (const CDockWidgetInfoArea *dockWidgetInfoArea : m_dockWidgetInfoAreas)
        {
            const QPixmap pm = this->indexToPixmap(i);
            const QString wt(dockWidgetInfoArea->windowTitleBackup());
            static const QString keys("123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ");
            QAction *action = new QAction(QIcon(pm), wt, parent);
            action->setData(i);
            action->setObjectName(this->objectName().append(":getInfoAreaSelectActions:").append(wt));
            if (withShortcut && i < keys.length())
            {
                // T600 The strings "Ctrl", "Shift", "Alt" and "Meta" are recognized
                // https://doc.qt.io/archives/qt-4.8/qkeysequence.html Standard shortcuts already used
                // "Ctrl+Shift+%1" als works
                action->setShortcut(QKeySequence(QStringLiteral("Ctrl+Shift+%1").arg(keys.at(i))));
            }

            connect(action, &QAction::triggered, this, &CInfoArea::selectAreaByAction);
            actions.append(action);
            i++;
        }
        return actions;
    }

    QList<QAction *> CInfoArea::getInfoAreaResetPositionActions(QWidget *parent) const
    {
        Q_ASSERT(parent);
        int i = 0;
        QList<QAction *> actions;
        for (const CDockWidgetInfoArea *dockWidgetInfoArea : m_dockWidgetInfoAreas)
        {
            const QPixmap pm = this->indexToPixmap(i);
            const QString wt(dockWidgetInfoArea->windowTitleBackup());
            QAction *action = new QAction(QIcon(pm), wt, parent);
            action->setData(i);
            action->setObjectName(this->objectName().append(":getInfoAreaResetPositionActions:").append(wt));
            connect(action, &QAction::triggered, this, &CInfoArea::resetPositionByAction);
            actions.append(action);
            i++;
        }
        return actions;
    }

    QList<QAction *> CInfoArea::getInfoAreaToggleFloatingActions(QWidget *parent) const
    {
        Q_ASSERT(parent);
        int i = 0;
        QList<QAction *> actions;
        for (const CDockWidgetInfoArea *dockWidgetInfoArea : m_dockWidgetInfoAreas)
        {
            const QPixmap pm = this->indexToPixmap(i);
            const QString wt(dockWidgetInfoArea->windowTitleBackup());
            QAction *action = new QAction(QIcon(pm), wt, parent);
            action->setData(i);
            action->setObjectName(this->objectName().append(":getInfoAreaToggleFloatingActions:").append(wt));
            connect(action, &QAction::triggered, this, &CInfoArea::toggleAreaFloatingByAction, Qt::QueuedConnection);
            actions.append(action);
            i++;
        }
        return actions;
    }

    QList<QAction *> CInfoArea::getInfoAreaRestoreActions(QWidget *parent) const
    {
        Q_ASSERT(parent);
        int i = 0;
        QList<QAction *> actions;
        for (const CDockWidgetInfoArea *dockWidgetInfoArea : m_dockWidgetInfoAreas)
        {
            const QPixmap pm = this->indexToPixmap(i);
            const QString wt(dockWidgetInfoArea->windowTitleBackup());
            QAction *action = new QAction(QIcon(pm), wt, parent);
            action->setData(i);
            action->setObjectName(this->objectName().append(":getInfoAreaRestoreActions:").append(wt));
            connect(action, &QAction::triggered, this, &CInfoArea::restoreDockWidgetInfoArea);
            actions.append(action);
            i++;
        }
        return actions;
    }

    QList<int> CInfoArea::getAreaIndexesDockedOrFloating(bool floating) const
    {
        QList<int> indexes;
        for (int i = 0; i < m_dockWidgetInfoAreas.size(); i++)
        {
            if (m_dockWidgetInfoAreas.at(i)->isFloating() == floating)
            {
                indexes.append(i);
            }
        }
        return indexes;
    }

    void CInfoArea::paintEvent(QPaintEvent *event)
    {
        CStyleSheetUtility::useStyleSheetInDerivedWidget(this, QStyle::PE_FrameWindow);
        Q_UNUSED(event)
    }

    void CInfoArea::keyPressEvent(QKeyEvent *event)
    {
        if (event->key() == Qt::Key_Right)
        {
            this->selectRightTab();
            event->accept();
        }
        else if (event->key() == Qt::Key_Left)
        {
            this->selectLeftTab();
            event->accept();
        }
        else
        {
            QWidget::keyPressEvent(event);
        }
    }

    void CInfoArea::dockAllWidgets()
    {
        this->tabifyAllWidgets();
    }

    void CInfoArea::adjustSizeForAllDockWidgets()
    {
        for (CDockWidgetInfoArea *dw : std::as_const(m_dockWidgetInfoAreas))
        {
            dw->adjustSize();
        }
    }

    void CInfoArea::floatAllWidgets()
    {
        for (CDockWidgetInfoArea *dw : std::as_const(m_dockWidgetInfoAreas))
        {
            if (dw->isFloating()) { continue; }
            dw->toggleFloating();
        }
    }

    void CInfoArea::resetAllFloatingWidgetSettings()
    {
        for (CDockWidgetInfoArea *dw : std::as_const(m_dockWidgetInfoAreas))
        {
            if (!dw || !dw->isFloating()) { continue; }
            dw->resetSettings();
        }
    }

    void CInfoArea::resetAllWidgetSettings()
    {
        for (CDockWidgetInfoArea *dw : std::as_const(m_dockWidgetInfoAreas))
        {
            if (!dw || !dw->isFloating()) { continue; }
            dw->resetSettings();
        }
    }

    void CInfoArea::allFloatingOnTop()
    {
        for (CDockWidgetInfoArea *dw : std::as_const(m_dockWidgetInfoAreas))
        {
            const bool f = dw->isFloating();
            CGuiUtility::stayOnTop(f, dw);
            if (f) { dw->show(); }
        }
    }

    void CInfoArea::toggleFloatingWholeInfoArea()
    {
        this->setWholeInfoAreaFloating(!m_infoAreaFloating);
    }

    void CInfoArea::toggleFloatingByIndex(int areaIndex)
    {
        if (!this->isValidAreaIndex(areaIndex)) { return; }
        CDockWidgetInfoArea *dw = m_dockWidgetInfoAreas.at(areaIndex);
        BLACK_VERIFY_X(dw, Q_FUNC_INFO, "Missing info area");
        if (!dw) { return; }
        dw->toggleFloating();
    }

    void CInfoArea::toggleVisibility(int areaIndex)
    {
        if (!this->isValidAreaIndex(areaIndex)) { return; }
        CDockWidgetInfoArea *dw = m_dockWidgetInfoAreas.at(areaIndex);
        BLACK_VERIFY_X(dw, Q_FUNC_INFO, "Missing info area");
        if (!dw) { return; }
        dw->toggleVisibility();
    }

    void CInfoArea::selectArea(int areaIndex)
    {
        CDockWidgetInfoArea *dw = m_dockWidgetInfoAreas.at(areaIndex);
        BLACK_VERIFY_X(dw, Q_FUNC_INFO, "Missing info area");
        if (!dw) { return; }
        Q_ASSERT(m_tabBar);
        if (m_tabBar->count() < 1) { return; }

        if (dw->isFloating())
        {
            dw->show();
        }
        else
        {
            this->selectArea(dw);
        }
    }

    void CInfoArea::resetPosition(int areaIndex)
    {
        CDockWidgetInfoArea *dw = m_dockWidgetInfoAreas.at(areaIndex);
        BLACK_VERIFY_X(dw, Q_FUNC_INFO, "Missing info area");
        if (!dw) { return; }
        dw->resetPosition();
    }

    void CInfoArea::selectAreaByAction()
    {
        const QObject *sender = QObject::sender();
        const QAction *action = qobject_cast<const QAction *>(sender);
        BLACK_VERIFY(action);
        if (!action) { return; }
        const int index = action->data().toInt();
        this->selectArea(index);
    }

    void CInfoArea::resetPositionByAction()
    {
        const QObject *sender = QObject::sender();
        const QAction *action = qobject_cast<const QAction *>(sender);
        BLACK_VERIFY(action);
        if (!action) { return; }
        const int index = action->data().toInt();
        this->resetPosition(index);
    }

    void CInfoArea::toggleAreaFloatingByAction()
    {
        const QObject *sender = QObject::sender();
        const QAction *action = qobject_cast<const QAction *>(sender);
        BLACK_VERIFY(action);
        if (!action) { return; }
        const int index = action->data().toInt();
        this->toggleFloatingByIndex(index);
    }

    void CInfoArea::restoreDockWidgetInfoArea()
    {
        const QObject *sender = QObject::sender();
        const QAction *action = qobject_cast<const QAction *>(sender);
        BLACK_VERIFY(action);
        if (!action) { return; }
        const int index = action->data().toInt();
        this->restoreDockWidgetInfoAreaByIndex(index);
    }

    void CInfoArea::restoreDockWidgetInfoAreaByIndex(int areaIndex)
    {
        if (!this->isValidAreaIndex(areaIndex)) { return; }
        CDockWidgetInfoArea *dw = m_dockWidgetInfoAreas.at(areaIndex);
        Q_ASSERT(dw);
        if (!dw) { return; }
        dw->restoreFromSettings();
    }

    void CInfoArea::selectLeftTab()
    {
        if (!m_tabBar) return;
        if (m_tabBar->count() < 2) return;
        if (m_tabBar->currentIndex() > 0)
        {
            m_tabBar->setCurrentIndex(m_tabBar->currentIndex() - 1);
        }
        else
        {
            m_tabBar->setCurrentIndex(m_tabBar->count() - 1);
        }
    }

    void CInfoArea::selectRightTab()
    {
        if (!m_tabBar) return;
        if (m_tabBar->count() < 2) return;
        if (m_tabBar->currentIndex() < m_tabBar->count() - 2)
        {
            m_tabBar->setCurrentIndex(m_tabBar->currentIndex() + 1);
        }
        else
        {
            m_tabBar->setCurrentIndex(0);
        }
    }

    void CInfoArea::displayStatusMessage(const CStatusMessage &statusMessage)
    {
        for (CDockWidgetInfoArea *dw : std::as_const(m_dockWidgetInfoAreas))
        {
            dw->displayStatusMessage(statusMessage);
        }
        for (CInfoArea *ia : std::as_const(m_childInfoAreas))
        {
            ia->displayStatusMessage(statusMessage);
        }
    }

    void CInfoArea::displayStatusMessages(const CStatusMessageList &statusMessages)
    {
        for (CDockWidgetInfoArea *dw : std::as_const(m_dockWidgetInfoAreas))
        {
            dw->displayStatusMessages(statusMessages);
        }
        for (CInfoArea *ia : std::as_const(m_childInfoAreas))
        {
            ia->displayStatusMessages(statusMessages);
        }
    }

    void CInfoArea::setDockArea(Qt::DockWidgetArea area)
    {
        for (CDockWidgetInfoArea *dw : std::as_const(m_dockWidgetInfoAreas))
        {
            Qt::DockWidgetAreas newAreas = static_cast<Qt::DockWidgetAreas>(area);
            Qt::DockWidgetAreas oldAreas = dw->allowedAreas();
            if (oldAreas == newAreas) { continue; }
            dw->setAllowedAreas(newAreas);
            this->addDockWidget(area, dw);
        }
    }

    void CInfoArea::setWholeInfoAreaFloating(bool floating)
    {
        // float whole info area
        m_infoAreaFloating = floating;
        if (m_infoAreaFloating)
        {
            QPoint p = CGuiUtility::mainWidgetGlobalPosition();
            this->setWindowFlags(Qt::Dialog);
            this->setWindowFlags(this->windowFlags() & ~Qt::WindowContextHelpButtonHint);
            this->move(p.rx() + 20, p.ry() + 20);
            this->show(); // not working without show
        }
        else
        {
            // make this compliant as QWidget
            // https://qt-project.org/forums/viewthread/17519
            // http://www.qtcentre.org/threads/12569-QMainWindow-as-a-child-of-QMainWindow

            // this->setParent(m_originalParent, this->windowFlags() & ~Qt::Window);
            this->setWindowFlags(this->windowFlags() & ~Qt::Window);

            // RW: The line below is commented to prevent making this widget visible as a top window
            // in case it is constructed without parent or anchestor widget. Contrary to the comment,
            // it does not seem to be necessary https://dev.vatsim-germany.org/issues/738
            // KB 2018-12 with T447 T452 re-eanled the line again, but with parent condition
            if (this->parentWidget()) // this line
            {
                this->setVisible(true); // after redocking this is required
            }
        }

        emit this->changedWholeInfoAreaFloating(floating);
    }

    void CInfoArea::tabifyAllWidgets()
    {
        if (!sGui || sGui->isShuttingDown()) { return; }
        this->setTabPosition(Qt::LeftDockWidgetArea, QTabWidget::East);
        const bool init = m_tabBar ? false : true;

        for (int i = 0; i < m_dockWidgetInfoAreas.size(); i++)
        {
            CDockWidgetInfoArea *first = i > 0 ? m_dockWidgetInfoAreas.at(i - 1) : nullptr;
            CDockWidgetInfoArea *after = m_dockWidgetInfoAreas.at(i);
            Q_ASSERT(after);

            // trick, init widget as floating
            // this completely initializes the tab bar and all docked widgets
            if (init)
            {
                // float
                const QPoint offset(i * 10, i * 10);
                // after->setVisible(false);
                // after->setFloating(true);
                after->setOffsetWhenFloating(offset, after->isFrameless());
                const QSize floatingSize = this->getPreferredSizeWhenFloating(i);
                after->setPreferredSizeWhenFloating(floatingSize);
                after->initialFloating();

                // dock again
                // after->setFloating(false);
                // after->setVisible(true);

                // reset floating flag, we want new resizing and position for first real floating
                after->resetWasAlreadyFloating();
            }
            else
            {
                after->setFloating(false);
            }

            // we can not tabify first
            if (!first) { continue; }

            this->tabifyDockWidget(first, after);
        }

        // as now tabified, set tab
        if (init)
        {
            m_tabBar = this->findChild<QTabBar *>();

            // if we have > 1 docked widgets, we have a tab bar
            if (m_tabBar && sGui)
            {
                const QString qss = sGui->getStyleSheetUtility().style(CStyleSheetUtility::fileNameDockWidgetTab());
                m_tabBar->setStyleSheet(qss);
                m_tabBar->setObjectName("comp_MainInfoAreaDockWidgetTab");
                m_tabBar->setMovable(false);
                m_tabBar->setElideMode(Qt::ElideNone);
                m_tabBar->setUsesScrollButtons(true);

                // East / West does not work (shown, but area itself empty)
                // South does not have any effect
                m_tabBar->setShape(QTabBar::TriangularSouth);

                // signals, use Qt::QueuedConnection to avoid issues during shutdown
                connect(m_tabBar, &QTabBar::tabBarDoubleClicked, this, &CInfoArea::onTabBarDoubleClicked, Qt::QueuedConnection);
                connect(m_tabBar, &QTabBar::currentChanged,      this, &CInfoArea::onTabBarIndexChanged,  Qt::QueuedConnection);
            }
            else
            {
                // <= 1 dock widget
                m_tabBar = new QTabBar(this);
                m_tabBar->hide();
            }
        }

        // set current index, and always set pixmaps
        if (this->countDockedWidgetInfoAreas() > 0) { m_tabBar->setCurrentIndex(0); }
        if (m_tabBar->count() > 0) { this->setTabPixmaps(); }
    }

    void CInfoArea::unTabifyAllWidgets()
    {
        if (m_dockWidgetInfoAreas.size() < 2) return;
        CDockWidgetInfoArea *first = m_dockWidgetInfoAreas.constFirst();
        for (int i = 1; i < m_dockWidgetInfoAreas.size(); i++)
        {
            CDockWidgetInfoArea *after = m_dockWidgetInfoAreas.at(i);
            Q_ASSERT(after);
            this->splitDockWidget(first, after, Qt::Horizontal);
        }
    }

    bool CInfoArea::isValidAreaIndex(int areaIndex) const
    {
        if (!m_tabBar) { return false; }
        return areaIndex >= 0 && areaIndex < m_dockWidgetInfoAreas.size();
    }

    void CInfoArea::connectTopLevelChanged()
    {
        for (CDockWidgetInfoArea *dw : std::as_const(m_dockWidgetInfoAreas))
        {
            connect(dw, &CDockWidgetInfoArea::widgetTopLevelChanged, this, &CInfoArea::onWidgetTopLevelChanged, Qt::QueuedConnection);
        }
    }

    QList<CDockWidgetInfoArea *> CInfoArea::findOwnDockWidgetInfoAreas() const
    {
        // own dock widget areas without nested ones
        return this->findChildren<CDockWidgetInfoArea *>(QString(), Qt::FindDirectChildrenOnly);
    }

    QList<CInfoArea *> CInfoArea::findOwnChildInfoAreas() const
    {
        return this->findChildren<CInfoArea *>(QString(), Qt::FindDirectChildrenOnly);
    }

    void CInfoArea::emitInfoAreaStatus()
    {
        if (!sGui || sGui->isShuttingDown()) { return; } // avoid access to deleted components
        const int sia = this->getSelectedDockInfoAreaIndex();
        const QList<int> floating = this->getAreaIndexesDockedOrFloating(true);
        const QList<int> docked = this->getAreaIndexesDockedOrFloating(false);
        emit this->changedInfoAreaStatus(sia, docked, floating);
    }

    void CInfoArea::onTabBarIndexChanged(int tabBarIndex)
    {
        emit this->changedInfoAreaTabBarIndex(tabBarIndex);
        this->emitInfoAreaStatus();
    }

    int CInfoArea::countDockedWidgetInfoAreas() const
    {
        if (!m_tabBar) { return 0; }
        return m_tabBar->count();
    }

    CDockWidgetInfoArea *CInfoArea::getDockWidgetInfoAreaByTabBarIndex(int tabBarIndex) const
    {
        if (tabBarIndex >= m_dockWidgetInfoAreas.count() || tabBarIndex < 0) { return nullptr; }
        if (!m_tabBar) { return nullptr; }
        const QString t(m_tabBar->tabText(tabBarIndex));

        // we have a title and search by that (best option, as order does not matter)
        if (!t.isEmpty()) { return this->getDockWidgetInfoAreaByWindowTitle(t); }

        // no title, we assume the n-th not floating tab is correct
        // this will work if the order in m_dockWidgetInfoAreas matches
        int c = 0;
        for (CDockWidgetInfoArea *dw : m_dockWidgetInfoAreas)
        {
            if (dw->isFloating()) { continue; }
            if (c == tabBarIndex) { return dw; }
            c++;
        }
        Q_ASSERT_X(false, Q_FUNC_INFO, "no dock widgte found");
        return nullptr;
    }

    CDockWidgetInfoArea *CInfoArea::getDockWidgetInfoAreaByWindowTitle(const QString &title) const
    {
        Q_ASSERT_X(!title.isEmpty(), Q_FUNC_INFO, "No title");
        for (CDockWidgetInfoArea *dw : m_dockWidgetInfoAreas)
        {
            if (CGuiUtility::lenientTitleComparison(dw->windowTitleOrBackup(), title)) { return dw; }
        }
        return nullptr;
    }

    int CInfoArea::getAreaIndexByWindowTitle(const QString &title) const
    {
        Q_ASSERT_X(!title.isEmpty(), Q_FUNC_INFO, "No title");

        for (int i = 0; i < m_dockWidgetInfoAreas.size(); i++)
        {
            if (CGuiUtility::lenientTitleComparison(m_dockWidgetInfoAreas.at(i)->windowTitleOrBackup(), title)) { return i; }
        }
        Q_ASSERT_X(false, Q_FUNC_INFO, "No area for title");
        return -1;
    }

    int CInfoArea::getTabBarIndexByTitle(const QString &title) const
    {
        BLACK_VERIFY_X(!title.isEmpty(), Q_FUNC_INFO, "No title");
        if (title.isEmpty()) { return -1; }

        if (m_tabBar->count() < 1) { return -1;}
        for (int i = 0; i < m_tabBar->count(); i++)
        {
            QString tt = m_tabBar->tabText(i);
            if (tt.isEmpty()) { tt = m_tabBar->tabToolTip(i); }
            if (tt.isEmpty()) { continue; } // cannot find by empty text
            if (CGuiUtility::lenientTitleComparison(tt, title)) { return i; }
        }
        return -1;
    }

    int CInfoArea::dockWidgetInfoAreaToTabBarIndex(const CDockWidgetInfoArea *dockWidgetInfoArea) const
    {
        if (!dockWidgetInfoArea) { return -1; }
        if (dockWidgetInfoArea->isFloating()) { return -1; }
        int index = this->getTabBarIndexByTitle(dockWidgetInfoArea->windowTitleOrBackup());
        if (index < 0)
        {
            // fallback if the tab has no name and cannot be found
            int i = 0;
            for (const CDockWidgetInfoArea *a : m_dockWidgetInfoAreas)
            {
                if (a == dockWidgetInfoArea)
                {
                    index = i;
                    break;
                }
                ++i;
            }
        }

        // sanity check
        if (index >= m_tabBar->count()) { index = -1; }

        // bye
        return index;
    }

    void CInfoArea::selectArea(const CDockWidgetInfoArea *dockWidgetInfoArea)
    {
        if (!m_tabBar) { return; }
        int tabIndex = this->dockWidgetInfoAreaToTabBarIndex(dockWidgetInfoArea);
        if (tabIndex >= 0 && tabIndex < m_tabBar->count())
        {
            m_tabBar->setCurrentIndex(tabIndex);
        }
    }

    void CInfoArea::setFeaturesForDockableWidgets(QDockWidget::DockWidgetFeatures features)
    {
        for (CDockWidgetInfoArea *dw : std::as_const(m_dockWidgetInfoAreas))
        {
            dw->setFeatures(features);
        }
    }

    void CInfoArea::setTabPixmaps()
    {
        if (!m_tabBar) { return; }
        for (int i = 0; i < m_tabBar->count(); i++)
        {
            const QString t(m_tabBar->tabText(i));
            const int areaIndex = t.isEmpty() ? i : this->getAreaIndexByWindowTitle(t);
            m_tabBar->setTabIcon(i, indexToPixmap(areaIndex));
            m_tabBar->setTabToolTip(i, t);
        }
    }

    void CInfoArea::onTabBarDoubleClicked(int tabBarIndex)
    {
        if (m_lockTabBar)
        {
            CLogMessage(this).info(u"Locked, double click will not cause floating");
            return;
        }
        CDockWidgetInfoArea *dw = this->getDockWidgetInfoAreaByTabBarIndex(tabBarIndex);
        if (!dw) { return; }
        dw->toggleFloating();

        QPointer<CInfoArea> myself(this);
        QTimer::singleShot(1000, this, [ = ]
        {
            if (!myself) { return; }
            myself->emitInfoAreaStatus();
        });
    }

    void CInfoArea::onWidgetTopLevelChanged(CDockWidget *dockWidget, bool topLevel)
    {
        Q_ASSERT(dockWidget);
        Q_UNUSED(topLevel)
        if (!dockWidget) { return; }

        // fix pixmaps
        this->setTabPixmaps();

        // select index
        if (!topLevel)
        {
            const CDockWidgetInfoArea *dwia = dynamic_cast<CDockWidgetInfoArea *>(dockWidget);
            this->selectArea(dwia);
        }

        // KB commented out with T592
        // when toplevel is changed, I need a round in the event loop until
        // current tab bar widget is visible
        // QTimer::singleShot(250, this, &CInfoArea::emitInfoAreaStatus);
    }

    void CInfoArea::onStyleSheetChanged()
    {
        if (m_tabBar)
        {
            if (!sGui || sGui->isShuttingDown()) { return; }
            const QString qss = sGui->getStyleSheetUtility().style(CStyleSheetUtility::fileNameDockWidgetTab());
            m_tabBar->setStyleSheet(qss);
        }
    }

    void CInfoArea::showContextMenu(const QPoint &pos)
    {
        QPoint globalPos = this->mapToGlobal(pos);
        QScopedPointer<QMenu> contextMenu(new QMenu(this));
        this->addToContextMenu(contextMenu.data());

        QAction *selectedItem = contextMenu.data()->exec(globalPos);
        Q_UNUSED(selectedItem)
    }

    void CInfoArea::showTabTexts(bool show)
    {
        if (show == m_showTabTexts) { return; }
        m_showTabTexts = show;
        for (CDockWidgetInfoArea *dw : std::as_const(m_dockWidgetInfoAreas))
        {
            dw->showTitleWhenDocked(show);
        }
    }

    void CInfoArea::showTabBar(bool show)
    {
        if (show == m_showTabBar) return;
        m_showTabBar = show;
        if (!m_tabBar) return;
        m_tabBar->setVisible(show); // not working, but setting right value will not harm anything
        m_tabBar->setMaximumHeight(show ? 10000 : 0); // does the trick
        this->adjustSizeForAllDockWidgets();
    }

    void CInfoArea::toggleTabBarLocked(bool locked)
    {
        m_lockTabBar = locked;
    }

    void CInfoArea::setTabBarPosition(QTabWidget::TabPosition position)
    {
        Q_ASSERT_X(position == QTabWidget::North || position == QTabWidget::South, Q_FUNC_INFO, "Wrong tabbar position");
        this->setTabPosition(Qt::TopDockWidgetArea, position);
    }

    void CInfoArea::toggleTabBarPosition()
    {
        QTabWidget::TabPosition p = (this->tabPosition(Qt::TopDockWidgetArea) == QTabWidget::North) ?
                                    QTabWidget::South : QTabWidget::North;
        this->setTabBarPosition(p);
    }

    void CInfoArea::closeEvent(QCloseEvent *event)
    {
        if (this->isFloating())
        {
            this->toggleFloatingWholeInfoArea();
            event->setAccepted(false); // refuse -> do not close
        }
        else
        {
            QMainWindow::closeEvent(event);
        }
    }
} // namespace
