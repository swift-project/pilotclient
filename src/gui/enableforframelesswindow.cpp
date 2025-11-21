// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#include "gui/enableforframelesswindow.h"

#include <QEvent>
#include <QFlags>
#include <QHBoxLayout>
#include <QMainWindow>
#include <QMenuBar>
#include <QMouseEvent>
#include <QObject>
#include <QPointer>
#include <QPushButton>
#include <QRect>
#include <QSizeGrip>
#include <QStatusBar>
#include <QThread>
#include <QTimer>
#include <QVariant>
#include <QWidget>
#include <QtGlobal>

#include "gui/guiutility.h"
#include "misc/icons.h"
#include "misc/stringutils.h"
#include "misc/worker.h"

using namespace swift::misc;

namespace swift::gui
{
    CEnableForFramelessWindow::CEnableForFramelessWindow(CEnableForFramelessWindow::WindowMode mode,
                                                         bool isMainApplicationWindow,
                                                         const char *framelessPropertyName,
                                                         QWidget *correspondingWidget)
        : m_windowMode(mode), m_isMainApplicationWindow(isMainApplicationWindow), m_widget(correspondingWidget),
          m_framelessPropertyName(framelessPropertyName)
    {
        Q_ASSERT(correspondingWidget);
        Q_ASSERT(!m_framelessPropertyName.isEmpty());
        m_originalWindowMode = mode;
        this->setWindowAttributes(mode);
        this->windowFlagsChanged();
    }

    void CEnableForFramelessWindow::setMode(CEnableForFramelessWindow::WindowMode mode)
    {
        if (mode == m_windowMode) { return; }
        m_windowMode = mode;

        // set the main window or dock widget flags and attributes
        m_widget->setWindowFlags(modeToWindowFlags(mode));
        this->windowFlagsChanged();
        this->setWindowAttributes(mode);
        m_widget->show();
    }

    void CEnableForFramelessWindow::setFrameless(bool frameless)
    {
        const bool isFrameless = this->isFrameless();
        if (isFrameless == frameless) { return; }

        QWidget *w = this->getWidget();
        if (!w) { return; }
        const QRect oldFrameGeometry = w->frameGeometry();
        const QRect oldGeometry = w->geometry();

        WindowMode nonFrameLessMode = m_originalWindowMode; // Tool/Normal Window
        if (nonFrameLessMode == WindowFrameless) { nonFrameLessMode = WindowNormal; }
        this->setMode(frameless ? WindowFrameless : nonFrameLessMode);

        if (frameless)
        {
            // from framed to frameless
            w->setGeometry(oldFrameGeometry);
            m_windowFrameSizeW = oldFrameGeometry.width() - oldGeometry.width();
            m_windowFrameSizeH = oldFrameGeometry.height() - oldGeometry.height();
        }
        else
        {
            if (m_windowFrameSizeW >= 0 && m_windowFrameSizeH >= 0)
            {
                QRect newGeometry = oldGeometry;
                // newGeometry.setWidth(oldGeometry.width() - m_windowFrameSizeW);
                // newGeometry.setHeight(oldGeometry.height() - m_windowFrameSizeH);
                newGeometry.setX(oldGeometry.x() + m_windowFrameSizeW);
                newGeometry.setY(oldGeometry.y() + m_windowFrameSizeH);
                w->setGeometry(newGeometry);
            }
        }
    }

    void CEnableForFramelessWindow::alwaysOnTop(bool onTop)
    {
        Qt::WindowFlags flags = m_widget->windowFlags();
        if (onTop) { flags |= Qt::WindowStaysOnTopHint; }
        else { flags &= ~Qt::WindowStaysOnTopHint; }
        m_widget->setWindowFlags(flags);
        this->windowFlagsChanged();
    }

    void CEnableForFramelessWindow::activate()
    {
        if (!m_widget) { return; }
        m_widget->setWindowState(Qt::WindowActive);
    }

    CEnableForFramelessWindow::WindowMode CEnableForFramelessWindow::stringToWindowMode(const QString &s)
    {
        const QString ws(s.trimmed().toLower());
        if (ws.isEmpty()) { return WindowNormal; }
        if (ws.contains("frameless") || ws.startsWith("f")) { return WindowFrameless; }
        if (ws.contains("tool") || ws.startsWith("t")) { return WindowTool; }
        return WindowNormal;
    }

