/* Copyright (C) 2014
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_ENABLEFORFRAMLESSWINDOW_H
#define BLACKGUI_ENABLEFORFRAMLESSWINDOW_H

#include <QWidget>
#include <QStatusBar>
#include <QMouseEvent>
#include <QPushButton>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QSizeGrip>

namespace BlackGui
{

    //! Main window which can be frameless
    //! \details QMainWindows cannot be promoted. Hence a derived class does not work properly here.
    //!          Furthermore frameless functionality is also required for CDockWidgets as well.
    class CEnableForFramelessWindow
    {
    public:
        //! Window modes
        enum WindowMode
        {
            WindowFrameless,
            WindowNormal
        };

        //! Constructor
        CEnableForFramelessWindow(WindowMode mode, bool isMainApplicationWindow, QWidget *correspondingWidget);

        //! Window mode
        void setMode(WindowMode mode);

        //! Framless
        virtual void setFrameless(bool frameless);

        //! Frameless?
        bool isFrameless() const { return this->m_windowMode == WindowFrameless; }

        //! The main application
        bool isMainApplicationWindow() const { return m_mainApplicationWindow; }

        //! Corresponding QMainWindow
        QWidget *getWidget() const { return m_widget; }

    protected:
        //! Resize grip handle
        void addFramelessSizeGripToStatusBar(QStatusBar *statusBar);

        //! Resize grip handle
        void hideFramelessSizeGripInStatusBar();

        //! Attributes
        void setWindowAttributes(WindowMode mode);

        //! Close button for frameless windows
        QHBoxLayout *addFramelessCloseButton(QMenuBar *menuBar);

        //! Translate mode
        static Qt::WindowFlags modeToWindowFlags(WindowMode mode);

        QPoint       m_framelessDragPosition;          //!< position, if moving is handled with frameless window */
        QPushButton *m_framelessCloseButton = nullptr; //!< close button
        WindowMode   m_windowMode = WindowNormal;      //!< Window mode, \sa WindowMode
        bool         m_mainApplicationWindow = false;  //!< is the main application window (only 1)
        QWidget     *m_widget = nullptr;               //!< corresponding main window or dock widget
        QSizeGrip   *m_framelessSizeGrip = nullptr;    //!< size grip object

        //! Mouse press, required for frameless window
        bool handleMousePressEvent(QMouseEvent *event);

        //! Mouse moving, required for frameless window
        bool handleMouseMoveEvent(QMouseEvent *event);

    };

} // namespace

#endif // guard
