/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackgui/components/navigatordialog.h"
#include "blackgui/components/marginsinput.h"
#include "blackgui/guiapplication.h"
#include "blackgui/guiutility.h"
#include "blackgui/stylesheetutility.h"
#include "blackmisc/logmessage.h"
#include "blackmisc/icons.h"
#include "ui_navigatordialog.h"

#include <QAction>
#include <QEvent>
#include <QFrame>
#include <QGridLayout>
#include <QIcon>
#include <QList>
#include <QMenu>
#include <QPoint>
#include <QSize>
#include <QString>
#include <QStyle>
#include <QToolButton>
#include <QVariant>
#include <Qt>
#include <QtGlobal>

using namespace BlackGui;
using namespace BlackGui::Settings;
using namespace BlackMisc;

namespace BlackGui
{
    namespace Components
    {
        // If the dialog is a normal window, it stays open when the parent is minimized
        // (and the parent is null for the dialog). If the dialog is a tool winow it is always
        // minimized, regardless of dialog`s parent
        CNavigatorDialog::CNavigatorDialog(QWidget *parent) :
            QDialog(parent, modeToWindowFlags(CEnableForFramelessWindow::WindowTool)),
            CEnableForFramelessWindow(CEnableForFramelessWindow::WindowTool, false, "navigatorFrameless", this),
            ui(new Ui::CNavigatorDialog)
        {
            ui->setupUi(this);

            // context menu
            this->setContextMenuPolicy(Qt::CustomContextMenu);
            this->m_input = new CMarginsInput(this);
            this->m_input->setMaximumWidth(150);
            this->m_marginMenuAction = new QWidgetAction(this);
            this->m_marginMenuAction->setDefaultWidget(this->m_input);

            this->setContextMenuPolicy(Qt::CustomContextMenu);
            connect(this->m_input, &CMarginsInput::changedMargins, this, &CNavigatorDialog::ps_menuChangeMargins);
            connect(this, &CNavigatorDialog::customContextMenuRequested, this, &CNavigatorDialog::ps_showContextMenu);
            connect(sGui, &CGuiApplication::styleSheetsChanged, this, &CNavigatorDialog::ps_onStyleSheetsChanged, Qt::QueuedConnection);
            this->ps_onStyleSheetsChanged();
        }

        CNavigatorDialog::~CNavigatorDialog()
        { }

        void CNavigatorDialog::buildNavigator(int columns)
        {
            if (this->m_firstBuild)
            {
                this->m_firstBuild = false;
                this->insertOwnActions();
            }

            this->ps_onStyleSheetsChanged();

            // remove old layout
            CGuiUtility::deleteLayout(ui->fr_NavigatorDialogInner->layout(), false);

            // new layout
            QGridLayout *gridLayout = new QGridLayout(ui->fr_NavigatorDialogInner);
            gridLayout->setObjectName("gl_CNavigatorDialog");
            gridLayout->setSpacing(0);
            gridLayout->setMargin(0);
            gridLayout->setContentsMargins(0, 0, 0, 0);
            ui->fr_NavigatorDialogInner->setLayout(gridLayout);
            int r = 0;
            int c = 0;

            for (const auto &action : this->actions())
            {
                QToolButton *tb = new QToolButton(ui->fr_NavigatorDialogInner);
                tb->setDefaultAction(action);
                tb->setObjectName(this->objectName().append(":").append(action->objectName()));

                gridLayout->addWidget(tb, r, c++);
                tb->show();
                if (c < columns) { continue; }
                c = 0;
                r++;
            }
            this->m_currentColumns = gridLayout->columnCount();
            this->adjustNavigatorSize(gridLayout);
        }

        void CNavigatorDialog::reject()
        {
            this->hide();
            emit navigatorClosed();
        }

        void CNavigatorDialog::toggleFrameless()
        {
            this->setFrameless(!this->isFrameless());
        }

        void CNavigatorDialog::toggleNavigator()
        {
            this->setVisible(!this->isVisible());
        }

        void CNavigatorDialog::restoreFromSettings()
        {
            const CNavigatorSettings s = this->m_settings.get();
            this->setContentsMargins(s.getMargins());
            if (this->isFrameless() != s.isFramless()) { this->toggleFrameless(); }
            this->buildNavigator(s.getColumns());
            const QByteArray geo(s.getGeometry());
            this->restoreGeometry(geo);
        }

        void CNavigatorDialog::saveToSettings()
        {
            CNavigatorSettings s = this->m_settings.get();
            s.setFrameless(this->isFrameless());
            s.setMargins(this->contentsMargins());
            s.setGeometry(this->saveGeometry());
            s.setColumns(this->m_currentColumns);
            const CStatusMessage m = this->m_settings.setAndSave(s);
            if (!m.isSuccess()) { CLogMessage::preformatted(m); }
        }

        void CNavigatorDialog::ps_onStyleSheetsChanged()
        {
            const QString fn(CStyleSheetUtility::fileNameNavigator());
            const QString qss(sGui->getStyleSheetUtility().style(fn));
            this->setStyleSheet(qss);
            this->adjustNavigatorSize();
            this->repaint();
        }

        void CNavigatorDialog::mouseMoveEvent(QMouseEvent *event)
        {
            if (!handleMouseMoveEvent(event)) { QDialog::mouseMoveEvent(event); } ;
        }

        void CNavigatorDialog::mousePressEvent(QMouseEvent *event)
        {
            if (!handleMousePressEvent(event)) { QDialog::mousePressEvent(event); }
        }