    const QString &CEnableForFramelessWindow::windowModeToString(CEnableForFramelessWindow::WindowMode m)
    {
        static const QString n("normal");
        static const QString f("frameless");
        static const QString t("tool");

        switch (m)
        {
        case WindowFrameless: return f;
        case WindowNormal: return n;
        case WindowTool: return t;
        default: break;
        }
        return n;
    }

    void CEnableForFramelessWindow::windowFlagsChanged()
    {
        // void
    }

    void CEnableForFramelessWindow::setWindowAttributes(CEnableForFramelessWindow::WindowMode mode)
    {
        Q_ASSERT_X(m_widget, "CEnableForFramelessWindow::setWindowAttributes", "Missing widget representing window");
        Q_ASSERT_X(!m_framelessPropertyName.isEmpty(), "CEnableForFramelessWindow::setWindowAttributes",
                   "Missing property name");

        const bool frameless = (mode == WindowFrameless);

        // http://stackoverflow.com/questions/18316710/frameless-and-transparent-window-qt5
        // https://bugreports.qt.io/browse/QTBUG-52206
        // UpdateLayeredWindowIndirect failed for ptDst
        if (m_isMainApplicationWindow && CGuiUtility::isTopLevelWindow(m_widget))
        {
            m_widget->setAttribute(Qt::WA_NativeWindow);

            // causing a BLACK background
            m_widget->setAttribute(Qt::WA_NoSystemBackground, frameless);
            m_widget->setAttribute(Qt::WA_TranslucentBackground, frameless); // causing QTBUG-52206
        }

        // Qt::WA_PaintOnScreen leads to a warning
        // setMask(QRegion(10, 10, 10, 10) would work, but requires "complex" calcs for rounded corners
        //! \fixme further improve transparent widget, try out void QWidget::setMask
        this->setDynamicProperties(frameless);
    }

    void CEnableForFramelessWindow::setDynamicProperties(bool frameless)
    {
        Q_ASSERT_X(m_widget, "CEnableForFramelessWindow::setDynamicProperties", "Missing widget representing window");
        Q_ASSERT_X(!m_framelessPropertyName.isEmpty(), "CEnableForFramelessWindow::setDynamicProperties",
                   "Missing property name");

        // property selector will check on string, so I directly provide a string
        const QString f(swift::misc::boolToTrueFalse(frameless));
        m_widget->setProperty(m_framelessPropertyName.constData(), f);
        for (QObject *w : m_widget->children())
        {
            if (w && w->isWidgetType()) { w->setProperty(m_framelessPropertyName.constData(), f); }
        }
    }

    void CEnableForFramelessWindow::showMinimizedModeChecked()
    {
        if (m_windowMode == CEnableForFramelessWindow::WindowTool) { this->toolToNormalWindow(); }
        m_widget->showMinimized();
    }

    void CEnableForFramelessWindow::showNormalModeChecked()
    {
        if (m_windowMode == CEnableForFramelessWindow::WindowTool) { this->normalToToolWindow(); }
        m_widget->showMinimized();
    }

    bool CEnableForFramelessWindow::handleMousePressEvent(QMouseEvent *event)
    {
        Q_ASSERT(m_widget);
        if (m_windowMode == WindowFrameless && event->button() == Qt::LeftButton)
        {
            m_framelessDragPosition = event->globalPosition().toPoint() - m_widget->frameGeometry().topLeft();
            event->accept();
            return true;
        }
        return false;
    }

    bool CEnableForFramelessWindow::handleMouseMoveEvent(QMouseEvent *event)
    {
        Q_ASSERT(m_widget);
        if (m_windowMode == WindowFrameless && event->buttons() == Qt::LeftButton && !m_framelessDragPosition.isNull())
        {
            const QSize s = m_widget->size();
            const bool changedSize = (m_moveSize != s);

            // avoid "jumping around" if window was resized
            // resizing in frameless is subject of QSizeGrip in the status bar
            if (changedSize)
            {
                m_moveSize = s;
                return false;
            }

            m_widget->move(event->globalPosition().toPoint() - m_framelessDragPosition);
            event->accept();
            return true;
        }
        return false;
    }

