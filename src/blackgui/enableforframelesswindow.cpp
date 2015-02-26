/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

#include "enableforframelesswindow.h"
#include "blackmisc/icons.h"
#include <QSizeGrip>
#include <QStatusBar>
#include <QPushButton>

using namespace BlackMisc;

namespace BlackGui
{

    CEnableForFramelessWindow::CEnableForFramelessWindow(CEnableForFramelessWindow::WindowMode mode, bool isMainApplicationWindow, QWidget *correspondingWidget) :
        m_windowMode(mode), m_mainApplicationWindow(isMainApplicationWindow), m_widget(correspondingWidget)
    {
        Q_ASSERT(correspondingWidget);
        this->setWindowAttributes(mode);
    }

    void CEnableForFramelessWindow::setMode(CEnableForFramelessWindow::WindowMode mode)
    {
        if (mode == this->m_windowMode) { return; }
        this->m_widget->setWindowFlags(modeToWindowFlags(mode));
        this->setWindowAttributes(mode);
        this->m_widget->show();
        this->m_windowMode = mode;
    }

    void CEnableForFramelessWindow::setFrameless(bool frameless)
    {
        setMode(frameless ? WindowFrameless : WindowNormal);
    }

    void CEnableForFramelessWindow::setWindowAttributes(CEnableForFramelessWindow::WindowMode mode)
    {
        bool frameless = (mode == WindowFrameless);
        // http://stackoverflow.com/questions/18316710/frameless-and-transparent-window-qt5
        this->m_widget->setAttribute(Qt::WA_NoSystemBackground, frameless);
        this->m_widget->setAttribute(Qt::WA_TranslucentBackground, frameless);
        this->m_widget->setProperty("frameless", frameless);
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

    void CEnableForFramelessWindow::addFramelessSizeGrip(QStatusBar *statusBar)
    {
        if (!statusBar) { return; }
        QSizeGrip *grip = new QSizeGrip(this->m_widget);
        grip->setObjectName("sg_FramelessSizeGrip");
        statusBar->addPermanentWidget(grip);
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

    Qt::WindowFlags CEnableForFramelessWindow::modeToWindowFlags(CEnableForFramelessWindow::WindowMode mode)
    {
        switch (mode)
        {
        case WindowFrameless:
            return (Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
        case WindowNormal:
        default:
            // tool window and minimized not supported on windows
            // tool window always with close button on windows
            return (Qt::Tool | Qt::WindowStaysOnTopHint | Qt::WindowMinimizeButtonHint | Qt::WindowCloseButtonHint);
        }
    }

} // namespace
