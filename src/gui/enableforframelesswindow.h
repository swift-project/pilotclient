// SPDX-FileCopyrightText: Copyright (C) 2014 swift Project Community / Contributors
// SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

//! \file

#ifndef SWIFT_GUI_ENABLEFORFRAMLESSWINDOW_H
#define SWIFT_GUI_ENABLEFORFRAMLESSWINDOW_H

#include <QByteArray>
#include <QPoint>
#include <QSize>
#include <QString>
#include <Qt>

#include "gui/swiftguiexport.h"

class QEvent;
class QHBoxLayout;
class QMenuBar;
class QMouseEvent;
class QPushButton;
class QSizeGrip;
class QStatusBar;
class QWidget;

namespace swift::gui
{
    //! Main window which can be frameless
    //! \details QMainWindows cannot be promoted. Hence a derived class does not work properly here.
    //!          Furthermore frameless functionality is also required for CDockWidgets as well.
    class SWIFT_GUI_EXPORT CEnableForFramelessWindow
    {
    public:
        //! Window modes
        enum WindowMode
        {
            WindowNormal,
            WindowFrameless,
            WindowTool
        };

        //! Constructor
        //! \param mode                    window mode as defined in WindowMode
        //! \param isMainApplicationWindow is this the main (there should be only one) application window
        //! \param framelessPropertyname   qss property indication frameless
        //! \param correspondingWidget     the widget representing the window
        CEnableForFramelessWindow(WindowMode mode, bool isMainApplicationWindow, const char *framelessPropertyname,
                                  QWidget *correspondingWidget);

        //! Destructor
        virtual ~CEnableForFramelessWindow() {}

        //! Copy constructor
        CEnableForFramelessWindow(const CEnableForFramelessWindow &) = delete;

        //! Copy assignment operator
        CEnableForFramelessWindow &operator=(const CEnableForFramelessWindow &) = delete;

        //! Window mode
        void setMode(WindowMode mode);

        //! Framless
        virtual void setFrameless(bool frameless);

        //! Frameless?
        bool isFrameless() const { return m_windowMode == WindowFrameless; }

        //! Is main application, explicitly set
        bool isMainApplicationWindow() const { return m_isMainApplicationWindow; }

        //! Always on top?
        void alwaysOnTop(bool onTop);

        //! Activates the window
        void activate();

        //! Corresponding QMainWindow
        QWidget *getWidget() const { return m_widget; }

        //! String to window mode
        static WindowMode stringToWindowMode(const QString &s);

        //! String to window mode
        static const QString &windowModeToString(WindowMode m);

    protected:
        QPoint m_framelessDragPosition; //!< position, if moving is handled with frameless window
        QSize m_moveSize; //!< size when moved (in frameless window)
        QPushButton *m_framelessCloseButton = nullptr; //!< close button
        WindowMode m_windowMode = WindowNormal; //!< Window mode, \sa WindowMode
        WindowMode m_originalWindowMode = WindowNormal; //!< mode when initialized
        bool m_isMainApplicationWindow = false; //!< is this the main application window (only 1)?
        QWidget *m_widget = nullptr; //!< corresponding window or dock widget
        QSizeGrip *m_framelessSizeGrip = nullptr; //!< size grip object
        QByteArray m_framelessPropertyName; //!< property name for frameless widgets
        int m_windowFrameSizeW = -1; //!< window frame width
        int m_windowFrameSizeH = -1; //!< window frame height

        //! Can be used as notification if window mode changes
        virtual void windowFlagsChanged();

        //! Resize grip handle
        void addFramelessSizeGripToStatusBar(QStatusBar *statusBar);

        //! Resize grip handle
        void hideFramelessSizeGripInStatusBar();

        //! Attributes
        void setWindowAttributes(WindowMode mode);

        //! Set dynamic properties such as frameless
        void setDynamicProperties(bool frameless);

        //! Close button for frameless windows
        QHBoxLayout *addFramelessCloseButton(QMenuBar *menuBar);

        //! Remove tool and add desktop window
        void toolToNormalWindow();

        //! Remove desktop and add tool window
        void normalToToolWindow();

        //! Tool window
        bool isToolWindow() const;

        //! Mouse press, required for frameless window
        bool handleMousePressEvent(QMouseEvent *event);

        //! Mouse moving, required for frameless window
        bool handleMouseMoveEvent(QMouseEvent *event);

        //! Mouse window change event
        bool handleChangeEvent(QEvent *event);

        //! Check mode and then show minimized
        void showMinimizedModeChecked();

        //! Check mode and then show normal
        void showNormalModeChecked();

        //! Translate mode
        static Qt::WindowFlags modeToWindowFlags(WindowMode mode);
    };
} // namespace swift::gui

#endif // SWIFT_GUI_ENABLEFORFRAMLESSWINDOW_H