    bool CEnableForFramelessWindow::handleChangeEvent(QEvent *event)
    {
        if (event->type() != QEvent::WindowStateChange) { return false; }
        if (m_windowMode != WindowTool) { return false; }
        if (!m_widget) { return false; }

        // make sure a tool window is changed to Normal window so it is show in taskbar
        // here we are already in transition state, so isMinimized means will be minimize right now
        // this check here is needed if minimized is called from somewhere else than ps_showMinimized

        QPointer<QWidget> widgetSelf(m_widget); // almost as good as myself
        if (m_widget->isMinimized())
        {
            // still tool, force normal window
            // decouple, otherwise we end up in infinite loop as it triggers a new changeEvent

            QTimer::singleShot(0, m_widget, [=] {
                if (!widgetSelf) { return; }
                this->showMinimizedModeChecked();
            });
        }
        else
        {
            // not tool, force tool window
            // decouple, otherwise we end up in infinite loop as it triggers a new changeEvent
            QTimer::singleShot(0, m_widget, [=] {
                if (!widgetSelf) { return; }
                this->showNormalModeChecked();
            });
        }
        event->accept();
        return true;
    }

    void CEnableForFramelessWindow::addFramelessSizeGripToStatusBar(QStatusBar *statusBar)
    {
        if (!statusBar) { return; }
        if (!m_framelessSizeGrip)
        {
            m_framelessSizeGrip = new QSizeGrip(m_widget);
            m_framelessSizeGrip->setObjectName("sg_FramelessSizeGrip");
            statusBar->addPermanentWidget(m_framelessSizeGrip);
        }
        else { m_framelessSizeGrip->show(); }
        statusBar->repaint();
    }

    void CEnableForFramelessWindow::hideFramelessSizeGripInStatusBar()
    {
        if (!m_framelessSizeGrip) { return; }
        m_framelessSizeGrip->hide();
    }

    QHBoxLayout *CEnableForFramelessWindow::addFramelessCloseButton(QMenuBar *menuBar)
    {
        Q_ASSERT(isFrameless());
        Q_ASSERT(menuBar);
        Q_ASSERT(m_widget);

        if (!m_framelessCloseButton)
        {
            m_framelessCloseButton = new QPushButton(m_widget);
            m_framelessCloseButton->setObjectName("pb_FramelessCloseButton");
            m_framelessCloseButton->setIcon(CIcons::close16());
            QObject::connect(m_framelessCloseButton, &QPushButton::clicked, m_widget, &QWidget::close,
                             Qt::QueuedConnection);
        }

        auto *menuBarLayout = new QHBoxLayout;
        menuBarLayout->setObjectName("hl_MenuBar");
        menuBarLayout->addWidget(menuBar, 0, Qt::AlignTop | Qt::AlignLeft);
        menuBarLayout->addWidget(m_framelessCloseButton, 0, Qt::AlignTop | Qt::AlignRight);
        return menuBarLayout;
    }

    void CEnableForFramelessWindow::toolToNormalWindow()
    {
        m_widget->setWindowFlags((m_widget->windowFlags() & (~Qt::Tool)) | Qt::Window);
        this->windowFlagsChanged();
        m_originalWindowMode = WindowNormal;
    }

    void CEnableForFramelessWindow::normalToToolWindow()
    {
        m_widget->setWindowFlags(m_widget->windowFlags() | Qt::Tool);
        this->windowFlagsChanged();
        m_originalWindowMode = WindowTool;
    }

    bool CEnableForFramelessWindow::isToolWindow() const { return (m_widget->windowFlags() & Qt::Tool) == Qt::Tool; }

    Qt::WindowFlags CEnableForFramelessWindow::modeToWindowFlags(CEnableForFramelessWindow::WindowMode mode)
    {
        switch (mode)
        {
        case WindowFrameless: return (Qt::Window | Qt::FramelessWindowHint);
        case WindowTool:
            // tool window and minimized not supported on Windows
            // tool window always with close button on Windows
            return (Qt::Tool | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);
        case WindowNormal:
        default:
            return (Qt::Window | Qt::WindowMaximizeButtonHint | Qt::WindowMinimizeButtonHint |
                    Qt::WindowCloseButtonHint);
        }
    }
} // namespace swift::gui
