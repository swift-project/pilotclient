/* Copyright (C) 2016
 * swift project Community / Contributors
 *
 * This file is part of swift Project. It is subject to the license terms in the LICENSE file found in the top-level
 * directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
 * including this file, may be copied, modified, propagated, or distributed except according to the terms
 * contained in the LICENSE file.
 */

//! \file

#ifndef BLACKGUI_GUIAPPLICATION_H
#define BLACKGUI_GUIAPPLICATION_H

#include "blackcore/application.h"
#include "blackgui/enableforframelesswindow.h"
#include "blackgui/blackguiexport.h"

namespace BlackGui
{
    /*!
     * \brief GUI application, a specialized version of BlackCore::CApplication for GUI applications.
     *
     * \details Analog to QCoreApplication and QApplication this class provides more details for swift
     * GUI applications. It is normally used via the global sGui pointer. As an example of how to extend this
     * class see CSwiftGuiStdApplication.
     */
    class BLACKGUI_EXPORT CGuiApplication : public BlackCore::CApplication
    {
        Q_OBJECT

    public:
        //! Similar to \sa QCoreApplication::instance() returns the single instance
        static CGuiApplication *instance();

        //! Constructor
        CGuiApplication(const QString &applicationName = executable(), const QPixmap &icon = BlackMisc::CIcons::swift48());

        //! Destructor
        virtual ~CGuiApplication();

        //! CMD line arguments
        void addWindowStateOption();

        //! CMD line arguments
        void addWindowModeOption();

        //! Window state
        Qt::WindowState getWindowState() const;

        //! Window mode (window flags)
        CEnableForFramelessWindow::WindowMode getWindowMode() const;

        //! Init the main application window based on information in this application
        void initMainApplicationWindow(QWidget *mainWindow) const;

        //! \name print messages generated during parsing / cmd handling
        //! @{
        virtual void cmdLineErrorMessage(const QString &cmdLineErrorMessage) const override;
        //! @}

        //! Set icon
        //! \note Pixmap requires a valid QApplication, so it cannot be passed as constructor parameter
        static void setWindowIcon(const QPixmap &icon);

        //! Exit application, perform graceful shutdown and exit
        static void exit(int retcode = 0);

    protected:
        //! \name print messages generated during parsing / cmd handling
        //! @{
        virtual void cmdLineHelpMessage() override;
        virtual void cmdLineVersionMessage() const override;
        //! @}

        //! Handle paring of special GUI cmd arguments
        virtual bool parsingHookIn() override;

    private:
        QPixmap m_windowIcon;
        QCommandLineOption m_cmdWindowStateMinimized { "empty" }; //!< window state (minimized)
        QCommandLineOption m_cmdWindowMode { "empty" };            //!< window mode (flags: frameless ...)
    };
} // ns

//! Single instance of GUI application object
extern BLACKGUI_EXPORT BlackGui::CGuiApplication *sGui;

#endif // guard
