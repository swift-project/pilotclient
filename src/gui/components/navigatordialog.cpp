// SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/components/navigatordialog.h"

#include <QAction>
#include <QEvent>
#include <QFrame>
#include <QGridLayout>
#include <QGuiApplication>
#include <QIcon>
#include <QList>
#include <QMainWindow>
#include <QMenu>
#include <QMouseEvent>
#include <QPoint>
#include <QSize>
#include <QString>
#include <QStringBuilder>
#include <QStyle>
#include <QToolButton>
#include <QVariant>
#include <Qt>
#include <QtGlobal>

#include "ui_navigatordialog.h"

#include "gui/components/marginsinput.h"
#include "gui/guiapplication.h"
#include "gui/guiutility.h"
#include "gui/stylesheetutility.h"
#include "misc/icons.h"
#include "misc/logmessage.h"

using namespace swift::gui;
using namespace swift::gui::settings;
using namespace swift::misc;

namespace swift::gui::components
{
    // If the dialog is a normal window, it stays open when the parent is minimized
    // (and the parent is null for the dialog). If the dialog is a tool winow it is always
    // minimized, regardless of dialog`s parent
    CNavigatorDialog::CNavigatorDialog(QWidget *parent)
        : QDialog(parent, modeToWindowFlags(CEnableForFramelessWindow::WindowTool)),
          CEnableForFramelessWindow(CEnableForFramelessWindow::WindowTool, false, "navigatorFrameless", this),
          ui(new Ui::CNavigatorDialog)
    {
        ui->setupUi(this);

        // context menu
        this->setContextMenuPolicy(Qt::CustomContextMenu);
        m_input = new CMarginsInput(this);
        m_input->setMaximumWidth(150);
        m_marginMenuAction = new QWidgetAction(this);
        m_marginMenuAction->setDefaultWidget(m_input);

        // Quit on window hack
        m_originalQuitOnLastWindow = QGuiApplication::quitOnLastWindowClosed();

        // timer
        m_watchdog.setObjectName(this->objectName() + ":m_timer");
        connect(&m_watchdog, &QTimer::timeout, this, &CNavigatorDialog::onWatchdog);

        this->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(m_input, &CMarginsInput::changedMargins, this, &CNavigatorDialog::menuChangeMargins);
        connect(this, &CNavigatorDialog::customContextMenuRequested, this, &CNavigatorDialog::showContextMenu);
        if (sGui)
        {
            connect(sGui, &CGuiApplication::styleSheetsChanged, this, &CNavigatorDialog::onStyleSheetsChanged,
                    Qt::QueuedConnection);
        }
        this->onStyleSheetsChanged();
    }

    CNavigatorDialog::~CNavigatorDialog() {}

    void CNavigatorDialog::buildNavigator(int columns)
    {
        if (m_firstBuild)
        {
            m_firstBuild = false;
            this->insertOwnActions();
        }

        this->onStyleSheetsChanged();

        // remove old layout
        CGuiUtility::deleteLayout(ui->fr_NavigatorDialogInner->layout(), false);

        // new layout
        auto *gridLayout = new QGridLayout(ui->fr_NavigatorDialogInner);
        gridLayout->setObjectName("gl_CNavigatorDialog");
        gridLayout->setSpacing(0);
        gridLayout->setContentsMargins(0, 0, 0, 0);
        ui->fr_NavigatorDialogInner->setLayout(gridLayout);
        int r = 0;
        int c = 0;

        // remark: the actions will be set from the main UI
        for (QAction *action : this->actions())
        {
            if (!action) { continue; }
            auto *tb = new QToolButton(ui->fr_NavigatorDialogInner);
            tb->setDefaultAction(action);
            tb->setObjectName(this->objectName() % u':' % action->objectName());
            if (!action->text().isEmpty()) { tb->setToolTip(action->text()); }
            gridLayout->addWidget(tb, r, c++);
            tb->show();
            if (c < columns) { continue; }
            c = 0;
            r++;
        }
        m_currentColumns = gridLayout->columnCount();
        this->adjustNavigatorSize(gridLayout);
        this->focusWidget();
    }

    void CNavigatorDialog::reject()
    {
        // workaround to avoid "closing issue with navigator",
        // https://discordapp.com/channels/539048679160676382/567139633964646411/620776182027386880
        if (m_mainWindow)
        {
            QGuiApplication::setQuitOnLastWindowClosed(m_originalQuitOnLastWindow);
            m_mainWindow->show();
        }

        this->hide();
        m_watchdog.stop();
        emit this->navigatorClosed();
    }

