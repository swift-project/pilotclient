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
    CDockWidget::CDockWidget(bool allowStatusBar, QWidget *parent) :
        QDockWidget(parent),
        CEnableForFramelessWindow(CEnableForFramelessWindow::WindowNormal, false, this),
        m_allowStatusBar(allowStatusBar)
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

    CDockWidget::CDockWidget(QWidget *parent): CDockWidget(true, parent)
    { }

    void CDockWidget::setOriginalTitleBar()
    {
        if (!this->m_titleBarWidgetOriginal) { this->initTitleBarWidgets(); }
        if (this->titleBarWidget() == this->m_titleBarWidgetOriginal) return; // on purpose, as I do not know what happens when I call setTitleBar
        this->setTitleBarWidget(this->m_titleBarWidgetOriginal);
    }

    void CDockWidget::setEmptyTitleBar()
    {
        if (!this->m_titleBarWidgetOriginal) { this->initTitleBarWidgets(); }
        if (this->titleBarWidget() == this->m_titleBarWidgetEmpty) { return; } // on purpose, as I do not know what happens when I call setTitleBar
        this->setTitleBarWidget(this->m_titleBarWidgetEmpty);
    }

    void CDockWidget::setNullTitleBarWidget()
    {
        this->setTitleBarWidget(nullptr);
    }

    void CDockWidget::setMarginsWhenFloating(const QMargins &margins)
    {
        this->m_marginsWhenFloating = margins;
    }

    void CDockWidget::setMarginsWhenFramelessFloating(const QMargins &margins)
    {
        this->m_marginsWhenFramelessFloating = margins;
    }

    void CDockWidget::setMarginsWhenFloating(int left, int top, int right, int bottom)
    {
        this->m_marginsWhenFloating = QMargins(left, top, right, bottom);
    }

    void CDockWidget::setMarginsWhenFramelessFloating(int left, int top, int right, int bottom)
    {
        this->m_marginsWhenFramelessFloating = QMargins(left, top, right, bottom);
    }

    void CDockWidget::setMarginsWhenDocked(const QMargins &margins)
    {
        this->m_marginsWhenDocked = margins;
    }

    void CDockWidget::setMarginsWhenDocked(int left, int top, int right, int bottom)
    {
        this->m_marginsWhenDocked = QMargins(left, top, right, bottom);
    }

    bool CDockWidget::isWidgetVisible() const
    {
        return this->m_dockWidgetVisible && this->isVisible();
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

    void CDockWidget::resetWasAlreadyFloating()
    {
        this->m_wasAlreadyFloating = false;
        this->m_resetedFloating = true;
    }

    void CDockWidget::setPreferredSizeWhenFloating(const QSize &size)
    {
        this->m_preferredSizeWhenFloating = size;
    }

    void CDockWidget::setFrameless(bool frameless)
    {
        CEnableForFramelessWindow::setFrameless(frameless);

        // grip
        bool hasStatusBar = this->m_statusBar.getStatusBar();
        if (frameless)
        {
            if (hasStatusBar)
            {
                this->addFramelessSizeGripToStatusBar(this->m_statusBar.getStatusBar());
            }
        }
        else
        {
            if (hasStatusBar)
            {
                this->hideFramelessSizeGripInStatusBar();
            }
        }

        // margins
        if (this->isFloating())
        {
            this->setContentsMargins(frameless ? this->m_marginsWhenFramelessFloating : this->m_marginsWhenFloating);
        }

        // resize
        if (frameless)
        {
            QWidget *innerWidget = this->widget(); // the inner widget containing the layout
            Q_ASSERT(innerWidget);
            this->resize(innerWidget->size());
        }

        //! \todo CDockWidget, check if style sheet reload is needed
        this->ps_onStyleSheetsChanged(); // force style sheet reload
    }

    void CDockWidget::toggleFloating()
    {
        bool floating = !this->isFloating();
        if (!floating)
        {
            this->setFrameless(false);
        }
        this->setFloating(floating);
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

    void CDockWidget::toggleFrameless()
    {
        if (this->isFrameless())
        {
            this->setFrameless(false);
        }
        else
        {
            this->setFrameless(true);
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
        CStyleSheetUtility::useStyleSheetInDerivedWidget(this, QStyle::PE_FrameDockWidget);
        QDockWidget::paintEvent(event);
    }

    void CDockWidget::mouseMoveEvent(QMouseEvent *event)
    {
        if (!handleMouseMoveEvent(event)) { QDockWidget::mouseMoveEvent(event); } ;
    }

    void CDockWidget::mousePressEvent(QMouseEvent *event)
    {
        if (!handleMousePressEvent(event)) { QDockWidget::mousePressEvent(event); }
    }

    void CDockWidget::addToContextMenu(QMenu *contextMenu) const
    {
        if (this->isFloating())
        {
            contextMenu->addAction(BlackMisc::CIcons::dockTop16(), "Dock", this, SLOT(toggleFloating()));
            if (this->isFrameless())
            {
                contextMenu->addAction(BlackMisc::CIcons::tableSheet16(), "Normal window", this, SLOT(toggleFrameless()));
            }
            else
            {
                contextMenu->addAction(BlackMisc::CIcons::tableSheet16(), "Frameless", this, SLOT(toggleFrameless()));
            }
            contextMenu->addAction(BlackMisc::CIcons::refresh16(), "Redraw", this, SLOT(update()));
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

    bool CDockWidget::setMarginsFromSettings(const QString &section)
    {
        QString sectionUsed(section.isEmpty() ? this->objectName() : section);
        if (sectionUsed.isEmpty()) { return false; }
        const QSettings *settings = CStyleSheetUtility::instance().iniFile();
        if (!settings) { return false; }

        // checked if value exists as there is no way to check if key/section exist
        if (settings->value(sectionUsed + "/margindocked.left").toString().isEmpty())
        {
            // no values considered as no section, now we check if an alias exists
            sectionUsed = settings->value("alias/" + sectionUsed).toString();
            if (sectionUsed.isEmpty()) { return false; }
            if (settings->value(sectionUsed + "/margindocked.left").toString().isEmpty()) { return false; }
        }

        if (settings)
        {
            this->setMarginsWhenDocked(
                settings->value(sectionUsed + "/margindocked.left", 1).toInt(),
                settings->value(sectionUsed + "/margindocked.top", 1).toInt(),
                settings->value(sectionUsed + "/margindocked.right", 1).toInt(),
                settings->value(sectionUsed + "/margindocked.bottom", 1).toInt());
            this->setMarginsWhenFloating(
                settings->value(sectionUsed + "/marginfloating.left", 10).toInt(),
                settings->value(sectionUsed + "/marginfloating.top", 10).toInt(),
                settings->value(sectionUsed + "/marginfloating.right", 10).toInt(),
                settings->value(sectionUsed + "/marginfloating.bottom", 10).toInt());
            this->setMarginsWhenFramelessFloating(
                settings->value(sectionUsed + "/marginfloating.frameless.left", 5).toInt(),
                settings->value(sectionUsed + "/marginfloating.frameless.top", 5).toInt(),
                settings->value(sectionUsed + "/marginfloating.frameless.right", 5).toInt(),
                settings->value(sectionUsed + "/marginfloating.frameless.bottom", 5).toInt());
        }
        return true;
    }

    void CDockWidget::ps_onTopLevelChanged(bool topLevel)
    {
        if (topLevel)
        {
            if (this->m_windowTitleBackup != QDockWidget::windowTitle())
            {
                QDockWidget::setWindowTitle(this->m_windowTitleBackup);
            }
            this->setNullTitleBarWidget();
            if (!this->m_wasAlreadyFloating) { this->initialFloating(); }

            this->setContentsMargins(
                this->isFrameless() ?
                this->m_marginsWhenFramelessFloating :
                this->m_marginsWhenFloating
            );
            this->m_statusBar.show();
            this->m_wasAlreadyFloating = true;
        }
        else
        {
            // frameless
            this->setFrameless(false);

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

        // relay
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

        QWidget *innerWidget = this->widget(); // the inner widget containing the layout
        Q_ASSERT(innerWidget);
        if (!innerWidget) { return; }
        QVBoxLayout *vLayout = qobject_cast<QVBoxLayout *>(innerWidget->layout());
        Q_ASSERT(vLayout);
        if (!vLayout) { return; }
        vLayout->addWidget(this->m_statusBar.getStatusBar(), 0, Qt::AlignBottom);

        // adjust stretching of the original widget. It was the only widget so far
        // and should occupy maximum space
        QWidget *compWidget = innerWidget->findChild<QWidget *>(QString(), Qt::FindDirectChildrenOnly);
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
        this->update();
    }
} // namespace