        void CNavigatorDialog::changeEvent(QEvent *evt)
        {
            QEvent::Type t = evt->type();
            if (t == QEvent::WindowStateChange)
            {
                evt->ignore();
                hide();
            }
            else
            {
                QDialog::changeEvent(evt);
            }
        }

        void CNavigatorDialog::windowFlagsChanged()
        {
            if (this->m_firstBuild) { return; }
            this->buildNavigator(this->m_currentColumns);
        }

        void CNavigatorDialog::paintEvent(QPaintEvent *event)
        {
            bool s = CStyleSheetUtility::useStyleSheetInDerivedWidget(this, QStyle::PE_Widget);
            if (s) { return; }
            QDialog::paintEvent(event);
        }

        void CNavigatorDialog::ps_showContextMenu(const QPoint &pos)
        {
            QPoint globalPos = this->mapToGlobal(pos);
            QScopedPointer<QMenu> contextMenu(new QMenu(this));
            this->addToContextMenu(contextMenu.data());
            QAction *selectedItem = contextMenu.data()->exec(globalPos);
            Q_UNUSED(selectedItem);
        }

        void CNavigatorDialog::ps_changeLayout()
        {
            QAction *a = qobject_cast<QAction *>(QObject::sender());
            if (!a) { return; }
            QString v(a->data().toString());
            if (v == "1c") { buildNavigator(1);}
            else if (v == "2c") { buildNavigator(2);}
            else if (v == "1r") { buildNavigator(columnsForRows(1));}
            else if (v == "2r") { buildNavigator(columnsForRows(2));}
        }

        void CNavigatorDialog::ps_menuChangeMargins(const QMargins &margins)
        {
            this->setContentsMargins(margins);
            this->adjustNavigatorSize();
        }

        void CNavigatorDialog::ps_dummy()
        {
            // void
        }

        void CNavigatorDialog::ps_settingsChanged()
        {
            // void
        }

        void CNavigatorDialog::insertOwnActions()
        {
            // add some space for frameless navigators where I can move the navigator
            QAction *a = new QAction(BlackMisc::CIcons::empty16(), "move navigator here", this);
            if (this->actions().isEmpty())
            {
                this->addAction(a);
            }
            else
            {
                this->insertAction(this->actions().first(), a);
            }

            a = new QAction(BlackMisc::CIcons::swiftLauncher16(), "Start launcher", this);
            bool c = connect(a, &QAction::triggered, sGui, &CGuiApplication::startLauncher);
            Q_ASSERT(c);
            this->addAction(a);

            // save
            a = new QAction(CIcons::save16(), "Save state", this);
            c = connect(a, &QAction::triggered, this, &CNavigatorDialog::saveToSettings);
            Q_ASSERT(c);
            this->addAction(a);

            // close
            const QIcon i(CIcons::changeIconBackgroundColor(this->style()->standardIcon(QStyle::SP_TitleBarCloseButton), Qt::white, QSize(16, 16)));
            a = new QAction(i, "Close", this);
            c = connect(a, &QAction::triggered, this, &CNavigatorDialog::close);
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

        QGridLayout *CNavigatorDialog::myGridLayout() const
        {
            return qobject_cast<QGridLayout *>(this->layout());
        }

        void CNavigatorDialog::adjustNavigatorSize(QGridLayout *layout)
        {
            QGridLayout *gridLayout = layout ? layout : this->myGridLayout();
            Q_ASSERT_X(gridLayout, Q_FUNC_INFO, "Missing layout");

            int w = 16 * gridLayout->columnCount();
            int h = 16 * gridLayout->rowCount();

            // margins
            QMargins margins = gridLayout->contentsMargins() + this->contentsMargins();
            h = h + margins.top() + margins.bottom();
            w = w + margins.left() + margins.right();

            // adjust
            const QSize min(w + 2, h + 2);
            ui->fr_NavigatorDialogInner->setMinimumSize(min);
            this->setMinimumSize(min);
            this->adjustSize();
        }

        void CNavigatorDialog::addToContextMenu(QMenu *contextMenu) const
        {
            QAction *a = contextMenu->addAction(CIcons::resize16(), "1 row", this, &CNavigatorDialog::ps_changeLayout);
            a->setData("1r");
            a = contextMenu->addAction(CIcons::resize16(), "2 rows", this, &CNavigatorDialog::ps_changeLayout);
            a->setData("2r");
            a = contextMenu->addAction(CIcons::resize16(), "1 column", this, &CNavigatorDialog::ps_changeLayout);
            a->setData("1c");
            a = contextMenu->addAction(CIcons::resize16(), "2 columns", this, &CNavigatorDialog::ps_changeLayout);
            a->setData("2c");
            const QString frameLessActionText = this->isFrameless() ? "Normal window" : "Frameless";
            contextMenu->addAction(BlackMisc::CIcons::tableSheet16(), frameLessActionText, this, SLOT(toggleFrameless()));
            contextMenu->addAction("Adjust margins", this, &CNavigatorDialog::ps_dummy);
            contextMenu->addAction(this->m_marginMenuAction);
            contextMenu->addSeparator();
            contextMenu->addAction(CIcons::load16(), "Restore state", this, &CNavigatorDialog::restoreFromSettings);
            contextMenu->addAction(CIcons::save16(), "Save state", this, &CNavigatorDialog::saveToSettings);
        }
    } // ns
} // ns
