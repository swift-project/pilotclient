/* Copyright (C) 2013
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "navigatordockwidget.h"
#include "blackgui/infoarea.h"
#include "blackgui/guiutility.h"
#include "blackgui/stylesheetutility.h"
#include "ui_navigatordockwidget.h"

#include <QToolButton>
#include <QGridLayout>
#include <QAction>

using namespace BlackGui;
using namespace BlackMisc;

namespace BlackGui
{
    namespace Components
    {
        CNavigatorDockWidget::CNavigatorDockWidget(QWidget *parent) :
            CDockWidgetInfoArea(parent),
            ui(new Ui::CNavigatorDockWidget)
        {
            this->allowStatusBar(false);
            ui->setupUi(this);
            this->ps_onStyleSheetsChanged();
        }

        CNavigatorDockWidget::~CNavigatorDockWidget()
        { }

        void CNavigatorDockWidget::addAction(QAction *action)
        {
            if (action)
            {
                this->m_actions.append(action);
                QToolButton *tb = new QToolButton(this->ui->fr_NavigatorDockWidgetInner);
                tb->setDefaultAction(action);
                tb->setObjectName(this->objectName().append(":").append(action->objectName()));
                this->m_widgets.append(tb);
            }
        }

        void CNavigatorDockWidget::addActions(QList<QAction *> actions)
        {
            if (actions.isEmpty()) { return; }
            for (QAction *a : actions)
            {
                this->addAction(a);
            }
        }

        void CNavigatorDockWidget::buildNavigator(int columns)
        {
            if (m_firstBuild)
            {
                m_firstBuild = false;
                this->insertOwnActions();
            }

            // remove old layout
            CGuiUtility::deleteLayout(this->ui->fr_NavigatorDockWidgetInner->layout(), false);

            // new layout
            QGridLayout *gridLayout = new QGridLayout(this->ui->fr_NavigatorDockWidgetInner);
            gridLayout->setObjectName("gl_CNavigatorDockWidget");
            gridLayout->setSpacing(0);
            gridLayout->setMargin(0);
            gridLayout->setContentsMargins(0, 0, 0, 0);

            this->ui->fr_NavigatorDockWidgetInner->setLayout(gridLayout);
            int r = 0;
            int c = 0;
            for (int i = 0; i < this->m_widgets.size(); i++)
            {
                gridLayout->addWidget(this->m_widgets[i], r, c++);
                this->m_widgets[i]->show();
                if (c < columns) { continue; }
                c = 0;
                r++;
            }

            // set the real values
            c = gridLayout->columnCount();
            r = gridLayout->rowCount();
            QSize ws = this->setMinimumSizeForWidgets(r, c);
            this->resize(ws);

            // see documentation, required as layout was changed
            // this requires setting widget again
            this->ui->fr_NavigatorDockWidgetInner->show();
            this->setWidget(this->ui->fr_NavigatorDockWidgetInner);
        }

        void CNavigatorDockWidget::addToContextMenu(QMenu *contextMenu) const
        {
            QAction *a;
            a = contextMenu->addAction(CIcons::resize16(), "1 row", this, SLOT(ps_changeLayout()));
            a->setData("1r");
            a = contextMenu->addAction(CIcons::resize16(), "2 rows", this, SLOT(ps_changeLayout()));
            a->setData("2r");
            a = contextMenu->addAction(CIcons::resize16(), "1 column", this, SLOT(ps_changeLayout()));
            a->setData("1c");
            a = contextMenu->addAction(CIcons::resize16(), "2 columns", this, SLOT(ps_changeLayout()));
            a->setData("2c");
            contextMenu->addSeparator();
            CDockWidgetInfoArea::addToContextMenu(contextMenu);
        }

        void CNavigatorDockWidget::ps_onStyleSheetsChanged()
        {
            const QString fn(CStyleSheetUtility::fileNameNavigator());
            const QString qss(CStyleSheetUtility::instance().style(fn));
            this->setStyleSheet(qss);
        }

        void CNavigatorDockWidget::ps_changeLayout()
        {
            QAction *a = qobject_cast<QAction *>(QObject::sender());
            if (!a) { return; }
            QString v(a->data().toString());
            if (v == "1c") { buildNavigator(1);}
            else if (v == "2c") { buildNavigator(2);}
            else if (v == "1r") { buildNavigator(columnsForRows(1));}
            else if (v == "2r") { buildNavigator(columnsForRows(2));}
        }

        void CNavigatorDockWidget::insertOwnActions()
        {
            QIcon i(CIcons::changeIconBackgroundColor(this->style()->standardIcon(QStyle::SP_TitleBarCloseButton), Qt::white, QSize(16, 16)));
            QAction *a = new QAction(i, "Close", this);
            connect(a, &QAction::triggered, this, &CNavigatorDockWidget::close);
            this->addAction(a);
        }

        int CNavigatorDockWidget::columnsForRows(int rows)
        {
            Q_ASSERT_X(rows >= 0, Q_FUNC_INFO, "no rows");
            int items = this->m_widgets.size();
            int c = items / rows;
            return (c * rows) < items ? c + 1 : c;
        }

        QSize CNavigatorDockWidget::setMinimumSizeForWidgets(int rows, int columns)
        {
            int w = 20 * columns;
            int h = 20 * rows;
            QSize min(w, h);
            this->ui->qw_NavigatorDockWidgetOuter->setMinimumSize(min);
            this->ui->fr_NavigatorDockWidgetInner->setMinimumSize(min);
            this->setMinimumSize(min);
            return min;
        }

    } // ns
} // ns
