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
#include <QLayout>

namespace BlackGui
{
    CDockWidget::CDockWidget(QWidget *parent) : QDockWidget(parent)
    {

        this->ps_onStyleSheetsChanged();
        this->initTitleBarWidgets();

        // context menu
        this->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(this, &CDockWidget::customContextMenuRequested, this, &CDockWidget::ps_showContextMenu);

        // connect
        connect(this, &QDockWidget::topLevelChanged, this, &CDockWidget::ps_onTopLevelChanged);
        connect(&CStyleSheetUtility::instance(), &CStyleSheetUtility::styleSheetsChanged, this, &CDockWidget::ps_onStyleSheetsChanged);
        connect(this, &QDockWidget::visibilityChanged, this, &CDockWidget::ps_onVisibilityChanged);

    }

    void CDockWidget::setOriginalTitleBar()
    {
        if (!this->m_titleBarWidgetOriginal) { this->initTitleBarWidgets(); }
        if (this->titleBarWidget() == this->m_titleBarWidgetOriginal) return; // on purpose, as I do not know what happens when I call setTitleBar
        this->setTitleBarWidget(this->m_titleBarWidgetOriginal);
    }

    void CDockWidget::setEmptyTitleBar()
    {
        if (!this->m_titleBarWidgetOriginal) { this->initTitleBarWidgets(); }
        if (this->titleBarWidget() == this->m_titleBarWidgetEmpty) return; // on purpose, as I do not know what happens when I call setTitleBar
        this->setTitleBarWidget(this->m_titleBarWidgetEmpty);
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

    void CDockWidget::displayStatusMessage(const BlackMisc::CStatusMessage &statusMessage)
    {
        if (!this->isFloating()) { return; }
        this->m_statusBar.displayStatusMessage(statusMessage);
    }

    void CDockWidget::displayStatusMessages(const BlackMisc::CStatusMessageList &statusMessages)
    {
        if (!this->isFloating()) { return; }
        this->m_statusBar.displayStatusMessages(statusMessages);
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

    void CDockWidget::toggleVisibility()
    {
        if (this->isVisible())
        {
            this->hide();
        }
        else
        {
            this->show();
        }
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
        Q_UNUSED(event);
        CStyleSheetUtility::useStyleSheetInDerivedWidget(this);
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

    void CDockWidget::initialFloating()
    {

        // init status bar, as we have now all structure set
        this->initStatusBar();

        // for the first time resize
        if (!this->m_preferredSizeWhenFloating.isNull())
        {
            this->m_initialDockedMinimumSize = this->minimumSize();
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
            if (!this->m_wasAlreadyFloating) { this->initialFloating(); }
            this->m_statusBar.show();
            this->m_wasAlreadyFloating = true;
        }
        else
        {
            if (!this->m_windowTitleWhenDocked) { QDockWidget::setWindowTitle(""); }
            this->m_statusBar.hide();
            this->setEmptyTitleBar();
            this->setContentsMargins(this->m_marginsWhenDocked);

            // sometimes floating sets a new minimum size, here we reset it
            if (this->minimumHeight() > this->m_initialDockedMinimumSize.height())
            {
                this->setMinimumSize(this->m_initialDockedMinimumSize);
            }
        }
        emit this->widgetTopLevelChanged(this, topLevel);
    }

    void CDockWidget::initTitleBarWidgets()
    {
        this->m_titleBarWidgetOriginal = this->titleBarWidget();
        this->m_titleBarWidgetEmpty = new QWidget(this);
        this->setTitleBarWidget(this->m_titleBarWidgetEmpty);
    }

    void CDockWidget::initStatusBar()
    {
        if (this->m_statusBar.getStatusBar()) { return; }
        if (!this->m_allowStatusBar) { return; }
        this->m_statusBar.initStatusBar();

        QWidget *innerDockWidget = this->widget(); // the inner widget containing the layout
        Q_ASSERT(innerDockWidget);
        if (!innerDockWidget) { return; }
        QVBoxLayout *vLayout = qobject_cast<QVBoxLayout *>(innerDockWidget->layout());
        Q_ASSERT(vLayout);
        if (!vLayout) { return; }
        vLayout->addWidget(this->m_statusBar.getStatusBar(), 0, Qt::AlignBottom);

        // adjust stretching of the original widget. It was the only widget so far
        // and should occupy maximum space
        QWidget *compWidget = innerDockWidget->findChild<QWidget *>(QString(), Qt::FindDirectChildrenOnly);
        Q_ASSERT(compWidget);
        if (!compWidget) { return; }
        QSizePolicy sizePolicy = compWidget->sizePolicy();
        sizePolicy.setVerticalStretch(1);
        compWidget->setSizePolicy(sizePolicy);
    }

    void CDockWidget::ps_showContextMenu(const QPoint &pos)
    {
        QPoint globalPos = this->mapToGlobal(pos);
        QScopedPointer<QMenu> contextMenu(new QMenu(this));
        this->addToContextMenu(contextMenu.data());
        QAction *selectedItem = contextMenu.data()->exec(globalPos);
        Q_UNUSED(selectedItem);
    }

    void CDockWidget::ps_onVisibilityChanged(bool visible)
    {
        this->m_dockWidgetVisible = visible;
    }

    void CDockWidget::ps_onStyleSheetsChanged()
    {
        // void, for further extensions
    }
} // namespace
