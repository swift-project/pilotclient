/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "navigatordialog.h"
#include "ui_navigatordialog.h"
#include "blackgui/infoarea.h"
#include "blackgui/guiutility.h"
#include "blackgui/stylesheetutility.h"

#include <QToolButton>
#include <QGridLayout>
#include <QAction>

using namespace BlackGui;
using namespace BlackMisc;

namespace BlackGui
{
    namespace Components
    {

        CNavigatorDialog::CNavigatorDialog(QWidget *parent) :
            QDialog(parent, Qt::Tool),
            CEnableForFramelessWindow(CEnableForFramelessWindow::WindowTool, false, "NavigatorDialog", this),
            ui(new Ui::CNavigatorDialog)
        {
            ui->setupUi(this);

            // context menu
            this->setContextMenuPolicy(Qt::CustomContextMenu);
            connect(this, &CNavigatorDialog::customContextMenuRequested, this, &CNavigatorDialog::ps_showContextMenu);

            connect(&CStyleSheetUtility::instance(), &CStyleSheetUtility::styleSheetsChanged, this, &CNavigatorDialog::ps_onStyleSheetsChanged);
            this->ps_onStyleSheetsChanged();
        }

        CNavigatorDialog::~CNavigatorDialog()
        { }

        void CNavigatorDialog::buildNavigator(int columns)
        {
            if (m_firstBuild)
            {
                m_firstBuild = false;
                this->insertOwnActions();
            }

            // remove old layout
            CGuiUtility::deleteLayout(this->ui->fr_NavigatorDialogInner->layout(), false);

            // new layout
            QGridLayout *gridLayout = new QGridLayout(this->ui->fr_NavigatorDialogInner);
            gridLayout->setObjectName("gl_CNavigatorDialog");
            gridLayout->setSpacing(0);
            gridLayout->setMargin(0);
            gridLayout->setContentsMargins(0, 0, 0, 0);

            this->ui->fr_NavigatorDialogInner->setLayout(gridLayout);
            int r = 0;
            int c = 0;
            for (const auto &action : actions())
            {
                QToolButton *tb = new QToolButton(this->ui->fr_NavigatorDialogInner);
                tb->setDefaultAction(action);
                tb->setObjectName(this->objectName().append(":").append(action->objectName()));

                gridLayout->addWidget(tb, r, c++);
                tb->show();
                if (c < columns) { continue; }
                c = 0;
                r++;
            }

            int w = 16 * gridLayout->columnCount();
            int h = 16 * gridLayout->rowCount();
            QSize min(w, h);
            this->ui->fr_NavigatorDialogInner->setMinimumSize(min);
            this->setMinimumSize(min);
            this->adjustSize();
        }

        void CNavigatorDialog::toggleFrameless()
        {
            this->setFrameless(!this->isFrameless());
        }

        void CNavigatorDialog::ps_onStyleSheetsChanged()
        {
            const QString fn(CStyleSheetUtility::fileNameNavigator());
            const QString qss(CStyleSheetUtility::instance().style(fn));
            this->setStyleSheet(qss);
        }

        void CNavigatorDialog::mouseMoveEvent(QMouseEvent *event)
        {
            if (!handleMouseMoveEvent(event)) { QDialog::mouseMoveEvent(event); } ;
        }

        void CNavigatorDialog::mousePressEvent(QMouseEvent *event)
        {
            if (!handleMousePressEvent(event)) { QDialog::mousePressEvent(event); }
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

        void CNavigatorDialog::insertOwnActions()
        {
            QIcon i(CIcons::changeIconBackgroundColor(this->style()->standardIcon(QStyle::SP_TitleBarCloseButton), Qt::white, QSize(16, 16)));
            QAction *a = new QAction(i, "Close", this);
            connect(a, &QAction::triggered, this, &CNavigatorDialog::close);
            this->addAction(a);
        }

        int CNavigatorDialog::columnsForRows(int rows)
        {
            Q_ASSERT_X(rows >= 0, Q_FUNC_INFO, "no rows");
            int items = this->actions().size();
            int c = items / rows;
            return (c * rows) < items ? c + 1 : c;
        }

        void CNavigatorDialog::addToContextMenu(QMenu *contextMenu) const
        {
            QAction *a;
            a = contextMenu->addAction(CIcons::resize16(), "1 row", this, &CNavigatorDialog::ps_changeLayout);
            a->setData("1r");
            a = contextMenu->addAction(CIcons::resize16(), "2 rows", this, &CNavigatorDialog::ps_changeLayout);
            a->setData("2r");
            a = contextMenu->addAction(CIcons::resize16(), "1 column", this, &CNavigatorDialog::ps_changeLayout);
            a->setData("1c");
            a = contextMenu->addAction(CIcons::resize16(), "2 columns", this, &CNavigatorDialog::ps_changeLayout);
            a->setData("2c");

            contextMenu->addSeparator();

            QString frameLessActionText = this->isFrameless() ? "Normal window" : "Frameless";
            contextMenu->addAction(BlackMisc::CIcons::tableSheet16(), frameLessActionText, this, &CNavigatorDialog::toggleFrameless);
        }

    } // ns
} // ns
