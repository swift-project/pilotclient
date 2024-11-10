// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "blackgui/components/marginsinput.h"
#include "blackgui/menus/fontmenus.h"
#include "blackgui/dockwidget.h"
#include "blackgui/guiapplication.h"
#include "blackgui/guiutility.h"
#include "blackgui/stylesheetutility.h"
#include "misc/icons.h"
#include "misc/logmessage.h"
#include "misc/verify.h"

#include <QCloseEvent>
#include <QFrame>
#include <QLayout>
#include <QLayoutItem>
#include <QMenu>
#include <QScopedPointer>
#include <QScreen>
#include <QSettings>
#include <QSizePolicy>
#include <QStatusBar>
#include <QStyle>
#include <QVBoxLayout>
#include <QVariant>
#include <QWidgetAction>
#include <QWidget>
#include <Qt>
#include <QTimer>
#include <QPointer>
#include <QtGlobal>
#include <QApplication>

using namespace swift::misc;
using namespace BlackGui::Components;
using namespace BlackGui::Settings;
using namespace BlackGui::Menus;

namespace BlackGui
{
    CDockWidget::CDockWidget(bool allowStatusBar, QWidget *parent) : COverlayMessagesDockWidget(parent),
                                                                     CEnableForFramelessWindow(CEnableForFramelessWindow::WindowTool, false, "framelessDockWidget", this),
                                                                     m_allowStatusBar(allowStatusBar)
    {
        // init settings
        this->onStyleSheetsChanged();
        this->initTitleBarWidgets();
        m_originalAreas = this->allowedAreas();

        // context menu
        m_input = new CMarginsInput(this);
        m_input->setMaximumWidth(150);
        m_marginMenuAction = new QWidgetAction(this);
        m_marginMenuAction->setDefaultWidget(m_input);
        m_fontMenu = new CFontMenu(this, Qt::WidgetWithChildrenShortcut);

        this->setContextMenuPolicy(Qt::CustomContextMenu);
        connect(this, &CDockWidget::customContextMenuRequested, this, &CDockWidget::showContextMenu, Qt::QueuedConnection);
        connect(m_input, &CMarginsInput::changedMargins, this, &CDockWidget::menuChangeMargins); // only works direct, as QMargins is not registered:  'QMargins' is registered using qRegisterMetaTyp

        // connect
        connect(sGui, &CGuiApplication::styleSheetsChanged, this, &CDockWidget::onStyleSheetsChanged, Qt::QueuedConnection);
        connect(this, &QDockWidget::topLevelChanged, this, &CDockWidget::onTopLevelChanged, Qt::QueuedConnection);
        connect(this, &QDockWidget::visibilityChanged, this, &CDockWidget::onVisibilityChanged, Qt::QueuedConnection);
        connect(m_fontMenu, &CFontMenu::fontSizeMinus, this, &CDockWidget::fontSizeMinus, Qt::QueuedConnection);
        connect(m_fontMenu, &CFontMenu::fontSizePlus, this, &CDockWidget::fontSizePlus, Qt::QueuedConnection);
    }

    void CDockWidget::setMargins()
    {
        if (this->isFloating())
        {
            this->setContentsMargins(this->isFrameless() ? this->getMarginsWhenFramelessFloating() : this->getMarginsWhenFloating());
        }
        else
        {
            this->setContentsMargins(this->getMarginsWhenDocked());
        }
    }

    CDockWidget::CDockWidget(QWidget *parent) : CDockWidget(true, parent)
    {}

    void CDockWidget::setOriginalTitleBar()
    {
        if (!m_titleBarWidgetOriginal) { this->initTitleBarWidgets(); }
        if (this->titleBarWidget() == m_titleBarWidgetOriginal) { return; } // on purpose, as I do not know what happens when I call setTitleBar
        this->setTitleBarWidget(m_titleBarWidgetOriginal);
    }

    void CDockWidget::setEmptyTitleBar()
    {
        if (!m_titleBarWidgetOriginal) { this->initTitleBarWidgets(); }
        if (this->titleBarWidget() == m_titleBarWidgetEmpty) { return; } // on purpose, as I do not know what happens when I call setTitleBar
        this->setTitleBarWidget(m_titleBarWidgetEmpty);
    }