    void CNavigatorDialog::toggleFrameless() { this->setFrameless(!this->isFrameless()); }

    void CNavigatorDialog::showNavigator(bool visible)
    {
        this->setVisible(visible);
        CGuiUtility::stayOnTop(visible, this);
        this->show();
        QGuiApplication::setQuitOnLastWindowClosed(
            visible ? false : m_originalQuitOnLastWindow); // avoid issues with a dialog closing everything

        if (visible) { m_watchdog.start(4000); }
        else { m_watchdog.stop(); }
    }

    void CNavigatorDialog::toggleNavigatorVisibility()
    {
        const bool visible = !this->isVisible();
        this->showNavigator(visible);
    }

    void CNavigatorDialog::restoreFromSettings()
    {
        const CNavigatorSettings s = m_settings.get();
        this->setContentsMargins(s.getMargins());
        if (this->isFrameless() != s.isFramless()) { this->toggleFrameless(); }
        this->buildNavigator(s.getColumns());
        const QByteArray geo(s.getGeometry());
        this->restoreGeometry(geo);
    }

    void CNavigatorDialog::saveToSettings()
    {
        CNavigatorSettings s = m_settings.get();
        s.setFrameless(this->isFrameless());
        s.setMargins(this->contentsMargins());
        s.setGeometry(this->saveGeometry());
        s.setColumns(m_currentColumns);
        const CStatusMessage m = m_settings.setAndSave(s);
        if (!m.isSuccess()) { CLogMessage::preformatted(m); }
    }

    void CNavigatorDialog::onStyleSheetsChanged()
    {
        if (!sGui || sGui->isShuttingDown()) { return; }
        const QString fn(CStyleSheetUtility::fileNameNavigator());
        const QString qss(sGui->getStyleSheetUtility().style(fn));
        this->setStyleSheet("");
        this->setStyleSheet(qss);
        this->adjustNavigatorSize();
        this->repaint();
    }

    void CNavigatorDialog::mouseMoveEvent(QMouseEvent *event)
    {
        if (handleMouseMoveEvent(event)) { return; }

        // frameless has moving already, but here we also do it dor dialog
        if (!this->isFrameless())
        {
            if (event->buttons() & Qt::LeftButton)
            {
                const QPoint pos = this->mapToParent(event->pos() - m_framelessDragPosition);
                this->move(pos);
                event->accept();
                return;
            }
        }
        QDialog::mouseMoveEvent(event);
    }

    void CNavigatorDialog::mousePressEvent(QMouseEvent *event)
    {
        if (handleMousePressEvent(event)) { return; }

        // frameless has moving already, but here we also do it dor dialog
        if (!this->isFrameless())
        {
            if (event->buttons() & Qt::LeftButton)
            {
                m_framelessDragPosition = event->pos();
                event->accept();
                return;
            }
        }
        QDialog::mousePressEvent(event);
    }

    void CNavigatorDialog::mouseReleaseEvent(QMouseEvent *event)
    {
        m_framelessDragPosition = QPoint();
        QDialog::mouseReleaseEvent(event);
    }

    void CNavigatorDialog::changeEvent(QEvent *evt)
    {
        const QEvent::Type t = evt->type();
        if (t == QEvent::WindowStateChange)
        {
            evt->ignore();
            hide();
        }
        else { QDialog::changeEvent(evt); }
    }

    void CNavigatorDialog::windowFlagsChanged()
    {
        if (m_firstBuild) { return; }
        this->buildNavigator(m_currentColumns);
    }

    void CNavigatorDialog::paintEvent(QPaintEvent *event)
    {
        const bool s = CStyleSheetUtility::useStyleSheetInDerivedWidget(this, QStyle::PE_Widget);
        if (s) { return; }
        QDialog::paintEvent(event);
    }

    void CNavigatorDialog::enterEvent(QEnterEvent *event)
    {
        // event called when mouse is over, acts as auto-focus
        activateWindow();
        QDialog::enterEvent(event);
    }

    void CNavigatorDialog::showContextMenu(const QPoint &pos)
    {
        const QPoint globalPos = this->mapToGlobal(pos);
        QScopedPointer<QMenu> contextMenu(new QMenu(this));
        this->addToContextMenu(contextMenu.data());
        QAction *selectedItem = contextMenu.data()->exec(globalPos);
        Q_UNUSED(selectedItem);
    }

