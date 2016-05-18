/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "blackgui/enableforframelesswindow.h"
#include "blackmisc/icons.h"
#include "blackmisc/stringutils.h"
#include "blackmisc/worker.h"

#include <QEvent>
#include <QFlags>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QMouseEvent>
#include <QObject>
#include <QPushButton>
#include <QRect>
#include <QSizeGrip>
#include <QStatusBar>
#include <QThread>
#include <QVariant>
#include <QWidget>
#include <QtGlobal>

using namespace BlackMisc;

namespace BlackGui
{
    CEnableForFramelessWindow::CEnableForFramelessWindow(CEnableForFramelessWindow::WindowMode mode, bool isMainApplicationWindow, const char *framelessPropertyName, QWidget *correspondingWidget) :
        m_windowMode(mode), m_mainApplicationWindow(isMainApplicationWindow), m_widget(correspondingWidget), m_framelessPropertyName(framelessPropertyName)
    {
        Q_ASSERT(correspondingWidget);
        Q_ASSERT(!m_framelessPropertyName.isEmpty());
        this->m_originalWindowMode = mode;
        this->setWindowAttributes(mode);
        this->windowFlagsChanged();
    }

    void CEnableForFramelessWindow::setMode(CEnableForFramelessWindow::WindowMode mode)
    {
        if (mode == this->m_windowMode) { return; }
        this->m_windowMode = mode;

        // set the main window or dock widget flags and attributes
        this->m_widget->setWindowFlags(modeToWindowFlags(mode));
        this->windowFlagsChanged();
        this->setWindowAttributes(mode);
        this->m_widget->show();
    }

    void CEnableForFramelessWindow::setFrameless(bool frameless)
    {
        WindowMode nonFrameLessMode = this->m_originalWindowMode;
        if (nonFrameLessMode == WindowFrameless) { nonFrameLessMode = WindowNormal; }
        setMode(frameless ? WindowFrameless : nonFrameLessMode);
    }

    void CEnableForFramelessWindow::alwaysOnTop(bool onTop)
    {
        Qt::WindowFlags flags = this->m_widget->windowFlags();
        if (onTop)
        {
            flags |= Qt::WindowStaysOnTopHint;
        }
        else
        {
            flags &= ~Qt::WindowStaysOnTopHint;
        }
        this->m_widget->setWindowFlags(flags);
        this->windowFlagsChanged();
    }

    CEnableForFramelessWindow::WindowMode CEnableForFramelessWindow::stringToWindowMode(const QString &s)
    {
        QString ws(s.trimmed().toLower());
        if (ws.isEmpty()) { return WindowNormal; }
        if (ws.contains("frameless") || ws.startsWith("f")) { return WindowFrameless; }
        if (ws.contains("tool") || ws.startsWith("t")) { return WindowTool; }
        return WindowNormal;
    }

    QString CEnableForFramelessWindow::windowModeToString(CEnableForFramelessWindow::WindowMode m)
    {
        switch (m)
        {
        case WindowFrameless: return "frameless";
        case WindowNormal: return "normal";
        case WindowTool: return "tool";
        default:
            break;
        }
        return "normal";
    }

    void CEnableForFramelessWindow::windowFlagsChanged()
    {
        // void
    }

    void CEnableForFramelessWindow::setWindowAttributes(CEnableForFramelessWindow::WindowMode mode)
    {
        Q_ASSERT_X(this->m_widget, "CEnableForFramelessWindow::setWindowAttributes", "Missing widget representing window");
        Q_ASSERT_X(!this->m_framelessPropertyName.isEmpty(), "CEnableForFramelessWindow::setWindowAttributes", "Missing property name");

        bool frameless = (mode == WindowFrameless);
        // http://stackoverflow.com/questions/18316710/frameless-and-transparent-window-qt5
        this->m_widget->setAttribute(Qt::WA_NoSystemBackground, frameless);
        this->m_widget->setAttribute(Qt::WA_TranslucentBackground, frameless);

        // Qt::WA_PaintOnScreen leads to a warning
        // setMask(QRegion(10, 10, 10, 10) would work, but requires "complex" calcs for rounded corners
        //! \todo Transparent widget, try out void QWidget::setMask
        this->setDynamicProperties(frameless);
    }

    void CEnableForFramelessWindow::setDynamicProperties(bool frameless)
    {
        Q_ASSERT_X(this->m_widget, "CEnableForFramelessWindow::setDynamicProperties", "Missing widget representing window");
        Q_ASSERT_X(!this->m_framelessPropertyName.isEmpty(), "CEnableForFramelessWindow::setDynamicProperties", "Missing property name");

        // property selector will check on string, so I directly provide a string
        const QString f(BlackMisc::boolToTrueFalse(frameless));
        this->m_widget->setProperty(this->m_framelessPropertyName.constData(), f);
        for (QObject *w : this->m_widget->children())
        {
            if (w && w->isWidgetType())
            {
                w->setProperty(this->m_framelessPropertyName.constData(), f);
            }
        }
    }

    void CEnableForFramelessWindow::showMinimizedModeChecked()
    {
        if (m_windowMode == CEnableForFramelessWindow::WindowTool) { this->toolToNormalWindow(); }
        this->m_widget->showMinimized();
    }