    void CDockWidget::setNullTitleBarWidget()
    {
        this->setTitleBarWidget(nullptr);
    }

    void CDockWidget::setMarginsWhenFloating(const QMargins &margins)
    {
        CDockWidgetSettings s = this->getSettings();
        s.setMarginsWhenFloating(margins);
        this->saveSettings(s);
    }

    void CDockWidget::setMarginsWhenFloating(int left, int top, int right, int bottom)
    {
        this->setMarginsWhenFloating(QMargins(left, top, right, bottom));
    }

    QMargins CDockWidget::getMarginsWhenFloating() const
    {
        return this->getSettings().getMarginsWhenFloating();
    }

    void CDockWidget::setMarginsWhenFramelessFloating(const QMargins &margins)
    {
        CDockWidgetSettings s = this->getSettings();
        s.setMarginsWhenFramelessFloating(margins);
        this->saveSettings(s);
    }

    void CDockWidget::setMarginsWhenFramelessFloating(int left, int top, int right, int bottom)
    {
        this->setMarginsWhenFramelessFloating(QMargins(left, top, right, bottom));
    }

    QMargins CDockWidget::getMarginsWhenFramelessFloating() const
    {
        return this->getSettings().getMarginsWhenFramelessFloating();
    }

    void CDockWidget::setMarginsWhenDocked(const QMargins &margins)
    {
        CDockWidgetSettings s = this->getSettings();
        s.setMarginsWhenDocked(margins);
        this->saveSettings(s);
    }

    void CDockWidget::setMarginsWhenDocked(int left, int top, int right, int bottom)
    {
        this->setMarginsWhenDocked(QMargins(left, top, right, bottom));
    }

    QMargins CDockWidget::getMarginsWhenDocked() const
    {
        return this->getSettings().getMarginsWhenDocked();
    }

    bool CDockWidget::isWidgetVisible() const
    {
        return m_dockWidgetVisible && this->isVisible();
    }

    void CDockWidget::setWindowTitle(const QString &title)
    {
        m_windowTitleBackup = title;
        QDockWidget::setWindowTitle(title);
    }

    void CDockWidget::displayStatusMessage(const CStatusMessage &statusMessage)
    {
        if (!m_allowStatusBar || !this->isFloating()) { return; }
        m_statusBar.displayStatusMessage(statusMessage);
    }

    void CDockWidget::displayStatusMessages(const CStatusMessageList &statusMessages)
    {
        if (!m_allowStatusBar || !this->isFloating()) { return; }
        m_statusBar.displayStatusMessages(statusMessages);
    }

    void CDockWidget::showTitleWhenDocked(bool show)
    {
        m_windowTitleWhenDocked = show;
        if (show)
        {
            QDockWidget::setWindowTitle(m_windowTitleBackup);
        }
        else
        {
            QDockWidget::setWindowTitle("");
        }
    }

    void CDockWidget::resetWasAlreadyFloating()
    {
        m_wasAlreadyFloating = false;
    }

    void CDockWidget::setPreferredSizeWhenFloating(const QSize &size)
    {
        m_preferredSizeWhenFloating = size;
    }

    void CDockWidget::setOffsetWhenFloating(const QPoint &point, bool frameless)
    {
        Q_UNUSED(frameless)
        m_offsetWhenFloating = point;
    }

    void CDockWidget::setFrameless(bool frameless)
    {
        CEnableForFramelessWindow::setFrameless(frameless);

        // grip
        bool hasStatusBar = m_statusBar.getStatusBar();
        if (frameless)
        {
            if (hasStatusBar)
            {
                this->addFramelessSizeGripToStatusBar(m_statusBar.getStatusBar());
            }
        }
        else
        {
            if (hasStatusBar)
            {
                this->hideFramelessSizeGripInStatusBar();
            }
        }

        // resize
        if (frameless)
        {
            QWidget *innerWidget = this->widget(); // the inner widget containing the layout
            Q_ASSERT(innerWidget);
            this->resize(innerWidget->size());
        }

        this->setMargins();
        this->forceStyleSheetUpdate(); // force style sheet reload
    }

    const QString &CDockWidget::propertyOuterWidget()
    {
        static const QString s("outerwidget");
        return s;
    }