    void CNavigatorDialog::changeLayout()
    {
        auto *a = qobject_cast<QAction *>(QObject::sender());
        if (!a) { return; }
        QString v(a->data().toString());
        if (v == "1c") { buildNavigator(1); }
        else if (v == "2c") { buildNavigator(2); }
        else if (v == "1r") { buildNavigator(columnsForRows(1)); }
        else if (v == "2r") { buildNavigator(columnsForRows(2)); }
    }

    void CNavigatorDialog::menuChangeMargins(const QMargins &margins)
    {
        this->setContentsMargins(margins);
        this->adjustNavigatorSize();
    }

    void CNavigatorDialog::dummyFunction()
    {
        // void
    }

    void CNavigatorDialog::onSettingsChanged()
    {
        // void
    }

    void CNavigatorDialog::insertOwnActions()
    {
        // add some space for frameless navigators where I can move the navigator
        QAction *a = nullptr; // new QAction(CIcons::empty16(), "move navigator here", this);
        bool c = false;

        // save
        a = new QAction(CIcons::save16(), "Save state", this);
        c = connect(a, &QAction::triggered, this, &CNavigatorDialog::saveToSettings, Qt::QueuedConnection);
        Q_ASSERT(c);
        this->addAction(a);

        // close
        const QIcon i(CIcons::changeIconBackgroundColor(this->style()->standardIcon(QStyle::SP_TitleBarCloseButton),
                                                        Qt::white, QSize(16, 16)));
        a = new QAction(i, "Close", this);
        c = connect(a, &QAction::triggered, this, &CNavigatorDialog::close, Qt::QueuedConnection);
        Q_ASSERT(c);
        this->addAction(a);
    }

    int CNavigatorDialog::columnsForRows(int rows)
    {
        Q_ASSERT_X(rows >= 0, Q_FUNC_INFO, "no rows");
        int items = this->actions().size();
        int c = items / rows;
        return (c * rows) < items ? c + 1 : c;
    }

    QGridLayout *CNavigatorDialog::myGridLayout() const { return qobject_cast<QGridLayout *>(this->layout()); }

    void CNavigatorDialog::adjustNavigatorSize(QGridLayout *layout)
    {
        QGridLayout *gridLayout = layout ? layout : this->myGridLayout();
        Q_ASSERT_X(gridLayout, Q_FUNC_INFO, "Missing layout");

        int w = 16 * gridLayout->columnCount();
        int h = 16 * gridLayout->rowCount();

        // margins
        const QMargins margins = gridLayout->contentsMargins() + this->contentsMargins();
        h = h + margins.top() + margins.bottom();
        w = w + margins.left() + margins.right();

        // adjust
        const QSize min(w + 2, h + 2);
        ui->fr_NavigatorDialogInner->setMinimumSize(min);
        this->setMinimumSize(min);
        this->adjustSize();
    }

    void CNavigatorDialog::onWatchdog()
    {
        // if (!this->isVisible()) { return; }
        CGuiUtility::stayOnTop(true, this);
        this->show();
    }

    void CNavigatorDialog::addToContextMenu(QMenu *contextMenu) const
    {
        QAction *a = contextMenu->addAction(CIcons::resize16(), "1 row", this, &CNavigatorDialog::changeLayout);
        a->setData("1r");
        a = contextMenu->addAction(CIcons::resize16(), "2 rows", this, &CNavigatorDialog::changeLayout);
        a->setData("2r");
        a = contextMenu->addAction(CIcons::resize16(), "1 column", this, &CNavigatorDialog::changeLayout);
        a->setData("1c");
        a = contextMenu->addAction(CIcons::resize16(), "2 columns", this, &CNavigatorDialog::changeLayout);
        a->setData("2c");
        const QString frameLessActionText = this->isFrameless() ? "Normal window" : "Frameless";
        contextMenu->addAction(CIcons::tableSheet16(), frameLessActionText, this, &CNavigatorDialog::toggleFrameless);
        contextMenu->addAction("Adjust margins", this, &CNavigatorDialog::dummyFunction);
        contextMenu->addAction(m_marginMenuAction);
        contextMenu->addSeparator();
        contextMenu->addAction(CIcons::load16(), "Restore from settings", this, &CNavigatorDialog::restoreFromSettings);
        contextMenu->addAction(CIcons::save16(), "Save state", this, &CNavigatorDialog::saveToSettings);
    }
} // namespace swift::gui::components
