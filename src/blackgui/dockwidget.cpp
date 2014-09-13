/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "dockwidget.h"
#include "blackmisc/icons.h"
#include "blackgui/stylesheetutility.h"
#include "blackgui/guiutility.h"
#include <QCloseEvent>
#include <QStyleOption>
#include <QPainter>

namespace BlackGui
{
    CDockWidget::CDockWidget(QWidget *parent) : QDockWidget(parent)
    {
        this->onStyleSheetsChanged();
        this->initTitleBarWidgets();

        // connect
        connect(this, &QDockWidget::topLevelChanged, this, &CDockWidget::ps_onTopLevelChanged);
        connect(&CStyleSheetUtility::instance(), &CStyleSheetUtility::styleSheetsChanged, this, &CDockWidget::onStyleSheetsChanged);

        // context menu
        this->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(this, &CDockWidget::customContextMenuRequested, this, &CDockWidget::ps_showContextMenu);
    }

    void CDockWidget::setOriginalTitleBar()
    {
        if (!this->m_titleBarOriginal) { this->initTitleBarWidgets(); }
        if (this->titleBarWidget() == this->m_titleBarOriginal) return; // on purpose, as I do not know what happens when I call setTitleBar
        this->setTitleBarWidget(this->m_titleBarOriginal);
    }

    void CDockWidget::setEmptyTitleBar()
    {
        if (!this->m_titleBarOriginal) { this->initTitleBarWidgets(); }
        if (this->titleBarWidget() == this->m_emptyTitleBar) return; // on purpose, as I do not know what happens when I call setTitleBar
        this->setTitleBarWidget(this->m_emptyTitleBar);
    }

    void CDockWidget::setNullTitleBar()
    {
        this->setTitleBarWidget(nullptr);
    }

    void CDockWidget::setWindowTitle(const QString &title)
    {
        this->m_windowTitleBackup = title;
        QDockWidget::setWindowTitle(title);
    }

    void CDockWidget::showTitleWhenDocked(bool show)
    {
        this->m_windowTitleWhenDocked = show;
        if (show)
        {
            QDockWidget::setWindowTitle(this->m_windowTitleBackup);
        }
        else
        {
            QDockWidget::setWindowTitle("");
        }
    }

    void CDockWidget::toggleFloating()
    {
        this->setFloating(!this->isFloating());
    }

    void CDockWidget::closeEvent(QCloseEvent *event)
    {
        if (this->isFloating())
        {
            this->toggleFloating();
            event->setAccepted(false); // refuse -> do not close
        }
        else
        {
            QDockWidget::closeEvent(event);
        }
    }

    void CDockWidget::paintEvent(QPaintEvent *event)
    {
        // KB: Should give me style sheet compliance, however I did not notice any difference
        // included for style sheet compliance
        // QStyleOption opt;
        // opt.init(this);
        // QPainter p(this);
        // style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
        QDockWidget::paintEvent(event);
    }

    void CDockWidget::hideEvent(QHideEvent *event)
    {
        qDebug() << "hide" << this->objectName() << "v:" << isVisible() << "h:" << isHidden();
        QDockWidget::hideEvent(event);
    }

    void CDockWidget::showEvent(QShowEvent *event)
    {
        qDebug() << "show" << this->objectName() << "v:" << isVisible() << "h:" << isHidden();
        QDockWidget::showEvent(event);
    }

    void CDockWidget::addToContextMenu(QMenu *contextMenu) const
    {
        if (this->isFloating())
        {
            contextMenu->addAction(BlackMisc::CIcons::dockTop16(), "Dock", this, SLOT(toggleFloating()));
        }
        else
        {
            contextMenu->addAction(BlackMisc::CIcons::floatOne16(), "Float", this, SLOT(toggleFloating()));
        }
    }

    void CDockWidget::initalFloating()
    {
        // for the first time resize
        if (!this->m_preferredSizeWhenFloating.isNull())
        {
            this->resize(this->m_preferredSizeWhenFloating);
        }

        // and move
        QPoint mainWindowPos = BlackGui::CGuiUtility::mainWindowPosition();
        if (!mainWindowPos.isNull())
        {
            int x = mainWindowPos.x() + this->m_offsetWhenFloating.x();
            int y = mainWindowPos.y() + this->m_offsetWhenFloating.y();
            this->move(x, y);
        }
    }

    void CDockWidget::ps_onTopLevelChanged(bool topLevel)
    {
        if (topLevel)
        {
            if (this->m_windowTitleBackup != QDockWidget::windowTitle())
            {
                QDockWidget::setWindowTitle(this->m_windowTitleBackup);
            }
            this->setNullTitleBar();
            this->setContentsMargins(this->m_marginsWhenFloating);
            if (!this->m_wasAlreadyFloating)
            {
                this->initalFloating();
            }
            this->m_wasAlreadyFloating = true;
        }
        else
        {
            if (!this->m_windowTitleWhenDocked)
            {
                QDockWidget::setWindowTitle("");
            }

            this->setEmptyTitleBar();
            this->setContentsMargins(this->m_marginsWhenDocked);
        }
        emit this->widgetTopLevelChanged(this, topLevel);
    }

    void CDockWidget::initTitleBarWidgets()
    {
        this->m_titleBarOriginal = this->titleBarWidget();
        this->m_emptyTitleBar = new QWidget(this);
        this->setTitleBarWidget(this->m_emptyTitleBar);
    }

    QList<QWidget *> CDockWidget::findEmbeddedRuntimeComponents() const
    {
        QList<QWidget *> widgets = this->findChildren<QWidget *>();
        QList<QWidget *> widgetsWithRuntimeComponent;
        foreach(QWidget * w, widgets)
        {
            if (dynamic_cast<Components::CRuntimeBasedComponent *>(w))
            {
                widgetsWithRuntimeComponent.append(w);
            }
        }
        return widgetsWithRuntimeComponent;
    }

    void CDockWidget::ps_showContextMenu(const QPoint &pos)
    {
        QPoint globalPos = this->mapToGlobal(pos);
        QScopedPointer<QMenu> contextMenu(new QMenu(this));
        this->addToContextMenu(contextMenu.data());
        QAction *selectedItem = contextMenu.data()->exec(globalPos);
        Q_UNUSED(selectedItem);
    }

    void CDockWidget::onStyleSheetsChanged()
    {
        // void
    }
}