    const QString &CDockWidget::propertyInnerWidget()
    {
        static const QString s("innerwidget");
        return s;
    }

    void CDockWidget::toggleFloating()
    {
        const bool changeToFloating = !this->isFloating();
        if (!changeToFloating) { this->setFrameless(false); } // remove frameless if not floating

        // disable the interactive docking
        if (changeToFloating)
        {
            m_originalAreas = this->allowedAreas();
            this->setAllowedAreas(Qt::NoDockWidgetArea);
        }
        else
        {
            this->setAllowedAreas(m_originalAreas);
        }

        this->setAlwaysOnTop(m_alwaysOnTop && changeToFloating);
        this->setFloating(changeToFloating);

        const Qt::KeyboardModifiers km = QGuiApplication::queryKeyboardModifiers();
        const bool shift = km.testFlag(Qt::ShiftModifier);

        // with shift ALWAYS reset
        if (shift && changeToFloating) { this->resetPosition(); }
        if (changeToFloating)
        {
            // check where we are, otherwise reset if NOT appropriate
            const QPoint p = this->rect().topLeft();
            if (p.x() < 1 || p.y() < 1)
            {
                this->resetPosition();
            }
        }
    }

    void CDockWidget::toggleVisibility()
    {
        this->setVisible(!this->isVisible());
    }

    void CDockWidget::toggleFrameless()
    {
        const bool frameless = this->isFrameless();
        this->setFrameless(!frameless);
    }

    /*!
    void CDockWidget::toggleFramelessDeferred(int delayMs)
    {
        QPointer<CDockWidget> myself(this);
        QTimer::singleShot(delayMs, this, [ = ]
        {
            if (myself) { myself->toggleFrameless(); }
        });
    }
    **/

    void CDockWidget::windowAlwaysOnTop()
    {
        if (this->isFloating())
        {
            this->setAlwaysOnTopFlag(true);
            m_alwaysOnTop = true;
        }
    }

    void CDockWidget::windowNotAlwaysOnTop()
    {
        if (this->isFloating())
        {
            this->setAlwaysOnTopFlag(false);
            m_alwaysOnTop = false;
        }
    }

    void CDockWidget::setAlwaysOnTopFlag(bool onTop)
    {
        if (onTop)
        {
            this->setWindowFlags(this->windowFlags() | Qt::WindowStaysOnTopHint);
        }
        else
        {
            this->setWindowFlags(this->windowFlags() & ~Qt::WindowStaysOnTopHint);
        }
    }

    bool CDockWidget::restoreFromSettings()
    {
        const CDockWidgetSettings s = this->getSettings();
        if (s.isFloating() != this->isFloating())
        {
            this->toggleFloating();
            QPointer<CDockWidget> myself(this);
            QTimer::singleShot(500, this, [=] {
                if (myself) { myself->restoreFromSettings(); }
            });
            return true;
        }

        if (s.isFramless() != this->isFrameless())
        {
            this->toggleFrameless();
            QPointer<CDockWidget> myself(this);
            QTimer::singleShot(500, this, [=] {
                if (myself) { myself->restoreFromSettings(); }
            });
            return true;
        }

        // now frameless and floating is correct
        const QByteArray geo(s.getGeometry());
        if (!geo.isEmpty())
        {
            const bool ok = this->restoreGeometry(geo);
            if (ok) { this->rememberFloatingSizeAndPosition(); }
        }

        this->setMargins();
        return true;
    }