    void CEnableForFramelessWindow::showNormalModeChecked()
    {
        if (m_windowMode == CEnableForFramelessWindow::WindowTool) { this->normalToToolWindow(); }
        this->m_widget->showMinimized();
    }

    bool CEnableForFramelessWindow::handleMousePressEvent(QMouseEvent *event)
    {
        Q_ASSERT(this->m_widget);
        if (this->m_windowMode == WindowFrameless && event->button() == Qt::LeftButton)
        {
            this->m_framelessDragPosition = event->globalPos() - this->m_widget->frameGeometry().topLeft();
            event->accept();
            return true;
        }
        return false;
    }

    bool CEnableForFramelessWindow::handleMouseMoveEvent(QMouseEvent *event)
    {
        Q_ASSERT(this->m_widget);
        if (this->m_windowMode == WindowFrameless && event->buttons() & Qt::LeftButton)
        {
            this->m_widget->move(event->globalPos() - this->m_framelessDragPosition);
            event->accept();
            return true;
        }
        return false;
    }

    bool CEnableForFramelessWindow::handleChangeEvent(QEvent *event)
    {
        if (event->type() != QEvent::WindowStateChange) { return false; }
        if (m_windowMode != WindowTool) { return false; }

        // make sure a tool window is changed to Normal window so it is show in taskbar
        // here we are already in transition state, so isMinimized means will be minimize right now
        // this check here is needed if minimized is called from somewhere else than ps_showMinimized
        if (m_widget->isMinimized())
        {
            // still tool, force normal window
            // decouple, otherwise we end up in infinite loop as it triggers a new changeEvent
            BlackMisc::singleShot(0, QThread::currentThread(), [ = ]()
            {
                this->showMinimizedModeChecked();
            });
        }
        else
        {
            // not tool, force tool window
            // decouple, otherwise we end up in infinite loop as it triggers a new changeEvent
            BlackMisc::singleShot(0, QThread::currentThread(), [ = ]()
            {
                this->showNormalModeChecked();
            });
        }
        event->accept();
        return true;
    }

    void CEnableForFramelessWindow::addFramelessSizeGripToStatusBar(QStatusBar *statusBar)
    {
        if (!statusBar) { return; }
        if (!this->m_framelessSizeGrip)
        {
            this->m_framelessSizeGrip = new QSizeGrip(this->m_widget);
            this->m_framelessSizeGrip->setObjectName("sg_FramelessSizeGrip");
            statusBar->addPermanentWidget(this->m_framelessSizeGrip);
        }
        else
        {
            this->m_framelessSizeGrip->show();
        }
        statusBar->repaint();
    }

    void CEnableForFramelessWindow::hideFramelessSizeGripInStatusBar()
    {
        if (!this->m_framelessSizeGrip) { return; }
        this->m_framelessSizeGrip->hide();
    }

    QHBoxLayout *CEnableForFramelessWindow::addFramelessCloseButton(QMenuBar *menuBar)
    {
        Q_ASSERT(isFrameless());
        Q_ASSERT(menuBar);
        Q_ASSERT(this->m_widget);

        if (!m_framelessCloseButton)
        {
            m_framelessCloseButton = new QPushButton(this->m_widget);
            m_framelessCloseButton->setObjectName("pb_FramelessCloseButton");
            m_framelessCloseButton->setIcon(CIcons::close16());
            QObject::connect(m_framelessCloseButton, &QPushButton::clicked, this->m_widget, &QWidget::close);
        }

        QHBoxLayout *menuBarLayout = new QHBoxLayout;
        menuBarLayout->setObjectName("hl_MenuBar");
        menuBarLayout->addWidget(menuBar, 0, Qt::AlignTop | Qt::AlignLeft);
        menuBarLayout->addWidget(m_framelessCloseButton, 0, Qt::AlignTop | Qt::AlignRight);
        return menuBarLayout;
    }

    void CEnableForFramelessWindow::toolToNormalWindow()
    {
        this->m_widget->setWindowFlags((this->m_widget->windowFlags() & (~Qt::Tool)) | Qt::Window);
        this->windowFlagsChanged();
        this->m_originalWindowMode = WindowNormal;
    }

    void CEnableForFramelessWindow::normalToToolWindow()
    {
        this->m_widget->setWindowFlags(this->m_widget->windowFlags() | Qt::Tool);
        this->windowFlagsChanged();
        this->m_originalWindowMode = WindowTool;
    }

    bool CEnableForFramelessWindow::isToolWindow() const
    {
        return (this->m_widget->windowFlags() & Qt::Tool) == Qt::Tool;
    }

    Qt::WindowFlags CEnableForFramelessWindow::modeToWindowFlags(CEnableForFramelessWindow::WindowMode mode)
    {
        switch (mode)
        {
        case WindowFrameless:
            return (Qt::Window | Qt::FramelessWindowHint);
        case WindowTool:
            // tool window and minimized not supported on Windows
            // tool window always with close button on Windows
            return (Qt::Tool | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);
        case WindowNormal:
        default:
            return (Qt::Window | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);
        }
    }
} // namespace