    void CDockWidget::closeEvent(QCloseEvent *event)
    {
        if (this->isFloating())
        {
            this->toggleFloating();

            /**
             * since update to Qt 5.14.1 this kills the window
             * https://discordapp.com/channels/539048679160676382/539846348275449887/691667651285483564
             * \todo code can be removed AFTER 8-2020

            QPointer<CDockWidget> myself(this);
            QTimer::singleShot(500, this, [ = ]
            {
                // if (myself) { myself->close(); }
            });
            **/

            event->setAccepted(false); // refuse -> do not close (otherwise crash)
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
        this->rememberFloatingSizeAndPosition();
    }

    void CDockWidget::mouseMoveEvent(QMouseEvent *event)
    {
        if (!handleMouseMoveEvent(event)) { QDockWidget::mouseMoveEvent(event); }
    }

    void CDockWidget::keyPressEvent(QKeyEvent *event)
    {
        if (event->key() == Qt::Key_Escape && this->isFloating()) { this->toggleFloating(); }
        QDockWidget::keyPressEvent(event);
    }

    void CDockWidget::mousePressEvent(QMouseEvent *event)
    {
        if (!handleMousePressEvent(event)) { QDockWidget::mousePressEvent(event); }
    }

    void CDockWidget::addToContextMenu(QMenu *contextMenu) const
    {
        if (this->isFloating())
        {
            const bool frameless = this->isFrameless();

            contextMenu->addAction(CIcons::dockTop16(), "Dock", this, &CDockWidget::toggleFloating);
            contextMenu->addAction(CIcons::tableSheet16(), frameless ? "Normal window" : "Frameless", this, &CDockWidget::toggleFrameless);
            contextMenu->addAction(CIcons::dockTop16(), "Always on top", this, &CDockWidget::windowAlwaysOnTop);
            contextMenu->addAction(CIcons::dockTop16(), "Not on top", this, &CDockWidget::windowNotAlwaysOnTop);
            contextMenu->addAction(CIcons::refresh16(), "Redraw", this, qOverload<>(&CDockWidget::update));
        }
        else
        {
            contextMenu->addAction(CIcons::floatOne16(), "Float", this, &CDockWidget::toggleFloating);
        }

        // Font actions
        Q_ASSERT_X(m_fontMenu, Q_FUNC_INFO, "Missing menu object");
        if (m_fontMenu)
        {
            contextMenu->addActions(m_fontMenu->getActions());
        }

        // State actions (windows state)
        contextMenu->addAction(CIcons::load16(), "Restore from settings", this, &CDockWidget::restoreFromSettings);
        contextMenu->addAction(CIcons::save16(), "Save state", this, &CDockWidget::saveCurrentStateToSettings);
        contextMenu->addAction(CIcons::refresh16(), "Reset to defaults", this, &CDockWidget::resetSettings);
        contextMenu->addAction(CIcons::refresh16(), "Reset position", this, &CDockWidget::resetPosition);

        m_input->setMargins(this->contentsMargins());
        contextMenu->addAction(CIcons::tableSheet16(), "Margins", this, &CDockWidget::dummy);
        contextMenu->addAction(m_marginMenuAction);
    }

    void CDockWidget::initialFloating()
    {
        // init status bar, as we have now all structures set and name is known
        this->initStatusBarAndProperties();

        // for the first time resize
        SWIFT_VERIFY(!m_preferredSizeWhenFloating.isNull());
        if (!m_preferredSizeWhenFloating.isNull())
        {
            m_initialDockedMinimumSize = this->minimumSize();
            this->resize(m_preferredSizeWhenFloating);
        }

        // and move
        this->resetPosition();

        // deferred int from settings
        this->restoreFloatingSizeAndPositionDeferred();
    }

    QString CDockWidget::windowTitleOrBackup() const
    {
        const QString t(windowTitle());
        if (t.isEmpty()) { return this->windowTitleBackup(); }
        return t;
    }

    void CDockWidget::onTopLevelChanged(bool topLevel)
    {
#ifdef Q_OS_LINUX
        // Give XCB platforms enough time to handle window events before adjusting it.
        // close T593 if this is no longer needed
        QThread::msleep(100);
#endif

        this->setMargins(); // from settings or default
        if (topLevel)
        {
            if (m_windowTitleBackup != QDockWidget::windowTitle())
            {
                QDockWidget::setWindowTitle(m_windowTitleBackup);
            }
            this->setNullTitleBarWidget();
            if (m_wasFrameless) { this->setFrameless(true); }

            /**
            if (!m_wasAlreadyFloating)
            {
                this->initialFloating();
                this->rememberFloatingSizeAndPosition();
            }
            else
            {
                this->restoreFloatingSizeAndPositionDeferred(); // after everything was applied move and resize
            }
            **/

            m_statusBar.show();
            m_wasAlreadyFloating = true;
        }
        else
        {
            // frameless
            this->setFrameless(false);

            if (!m_windowTitleWhenDocked) { QDockWidget::setWindowTitle(""); }
            m_statusBar.hide();
            this->setEmptyTitleBar();

            // sometimes floating sets a new minimum size, here we reset it
            if (this->minimumHeight() > m_initialDockedMinimumSize.height())
            {
                this->setMinimumSize(m_initialDockedMinimumSize);
            }
        }

        // relay
        emit this->widgetTopLevelChanged(this, topLevel);
    }

    void CDockWidget::initTitleBarWidgets()
    {
        m_titleBarWidgetOriginal = this->titleBarWidget();
        m_titleBarWidgetEmpty = new QWidget(this);
        this->setTitleBarWidget(m_titleBarWidgetEmpty);
    }

    void CDockWidget::initStatusBarAndProperties()
    {
        if (m_statusBar.getStatusBar()) { return; }

        // Typical reasons for asserts here
        // 1) Check the structure, we expect the following hierarchy:
        //    QDockWidget (CDockWidget/CDockWidgetInfoArea) -> QWidget (outer widget) -> QFrame (inner widget)
        //    Structure used for frameless floating windows
        // 2) Check if the "floating" flag is accidentally set for the dock widget in the GUI builder
        // 3) Is the dock widget promoted BlackGui::CDockWidgetInfoArea?
        QWidget *outerWidget = this->widget(); // the outer widget containing the layout
        Q_ASSERT_X(outerWidget, "CDockWidget::initStatusBar", "No outer widget");
        if (!outerWidget) { return; }
        outerWidget->setProperty("dockwidget", propertyOuterWidget());

        Q_ASSERT_X(outerWidget->layout(), "CDockWidget::initStatusBar", "No outer widget layout");
        if (!outerWidget->layout()) { return; }
        Q_ASSERT_X(outerWidget->layout()->itemAt(0) && outerWidget->layout()->itemAt(0)->widget(), "CDockWidget::initStatusBar", "No outer widget layout item");
        if (!outerWidget->layout()->itemAt(0) || !outerWidget->layout()->itemAt(0)->widget())
        {
            m_allowStatusBar = false;
            return;
        }

        // Inner widget is supposed to be a QFrame / promoted QFrame
        QFrame *innerWidget = qobject_cast<QFrame *>(outerWidget->layout()->itemAt(0)->widget()); // the inner widget containing the layout
        Q_ASSERT_X(innerWidget, "CDockWidget::initStatusBar", "No inner widget");
        if (!innerWidget)
        {
            m_allowStatusBar = false;
            return;
        }
        innerWidget->setProperty("dockwidget", propertyInnerWidget());

        // status bar
        if (!m_allowStatusBar) { return; }
        m_statusBar.initStatusBar();

        // layout
        QVBoxLayout *vLayout = qobject_cast<QVBoxLayout *>(innerWidget->layout());
        Q_ASSERT_X(vLayout, "CDockWidget::initStatusBar", "No outer widget layout");
        if (!vLayout)
        {
            m_allowStatusBar = false;
            return;
        }
        vLayout->addWidget(m_statusBar.getStatusBar(), 0, Qt::AlignBottom); // 0->vertical stretch minimum

        // adjust stretching of the original widget. It was the only widget so far
        // and should occupy maximum space
        QWidget *compWidget = innerWidget->findChild<QWidget *>(QString(), Qt::FindDirectChildrenOnly);
        Q_ASSERT(compWidget);
        if (!compWidget) { return; }
        QSizePolicy sizePolicy = compWidget->sizePolicy();
        sizePolicy.setVerticalStretch(1); // make the original widget occupying maximum space
        compWidget->setSizePolicy(sizePolicy);

        // hide status bar if not floating T592 no longer init floating
        if (!this->isFloating()) { m_statusBar.hide(); }
    }

    void CDockWidget::showContextMenu(const QPoint &pos)
    {
        const QPoint globalPos = this->mapToGlobal(pos);
        QScopedPointer<QMenu> contextMenu(new QMenu(this));
        this->addToContextMenu(contextMenu.data());
        QAction *selectedItem = contextMenu.data()->exec(globalPos);
        Q_UNUSED(selectedItem)
    }

    void CDockWidget::onVisibilityChanged(bool visible)
    {
        m_dockWidgetVisible = visible;
    }

    void CDockWidget::menuChangeMargins(const QMargins &margins)
    {
        const bool frameless = this->isFrameless();
        const bool floating = this->isFloating();
        if (floating)
        {
            if (frameless)
            {
                this->setMarginsWhenFramelessFloating(margins);
            }
            else
            {
                this->setMarginsWhenFloating(margins);
            }
        }
        else
        {
            this->setMarginsWhenDocked(margins);
        }
        this->setContentsMargins(margins);
        this->repaint();
    }

    void CDockWidget::settingsChanged()
    {
        // void, normally not used
    }

    void CDockWidget::dummy()
    {
        // void
    }

    void CDockWidget::onStyleSheetsChanged()
    {
        // style sheet changes go here
    }

    void CDockWidget::forceStyleSheetUpdate()
    {
        const QString qss = this->styleSheet();
        this->setStyleSheet(qss.isEmpty() ? QStringLiteral(" ") : QString());
        this->setStyleSheet(qss);
    }

    void CDockWidget::rememberFloatingSizeAndPosition()
    {
        if (!this->isFloating()) { return; }
        m_lastFloatingSize = this->size();
        m_lastFloatingPosition = this->pos();
    }

    void CDockWidget::restoreFloatingSizeAndPosition()
    {
        if (!m_lastFloatingSize.isValid() || m_lastFloatingPosition.isNull()) { return; }
        this->resize(m_lastFloatingSize);
        this->move(m_lastFloatingPosition);
    }

    void CDockWidget::restoreFloatingSizeAndPositionDeferred()
    {
        // if (!m_lastFloatingSize.isValid() || m_lastFloatingPosition.isNull()) { return; }
        QPointer<CDockWidget> myself(this);
        QTimer::singleShot(2500, this, [=] {
            if (!myself) { return; }
            const Qt::KeyboardModifiers km = QGuiApplication::queryKeyboardModifiers();
            const bool shift = km.testFlag(Qt::ShiftModifier);
            if (shift) { return; }

            if (!sGui || sGui->isCmdWindowSizeResetSet()) { return; }
            myself->restoreFromSettings();
        });
    }

    CDockWidgetSettings CDockWidget::getSettings() const
    {
        if (this->objectName().isEmpty()) { return CDockWidgetSettings(); }

        // we need object name for settings %OwnerName%"
        const CDockWidgetSettings s = m_settings.get();
        return s;
    }

    void CDockWidget::saveSettings(const CDockWidgetSettings &settings)
    {
        SWIFT_VERIFY_X(!this->objectName().isEmpty(), Q_FUNC_INFO, "Need object name for settings %OwnerName%");
        if (this->objectName().isEmpty())
        {
            CStatusMessage(this).error(u"Settings cannot be saved!");
            return;
        }

        const CStatusMessage m = m_settings.setAndSave(settings);
        if (m.isFailure())
        {
            CLogMessage::preformatted(m);
        }
    }

    void CDockWidget::saveCurrentStateToSettings()
    {
        CDockWidgetSettings s = this->getSettings();
        const bool floating = this->isFloating();
        const bool frameless = this->isFrameless();
        const QByteArray geo = this->saveGeometry();
        s.setFloating(floating);
        s.setFrameless(frameless);
        s.setGeometry(geo);
        this->saveSettings(s);
    }

    void CDockWidget::resetSettings()
    {
        CDockWidgetSettings s = this->getSettings();
        s.reset();
        this->saveSettings(s);
        this->restoreFromSettings();
    }

    void CDockWidget::resetPosition()
    {
        // center on screen when floating
        if (!this->isFloating()) { return; }
        if (!sGui) { return; }

        // pos can be null during init
        const QWidget *mw = CGuiUtility::mainApplicationWidget();
        QPoint pos = mw && mw->isVisible() ? CGuiUtility::mainWidgetGlobalPosition() : QPoint();
        if (pos.isNull())
        {
            pos = CGuiApplication::currentScreen()->geometry().center() - this->rect().center();
        }

        const int osFloatingX = m_offsetWhenFloating.x();
        const int osFloatingY = m_offsetWhenFloating.y();
        const int x = pos.x() + osFloatingX;
        const int y = pos.y() + osFloatingY;
        this->move(x, y);
    }
} // namespace
